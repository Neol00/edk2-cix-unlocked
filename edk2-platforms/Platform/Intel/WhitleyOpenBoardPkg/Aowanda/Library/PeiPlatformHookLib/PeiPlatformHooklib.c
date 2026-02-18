/** @file
  PEI Library Functions. Initialize GPIOs

  Copyright 1999 - 2021 Intel Corporation. <BR>
  Copyright (c) 2021, American Megatrends International LLC. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Guid/PlatformInfo.h>
#include <Library/DebugLib.h>
#include <Library/UbaGpioInitLib.h>
#include <Library/PeiPlatformHooklib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/DynamicSiLibraryPpi.h>

/**
  Configure GPIO

  @param[in]  None

  @retval     None
**/
VOID
GpioInit (
  VOID
  )
{
  PlatformInitGpios ();
}

/**
  Disables ME PCI devices like IDE-R , KT

  @param[in]  None
  @retval  EFI_SUCCESS   Operation success.

**/
EFI_STATUS
DisableMEDevices (
  VOID
  )
{
  EFI_STATUS             Status = EFI_SUCCESS;
  DYNAMIC_SI_LIBARY_PPI  *DynamicSiLibraryPpi;

  DynamicSiLibraryPpi = NULL;

  DEBUG ((DEBUG_INFO, "DisableMEDevices\n"));

  Status = PeiServicesLocatePpi (&gDynamicSiLibraryPpiGuid, 0, NULL, (VOID **) &DynamicSiLibraryPpi);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  //
  // Disable IDE-R
  //
  DynamicSiLibraryPpi->PchPcrAndThenOr32 (
                         PID_PSF1,
                         (R_PCH_H_PCR_PSF1_T0_SHDW_IDER_REG_BASE + R_PCH_PSFX_PCR_T0_SHDW_PCIEN),
                         (UINT32)~0,
                         B_PCH_PSFX_PCR_T0_SHDW_PCIEN_FUNDIS
                         );

  //
  // Disable KT
  //
  DynamicSiLibraryPpi->PchPcrAndThenOr32 (
                         PID_PSF1,
                         (R_PCH_H_PCR_PSF1_T0_SHDW_KT_REG_BASE + R_PCH_PSFX_PCR_T0_SHDW_PCIEN),
                         (UINT32)~0,
                         B_PCH_PSFX_PCR_T0_SHDW_PCIEN_FUNDIS
                         );
  return EFI_SUCCESS;
}

/**
  Configure GPIO and SIO

  @retval  EFI_SUCCESS   Operation success.
**/
EFI_STATUS
BoardInit (
  )
{
  GpioInit ();
  DisableMEDevices ();

  return EFI_SUCCESS;
}
