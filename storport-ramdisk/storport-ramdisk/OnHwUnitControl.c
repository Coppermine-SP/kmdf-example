/*
*	storport-ramdisk - OnHwUnitControl.c
*	램 디스크 가상 장치를 구현하는 Storport Virutal Miniport Driver 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

#include "storport-ramdisk.h"
#include "dbglog.h"

SCSI_UNIT_CONTROL_STATUS OnHwUnitControl(
	IN PVOID DeviceExtension,
	IN SCSI_UNIT_CONTROL_TYPE ControlType,
	IN PVOID Parameters
) {
	if(DeviceExtension == NULL)
		return ScsiUnitControlUnsuccessful;

	switch (ControlType) {
		case ScsiQuerySupportedUnitControlTypes:
		{
			PSCSI_SUPPORTED_CONTROL_TYPE_LIST pControlTypeList = NULL;
			ULONG max = ScsiUnitControlMax;
			BOOLEAN supportedTypes[ScsiUnitControlMax] = {
				TRUE,	// ScsiQuerySupportedUnitControlTypes
				FALSE,	// ScsiUnitUsage
				FALSE,	// ScsiUnitStart
				FALSE,	// ScsiUnitPower
				FALSE,	// ScsiUnitPoFxPowerInfo
				FALSE,	// ScsiUnitPoFxPowerRequired
				FALSE,	// ScsiUnitPoFxPowerActive
				FALSE,	// ScsiUnitPoFxPowerSetFState
				FALSE,	// ScsiUnitPoFxPowerControl
				FALSE,	// ScsiUnitRemove
				FALSE,	// ScsiUnitSurpriseRemoval
				TRUE,	// ScsiUnitRichDescription
				FALSE,	// ScsiUnitQueryBusType
				FALSE,	// ScsiUnitQueryFruId
				FALSE,	// ScsiUnitReportInternalData
				FALSE,	// ScsiUnitKsrPowerDown
				FALSE,	// ScsiUnitNvmeIceInformation
			};
			pControlTypeList = (PSCSI_SUPPORTED_CONTROL_TYPE_LIST)Parameters;
			if(pControlTypeList == NULL) 
				return ScsiUnitControlUnsuccessful;

			if (pControlTypeList->MaxControlType < max)
				max = pControlTypeList->MaxControlType;

			for (ULONG i = 0; i < max; i++)
				pControlTypeList->SupportedTypeList[i] = supportedTypes[i];

			break;
		}
		case ScsiUnitRichDescription:
		{
			const PCHAR VendorId = "CloudInteractive";
			const PCHAR ProductId = "Virtual Disk Device";
			const PCHAR ProductRev = "1.00";
			PSTOR_RICH_DEVICE_DESCRIPTION pDesc = (PSTOR_RICH_DEVICE_DESCRIPTION)Parameters;
			if (pDesc == NULL)
				return ScsiUnitControlUnsuccessful;

			RtlCopyMemory(pDesc->VendorId, VendorId, min(strlen(VendorId), STOR_VENDOR_ID_LENGTH));
			RtlCopyMemory(pDesc->ModelNumber, ProductId, min(strlen(ProductId), STOR_MODEL_NUMBER_LENGTH));
			RtlCopyMemory(pDesc->FirmwareRevision, ProductRev, min(strlen(ProductRev), STOR_FIRMWARE_REVISION_LENGTH));
			break;
		}
		default:
			return ScsiUnitControlUnsuccessful;
	}

	return ScsiUnitControlSuccess;
}
