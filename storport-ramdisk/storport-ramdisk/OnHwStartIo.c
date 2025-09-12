/*
*	storport-ramdisk - OnHwStartIo.c
*	램 디스크 가상 장치를 구현하는 Storport Virutal Miniport Driver 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "storport-ramdisk.h"
#include "dbglog.h"

BOOLEAN OnHwStartIo(
	IN PVOID DeviceExtension,
	IN PSCSI_REQUEST_BLOCK Srb
) {
	PHW_DEVICE_EXTENSION pDeviceExtension = NULL;
	PCDB pCdb = NULL;
	PVOID pBuf = NULL;
	UCHAR SrbStatus = SRB_STATUS_SUCCESS;

	if((DeviceExtension == NULL) || (Srb == NULL))
		return FALSE;

	pDeviceExtension = (PHW_DEVICE_EXTENSION)DeviceExtension;
	pCdb = (PCDB)Srb->Cdb;
	
	switch (Srb->Function) {
		case SRB_FUNCTION_EXECUTE_SCSI: {
			ULONG status = StorPortGetSystemAddress(DeviceExtension, Srb, &pBuf);
			if (status != STOR_STATUS_SUCCESS && status != STOR_STATUS_INVALID_PARAMETER) {
				DBGPRINT_ERROR("StorPortGetSystemAddress() failed 0x%8X, OpCode=0x%2X", status, pCdb->CDB6GENERIC.OperationCode);
				SrbStatus = SRB_STATUS_INTERNAL_ERROR;
				break;
			}

			if (pCdb->CDB6GENERIC.OperationCode == SCSIOP_REPORT_LUNS) {
				DBGPRINT_INFO("SCSIOP_REPORT_LUNS");

				struct _REPORT_LUNS* pReportLuns = NULL;
				PLUN_LIST pLunList = NULL;

				if (Srb->CdbLength != 12) {
					DBGPRINT_ERROR("Invalid CdbLength for REPORT_LUNS: %d", Srb->CdbLength);
					SrbStatus = SRB_STATUS_INVALID_PARAMETER;
					break;
				}

				pReportLuns = (struct _REPORT_LUNS*)Srb->Cdb;
				if (pReportLuns->AllocationLength <= 0) {
					SrbStatus = SRB_STATUS_INVALID_PARAMETER;
					break;
				}

				if (Srb->TargetId != 0) {
					Srb->ScsiStatus = SCSISTAT_CHECK_CONDITION;
					SrbStatus = SRB_STATUS_NO_DEVICE;
					break;
				}

				pLunList = (PLUN_LIST)pBuf;
				RtlZeroMemory(pLunList, Srb->DataTransferLength);

				pLunList->LunListLength[3] = 8; // 1 LUN * 8 bytes
				pLunList->Lun[0][0] = 0; 
				pLunList->Lun[0][1] = 0; // LUN 0

				Srb->DataTransferLength = sizeof(LUN_LIST);
				Srb->ScsiStatus = SCSISTAT_GOOD;
			}
			else if (pCdb->CDB6GENERIC.OperationCode == SCSIOP_READ_CAPACITY) {
				DBGPRINT_INFO("SCSIOP_READ_CAPACITY");

				PREAD_CAPACITY_DATA pCapacity = NULL;
				ULONG logicalBlockAddress = 0;
				ULONG bytesPerBlock = 0;

				pCapacity = (PREAD_CAPACITY_DATA)pBuf;
				logicalBlockAddress = pDeviceExtension->TotalBlocks - 1;
				bytesPerBlock = SIZE_LOGICAL_BLOCK;

				REVERSE_BYTES(&pCapacity->LogicalBlockAddress, &logicalBlockAddress);
				REVERSE_BYTES(&pCapacity->BytesPerBlock, &bytesPerBlock);
				Srb->ScsiStatus = SCSISTAT_GOOD;
			}
			else if (pCdb->CDB6GENERIC.OperationCode == SCSIOP_INQUIRY) {
				DBGPRINT_INFO("SCSIOP_INQUIRY");

				PINQUIRYDATA pInquiryData = (PINQUIRYDATA)pBuf;
				if(pInquiryData == NULL) {
					SrbStatus = SRB_STATUS_INVALID_PARAMETER;
					goto exit;
				}

				for (ULONG i = 0; i < Srb->DataTransferLength; i++)
					((PUCHAR)pInquiryData)[i] = 0;

				if (pCdb->CDB6INQUIRY3.EnableVitalProductData) {
					SrbStatus = OnInquiryVpdRequest(pDeviceExtension, Srb);
					goto exit;
				}

				pInquiryData->DeviceType = DIRECT_ACCESS_DEVICE;
				pInquiryData->Versions = 0x06;				
				pInquiryData->AdditionalLength = 91;			
				pInquiryData->SoftReset = 0;
				pInquiryData->CommandQueue = 1;
				pInquiryData->TransferDisable = 0;
				pInquiryData->LinkedCommands = 1;
				pInquiryData->Synchronous = 1;
				pInquiryData->Wide16Bit = 1;
				pInquiryData->Wide32Bit = 0;
				pInquiryData->RelativeAddressing = 0;

				RtlCopyMemory(pInquiryData->VendorId, "CINT\0\0\0", 8);
				RtlCopyMemory(pInquiryData->ProductId, "VIRTUAL_DISK\0\0\0", 16);
				RtlCopyMemory(pInquiryData->ProductRevisionLevel, "1.00", 4);

				Srb->ScsiStatus = SCSISTAT_GOOD;
				break;
			}
			else if (pCdb->CDB6GENERIC.OperationCode == SCSIOP_MODE_SENSE) {
				if((pBuf == NULL) || (Srb->DataTransferLength <= 0))
					break;

				RtlZeroMemory(pBuf, Srb->DataTransferLength);

				if (pCdb->MODE_SENSE.Pc != 1) {
					PMODE_PARAMETER_HEADER pModeHeader = NULL;
					ULONG diskSize = 0;

					diskSize = pDeviceExtension->TotalBlocks;
					pModeHeader = (PMODE_PARAMETER_HEADER)pBuf;
					pModeHeader->ModeDataLength = sizeof(MODE_PARAMETER_HEADER);
					pModeHeader->MediumType = 0;
					pModeHeader->DeviceSpecificParameter = 0;

					if (!pCdb->MODE_SENSE.Dbd && (pCdb->MODE_SENSE.AllocationLength >= (pModeHeader->ModeDataLength + sizeof(MODE_PARAMETER_BLOCK)))) {
						PMODE_PARAMETER_BLOCK block = NULL;
						ULONG logicalBlockAddress = 0;
						ULONG bytesPerBlock = 0;

						block = (PMODE_PARAMETER_BLOCK)((PUCHAR)pModeHeader + pModeHeader->ModeDataLength);
						logicalBlockAddress = diskSize;
						bytesPerBlock = SIZE_LOGICAL_BLOCK;
						
						pModeHeader->BlockDescriptorLength = sizeof(MODE_PARAMETER_BLOCK);
						pModeHeader->ModeDataLength += sizeof(MODE_PARAMETER_BLOCK);
						block->DensityCode = 0;

						block->NumberOfBlocks[0] = (UCHAR)((logicalBlockAddress >> 16) & 0xFF);
						block->NumberOfBlocks[1] = (UCHAR)((logicalBlockAddress >> 8) & 0xFF);
						block->NumberOfBlocks[2] = (UCHAR)(logicalBlockAddress & 0xFF);

						block->BlockLength[0] = (UCHAR)((bytesPerBlock >> 16) & 0xFF);
						block->BlockLength[1] = (UCHAR)((bytesPerBlock >> 8) & 0xFF);
						block->BlockLength[2] = (UCHAR)(bytesPerBlock & 0xFF);
					}
					else 
						pModeHeader->BlockDescriptorLength = 0;

					if ((pCdb->MODE_SENSE.AllocationLength >= (pModeHeader->ModeDataLength + sizeof(MODE_PAGE_FORMAT_DEVICE))) &&
						((pCdb->MODE_SENSE.PageCode == MODE_SENSE_RETURN_ALL) ||
							(pCdb->MODE_SENSE.PageCode == MODE_PAGE_FORMAT_DEVICE)))
					{
						PMODE_FORMAT_PAGE format = NULL;

						format = (PMODE_FORMAT_PAGE)((PUCHAR)pModeHeader + pModeHeader->ModeDataLength);
						pModeHeader->ModeDataLength += sizeof(MODE_FORMAT_PAGE);

						format->PageCode = MODE_PAGE_FORMAT_DEVICE;
						format->PageLength = sizeof(MODE_FORMAT_PAGE);
						format->TracksPerZone[0] = 0;		   // we have only one zone
						format->TracksPerZone[1] = 0;
						format->AlternateSectorsPerZone[0] = 0;
						format->AlternateSectorsPerZone[1] = 0;
						format->AlternateTracksPerZone[0] = 0;
						format->AlternateTracksPerZone[1] = 0;
						format->AlternateTracksPerLogicalUnit[0] = 0;
						format->AlternateTracksPerLogicalUnit[1] = 0;
						format->SectorsPerTrack[0] = (UCHAR)((diskSize >> 8) & 0xFF);
						format->SectorsPerTrack[1] = (UCHAR)((diskSize >> 0) & 0xFF);
						format->BytesPerPhysicalSector[0] = (UCHAR)((512 >> 8) & 0xFF);
						format->BytesPerPhysicalSector[1] = (UCHAR)((512 >> 0) & 0xFF);
						format->SoftSectorFormating = 1;
					}

					if ((pCdb->MODE_SENSE.AllocationLength >= (pModeHeader->ModeDataLength + sizeof(MODE_DISCONNECT_PAGE))) &&
						((pCdb->MODE_SENSE.PageCode == MODE_SENSE_RETURN_ALL) ||
							(pCdb->MODE_SENSE.PageCode == MODE_PAGE_DISCONNECT)))
					{
						PMODE_DISCONNECT_PAGE disconnectPage = NULL;

						disconnectPage = (PMODE_DISCONNECT_PAGE)((PUCHAR)pModeHeader + pModeHeader->ModeDataLength);
						pModeHeader->ModeDataLength += sizeof(MODE_DISCONNECT_PAGE);

						disconnectPage->PageCode = MODE_PAGE_DISCONNECT;
						disconnectPage->PageLength = sizeof(MODE_DISCONNECT_PAGE);
						disconnectPage->BufferFullRatio = 0xFF;
						disconnectPage->BufferEmptyRatio = 0xFF;
						disconnectPage->BusInactivityLimit[0] = 0;
						disconnectPage->BusInactivityLimit[1] = 1;
						disconnectPage->BusDisconnectTime[0] = 0;
						disconnectPage->BusDisconnectTime[1] = 1;
						disconnectPage->BusConnectTime[0] = 0;
						disconnectPage->BusConnectTime[1] = 1;
						disconnectPage->MaximumBurstSize[0] = 0;
						disconnectPage->MaximumBurstSize[1] = 1;
						disconnectPage->DataTransferDisconnect = 1;
					}
					Srb->DataTransferLength = pModeHeader->ModeDataLength;
					Srb->ScsiStatus = SCSISTAT_GOOD;
					break;
				}
				break;
			}
			else if (pCdb->CDB6GENERIC.OperationCode == SCSIOP_TEST_UNIT_READY) {
				DBGPRINT_INFO("SCSIOP_TEST_UNIT_READY");
				Srb->SrbStatus = SRB_STATUS_SUCCESS;
				Srb->ScsiStatus = SCSISTAT_GOOD;
				Srb->DataTransferLength = 0;
				break;
			}
			else {
				switch (pCdb->CDB6GENERIC.OperationCode) {
					case SCSIOP_READ:
					case SCSIOP_READ6:
					case SCSIOP_READ12:
					case SCSIOP_READ16:
					case SCSIOP_WRITE:
					case SCSIOP_WRITE6:
					case SCSIOP_WRITE12:
					case SCSIOP_WRITE16:
					{
						SrbStatus = OnScsiReadWrite(pDeviceExtension, pCdb, Srb, pBuf);
						break;
					}
				}
			}
			break;
		}
		default: {
			SrbStatus = SRB_STATUS_BAD_FUNCTION;
			Srb->DataTransferLength = 0;
			Srb->ScsiStatus = SCSISTAT_CHECK_CONDITION;

			break;
		}
	}

	exit:
	Srb->SrbStatus = SrbStatus;
	StorPortNotification(RequestComplete, DeviceExtension, Srb);
	return TRUE;
}