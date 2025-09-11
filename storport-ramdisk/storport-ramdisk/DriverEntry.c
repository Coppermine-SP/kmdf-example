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
	HW_INITIALIZATION_DATA initData;
	RtlZeroMemory(&initData, sizeof(initData));

	initData.HwInitializationDataSize = sizeof(initData);
	initData.HwFindAdapter = (PVOID)OnVirtualHwFindAdapter;
	initData.HwInitialize = OnHwInitialize;
	initData.HwStartIo = OnHwStartIo;
	initData.HwBuildIo = NULL;
	initData.HwDmaStarted = NULL;
	initData.HwInterrupt = NULL;
	initData.HwAdapterState = NULL;
	initData.HwAdapterControl = OnHwAdapterControl;
	initData.HwResetBus = OnHwResetBus;
	initData.HwUnitControl = OnHwUnitControl;
	initData.HwFreeAdapterResources = OnHwFreeAdapterResource;

	initData.AdapterInterfaceType = Internal;
	initData.NumberOfAccessRanges = 0,
	initData.AutoRequestSense = TRUE;
	initData.ReceiveEvent = FALSE;
	initData.TaggedQueuing = TRUE;
	initData.MultipleRequestPerLu = TRUE;
	initData.AddressTypeFlags = ADDRESS_TYPE_FLAG_BTL8;
	initData.SrbTypeFlags = SRB_TYPE_FLAG_SCSI_REQUEST_BLOCK;

	initData.DeviceExtensionSize = sizeof(HW_DEVICE_EXTENSION);
	initData.SrbExtensionSize = 0;
	initData.SpecificLuExtensionSize = 0;
	initData.FeatureSupport |= STOR_FEATURE_VIRTUAL_MINIPORT
		| STOR_FEATURE_DEVICE_NAME_NO_SUFFIX
		| STOR_FEATURE_ADAPTER_NOT_REQUIRE_IO_PORT;


	ULONG status = StorPortInitialize(DriverObject, RegistryPath, &initData, NULL);
	if( status != STOR_STATUS_SUCCESS ) {
		DBGPRINT_ERROR("StorPortInitialize() failed with STOR_STATUS=0x%8x", status);
		return STATUS_UNSUCCESSFUL;
	}

	return STATUS_SUCCESS;
}