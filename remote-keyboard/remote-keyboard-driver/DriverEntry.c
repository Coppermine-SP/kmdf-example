/*
*	remote-keyboard-driver - DriverEntry.c
*	���� ���� Ű���� ��ġ�� �����ϴ� �����Դϴ�.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "remote-keyboard-driver.h"
#include "dbglog.h"

NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath
) {
	NTSTATUS status = STATUS_SUCCESS;
	WDF_DRIVER_CONFIG config;
	WDF_OBJECT_ATTRIBUTES attr;

	//WDF Driver Init
	WDF_DRIVER_CONFIG_INIT(&config, OnDeviceAdd);
	WDF_OBJECT_ATTRIBUTES_INIT(&attr);
	attr.EvtCleanupCallback = OnDriverCleanup;

	status = WdfDriverCreate(DriverObject, RegistryPath, &attr, &config, WDF_NO_HANDLE);
	return status;
}