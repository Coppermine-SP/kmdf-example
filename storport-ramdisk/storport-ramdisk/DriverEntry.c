/*
*	storport-ramdisk - DriverEntry.c
*	램 디스크 가상 장치를 구현하는 Storport Virutal Miniport Driver 예제입니다.
* 
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "storport-ramdisk.h"
#include "dbglog.h"

NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath
) {
	VIRTUAL_HW_INITIALIZATION_DATA initData;
	RtlZeroMemory(&initData, sizeof(initData));
	
	initData.HwInitializationDataSize = sizeof(initData);
	initData.HwFindAdapter = OnVirtualHwFindAdapter;
	initData.HwInitialize = OnHwInitialize;
	initData.HwStartIo = OnHwStartIo;
	initData.HwBuildIo = NULL;
	initData.HwDmaStarted = NULL;
	initData.HwInterrupt = NULL;
	initData.HwAdapterState = NULL;
	initData.HwAdapterControl = OnHwAdapterControl;
	initData.HwResetBus = OnHwResetBus;
	initData.HwFreeAdapterResources = OnHwFreeAdapterResource;

	initData.AdapterInterfaceType = Internal;
	initData.MapBuffers = STOR_MAP_ALL_BUFFERS_INCLUDING_READ_WRITE;
	initData.NeedPhysicalAddresses = FALSE;
	initData.NumberOfAccessRanges = 0,
	initData.AutoRequestSense = TRUE;
	initData.ReceiveEvent = FALSE;
	initData.TaggedQueuing = TRUE;
	initData.MultipleRequestPerLu = TRUE;

	initData.DeviceExtensionSize = sizeof(HW_DEVICE_EXTENSION);
	initData.SrbExtensionSize = 0;
	initData.SpecificLuExtensionSize = 0;

	ULONG status = StorPortInitialize(DriverObject, RegistryPath, (PHW_INITIALIZATION_DATA)&initData, NULL);
	if( status != STOR_STATUS_SUCCESS ) {
		DBGPRINT_ERROR("StorPortInitialize() failed with STOR_STATUS=0x%8x", status);
		return STATUS_UNSUCCESSFUL;
	}

	return STATUS_SUCCESS;
}