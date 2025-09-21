/*
*	remote-keyboard-driver - remote-keyboard-driver.h
*	원격 가상 키보드 장치를 구현하는 예제입니다.
* 
*	Copyright (C) 2025-2026 Coppermine-SP 
*/

#pragma once

#pragma region Includes
#include <ntddk.h>
#include <wdf.h>
#include <ws2def.h>
#include <ws2ipdef.h>
#include <vhf.h>
#include "wsk.h"
#pragma endregion

#pragma region Defines
typedef struct _DEVICE_CONTEXT {
	VHFHANDLE Vhf;
	PWSK_SOCKET Socket;
	WSK_CLIENT_NPI WskClient;
	WSK_REGISTRATION WskReg;
	WSK_PROVIDER_NPI WskProvider;
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, GetDeviceCtx)

typedef struct _IRP_SYNC_CONTEXT {
    KEVENT Event;
    PDEVICE_CONTEXT Context;
	IO_STATUS_BLOCK IoStatus;
    BOOLEAN IsCreating;
} IRP_SYNC_CONTEXT;

#pragma endregion

#pragma region Functions
IO_COMPLETION_ROUTINE OnSocketIoComplete;

EVT_WDF_DRIVER_DEVICE_ADD OnDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP OnDriverCleanup;

NTSTATUS CreateVirtualKeyboardDevice(
	IN WDFDEVICE Device,
	OUT PDEVICE_CONTEXT DeviceContext
);

NTSTATUS CreateUdpListener(
	IN PDEVICE_CONTEXT DeviceContext
);

NTSTATUS WSKAPI OnDgramReceive(
    IN PVOID SocketContext,
    IN ULONG Flags,
    IN PWSK_DATAGRAM_INDICATION Indication
);

VOID SubmitKeyboardReport(
    IN PDEVICE_CONTEXT DeviceContext,
    IN PUCHAR Report
);
#pragma endregion

#pragma region Global Variables
static const WSK_CLIENT_DATAGRAM_DISPATCH g_WskDgramDispatch = {
    OnDgramReceive
};
static const WSK_CLIENT_DISPATCH g_WskClientDispatch = {
    MAKE_WSK_VERSION(1, 0),
    0,
    NULL
};
static const UCHAR g_KeyboardReportDesc[] = {
        0x05,0x01,   // Usage Page (Generic Desktop)
        0x09,0x06,   // Usage (Keyboard)
        0xA1,0x01,   // Collection (Application)
        0x05,0x07,   //   Usage Page (Key Codes)
        0x19,0xE0,   //   Usage Minimum (224) - Left Control
        0x29,0xE7,   //   Usage Maximum (231) - Right GUI
        0x15,0x00,   //   Logical Minimum (0)
        0x25,0x01,   //   Logical Maximum (1)
        0x75,0x01,   //   Report Size (1)
        0x95,0x08,   //   Report Count (8) -> Modifiers
        0x81,0x02,   //   Input (Data,Var,Abs)

        0x95,0x01,   //   Report Count (1)
        0x75,0x08,   //   Report Size (8)
        0x81,0x01,   //   Input (Const,Array,Abs) -> Reserved

        0x95,0x06,   //   Report Count (6) -> 6 keycodes
        0x75,0x08,   //   Report Size (8)
        0x15,0x00,   //   Logical Min (0)
        0x25,0x65,   //   Logical Max (101)
        0x05,0x07,   //   Usage Page (Key Codes)
        0x19,0x00,   //   Usage Min (0)
        0x29,0x65,   //   Usage Max (101)
        0x81,0x00,   //   Input (Data,Array)
        0xC0         // End Collection
};
#pragma endregion

