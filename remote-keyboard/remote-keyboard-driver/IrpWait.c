/*
*	remote-keyboard-drvier - IrpWait.c
*	원격 가상 키보드 장치를 구현하는 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "remote-keyboard-driver.h"

NTSTATUS IrpWait(
	IN PIRP Irp,
	IN NTSTATUS Status
) {
	if (Status == STATUS_PENDING) {
		IRP_SYNC_CONTEXT ctx;
		KeInitializeEvent(&ctx.Event, NotificationEvent, FALSE);
		IoSetCompletionRoutine(Irp, IrpComplete, &ctx, TRUE, TRUE, TRUE);
		KeWaitForSingleObject(&ctx.Event, Executive, KernelMode, FALSE, NULL);
		Status = Irp->IoStatus.Status;
	}

	return Status;
}