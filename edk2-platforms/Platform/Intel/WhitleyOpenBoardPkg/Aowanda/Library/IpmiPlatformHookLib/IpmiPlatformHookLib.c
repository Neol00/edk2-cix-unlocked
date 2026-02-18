/** @file
  This file implements the IPMI Platform hook functions

  Copyright (c) 2021, American Megatrends International LLC. <BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Ppi/DynamicSiLibraryPpi.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>

#define KCS_BASE_ADDRESS_MASK      0xFFF0
#define NUMBER_OF_BYTES_TO_DECODE  0x10

/**
  This function sets IO Decode Range in LPC registers

  @param[in]  IpmiIoBase  - IPMI Base IO address

  @retval  EFI_SUCCESS    - Operation success.

**/
EFI_STATUS
EFIAPI
PlatformIpmiIoRangeSet (
  UINT16  IpmiIoBase
  )
{
  EFI_STATUS             Status;
  DYNAMIC_SI_LIBARY_PPI  *DynamicSiLibraryPpi;

  DynamicSiLibraryPpi = NULL;

  DEBUG ((DEBUG_INFO, "PlatformIpmiIoRangeSet IpmiIoBase %x\n", IpmiIoBase));

  Status = PeiServicesLocatePpi (&gDynamicSiLibraryPpiGuid, 0, NULL, (VOID **) &DynamicSiLibraryPpi);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "PeiServicesLocatePpi for gDynamicSiLibraryPpiGuid failed. Status %r\n", Status));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  DynamicSiLibraryPpi->PchLpcGenIoRangeSet ((IpmiIoBase & KCS_BASE_ADDRESS_MASK), NUMBER_OF_BYTES_TO_DECODE);
  return EFI_SUCCESS;
}
