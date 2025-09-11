/*
*	storport-ramdisk - OnHw_PassiveInitialize.c
*	램 디스크 가상 장치를 구현하는 Storport Virutal Miniport Driver 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "dbglog.h"
#include "storport-ramdisk.h"

BOOLEAN OnHwPassiveInitialize(
	IN PVOID DeviceExtension
) {
	if (DeviceExtension == NULL) {
		DBGPRINT_ERROR("Invalid DeviceExtension pointer");
		return FALSE;
	}

	PVOID pBuffer = NULL;
	PHW_DEVICE_EXTENSION pHwDeviceExtension = (PHW_DEVICE_EXTENSION)DeviceExtension;

	PAGED_CODE();
	
	pHwDeviceExtension->Base = NULL;
	pHwDeviceExtension->TotalBlocks = 0;
	pHwDeviceExtension->DiskSize = DISK_SIZE_BYTES;
	
	ULONG status = StorPortAllocatePool(DeviceExtension, DISK_SIZE_BYTES, 'KSDR', &pBuffer);
	if (status != STOR_STATUS_SUCCESS) {
		DBGPRINT_ERROR("StorPortAllocatePool() failed with STOR_STATUS=0x%8x", status);
		return FALSE;
	}

	pHwDeviceExtension->Base = (PUCHAR)pBuffer;
	pHwDeviceExtension->TotalBlocks = DISK_TOTAL_BLOCKS;

	DBGPRINT_INFO("Allocated %llu bytes for RAM Disk at %p", DISK_SIZE_BYTES, pHwDeviceExtension->Base);
	return TRUE;
}