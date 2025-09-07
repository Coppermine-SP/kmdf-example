/*
*	kmdf-echo-driver
*	간단한 Echo를 구현하는 KMDF 예제입니다.
* 
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "main.h"

NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath
)
{
	NTSTATUS result = STATUS_SUCCESS;
	WDF_DRIVER_CONFIG config;

	WDF_DRIVER_CONFIG_INIT(&config, OnDeviceAdd);
	result = WdfDriverCreate(
		DriverObject,
		RegistryPath,
		WDF_NO_OBJECT_ATTRIBUTES,
		&config,
		WDF_NO_HANDLE
	);

	if (!NT_SUCCESS(result)) {
		KeBugCheck(IRQL_NOT_LESS_OR_EQUAL);
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "kmdf-echo-drvier.DriverEntry(): Success");
	return result;
}

NTSTATUS OnDeviceAdd(
	IN WDFDRIVER Driver,
	IN PWDFDEVICE_INIT DeviceInit
) 
{
	NTSTATUS result = STATUS_SUCCESS;
	WDF_IO_QUEUE_CONFIG queueConfig;
	WDF_OBJECT_ATTRIBUTES fdoAttributes;
	WDFDEVICE hDevice;
	WDFQUEUE queue;
	PFDO_DATA fdoData;

	UNREFERENCED_PARAMETER(Driver);
	PAGED_CODE();

	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&fdoAttributes, FDO_DATA);
	result = WdfDeviceCreate(&DeviceInit, &fdoAttributes, &hDevice);
	if (!NT_SUCCESS(result)) return result;

	fdoData = SIMPLEFdoGetData(hDevice);

	WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchParallel);
	queueConfig.EvtIoDeviceControl = OnIoDeviceControl;

	result = WdfIoQueueCreate(hDevice, &queueConfig, WDF_NO_OBJECT_ATTRIBUTES, &queue);
	result = WdfDeviceCreateDeviceInterface(hDevice, (LPGUID)&GUID_DRIVER, NULL);

	if (NT_SUCCESS(result)) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "kmdf-echo-drvier.OnDeviceAdd(): Success");
	}
	else {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "kmdf-echo-driver.OnDeviceAdd(): Error(NTSTATUS: %.8x)", result);
	}

	return result;
}

VOID OnIoDeviceControl(
	IN WDFQUEUE Queue,
	IN WDFREQUEST Request,
	IN size_t OutputBufferLength,
	IN size_t InputBufferLength,
	IN ULONG IoControlCode
)
{
	NTSTATUS result = STATUS_SUCCESS; 
	UNREFERENCED_PARAMETER(Queue);
	PAGED_CODE();

	if (IoControlCode != IOCTL_ECHO_CONTROL) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "kmdf-echo-driver.OnIoDeviceControl(): Unknown IOCTL Code");
		result = STATUS_INVALID_DEVICE_REQUEST;
		goto exit;
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "kmdf-echo-driver.OnIoDeviceControl(): OutputBufferLength=%lld, InputBufferLength=%lld", (ULONGLONG)OutputBufferLength, (ULONGLONG)InputBufferLength);

	exit:
	WdfRequestCompleteWithInformation(Request, result, (ULONG_PTR)0);
}