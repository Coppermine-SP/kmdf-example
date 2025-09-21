/*
*	remote-keyboard-driver - SubmitKeyboardReport.c
*	원격 가상 키보드 장치를 구현하는 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "remote-keyboard-driver.h"
#include "dbglog.h"

VOID SubmitKeyboardReport(
	IN PDEVICE_CONTEXT DeviceContext,
	IN PUCHAR Report
) {
	if (DeviceContext->Vhf == NULL) return;

	HID_XFER_PACKET packet = { 0, };
	UCHAR local[8];

	RtlCopyMemory(local, Report, 8);
	packet.reportBuffer = local;
	packet.reportBufferLen = 8;
	packet.reportId = 0; //Boot keyboard has no report Id.

	DBGPRINT_INFO("[%02X %02X %02X %02X %02X %02X %02X %02X]",
		local[0], local[1], local[2], local[3], local[4], local[5], local[6], local[7]);
	(void)VhfReadReportSubmit(DeviceContext->Vhf, &packet);
}