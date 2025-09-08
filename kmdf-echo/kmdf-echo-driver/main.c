/*
*	kmdf-echo-driver - main.c
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

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "kmdf-echo-drvier.DriverEntry(): Success\n");
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
	result = WdfDeviceInitAssignName(DeviceInit, &driverNtName);
	result = WdfDeviceCreate(&DeviceInit, &fdoAttributes, &hDevice);
	if (!NT_SUCCESS(result)) return result;

	fdoData = SIMPLEFdoGetData(hDevice);

	WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchParallel);
	queueConfig.EvtIoDeviceControl = OnIoDeviceControl;

	result = WdfIoQueueCreate(hDevice, &queueConfig, WDF_NO_OBJECT_ATTRIBUTES, &queue);
	result = WdfDeviceCreateDeviceInterface(hDevice, (LPGUID)&GUID_DRIVER, NULL);
	result = WdfDeviceCreateSymbolicLink(hDevice, &driverDosName);

	if (NT_SUCCESS(result)) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "kmdf-echo-drvier.OnDeviceAdd(): Success\n");
	}
	else {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "kmdf-echo-driver.OnDeviceAdd(): Error(NTSTATUS=%.8x)\n", result);
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
	PVOID inBuf;
	PVOID outBuf;
	size_t inLen;
	size_t outLen;
	UNICODE_STRING string;
	UNREFERENCED_PARAMETER(Queue);
	PAGED_CODE();

	if (IoControlCode != IOCTL_ECHO_CONTROL) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "kmdf-echo-driver.OnIoDeviceControl(): Unknown IOCTL Code\n");
		result = STATUS_INVALID_DEVICE_REQUEST;
		goto exit;
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "kmdf-echo-driver.OnIoDeviceControl(): OutputBufferLength=%lld, InputBufferLength=%lld\n", (ULONGLONG)OutputBufferLength, (ULONGLONG)InputBufferLength);
	if (InputBufferLength == 0) goto exit;

	result = WdfRequestRetrieveInputBuffer(Request, InputBufferLength, &inBuf, &inLen);
	result = WdfRequestRetrieveOutputBuffer(Request, sizeof(int), &outBuf, &outLen);
	if (!NT_SUCCESS(result)) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "kmdf-echo-driver.OnIoDeviceControl(): WdfRequestRetrieveBuffer() Failed (NTSTATUS=%.8x)\n", result);
		goto exit;
	}

	RtlInitUnicodeString(&string, inBuf);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "kmdf-echo-driver.OnIoDeviceControl(): Received=%wZ\n", &string);
	WriteOnLogFile(&string);
	
	outLen = sizeof(int);
	*(int*)outBuf = string.Length;
	exit:
	WdfRequestCompleteWithInformation(Request, result, sizeof(int));
}

VOID WriteOnLogFile(PUNICODE_STRING String) {
	NTSTATUS result = STATUS_SUCCESS;
	HANDLE hFile = NULL;
	OBJECT_ATTRIBUTES attr;
	IO_STATUS_BLOCK status;
	UNICODE_STRING fileName;

	RtlInitUnicodeString(&fileName, L"\\??\\C:\\kmdf-echo-driver.logs");
	InitializeObjectAttributes(&attr, &fileName, OBJ_CASE_INSENSITIVE, NULL, NULL);

	result = ZwCreateFile(&hFile, FILE_APPEND_DATA, &attr, &status, NULL, FILE_ATTRIBUTE_NORMAL, 0, FILE_OPEN_IF, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);

	if (status.Information == FILE_CREATED) {
		static const WCHAR bom = 0xFEFF; // UTF-16LE BOM
		result = ZwWriteFile(hFile, NULL, NULL, NULL, &status,
			(PVOID)&bom, sizeof(bom), NULL, NULL);
		if (!NT_SUCCESS(result)) goto exit;

	}

	result = ZwWriteFile(hFile, NULL, NULL, NULL, &status, (PVOID)String->Buffer, String->Length, NULL, NULL);
	
	static const WCHAR crlf[] = L"\r\n";
	result = ZwWriteFile(hFile, NULL, NULL, NULL, &status,
		(PVOID)crlf, sizeof(crlf) - sizeof(WCHAR), NULL, NULL);

	exit:
	if (!NT_SUCCESS(result)) 
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "kmdf-echo-driver.WriteOnLogFile(): NTSTATUS=%.8x\n", result);
	else
		ZwClose(hFile);
	
}