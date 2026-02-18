/** @file

  @copyright
  Copyright 2013 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include <Library/UbaAcpiUpdateLib.h>

#include <Protocol/UbaCfgDb.h>

EFI_STATUS
PlatformGetAcpiFixTableDataPointer (
  IN  VOID                               **TablePtr
  )
{
  EFI_STATUS                              Status;

  UBA_CONFIG_DATABASE_PROTOCOL            *UbaConfigProtocol = NULL;
  UINTN                                   DataLength = 0;
  ACPI_FIX_UPDATE_TABLE                   AcpiFixUpdateTable;

  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &UbaConfigProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DataLength  = sizeof (AcpiFixUpdateTable);
  Status = UbaConfigProtocol->GetData (
                                    UbaConfigProtocol,
                                    &gPlatformAcpiFixTableGuid,
                                    &AcpiFixUpdateTable,
                                    &DataLength
                                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (AcpiFixUpdateTable.Signature == PLATFORM_ACPI_FIX_UPDATE_SIGNATURE);
  ASSERT (AcpiFixUpdateTable.Version == PLATFORM_ACPI_FIX_UPDATE_VERSION);

  *TablePtr = AcpiFixUpdateTable.TablePtr;

  return EFI_SUCCESS;
}
