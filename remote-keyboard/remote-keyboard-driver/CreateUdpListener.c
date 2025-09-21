/*
*	remote-keyboard-driver - CreateUdpListener.c
*	원격 가상 키보드 장치를 구현하는 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "remote-keyboard-driver.h"
#include "dbglog.h"

NTSTATUS CreateUdpListener(
	IN PDEVICE_CONTEXT DeviceContext
) {
	NTSTATUS status = STATUS_SUCCESS;
	PIRP irp;
	IRP_SYNC_CONTEXT sctx;

	sctx.Context = DeviceContext;
	sctx.IsCreating = TRUE;
	irp = IoAllocateIrp(1, FALSE);
	if (irp == NULL)
		return STATUS_INSUFFICIENT_RESOURCES;

	DBGPRINT_INFO("Creating UDP listening socket...\n");
	KeInitializeEvent(&sctx.Event, NotificationEvent, FALSE);
	IoSetCompletionRoutine(irp, OnSocketIoComplete, &sctx, TRUE, TRUE, TRUE);
	status = DeviceContext->WskProvider.Dispatch->WskSocket(
		DeviceContext->WskProvider.Client,
		AF_INET,
		SOCK_DGRAM,
		IPPROTO_UDP,
		WSK_FLAG_DATAGRAM_SOCKET,
		DeviceContext,
		&g_WskDgramDispatch,
		NULL, NULL, NULL,
		irp
	);

	if (status == STATUS_PENDING) {
		KeWaitForSingleObject(&sctx.Event, Executive, KernelMode, FALSE, NULL);
		status = sctx.IoStatus.Status;
	}

	IoFreeIrp(irp);
	if (DeviceContext->Socket == NULL) {
		return STATUS_INVALID_DEVICE_STATE;
	}

	//Binding
	SOCKADDR_IN local = { 0 };
	local.sin_family = AF_INET;
	local.sin_addr.S_un.S_addr = INADDR_ANY;
	local.sin_port = RtlUshortByteSwap(2381);

	PWSK_PROVIDER_DATAGRAM_DISPATCH dispatch = (PWSK_PROVIDER_DATAGRAM_DISPATCH)DeviceContext->Socket->Dispatch;
	irp = IoAllocateIrp(1, FALSE);
	if (irp == NULL)
		return STATUS_INSUFFICIENT_RESOURCES;

	DBGPRINT_INFO("Binding to port %d...", RtlUshortByteSwap(local.sin_port));
	sctx.IsCreating = FALSE;
	IoSetCompletionRoutine(irp, OnSocketIoComplete, &sctx, TRUE, TRUE, TRUE);
	KeInitializeEvent(&sctx.Event, NotificationEvent, FALSE);
	status = dispatch->WskBind(DeviceContext->Socket, (PSOCKADDR)&local, 0, irp);

	if (status == STATUS_PENDING){
		KeWaitForSingleObject(&sctx.Event, Executive, KernelMode, FALSE, NULL);
		status = sctx.IoStatus.Status;
	}

	IoFreeIrp(irp);
	if (!NT_SUCCESS(status)) {
		DBGPRINT_ERROR("WskBind failed: 0x%08X", status);
		return status;
	}

	//Enable Event callback
	WSK_EVENT_CALLBACK_CONTROL ecc = { 0 };
	ecc.NpiId = &NPI_WSK_INTERFACE_ID;
	ecc.EventMask = WSK_EVENT_RECEIVE_FROM;
	status = dispatch->WskControlSocket(DeviceContext->Socket, WskSetOption, SO_WSK_EVENT_CALLBACK, SOL_SOCKET, sizeof(ecc), &ecc, 0, NULL, NULL, NULL);
	if (!NT_SUCCESS(status)) {
		DBGPRINT_ERROR("WskControlSocket failed: 0x%08X", status);
		return status;
	}

	return status;
}