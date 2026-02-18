/** I2cDxe.c
  I2c driver APIs for read, write, initialize, set speed and reset

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/I2cLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/CixPostCodeLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Protocol/ConfigParamsManageProtocol.h>
#include "I2cDxe.h"

STATIC EFI_EVENT  mI2cMemoryInitEvent;
STATIC EFI_EVENT  mI2cVirtualAddrChangeEvent;

STATIC I2C_CONTROLLER_CONTEXT  *mI2cBusRuntime[7] = { NULL };

STATIC CONST EFI_I2C_CONTROLLER_CAPABILITIES  mI2cControllerCapabilities = {
  0,
  0,
  0,
  0
};

STATIC I2C_DEVICE_PATH  mI2cDevicePathProtocolTemplate = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8)(OFFSET_OF (I2C_DEVICE_PATH, End)),
        (UINT8)(OFFSET_OF (I2C_DEVICE_PATH, End) >> 8),
      },
    },
    {
      0  // GUID
    },
  },
  0,    // MmioBase
  0xFF, // I2c Bus
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
BOOLEAN
IsI2cBusRuntime (
  IN UINT32  I2cBus
  )
{
  switch (I2cBus) {
    case 0:
      return FixedPcdGetBool (PcdI2c0Runtime);
    case 1:
      return FixedPcdGetBool (PcdI2c1Runtime);
    case 2:
      return FixedPcdGetBool (PcdI2c2Runtime);
    case 3:
      return FixedPcdGetBool (PcdI2c3Runtime);
    case 4:
      return FixedPcdGetBool (PcdI2c4Runtime);
    case 5:
      return FixedPcdGetBool (PcdI2c5Runtime);
    case 6:
      return FixedPcdGetBool (PcdI2c6Runtime);
    case 7:
      return FixedPcdGetBool (PcdI2c7Runtime);
    default:
      return FALSE;
  }
}

/**
  Function to set i2c bus frequency

  @param   This            Pointer to I2c master protocol
  @param   BusClockHertz   value to be set

  @retval EFI_SUCCESS      Operation successfull
**/
STATIC
EFI_STATUS
EFIAPI
SetBusFrequency (
  IN CONST EFI_I2C_MASTER_PROTOCOL  *This,
  IN OUT UINTN                      *BusClockHertz
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  I2C_CONTROLLER_CONTEXT  *I2c;

  I2c = I2C_CONTROLLER_FROM_THIS (This);

  I2c->Descriptor.BusClk = *BusClockHertz;

  Status = I2cSetClk (&I2c->Descriptor);

  return Status;
}

/**
  Function to reset I2c Controller

  @param  This             Pointer to I2c master protocol

  @return EFI_SUCCESS      Operation successfull
**/
STATIC
EFI_STATUS
EFIAPI
Reset (
  IN CONST EFI_I2C_MASTER_PROTOCOL  *This
  )
{
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
StartRequest (
  IN CONST EFI_I2C_MASTER_PROTOCOL  *This,
  IN UINTN                          SlaveAddress,
  IN EFI_I2C_REQUEST_PACKET         *RequestPacket,
  IN EFI_EVENT                      Event            OPTIONAL,
  OUT EFI_STATUS                    *I2cStatus       OPTIONAL
  )
{
  EFI_STATUS              Status;
  I2C_CONTROLLER_CONTEXT  *I2c;

  I2c = I2C_CONTROLLER_FROM_THIS (This);

  Status = I2cMasterXfer (&I2c->Descriptor, SlaveAddress, RequestPacket);

  return Status;
}

EFI_STATUS
I2cHostInit (
  IN UINT32  I2cBus,
  IN UINT32  Clock
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  I2C_CONTROLLER_CONTEXT  *I2c   = NULL;

  I2c = AllocateRuntimeZeroPool (sizeof (I2C_CONTROLLER_CONTEXT));
  if (I2c != NULL) {
    I2c->Signature                           = I2C_CONTROLLER_SIGNATURE;
    I2c->Descriptor.Bus                      = I2cBus;
    I2c->Descriptor.MemBase                  = I2cGetMemBase (I2cBus);
    I2c->Descriptor.BusClk                   = Clock;
    I2c->I2cMaster.SetBusFrequency           = SetBusFrequency;
    I2c->I2cMaster.Reset                     = Reset;
    I2c->I2cMaster.StartRequest              = StartRequest;
    I2c->I2cMaster.I2cControllerCapabilities = &mI2cControllerCapabilities;

    Status = I2cInitialize (&I2c->Descriptor);

    CopyMem ((VOID *)(UINTN)&(I2c->DevicePath), (VOID *)(UINTN)&mI2cDevicePathProtocolTemplate, sizeof (I2C_DEVICE_PATH));
    CopyGuid (&I2c->DevicePath.Vendor.Guid, &gCixI2cDevicePathGuid);
    I2c->DevicePath.MmioBase = I2c->Descriptor.MemBase;
    I2c->DevicePath.Bus      = I2c->Descriptor.Bus;

    Status = gBS->InstallMultipleProtocolInterfaces (
                    &I2c->Controller,
                    &gEfiI2cMasterProtocolGuid,
                    (VOID **)&I2c->I2cMaster,
                    &gEfiDevicePathProtocolGuid,
                    &I2c->DevicePath,
                    NULL
                    );

    if (EFI_ERROR (Status)) {
      FreePool (I2c);
    }
  } else {
    DEBUG ((DEBUG_ERROR, "%a failed to allocate buffer(size %x) for I2C host(base %x)\n", __FUNCTION__, sizeof (I2C_CONTROLLER_CONTEXT), I2c->Descriptor.MemBase));
    Status = EFI_OUT_OF_RESOURCES;
  }

  if (IsI2cBusRuntime (I2cBus)) {
    mI2cBusRuntime[I2cBus] = I2c;
  }

  return Status;
}

VOID
EFIAPI
I2cMemoryInitEventNotify (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                       Status;
  UINTN                            RuntimeMmioRegionBase;
  UINTN                            RuntimeMmioRegionSize;
  UINTN                            NumberOfDescriptors, Index;
  UINT32                           I2cBus;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR  *MemSpaceMap;

  for (I2cBus = 0; I2cBus <= 7; I2cBus++) {
    if (!IsI2cBusRuntime (I2cBus)) {
      continue;
    }

    // to cover I2C host MMIO space
    RuntimeMmioRegionBase = I2cGetMemBase (I2cBus);
    RuntimeMmioRegionSize = SIZE_4KB;

    Status = gDS->GetMemorySpaceMap (&NumberOfDescriptors, &MemSpaceMap);

    for (Index = 0; Index < NumberOfDescriptors; Index++) {
      if ((RuntimeMmioRegionBase >= MemSpaceMap[Index].BaseAddress) &&
          ((RuntimeMmioRegionBase + RuntimeMmioRegionSize) <= (MemSpaceMap[Index].BaseAddress + MemSpaceMap[Index].Length)) &&
          (MemSpaceMap[Index].GcdMemoryType == EfiGcdMemoryTypeMemoryMappedIo))
      {
        DEBUG ((DEBUG_INFO, "%a: memory space base 0x%x, length 0x%x already added\n", __FUNCTION__, RuntimeMmioRegionBase, RuntimeMmioRegionSize));
        return;
      }
    }

    Status = gDS->AddMemorySpace (
                    EfiGcdMemoryTypeMemoryMappedIo,
                    RuntimeMmioRegionBase,
                    RuntimeMmioRegionSize,
                    EFI_MEMORY_UC | EFI_MEMORY_RUNTIME
                    );
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "%a: fail to add memory space base 0x%x, length 0x%x, status %r\n", __FUNCTION__, RuntimeMmioRegionBase, RuntimeMmioRegionSize, Status);
    } else {
      DEBUG ((DEBUG_INFO, "%a: add memory space base 0x%x, length 0x%x\n", __FUNCTION__, RuntimeMmioRegionBase, RuntimeMmioRegionSize));
      Status = gDS->SetMemorySpaceAttributes (
                      RuntimeMmioRegionBase,
                      RuntimeMmioRegionSize,
                      EFI_MEMORY_UC | EFI_MEMORY_RUNTIME
                      );
      if (EFI_ERROR (Status)) {
        DebugPrint (DEBUG_ERROR, "%a: fail to set memory attributes base 0x%x, length 0x%x, status %r\n", __FUNCTION__, RuntimeMmioRegionBase, RuntimeMmioRegionSize, Status);
        gDS->RemoveMemorySpace (
               RuntimeMmioRegionBase,
               RuntimeMmioRegionSize
               );
      } else {
        DEBUG ((DEBUG_INFO, "%a: set memory space attribute 0x%llx\n", __FUNCTION__, EFI_MEMORY_UC | EFI_MEMORY_RUNTIME));
      }
    }
  }
}

VOID
LibI2cVirtualNotifyEvent (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  UINT32  I2cBus;

  for (I2cBus = 0; I2cBus <= 7; I2cBus++) {
    if (!IsI2cBusRuntime (I2cBus)) {
      continue;
    }

    EfiConvertPointer (0x0, (VOID **)&mI2cBusRuntime[I2cBus]->I2cMaster.StartRequest);
    EfiConvertPointer (0x0, (VOID **)&mI2cBusRuntime[I2cBus]->I2cMaster.SetBusFrequency);
    EfiConvertPointer (0x0, (VOID **)&mI2cBusRuntime[I2cBus]->I2cMaster.Reset);
    EfiConvertPointer (0x0, (VOID **)&mI2cBusRuntime[I2cBus]->I2cMaster.I2cControllerCapabilities);
    EfiConvertPointer (0x0, (VOID **)&mI2cBusRuntime[I2cBus]->Descriptor.MemBase);
  }
}

EFI_STATUS
EFIAPI
I2cDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                         Status;
  CONFIG_PARAMS_DATA_BLOCK           *ConfigData;
  CIX_CONFIG_PARAMS_MANAGE_PROTOCOL  *ConfigManage;

  POST_CODE (I2cInitDxeStart);

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

  for (UINT8 I2cBus = 0; I2cBus < I2C_CTRL_NUM; I2cBus++) {
    if (!ConfigData->Fch.I2c[I2cBus].Enable) {
      continue;
    }

    I2cEnvInit (I2cBus);
    Status = I2cHostInit (I2cBus, ConfigData->Fch.I2c[I2cBus].BusFreq);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Failed to initialize I2C[%d], status: %r\n", I2cBus, Status));
      // ASSERT_EFI_ERROR(Status);
    }
  }

  EfiCreateProtocolNotifyEvent (
    &gEfiCpuArchProtocolGuid,
    TPL_CALLBACK,
    I2cMemoryInitEventNotify,
    NULL,
    &mI2cMemoryInitEvent
    );

  //
  // Register for the virtual address change event
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  LibI2cVirtualNotifyEvent,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &mI2cVirtualAddrChangeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  POST_CODE (I2cInitDxeEnd);

  return EFI_SUCCESS;
}
