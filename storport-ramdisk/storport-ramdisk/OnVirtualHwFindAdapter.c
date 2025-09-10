/*
*	storport-ramdisk - OnVirtualHwFindAdapter.c
*	램 디스크 가상 장치를 구현하는 Storport Virutal Miniport Driver 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "storport-ramdisk.h"

ULONG OnVirtualHwFindAdapter(
	IN PVOID DeviceExtension,
	IN PVOID HwContext,
	IN PVOID BusInformation,
	IN PVOID LowerDevice,
	IN PCHAR ArgumentString,
	IN OUT PPORT_CONFIGURATION_INFORMATION ConfigInfo,
	IN PBOOLEAN Again
) {
	UNREFERENCED_PARAMETER(HwContext);
	UNREFERENCED_PARAMETER(ArgumentString);
	UNREFERENCED_PARAMETER(BusInformation);
	UNREFERENCED_PARAMETER(LowerDevice);
	UNREFERENCED_PARAMETER(Again);

	if ((DeviceExtension == NULL) || (ConfigInfo == NULL))
		return SP_RETURN_NOT_FOUND;

	//Indicates this device is virtual.
	ConfigInfo->AdapterInterfaceType = Internal;
	ConfigInfo->VirtualDevice = TRUE;

	ConfigInfo->InterruptMode = Latched;
	ConfigInfo->MaximumTransferLength = MAX_DMA_TRANSFER_SIZE;
	ConfigInfo->NumberOfPhysicalBreaks = SCSI_MAXIMUM_PHYSICAL_BREAKS;
	ConfigInfo->DmaChannel = SP_UNINITIALIZED_VALUE;
	ConfigInfo->DmaPort = SP_UNINITIALIZED_VALUE;
	ConfigInfo->DmaWidth = MaximumDmaWidth; //Width32Bits;
	ConfigInfo->DmaSpeed = MaximumDmaSpeed; //Compatible;
	ConfigInfo->AlignmentMask = 3;	// DWORD Alignment	// scatter & gather option
	ConfigInfo->NumberOfBuses = 1;
	ConfigInfo->ScatterGather = TRUE;
	ConfigInfo->Master = TRUE;
	ConfigInfo->CachesData = FALSE;
	ConfigInfo->AdapterScansDown = FALSE;
	ConfigInfo->Dma32BitAddresses = TRUE;
	ConfigInfo->MaximumNumberOfTargets = 1;
	ConfigInfo->MaximumNumberOfLogicalUnits = 1;
	ConfigInfo->WmiDataProvider = FALSE;

	ConfigInfo->ResetTargetSupported = TRUE;
	ConfigInfo->SynchronizationModel = StorSynchronizeFullDuplex;
	ConfigInfo->MapBuffers = STOR_MAP_ALL_BUFFERS_INCLUDING_READ_WRITE;
	return SP_RETURN_FOUND;
}