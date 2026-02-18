/** @file

  @copyright
  Copyright 1996 - 2020 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _ACPI_PLATFORM_HOOKS_H_
#define _ACPI_PLATFORM_HOOKS_H_

//
// Statements that include other header files
//
#include <PiDxe.h>
#include <Library/CpuConfigLib.h>
#include <Library/SetupLib.h>
#include <Library/LocalApicLib.h>

EFI_STATUS
PlatformHookInit (
  VOID
  );

VOID
DisableAriForwarding (
  VOID
  );

EFI_STATUS
AllocateRasfSharedMemory (
  VOID
  );

UINT8
EFIAPI
DetectHwpFeature (
  VOID
  );

VOID
InstallAndPatchAcpiTable (
  UINT32
  );

VOID
InstallXhciAcpiTable (
  VOID
  );

#endif
