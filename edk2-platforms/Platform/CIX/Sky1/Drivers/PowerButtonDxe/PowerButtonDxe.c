/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Uefi.h>
#include <Uefi/UefiBaseType.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/EfiResetSystemLib.h>
#include <Library/EcLib.h>
#include <Library/CixPostCodeLib.h>
#include <Protocol/PowerButtonProtocol.h>

#define  POWER_BUTTON_POLLING_INTERVAL  500 * 1000 * 10
#define  POWER_BUTTON_VALID             BIT1

EFI_EVENT  PwrBtnEvent;

VOID
EFIAPI
PowerButtonCallback (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  EFI_STATUS                  Status;
  EC_RESPONSE_ACPI_INT_EVENT  AcpiIntEvent;

  DEBUG ((DEBUG_VERBOSE, "\n[PBTN]callback.\n"));
  Status = GetAcpiIntEvent (&AcpiIntEvent);
  if (!EFI_ERROR (Status)) {
    if ( AcpiIntEvent.Event & POWER_BUTTON_VALID) {
      DEBUG ((DEBUG_VERBOSE, "\n[PBTN]pressed.\n"));
      LibResetSystem (EfiResetShutdown, EFI_SUCCESS, 0, NULL);
    }
  }
}

STATIC
EFI_STATUS
EFIAPI
CreatPowerButtonCallBack (
  )
{
  EFI_STATUS  Status;

  Status = gBS->CreateEvent (
                             EVT_NOTIFY_SIGNAL | EVT_TIMER,
                             TPL_CALLBACK,
                             PowerButtonCallback,
                             NULL,
                             &PwrBtnEvent
                             );
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  Status = gBS->SetTimer (PwrBtnEvent, TimerPeriodic, POWER_BUTTON_POLLING_INTERVAL);
Exit:
  DEBUG ((DEBUG_INFO, "[PBTN]Create Power Button callback %r\n", Status));
  return Status;
}

EFI_STATUS
EFIAPI
EnablePowerButtonDetect (
  VOID
  )
{
  EFI_STATUS  Status;
  EC_RESPONSE_ACPI_INT_EVENT  AcpiIntEvent;

  GetAcpiIntEvent (&AcpiIntEvent); //clear

  Status = gBS->SetTimer (PwrBtnEvent, TimerPeriodic, POWER_BUTTON_POLLING_INTERVAL);
  DEBUG ((DEBUG_INFO, "[PBTN] Enable Power Button Detect %r\n", Status));
  return Status;
}

VOID
EFIAPI
DisablePowerButtonDetect (
  VOID
  )
{
  gBS->SetTimer (PwrBtnEvent, TimerCancel, 0);
}

STATIC CIX_POWER_BUTTON_PROTOCOL  CixPowerButtonProtocol = {
  EnablePowerButtonDetect,
  DisablePowerButtonDetect
};

EFI_STATUS
EFIAPI
PowerButtonDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EC_RESPONSE_ACPI_INT_EVENT  AcpiIntEvent;

  GetAcpiIntEvent (&AcpiIntEvent); //clear
  Status = CreatPowerButtonCallBack ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->InstallProtocolInterface (
    &ImageHandle,
    &gCixPowerButtonProtocolGuid,
    EFI_NATIVE_INTERFACE,
    &CixPowerButtonProtocol
    );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to install power button protocol %r\n", __FUNCTION__, Status));
    return Status;
  }

  return EFI_SUCCESS;
}
