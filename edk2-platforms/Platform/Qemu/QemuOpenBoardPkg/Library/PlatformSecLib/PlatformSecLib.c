/** @file
  PlatformSecLib library functions

  Copyright (c) 2022 Theo Jehl All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiPei.h>
#include <Ppi/SecPlatformInformation.h>
#include <Ppi/TemporaryRamSupport.h>
#include <Library/PcdLib.h>
#include <Ppi/PeiCoreFvLocation.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/MtrrLib.h>
#include <Library/PlatformSecLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/IoLib.h>

#include <Library/LocalApicLib.h>

EFI_PEI_CORE_FV_LOCATION_PPI  gEfiPeiCoreFvLocationPpi = {
  (VOID *)FixedPcdGet32 (PcdFlashFvFspMBase)
};

STATIC EFI_PEI_PPI_DESCRIPTOR  mPeiSecPlatformPpi[] = {
  //
  // This must be the second PPI in the list because it will be patched in SecPlatformMain ();
  //
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gTopOfTemporaryRamPpiGuid,
    NULL
  }
};

EFI_PEI_PPI_DESCRIPTOR  gEfiPeiCoreFvLocationDescriptor = {
  EFI_PEI_PPI_DESCRIPTOR_PPI,
  &gEfiPeiCoreFvLocationPpiGuid,
  &gEfiPeiCoreFvLocationPpi
};

EFI_PEI_PPI_DESCRIPTOR *
EFIAPI
SecPlatformMain (
  IN OUT   EFI_SEC_PEI_HAND_OFF  *SecCoreData
  )
{
  // Use half of available heap size for PpiList
  EFI_PEI_PPI_DESCRIPTOR  *PpiList;

  PpiList = (VOID *)((UINTN)SecCoreData->PeiTemporaryRamBase + (UINTN)SecCoreData->PeiTemporaryRamSize / 2);

  CopyMem (PpiList, &gEfiPeiCoreFvLocationDescriptor, sizeof (EFI_PEI_PPI_DESCRIPTOR));

  CopyMem (&PpiList[1], &mPeiSecPlatformPpi, sizeof (EFI_PEI_PPI_DESCRIPTOR));

  // Patch the top of RAM PPI
  PpiList[1].Ppi = (VOID *)((UINTN)SecCoreData->TemporaryRamBase + SecCoreData->TemporaryRamSize);
  DEBUG ((DEBUG_INFO, "SecPlatformMain(): Top of memory %p\n", PpiList[1].Ppi));

  return PpiList;
}

/**
  This interface conveys state information out of the Security (SEC) phase into PEI.

  @param  PeiServices               Pointer to the PEI Services Table.
  @param  StructureSize             Pointer to the variable describing size of the input buffer.
  @param  PlatformInformationRecord Pointer to the EFI_SEC_PLATFORM_INFORMATION_RECORD.

  @retval EFI_SUCCESS           The data was successfully returned.
  @retval EFI_BUFFER_TOO_SMALL  The buffer was too small.

**/
EFI_STATUS
EFIAPI
SecPlatformInformation (
  IN CONST EFI_PEI_SERVICES                  **PeiServices,
  IN OUT   UINT64                            *StructureSize,
  OUT   EFI_SEC_PLATFORM_INFORMATION_RECORD  *PlatformInformationRecord
  )
{
  UINT32      TopOfTemporaryRam;
  VOID        *TopOfRamPpi;
  EFI_STATUS  Status;
  UINT32      Count;
  UINT32      *BistStart;
  UINT32      Length;

  Status = (*PeiServices)->LocatePpi (PeiServices, &gTopOfTemporaryRamPpiGuid, 0, NULL, &TopOfRamPpi);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  TopOfTemporaryRam = (UINT32)TopOfRamPpi;

  DEBUG ((DEBUG_INFO, "SecPlatformInformation: Top of memory is %p\n", TopOfRamPpi));

  Count  = *(UINT32 *)(TopOfTemporaryRam - sizeof (UINT32));
  Length = Count * sizeof (UINT32);

  BistStart = (UINT32 *)(TopOfTemporaryRam - sizeof (UINT32) - Length);

  DEBUG ((DEBUG_INFO, "SecPlatformInformation: Found %u processors with BISTs starting at %p\n", Count, BistStart));

  if (*StructureSize < Length) {
    *StructureSize = Length;
    return EFI_BUFFER_TOO_SMALL;
  }

  CopyMem (PlatformInformationRecord, BistStart, Length);
  *StructureSize = Length;

  // Mask the PIC to avoid any interruption down the line
  IoWrite8 (0x21, 0xff);
  IoWrite8 (0xA1, 0xff);

  DEBUG ((DEBUG_INFO, "Initialize APIC Timer \n"));
  InitializeApicTimer (0, MAX_UINT32, TRUE, 5);

  DEBUG ((DEBUG_INFO, "Disable APIC Timer interrupt\n"));
  DisableApicTimerInterrupt ();

  return EFI_SUCCESS;
}

/**
  This interface disables temporary memory in SEC Phase.
**/
VOID
EFIAPI
SecPlatformDisableTemporaryMemory (
  VOID
  )
{
  return;
}
