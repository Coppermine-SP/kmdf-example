/*
*	remote-keyboard-drvier - dbglog.c
*	���� ���� Ű���� ��ġ�� �����ϴ� �����Դϴ�.
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

    // Fmt�� '\n'���� ������ ������, �۾��� ���˿� ������ �ٿ� ���
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

    // Args�� ���⼭ �� 1ȸ�� �Һ��ϰ�, ���� ����/�������� ����
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
