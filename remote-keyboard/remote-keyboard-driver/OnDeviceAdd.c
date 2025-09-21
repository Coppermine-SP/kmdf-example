/*
*	remote-keyboard-drvier - OnDeviceAdd.c
*	원격 가상 키보드 장치를 구현하는 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "remote-keyboard-driver.h"
#include "dbglog.h"
NTSTATUS OnDeviceAdd(
	IN WDFDRIVER Driver,
	IN PWDFDEVICE_INIT DeviceInit
) {
	UNREFERENCED_PARAMETER(Driver);
	NTSTATUS status = STATUS_SUCCESS;
	WDFDEVICE device;
	WDF_OBJECT_ATTRIBUTES attr;

	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attr, DEVICE_CONTEXT);
	status = WdfDeviceCreate(&DeviceInit, &attr, &device);
	if (!NT_SUCCESS(status))
		return status;

	PDEVICE_CONTEXT ctx = GetDeviceCtx(device);
	RtlZeroMemory(ctx, sizeof(*ctx));

	status = CreateVirtualKeyboardDevice(device, ctx);
	if (!NT_SUCCESS(status))
		return status;

	//Winsock Kernel Init
	ctx->WskClient.ClientContext = NULL;
	ctx->WskClient.Dispatch = &g_WskClientDispatch;
	status = WskRegister(&ctx->WskClient, &ctx->WskReg);

	if (!NT_SUCCESS(status)) {
		DBGPRINT_ERROR("WskRegister failed: 0x%08X", status);
		return status;
	}

	status = WskCaptureProviderNPI(&ctx->WskReg, WSK_INFINITE_WAIT, &ctx->WskProvider);
	if (!NT_SUCCESS(status))
	{
		DBGPRINT_ERROR("WskCaptureProviderNPI failed: 0x%08X", status);
		WskDeregister(&ctx->WskReg);
		return status;
	}

	status = CreateUdpListener(ctx);
	return status;
}