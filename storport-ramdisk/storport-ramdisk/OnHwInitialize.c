/*
*	storport-ramdisk - OnHwInitalize.c
*	�� ��ũ ���� ��ġ�� �����ϴ� Storport Virutal Miniport Driver �����Դϴ�.
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