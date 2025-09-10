/*
*	storport-ramdisk - OnHwStartIo.c
*	램 디스크 가상 장치를 구현하는 Storport Virutal Miniport Driver 예제입니다.
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