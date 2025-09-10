/*
*	storport-ramdisk - dbglog.h
*	램 디스크 가상 장치를 구현하는 Storport Virutal Miniport Driver 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/
#pragma once

#include <ntddk.h>
#include <ntstrsafe.h>

#define LOG_PROJECT_NAME "storport-ramdisk"

EXTERN_C
VOID
DbgLogV(
    _In_ ULONG Level,
    _In_z_ PCSTR Func,
    _In_ int Line,
    _In_z_ _Printf_format_string_ PCSTR Fmt,
    _In_ va_list Args
);

EXTERN_C
VOID
DbgLog(
    _In_ ULONG Level,
    _In_z_ PCSTR Func,
    _In_ int Line,
    _In_z_ _Printf_format_string_ PCSTR Fmt,
    ...
);

#if DBG
#define DBGPRINT_INFO(...)   DbgLog(DPFLTR_INFO_LEVEL,    __FUNCTION__, __LINE__, __VA_ARGS__)
#define DBGPRINT_WARN(...)   DbgLog(DPFLTR_WARNING_LEVEL, __FUNCTION__, __LINE__, __VA_ARGS__)
#define DBGPRINT_ERROR(...)  DbgLog(DPFLTR_ERROR_LEVEL,   __FUNCTION__, __LINE__, __VA_ARGS__)
#define DBGPRINT_TRACE(...)  DbgLog(DPFLTR_TRACE_LEVEL,   __FUNCTION__, __LINE__, __VA_ARGS__)
#else
#define DBGPRINT_INFO(...)   ((void)0)
#define DBGPRINT_WARN(...)   ((void)0)
#define DBGPRINT_ERROR(...)  ((void)0)
#define DBGPRINT_TRACE(...)  ((void)0)
#endif
