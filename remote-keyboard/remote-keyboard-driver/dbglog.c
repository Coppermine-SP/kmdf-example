/*
*	remote-keyboard-drvier - dbglog.c
*	원격 가상 키보드 장치를 구현하는 예제입니다.
* 
*   Copyright (C) 2025-2026 Coppermine-SP
*/

#include "dbglog.h"
#include <ntstrsafe.h>

_Use_decl_annotations_
VOID
DbgLogV(ULONG Level, PCSTR Func, int Line, PCSTR Fmt, va_list Args)
{
    CHAR prefix[128];
    CHAR fmtWork[512];
    PCSTR fmtToUse = Fmt;

    // "storport-ramdisk.Func():Line -> "
    RtlStringCbPrintfA(prefix, sizeof(prefix),
        LOG_PROJECT_NAME ".%s():%d -> ", Func, Line);

    // Fmt가 '\n'으로 끝나지 않으면, 작업용 포맷에 개행을 붙여 사용
    size_t fmtLenCch = 0;
    if (NT_SUCCESS(RtlStringCchLengthA(Fmt, NTSTRSAFE_MAX_CCH, &fmtLenCch))) {
        const BOOLEAN needNl = (fmtLenCch == 0 || Fmt[fmtLenCch - 1] != '\n');
        if (needNl && (fmtLenCch + 2) < RTL_NUMBER_OF(fmtWork)) {
            if (NT_SUCCESS(RtlStringCbCopyA(fmtWork, sizeof(fmtWork), Fmt)) &&
                NT_SUCCESS(RtlStringCbCatA(fmtWork, sizeof(fmtWork), "\n"))) {
                fmtToUse = fmtWork;
            }
        }
    }

    // Args는 여기서 단 1회만 소비하고, 이후 재사용/접근하지 않음
    vDbgPrintExWithPrefix(prefix, DPFLTR_IHVDRIVER_ID, Level, fmtToUse, Args);
}

_Use_decl_annotations_
VOID
DbgLog(ULONG Level, PCSTR Func, int Line, PCSTR Fmt, ...)
{
    va_list args;
    va_start(args, Fmt);
    DbgLogV(Level, Func, Line, Fmt, args);
    va_end(args);
}
