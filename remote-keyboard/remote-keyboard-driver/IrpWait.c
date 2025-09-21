/*
*	remote-keyboard-drvier - IrpWait.c
*	���� ���� Ű���� ��ġ�� �����ϴ� �����Դϴ�.
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