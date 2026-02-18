/**

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/EcLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/DxeServicesTableLib.h>

extern I2C_HOST_DESCRIPTOR  *mHost;

STATIC EFI_EVENT  mEcVirtualAddrChangeEvent;
STATIC EFI_EVENT  mEcExitBootServicesEvent;
STATIC EFI_EVENT  mEcMemoryInitEvent;

VOID
EFIAPI
I2cMemoryInitEventNotify (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS  Status;
  UINTN       RuntimeMmioRegionBase;
  UINTN       RuntimeMmioRegionSize;

  if (Context == NULL) {
    return;
  }

  // to cover I2C host MMIO space
  RuntimeMmioRegionBase = ((I2C_HOST_DESCRIPTOR *)Context)->MemBase;
  RuntimeMmioRegionSize = SIZE_4KB;

  Status = gDS->AddMemorySpace (
                  EfiGcdMemoryTypeMemoryMappedIo,
                  RuntimeMmioRegionBase,
                  RuntimeMmioRegionSize,
                  EFI_MEMORY_UC | EFI_MEMORY_RUNTIME
                  );
  if (EFI_ERROR (Status)) {
    DebugPrint (DEBUG_ERROR, "%a: fail to add memory space base 0x%x, size 0x%x, status %r\n", __FUNCTION__, RuntimeMmioRegionBase, RuntimeMmioRegionSize, Status);
    ASSERT_EFI_ERROR (Status);
  }

  DEBUG ((DEBUG_INFO, "%a: add memory space base 0x%x, length 0x%x\n", __FUNCTION__, RuntimeMmioRegionBase, RuntimeMmioRegionSize));

  Status = gDS->SetMemorySpaceAttributes (
                  RuntimeMmioRegionBase,
                  RuntimeMmioRegionSize,
                  EFI_MEMORY_UC | EFI_MEMORY_RUNTIME
                  );
  if (EFI_ERROR (Status)) {
    DebugPrint (DEBUG_ERROR, "%a: fail to set memory attributes base 0x%x, size 0x%x, status %r\n", __FUNCTION__, RuntimeMmioRegionBase, RuntimeMmioRegionSize, Status);
    gDS->RemoveMemorySpace (
           RuntimeMmioRegionBase,
           RuntimeMmioRegionSize
           );
    ASSERT_EFI_ERROR (Status);
  }

  DEBUG ((DEBUG_INFO, "%a: set memory space attribute 0x%llx\n", __FUNCTION__, EFI_MEMORY_UC | EFI_MEMORY_RUNTIME));
}

/**
  Fixup internal data so that EFI can be call in virtual mode.
  Call the passed in Child Notify event and convert any pointers in
  lib to virtual mode.

  @param[in]    Event   The Event that is being processed
  @param[in]    Context Event Context
**/
VOID
EFIAPI
LibEcVirtualNotifyEvent (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EfiConvertPointer (0x0, (VOID **)&mHost->MemBase);
  EfiConvertPointer (0x0, (VOID **)&mHost);
}

VOID
EFIAPI
LibEcExitBootServicesNotifyEvent (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  mI2cLibAtRuntime = TRUE;
}

RETURN_STATUS
EFIAPI
EcRuntimeLibConstructor (
  VOID
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  mHost          = AllocateRuntimeZeroPool (sizeof (I2C_HOST_DESCRIPTOR));
  mHost->Bus     = FixedPcdGet8 (PcdEcI2cBus);
  mHost->MemBase = I2cGetMemBase (mHost->Bus);
  mHost->BusClk  = I2cGetBusClk (mHost->Bus);

  if (PcdGetBool (PcdEcI2cHostInitialized) == FALSE) {
    I2cEnvInit (mHost->Bus);
    Status = I2cInitialize (mHost);
    if (Status == EFI_SUCCESS) {
      PcdSetBoolS (PcdEcI2cHostInitialized, TRUE);
    }
  }

  EfiCreateProtocolNotifyEvent (
    &gEfiCpuArchProtocolGuid,
    TPL_CALLBACK,
    I2cMemoryInitEventNotify,
    mHost,
    &mEcMemoryInitEvent
    );

  Status = gBS->CreateEvent (
                  EVT_SIGNAL_EXIT_BOOT_SERVICES,
                  TPL_CALLBACK,
                  LibEcExitBootServicesNotifyEvent,
                  NULL,
                  &mEcExitBootServicesEvent
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register for the virtual address change event
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  LibEcVirtualNotifyEvent,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &mEcVirtualAddrChangeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
