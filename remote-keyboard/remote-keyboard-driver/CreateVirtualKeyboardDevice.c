/*
*	remote-keyboard-drvier - CreateVirtualKeyboardDevice.c
*	원격 가상 키보드 장치를 구현하는 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "remote-keyboard-driver.h"

NTSTATUS CreateVirtualKeyboardDevice(
	IN WDFDEVICE Device,
	OUT PDEVICE_CONTEXT DeviceContext
) {
	NTSTATUS status = STATUS_SUCCESS;
	VHF_CONFIG config;
	PDEVICE_OBJECT pdo = WdfDeviceWdmGetDeviceObject(Device);

	VHF_CONFIG_INIT(&config, pdo, sizeof(g_KeyboardReportDesc), (PUCHAR)g_KeyboardReportDesc);
	config.VendorID = 0x1C1F;
	config.ProductID = 0x0012;
	config.VersionNumber = 0x0001;

	status = VhfCreate(&config, &DeviceContext->Vhf);
	status = VhfStart(DeviceContext->Vhf);

	return status;
}