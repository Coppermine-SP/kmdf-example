/*
*	storport-ramdisk - OnHwFreeAdapterResource.c
*	램 디스크 가상 장치를 구현하는 Storport Virutal Miniport Driver 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "dbglog.h"
#include "storport-ramdisk.h"

VOID OnHwFreeAdapterResource(
	IN PVOID DeviceExtension
) {
	PHW_DEVICE_EXTENSION pHwDeviceExtension = NULL;
	if(DeviceExtension == NULL)
		return;

	pHwDeviceExtension = (PHW_DEVICE_EXTENSION)DeviceExtension;
	ULONG status = StorPortFreePool(DeviceExtension, pHwDeviceExtension->Base);

	if(status != STOR_STATUS_SUCCESS) {
		DBGPRINT_ERROR("StorPortFreePool() failed with STOR_STATUS=0x%8x", status);
	} else {
		DBGPRINT_INFO("Freed RAM Disk memory at %p", pHwDeviceExtension->Base);
		pHwDeviceExtension->Base = NULL;
	}
}