/*
*	storport-ramdisk - OnHwStartIo.c
*	�� ��ũ ���� ��ġ�� �����ϴ� Storport Virutal Miniport Driver �����Դϴ�.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "storport-ramdisk.h"

BOOLEAN OnHwStartIo(
	IN PVOID DeviceExtension,
	IN PSCSI_REQUEST_BLOCK Srb
) {
	//TODO: Implement the data transfer logic here.
	UNREFERENCED_PARAMETER(DeviceExtension);
	UNREFERENCED_PARAMETER(Srb);
	return FALSE;
}