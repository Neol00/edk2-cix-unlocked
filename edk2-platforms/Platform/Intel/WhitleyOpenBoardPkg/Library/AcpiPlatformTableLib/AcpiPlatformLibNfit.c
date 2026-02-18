/** @file
  ACPI Platform Driver Hooks

  @copyright
  Copyright 1996 - 2019 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"

/**
  This function locates the CrystalRidge protocol and JedecNvdimm protocol
  and calls the update ACPI tables functions defined there to update/build
  the NVDIMM F/W Interface Table (NFIT). It builds the NFIT table which gets
  published in ACPI XSDT.

  @param[in,out] Table      Pointer to NFIT which will be build in
                            CR Protocol and will be publised in ACPI XSDT.

  @retval EFI_SUCCESS       Table successfully updated.
  @retval EFI_UNSUPPORTED   Table not updated.
**/
EFI_STATUS
UpdateNfitTable (
  IN OUT   EFI_ACPI_COMMON_HEADER  *Table
  )
{
  EFI_NFIT_TABLE_UPDATE_PROTOCOL    *NfitTableUpdateProtocol = NULL;
  EFI_STATUS                        Status;

  Status = gBS->LocateProtocol (&gEfiNfitTableUpdateProtocolGuid, NULL, (VOID **) &NfitTableUpdateProtocol);

  if (!EFI_ERROR (Status)) {
    Status = NfitTableUpdateProtocol->UpdateAcpiTable ((UINT64*) Table);
  } else {
    DEBUG ((DEBUG_ERROR, "Cannot find NfitTableUpdateProtocol\n"));
  }
  DEBUG ((DEBUG_INFO, "NFIT Update Status: 0x%x\n", Status));

  return Status;
}
