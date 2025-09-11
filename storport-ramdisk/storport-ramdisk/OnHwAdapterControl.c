/*
*	storport-ramdisk - OnHwAdapterControl.c
*	램 디스크 가상 장치를 구현하는 Storport Virutal Miniport Driver 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "storport-ramdisk.h"

SCSI_ADAPTER_CONTROL_STATUS OnHwAdapterControl(
	IN PVOID DeviceExtension,
	IN SCSI_ADAPTER_CONTROL_TYPE ControlType,
	IN PVOID Parameters
) {
	UNREFERENCED_PARAMETER(Parameters);
	PHW_DEVICE_EXTENSION pDeviceExtension = NULL;
	PSCSI_SUPPORTED_CONTROL_TYPE_LIST pControlTypeList = NULL;

	if(DeviceExtension == NULL) {
	
		return ScsiAdapterControlUnsuccessful;
	}
	pDeviceExtension = (PHW_DEVICE_EXTENSION)DeviceExtension;

	switch (ControlType) {
		case ScsiQuerySupportedControlTypes:
		{
			BOOLEAN supportedTypes[ScsiAdapterControlMax] = {
				TRUE,
				TRUE,
				TRUE,
				FALSE,
				FALSE
			};

			ULONG max = ScsiAdapterControlMax;

			pControlTypeList = (PSCSI_SUPPORTED_CONTROL_TYPE_LIST)Parameters;
			if(pControlTypeList == NULL) 
				return ScsiAdapterControlUnsuccessful;

			if (pControlTypeList->MaxControlType < max)
				max = pControlTypeList->MaxControlType;

			for(ULONG i = 0; i < max; i++)
				pControlTypeList->SupportedTypeList[i] = supportedTypes[i];

			break;
		}
		case ScsiStopAdapter:
		{
			break;
		}
		case ScsiRestartAdapter:
		{
			break;
		}
		default:
		{
			break;
		}
	}

	return ScsiAdapterControlSuccess;
}