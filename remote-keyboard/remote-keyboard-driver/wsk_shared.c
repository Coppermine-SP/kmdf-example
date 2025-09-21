/*
*	remote-keyboard-drvier - wsk_shared.c
*	원격 가상 키보드 장치를 구현하는 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "remote-keyboard-driver.h"

WSK_REGISTRATION g_WskReg = { 0, };
WSK_PROVIDER_NPI g_WskProvider = { 0, };