/*
*	storport-ramdisk - OnInquiryVpdRequest.c
*	램 디스크 가상 장치를 구현하는 Storport Virutal Miniport Driver 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "dbglog.h"
#include "storport-ramdisk.h"

/*
* 이 함수는 SCSI INQUIRY 명령어에서 Vital Product Data (VPD) 페이지 요청을 처리합니다.
* 이것은 스토리지 장치의 특정 정보를 요청하는 데 사용됩니다.
*/
UCHAR OnInquiryVpdRequest(
	IN PHW_DEVICE_EXTENSION DeviceExtension,
	IN PSCSI_REQUEST_BLOCK Srb
) {
	PVOID buf = NULL;
	ULONG alloc = Srb->DataTransferLength;
	ULONG written = 0;
	PCDB cdb = (PCDB)Srb->Cdb;
	UCHAR page = cdb->CDB6INQUIRY3.PageCode;

	DBGPRINT_INFO("PageCode=0x%02x", page);
	if(StorPortGetSystemAddress(DeviceExtension, Srb, &buf) != STOR_STATUS_SUCCESS) {
		DBGPRINT_ERROR("StorPortGetSystemAddress() failed");
		return SRB_STATUS_INTERNAL_ERROR;
	}

	switch (page) {
	case 0x00:	// Supported VPD Pages
	{
		UCHAR data[] = {
			0x00, 0x00, 0x00, 0x03, // Peripheral Qualifier & Device Type, Page Code, Page Length
			0x00, 0x80, 0xB1		// Supported VPD Pages
		};
		written = sizeof(data);
		if (alloc < written)
			written = alloc;
		RtlCopyMemory(buf, data, written);
		break;
	}
	case 0xB1: // Block Device Characteristics
	{
		UCHAR data[64] = {
			0x00, 0xB1, 0x00, 0x3C,
			0x00, 0x01, 0x00,
		};
		written = sizeof(data);
		if (alloc < written)
			written = alloc;
		RtlCopyMemory(buf, data, written);
		break;
	}
	case 0x80: { // Unit Serial Number
		const char* serial = "CINT-VDISK-0001";
		USHORT slen = (USHORT)strlen(serial);
		UCHAR header[4] = { 0x00, 0x80, (UCHAR)(slen >> 8), (UCHAR)(slen & 0xFF) };

		if (alloc >= 4) {
			RtlCopyMemory(buf, header, 4);
			ULONG tocpy = min(alloc - 4, slen);
			RtlCopyMemory((UCHAR*)buf + 4, serial, tocpy);
			written = 4 + tocpy;
		}
		else {
			written = alloc;
			RtlCopyMemory(buf, header, written);
		}

		break;
	}
	default: 
	{
		PSENSE_DATA sense = (PSENSE_DATA)Srb->SenseInfoBuffer;
		UCHAR len = (UCHAR)Srb->SenseInfoBufferLength;

		Srb->ScsiStatus = SCSISTAT_CHECK_CONDITION;
		DBGPRINT_ERROR("Unsupported PageCode");
		if (sense && len >= sizeof(SENSE_DATA)) {
			RtlZeroMemory(sense, len);
			sense->ErrorCode = 0x70; // Current errors
			sense->SenseKey = SCSI_SENSE_ILLEGAL_REQUEST;
			sense->AdditionalSenseLength = 0x0A;
			sense->AdditionalSenseCode = 0x24; // Invalid field in CDB

			return SRB_STATUS_AUTOSENSE_VALID | SRB_STATUS_ERROR;
		}
		else {
			return SRB_STATUS_ERROR;
		}

	}
	}

	Srb->DataTransferLength = written;
	Srb->ScsiStatus = SCSISTAT_GOOD;
	return SRB_STATUS_SUCCESS;
}