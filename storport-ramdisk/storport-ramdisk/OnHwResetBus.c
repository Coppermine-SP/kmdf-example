/*
*	storport-ramdisk - OnHwResetBus.c
*	�� ��ũ ���� ��ġ�� �����ϴ� Storport Virutal Miniport Driver �����Դϴ�.
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
