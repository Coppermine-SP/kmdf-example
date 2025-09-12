/*
*	storport-ramdisk - OnScsiRead.c
*	램 디스크 가상 장치를 구현하는 Storport Virutal Miniport Driver 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "dbglog.h"
#include "storport-ramdisk.h"

VOID SetSenseLbaOutOfRange(
	IN PSCSI_REQUEST_BLOCK Srb
) 
{
	Srb->SrbStatus = SRB_STATUS_ERROR;
	Srb->ScsiStatus = SCSISTAT_CHECK_CONDITION;
	if (Srb->SenseInfoBuffer && Srb->SenseInfoBufferLength >= 14) {
		UCHAR* sd = (UCHAR*)Srb->SenseInfoBuffer;
		RtlZeroMemory(sd, Srb->SenseInfoBufferLength);
		sd[0] = 0x70;       // fixed format
		sd[2] = 0x05;       // ILLEGAL REQUEST
		sd[7] = 0x0a;       // addl length
		sd[12] = 0x21;      // ASC: LOGICAL BLOCK ADDRESS OUT OF RANGE
		sd[13] = 0x00;      // ASCQ
		Srb->SrbStatus |= SRB_STATUS_AUTOSENSE_VALID;
	}
}

UCHAR OnScsiReadWrite(
	IN PHW_DEVICE_EXTENSION DeviceExtension,
	IN PCDB Cdb,
	IN PSCSI_REQUEST_BLOCK Srb,
	IN PVOID SystemBuffer
) 
{
	BOOLEAN isWrite = FALSE;
	ULONGLONG lba = 0;
	ULONGLONG blocks = 0;

	if (SystemBuffer == NULL)
		return SRB_STATUS_INVALID_PARAMETER;

	switch (Cdb->CDB6GENERIC.OperationCode) {

		case SCSIOP_WRITE:
			isWrite = TRUE;
		case SCSIOP_READ:
		{
			lba = ((ULONGLONG)Cdb->CDB10.LogicalBlockByte0 << 24) |
				  ((ULONGLONG)Cdb->CDB10.LogicalBlockByte1 << 16) |
				  ((ULONGLONG)Cdb->CDB10.LogicalBlockByte2 << 8) |
				  ((ULONGLONG)Cdb->CDB10.LogicalBlockByte3);
			blocks = ((ULONG)Cdb->CDB10.TransferBlocksMsb << 8) | Cdb->CDB10.TransferBlocksLsb;
			break;
		}
		case SCSIOP_WRITE6:
			isWrite = TRUE;
		case SCSIOP_READ6:
		{
			lba = ((ULONGLONG)Cdb->CDB6READWRITE.LogicalBlockMsb1 << 16) |
				  ((ULONGLONG)Cdb->CDB6READWRITE.LogicalBlockMsb0 << 8) |
				  ((ULONGLONG)Cdb->CDB6READWRITE.LogicalBlockLsb);
			blocks = Cdb->CDB6READWRITE.TransferBlocks ? Cdb->CDB6READWRITE.TransferBlocks : 256;
			break;
		}
		case SCSIOP_WRITE12:
			isWrite = TRUE;
		case SCSIOP_READ12:
		{
			lba = ((ULONGLONG)Cdb->CDB12.LogicalBlock[0] << 24) |
				  ((ULONGLONG)Cdb->CDB12.LogicalBlock[1] << 16) |
				  ((ULONGLONG)Cdb->CDB12.LogicalBlock[2] << 8) |
				  ((ULONGLONG)Cdb->CDB12.LogicalBlock[3]);
			blocks = ((ULONGLONG)Cdb->CDB12.TransferLength[0] << 24) |
				     ((ULONGLONG)Cdb->CDB12.TransferLength[1] << 16) |
			      	 ((ULONGLONG)Cdb->CDB12.TransferLength[2] << 8) |
					 ((ULONGLONG)Cdb->CDB12.TransferLength[3]);
			break;
		}
		case SCSIOP_WRITE16:
			isWrite = TRUE;
		case SCSIOP_READ16:
		{
			lba = ((ULONGLONG)Cdb->CDB16.LogicalBlock[0] << 56) |
			      ((ULONGLONG)Cdb->CDB16.LogicalBlock[1] << 48) |
				  ((ULONGLONG)Cdb->CDB16.LogicalBlock[2] << 40) |
				  ((ULONGLONG)Cdb->CDB16.LogicalBlock[3] << 32) |
				  ((ULONGLONG)Cdb->CDB16.LogicalBlock[4] << 24) |
				  ((ULONGLONG)Cdb->CDB16.LogicalBlock[5] << 16) |
				  ((ULONGLONG)Cdb->CDB16.LogicalBlock[6] << 8) |
				  ((ULONGLONG)Cdb->CDB16.LogicalBlock[7]);
			blocks = ((ULONGLONG)Cdb->CDB16.TransferLength[0] << 24) |
					 ((ULONGLONG)Cdb->CDB16.TransferLength[1] << 16) |
					 ((ULONGLONG)Cdb->CDB16.TransferLength[2] << 8) |
					 ((ULONGLONG)Cdb->CDB16.TransferLength[3]);
			break;
		}
		default:
			return SRB_STATUS_INVALID_REQUEST;

	}

	if (blocks == 0) {
		Srb->DataTransferLength = 0;
		Srb->ScsiStatus = SCSISTAT_GOOD;
		return SRB_STATUS_SUCCESS;
	}

	const ULONGLONG offset = lba * SIZE_LOGICAL_BLOCK;
	const ULONGLONG size = blocks * SIZE_LOGICAL_BLOCK;

	if (lba >= DISK_TOTAL_BLOCKS || (blocks > 0 && (lba + blocks > DISK_TOTAL_BLOCKS))) {
		DBGPRINT_ERROR("LBA out of range: LBA=%llu, Blocks=%llu", lba, blocks);
		SetSenseLbaOutOfRange(Srb);
		return SRB_STATUS_ERROR;
	}

	if (offset > DISK_SIZE_BYTES || size > DISK_SIZE_BYTES || offset + size > DISK_SIZE_BYTES) {
		DBGPRINT_ERROR("Offset/Size out of range: Offset=%llu, Size=%llu", offset, size);
		SetSenseLbaOutOfRange(Srb);
		return SRB_STATUS_ERROR;
	}

	if (isWrite) {
		StorPortMoveMemory(DeviceExtension->Base + offset, SystemBuffer, (ULONG)size);
	}
	else {
		StorPortMoveMemory(SystemBuffer, DeviceExtension->Base + offset,(ULONG)size);
	}

	DBGPRINT_INFO("%s LBA=%llu, Blocks=%llu, Offset=%llu, Size=%llu", isWrite ? "WRITE" : "READ", lba, blocks, offset, size);
	Srb->DataTransferLength = (ULONG)size;
	Srb->ScsiStatus = SCSISTAT_GOOD;
	return SRB_STATUS_SUCCESS;
}