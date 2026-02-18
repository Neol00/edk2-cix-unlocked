/**
  Enroll default PK, KEK, db, dbx.
Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
Copyright (c) 2021, ARM Ltd. All rights reserved.<BR>
Copyright (c) 2021, Semihalf All rights reserved.<BR>

SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Guid/AuthenticatedVariableFormat.h>    // gEfiCustomModeEnableGuid
#include <Guid/GlobalVariable.h>                 // EFI_SETUP_MODE_NAME
#include <Guid/ImageAuthentication.h>            // EFI_IMAGE_SECURITY_DATABASE
#include <Library/BaseLib.h>                     // GUID_STRING_LENGTH
#include <Library/BaseMemoryLib.h>               // CopyGuid()
#include <Library/DebugLib.h>                    // ASSERT()
#include <Library/MemoryAllocationLib.h>         // FreePool()
#include <Library/UefiBootServicesTableLib.h>    // gBS
#include <Library/UefiLib.h>                     // AsciiPrint()
#include <Library/UefiRuntimeServicesTableLib.h> // gRT
#include <UefiSecureBoot.h>
#include <Library/SecureBootVariableLib.h>
#include <Library/SecureBootVariableProvisionLib.h>

VOID
EFIAPI
OnReadyToBoot (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS  Status;
  UINT8       SetupMode;

  gBS->CloseEvent (Event);

  Status = GetSetupMode (&SetupMode);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, " Cannot get SetupMode variable: %r\n", Status));
    return;
  }

  if (SetupMode == USER_MODE) {
    DEBUG ((DEBUG_INFO, " Skipped - USER_MODE\n"));
    return;
  }

  Status = SetSecureBootMode (CUSTOM_SECURE_BOOT_MODE);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, " Cannot set CUSTOM_SECURE_BOOT_MODE: %r\n", Status));
    return;
  }

  Status = EnrollDbFromDefault ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, " Cannot enroll db: %r\n", Status));
    goto error;
  }

  Status = EnrollDbxFromDefault ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, " Cannot enroll dbt: %r\n", Status));
  }

  Status = EnrollDbtFromDefault ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, " Cannot enroll dbx: %r\n", Status));
  }

  Status = EnrollKEKFromDefault ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, " Cannot enroll KEK: %r\n", Status));
    goto cleardbs;
  }

  Status = EnrollPKFromDefault ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, " Cannot enroll PK: %r\n", Status));
    goto clearKEK;
  }

  Status = SetSecureBootMode (STANDARD_SECURE_BOOT_MODE);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      " Cannot set CustomMode to STANDARD_SECURE_BOOT_MODE\n"
      "Please do it manually, otherwise system can be easily compromised\n"
      ));
  }

  return;

clearKEK:
  DeleteKEK ();

cleardbs:
  DeleteDbt ();
  DeleteDbx ();
  DeleteDb ();

error:
  Status = SetSecureBootMode (STANDARD_SECURE_BOOT_MODE);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      " Cannot set CustomMode to STANDARD_SECURE_BOOT_MODE\n"
      "Please do it manually, otherwise system can be easily compromised\n"
      ));
  }
}

/**
  Entry point function of platform secure boot keys enrollment.
  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval 0       The entry point is executed successfully.
  @retval other   Some error occurs when executing this entry point.
**/
EFI_STATUS
EFIAPI
SecureBootDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   ReadyToBootEvent;

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             OnReadyToBoot,
             NULL,
             &ReadyToBootEvent
             );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
