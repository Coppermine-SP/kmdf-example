/*
*	storport-ramdisk - storport-ramdisk.h
*	램 디스크 가상 장치를 구현하는 Storport Virutal Miniport Driver 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/
#pragma once

#pragma region Includes
#include <ntddk.h>
#include <storport.h>
#pragma endregion

#pragma region Defines
#define MAX_DMA_BLOCK_SIZE 				(0x80000UL)
#define MAX_PHYSICAL_PAGES				(0x400UL)
#define MAX_DMA_TRANSFER_SIZE			(MAX_PHYSICAL_PAGES * PAGE_SIZE)	
#define SIZE_LOGICAL_BLOCK				512

#define DISK_SIZE_MB 2048
#define DISK_SIZE_BYTES ((ULONGLONG)1024*1024*DISK_SIZE_MB)
#define DISK_TOTAL_BLOCKS ((ULONG)2*1024*DISK_SIZE_MB)

#define DBG 1
#pragma endregion

#pragma region Structs
typedef struct _HW_DEVICE_EXTENSION
{
	PSCSI_REQUEST_BLOCK CurrentSrb;
	ULONGLONG DiskSize;
	ULONG TotalBlocks;
	PUCHAR Base;
	UCHAR DataBuff[512];
} HW_DEVICE_EXTENSION, * PHW_DEVICE_EXTENSION;
#pragma endregion


#pragma region Functions
HW_INITIALIZE OnHwInitialize;
HW_PASSIVE_INITIALIZE_ROUTINE OnHwPassiveInitialize;
VIRTUAL_HW_FIND_ADAPTER OnVirtualHwFindAdapter;
HW_STARTIO OnHwStartIo;
HW_ADAPTER_CONTROL OnHwAdapterControl;
HW_RESET_BUS OnHwResetBus;
HW_FREE_ADAPTER_RESOURCES OnHwFreeAdapterResource;
HW_UNIT_CONTROL OnHwUnitControl;

UCHAR OnInquiryVpdRequest(
	IN PHW_DEVICE_EXTENSION DeviceExtension,
	IN PSCSI_REQUEST_BLOCK Srb
);
#pragma endregion

