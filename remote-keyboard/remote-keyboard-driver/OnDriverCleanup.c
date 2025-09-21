/*
*	remote-keyboard-driver - CreateVirtualKeyboardDevice.c
*	원격 가상 키보드 장치를 구현하는 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "remote-keyboard-driver.h"

VOID OnDriverCleanup(
	IN WDFOBJECT DriverObject
) {
	UNREFERENCED_PARAMETER(DriverObject);
}