/*
*	kmdf-echo-app - main.c
*	간단한 Echo를 구현하는 KMDF 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "echo-common.h"
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <winioctl.h>

#define QUIT_COMMAND L"quit"
#define SEND_BUF_SIZE 100

int main()
{
	HANDLE hDevice;

	hDevice = CreateFile(L"\\\\.\\kmdf-echo-driver", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hDevice == (HANDLE)-1) {
		printf("Win32 CreateFile failed.");
		return 0;
	};

	WCHAR buf[SEND_BUF_SIZE] = L"";
	int recv;
	DWORD ret;

	printf("Connected. type ");
	printf("%ls", QUIT_COMMAND);
	printf(" to exit.\n\n");

	while (TRUE) {
		printf("> ");
		wscanf_s(L"%ls", buf, SEND_BUF_SIZE);

		if (lstrcmp(buf, QUIT_COMMAND) == 0) {
			break;
		}

		if (DeviceIoControl(hDevice, IOCTL_ECHO_CONTROL, buf, SEND_BUF_SIZE, &recv, sizeof(int), &ret, NULL)) {
			printf("OK. (Recevied %d bytes)\n", recv);
		}
		else {
			printf("Error!\n");
		}
	}

	printf("Exit.");
	CloseHandle(hDevice);
	return 0;
}
