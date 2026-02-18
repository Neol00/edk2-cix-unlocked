/** @file
  ACPI Platform Driver Hooks

  @copyright
  Copyright 1996 - 2016 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"

/**
  This function  locates the CrystalRidge Protocol and calls
  into one of its interface function (UpdateAcpiPcatTable) to
  update/build the PCAT (Platform Capability Attribute Table).
  And this table gets published in ACPI XSDT.

  @param *Table       - Pointer to PCAT table which will be
          build in CR Protocol and will be publised in ACPI
          XSDT.
  @retval Status      - Return Status
**/
EFI_STATUS
UpdatePcatTable (
   IN OUT   EFI_ACPI_COMMON_HEADER  *Table
   )
{
  EFI_STATUS                      Status;
  DYNAMIC_SI_LIBARY_PROTOCOL2     *DynamicSiLibraryProtocol2 = NULL;

  Status = gBS->LocateProtocol (&gDynamicSiLibraryProtocol2Guid, NULL, (VOID **) &DynamicSiLibraryProtocol2);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  Status = DynamicSiLibraryProtocol2->UpdatePcatTable (Table);
  return EFI_SUCCESS;
}
