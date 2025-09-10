/*
*	storport-ramdisk - OnHwResetBus.c
*	램 디스크 가상 장치를 구현하는 Storport Virutal Miniport Driver 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "storport-ramdisk.h"

BOOLEAN OnHwResetBus(
	IN PVOID DeviceExtension,
	IN ULONG PathId
) {
	UNREFERENCED_PARAMETER(PathId);

	if(DeviceExtension == NULL)
		return FALSE;

	return TRUE;
}
