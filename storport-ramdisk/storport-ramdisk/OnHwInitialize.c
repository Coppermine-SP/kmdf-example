/*
*	storport-ramdisk - OnHwInitalize.c
*	램 디스크 가상 장치를 구현하는 Storport Virutal Miniport Driver 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "dbglog.h"
#include "storport-ramdisk.h"

BOOLEAN OnHwInitialize(
	PVOID DeviceExtension
) {
	if( DeviceExtension == NULL )
		return FALSE;

	(void)StorPortEnablePassiveInitialization(DeviceExtension, OnHwPassiveInitialize);
	return TRUE;
}