/*
*	remote-keyboard-driver - OnSocketIoComplete.c
*	원격 가상 키보드 장치를 구현하는 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "remote-keyboard-driver.h"
#include "dbglog.h"

NTSTATUS OnSocketIoComplete(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp,
	IN PVOID Context
) {
	UNREFERENCED_PARAMETER(DeviceObject);
	
	if (Context == NULL) {
		DBGPRINT_ERROR("Context is NULL");
		return STATUS_INVALID_PARAMETER;
	}
	IRP_SYNC_CONTEXT* ctx = (IRP_SYNC_CONTEXT*)Context;


	if(Irp->IoStatus.Status == STATUS_SUCCESS) {
		DBGPRINT_INFO("Socket I/O Success.");

		if (ctx->IsCreating) 
			ctx->Context->Socket = (PWSK_SOCKET)Irp->IoStatus.Information;
	} 
	else {
		DBGPRINT_ERROR("failed: 0x%08X", Irp->IoStatus.Status);
	}


	KeSetEvent(&ctx->Event, IO_NO_INCREMENT, FALSE);
	ctx->IoStatus = Irp->IoStatus;
	return STATUS_MORE_PROCESSING_REQUIRED;
}