/*
*	storport-ramdisk - OnHw_PassiveInitialize.c
*	�� ��ũ ���� ��ġ�� �����ϴ� Storport Virutal Miniport Driver �����Դϴ�.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "dbglog.h"
#include "storport-ramdisk.h"

BOOLEAN OnHwPassiveInitialize(
	IN PVOID DeviceExtension
) {
	PVOID pBuffer;
	PHW_DEVICE_EXTENSION pHwDeviceExtension = (PHW_DEVICE_EXTENSION)DeviceExtension;

	PAGED_CODE();
	ULONG status = StorPortAllocatePool(DeviceExtension, DISK_SIZE_BYTES, 'KSDR', &pBuffer);
	if (status != STOR_STATUS_SUCCESS) {
		DBGPRINT_ERROR("StorPortAllocatePool() failed with STOR_STATUS=0x%8x", status);
		return FALSE;
	}
	pHwDeviceExtension->Base = (PUCHAR)pBuffer;

	DBGPRINT_INFO("Allocated %llu bytes for RAM Disk at %p", DISK_SIZE_BYTES, pHwDeviceExtension->Base);
	pHwDeviceExtension->TotalBlocks = DISK_TOTAL_BLOCKS;
	return TRUE;
}