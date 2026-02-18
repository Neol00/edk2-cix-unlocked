/**
  SPI driver APIs for transfer, initialize, set speed
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved

  Copyright 2023-2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "SpiDxe.h"

STATIC SPI_DEVICE_PATH  mSpiDevicePathProtocolTemplate = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8)(OFFSET_OF (SPI_DEVICE_PATH, End)),
        (UINT8)(OFFSET_OF (SPI_DEVICE_PATH, End) >> 8),
      },
    },
    {
      0  // GUID
    },
  },
  0,  // MmioBase
  0xFF,
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      sizeof (EFI_DEVICE_PATH_PROTOCOL),
      0
    }
  }
};

STATIC
VOID
SpiSetupTransfer (
  IN SPI_HOST_PROTOCOL  *This,
  IN SPI_DEVICE         *Device
  )
{
  SPI_HOST  *Host;

  if (Device == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: invalid SPI device %r\n", __FUNCTION__, EFI_INVALID_PARAMETER));
    return;
  }

  Host = SPI_HOST_FROM_SPI_HOST_PROTOCOL (This);

  EfiAcquireLock (&Host->Lock);

  SpiInit (Device);

  EfiReleaseLock (&Host->Lock);
}

EFI_STATUS
EFIAPI
SpiTransferData (
  IN SPI_HOST_PROTOCOL  *This,
  IN SPI_DEVICE         *Device
  )
{
  SPI_HOST  *Host;

  if (Device == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: invalid SPI device %r\n", __FUNCTION__, EFI_INVALID_PARAMETER));
    return EFI_INVALID_PARAMETER;
  }

  if ((Device->TxBuf == NULL) || (Device->RxBuf == NULL)) {
    DEBUG ((DEBUG_ERROR, "%a: invalid SPI rx/tx buffer\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  if ((Device->TxBytes == 0) || (Device->TxBytes == 0)) {
    DEBUG ((DEBUG_ERROR, "%a: invalid SPI rx/tx data size\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  Host = SPI_HOST_FROM_SPI_HOST_PROTOCOL (This);

  EfiAcquireLock (&Host->Lock);

  SpiTransfer (Device);

  EfiReleaseLock (&Host->Lock);

  return EFI_SUCCESS;
}

SPI_DEVICE *
EFIAPI
SpiSetupDevice (
  IN SPI_HOST_PROTOCOL  *This,
  IN CS_MODE            ChipSelect,
  IN SPI_MODE           Mode,
  IN UINTN              Clock
  )
{
  SPI_HOST    *Host   = NULL;
  SPI_DEVICE  *Device = NULL;

  Host = SPI_HOST_FROM_SPI_HOST_PROTOCOL (This);

  Device = AllocateZeroPool (sizeof (SPI_DEVICE));
  if (Device == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: can not allocate memory for SPI device on cs# %x\n", __FUNCTION__, ChipSelect));
    return NULL;
  }

  Device->HostBase    = (VOID *)Host->BaseAddr;
  Device->ClkRate     = Clock;
  Device->IsDecodedCs = FALSE;
  Device->ChipSelect  = ChipSelect;
  Device->Mode        = Mode;

  SpiSetupTransfer (This, Device);

  return Device;
}

EFI_STATUS
EFIAPI
SpiFreeDevice (
  IN SPI_HOST_PROTOCOL  *This,
  IN SPI_DEVICE         *Device
  )
{
  if (Device == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: invalid SPI device %r\n", __FUNCTION__, EFI_INVALID_PARAMETER));
    return EFI_INVALID_PARAMETER;
  }

  FreePool (Device);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
SpiHostInit (
  IN UINT8  Bus
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  SPI_HOST    *Host;

  Host = AllocateZeroPool (sizeof (SPI_HOST));
  if (Host == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: failed to allocate buffer(size %x) for SPI host[%d]\n", __FUNCTION__, sizeof (SPI_HOST), Bus));
    return EFI_OUT_OF_RESOURCES;
  }

  EfiInitializeLock (&Host->Lock, TPL_NOTIFY);

  Host->Signature             = SPI_HOST_SIGNATURE;
  Host->BaseAddr              = GetMmioBaseAddress (Bus);
  Host->HostProtocol.Setup    = SpiSetupDevice;
  Host->HostProtocol.Free     = SpiFreeDevice;
  Host->HostProtocol.Transfer = SpiTransferData;

  CopyMem ((VOID *)(UINTN)&(Host->DevicePath), (VOID *)(UINTN)&mSpiDevicePathProtocolTemplate, sizeof (SPI_DEVICE_PATH));
  CopyGuid (&Host->DevicePath.Vendor.Guid, &gCixSpiDevicePathGuid);
  Host->DevicePath.MmioBase = Host->BaseAddr;
  Host->DevicePath.Bus      = Bus;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &(Host->Handle),
                  &gCixSpiHostProtocolGuid,
                  (VOID **)&(Host->HostProtocol),
                  &gEfiDevicePathProtocolGuid,
                  &(Host->DevicePath),
                  NULL
                  );

  if (EFI_ERROR (Status)) {
    FreePool (Host);
  }

  return Status;
}

EFI_STATUS
EFIAPI
SpiDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                         Status;
  CONFIG_PARAMS_DATA_BLOCK           *ConfigData;
  CIX_CONFIG_PARAMS_MANAGE_PROTOCOL  *ConfigManage;

  POST_CODE (SpiInitDxeStart);

  Status = gBS->LocateProtocol (
                  &gCixConfigParamsManageProtocolGuid,
                  NULL,
                  (VOID **)&ConfigManage
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: config parameters invalid %r\n", __FUNCTION__));
    return EFI_UNSUPPORTED;
  }

  ConfigData = ConfigManage->Data;

  for (UINT8 SpiBus = 0; SpiBus < SPI_CTRL_NUM; SpiBus++) {
    if (!ConfigData->Fch.Spi[SpiBus].Enable) {
      continue;
    }

    SpiClkConfig (SpiBus);
    Status = SpiHostInit (SpiBus);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Failed to initialize SPI[%d], status: %r\n", SpiBus, Status));
    }
  }

  POST_CODE (SpiInitDxeEnd);

  return EFI_SUCCESS;
}
