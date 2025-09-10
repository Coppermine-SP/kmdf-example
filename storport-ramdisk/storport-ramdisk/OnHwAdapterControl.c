/*
*	storport-ramdisk - OnHwAdapterControl.c
*	램 디스크 가상 장치를 구현하는 Storport Virutal Miniport Driver 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "storport-ramdisk.h"

SCSI_ADAPTER_CONTROL_STATUS OnHwAdapterControl(
	IN PVOID DeviceExtension,
	IN SCSI_ADAPTER_CONTROL_TYPE ControlType,
	IN PVOID Parameters
) {
	//TODO: Implement adapter control operations here.
	UNREFERENCED_PARAMETER(DeviceExtension);
	UNREFERENCED_PARAMETER(ControlType);
	UNREFERENCED_PARAMETER(Parameters);
	return FALSE;
}