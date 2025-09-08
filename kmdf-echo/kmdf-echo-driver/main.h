#pragma once

#include <ntddk.h>
#include <wdf.h>
#include <initguid.h>
#include <ntstrsafe.h>
#include "echo-common.h"

DEFINE_GUID(GUID_DRIVER, 0x56f5dec1, 0xab0b, 0x11d1, 0xb9, 0x84, 0x0, 0x60, 0xaf, 0x17, 0x11, 0x03);

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD OnDeviceAdd;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL OnIoDeviceControl;
VOID WriteOnLogFile(PUNICODE_STRING String);

typedef struct _FDO_DATA {
	char Reserved;
} FDO_DATA, * PFDO_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(FDO_DATA, SIMPLEFdoGetData)

DECLARE_CONST_UNICODE_STRING(driverNtName, L"\\Device\\kmdf-echo-driver");
DECLARE_CONST_UNICODE_STRING(driverDosName, L"\\DosDevices\\kmdf-echo-driver");