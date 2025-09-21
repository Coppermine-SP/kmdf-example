/*
*	remote-keyboard-drvier - IrpComplete.c
*	원격 가상 키보드 장치를 구현하는 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "remote-keyboard-driver.h"

NTSTATUS IrpComplete(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp,
	IN PVOID Context
) {
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Irp);

	IRP_SYNC_CONTEXT* ctx = (IRP_SYNC_CONTEXT*)Context;
	KeSetEvent(&ctx->Event, IO_NO_INCREMENT, FALSE);
	return STATUS_MORE_PROCESSING_REQUIRED;
}