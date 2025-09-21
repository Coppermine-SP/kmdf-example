/*
*	remote-keyboard-driver - OnDgramReceive.c
*	원격 가상 키보드 장치를 구현하는 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "remote-keyboard-driver.h"

NTSTATUS WSKAPI OnDgramReceive(
    IN PVOID SocketContext,
    IN ULONG Flags,
    IN PWSK_DATAGRAM_INDICATION Indication
) {
    UNREFERENCED_PARAMETER(Flags);

	PDEVICE_CONTEXT ctx = (PDEVICE_CONTEXT)SocketContext;

    while (Indication) {
        const WSK_BUF* buf = &Indication->Buffer;
        if (buf->Mdl && buf->Length >= 8) {
			PUCHAR base = (PUCHAR)MmGetSystemAddressForMdlSafe(buf->Mdl, NormalPagePriority);
            if (base) {
                UCHAR report[8] = { 0, };
                RtlCopyMemory(report, base + buf->Offset, 8);
				SubmitKeyboardReport(ctx, report);
            }
        }
		Indication = Indication->Next;
    }

    return STATUS_SUCCESS;
}