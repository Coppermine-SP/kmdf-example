/*
*	remote-keyboard-drvier - OnDgramReceive.c
*	원격 가상 키보드 장치를 구현하는 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "remote-keyboard-driver.h"
#include "dbglog.h"

NTSTATUS WSKAPI OnDgramReceive(
    IN PVOID SocketContext,
    IN ULONG Flags,
    IN PWSK_DATAGRAM_INDICATION Indication
) {
    UNREFERENCED_PARAMETER(SocketContext);
    UNREFERENCED_PARAMETER(Flags);
    UNREFERENCED_PARAMETER(Indication);

	DBGPRINT_INFO("OnDgramReceive called");
	return STATUS_SUCCESS;
}