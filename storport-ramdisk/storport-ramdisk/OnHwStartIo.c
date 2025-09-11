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
			else if (pCdb->CDB6GENERIC.OperationCode == SCSIOP_TEST_UNIT_READY) {
				DBGPRINT_INFO("SCSIOP_TEST_UNIT_READY");
				Srb->SrbStatus = SRB_STATUS_SUCCESS;
				Srb->ScsiStatus = SCSISTAT_GOOD;
				Srb->DataTransferLength = 0;
				break;
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