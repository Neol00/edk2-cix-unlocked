/** @file
  Source code file for S3 PEI module

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
Copyright (c) 2022, Baruch Binyamin Doron.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/SmmAccessLib.h>
#include <Library/SmmControlLib.h>

// TODO: Finalise implementation factoring
#define R_SA_PAM0  (0x80)
#define R_SA_PAM5  (0x85)
#define R_SA_PAM6  (0x86)

/**
  This function is called after FspSiliconInitDone installed PPI.
  For FSP API mode, this is when FSP-M HOBs are installed into EDK2.

  @param[in] PeiServices    Pointer to PEI Services Table.
  @param[in] NotifyDesc     Pointer to the descriptor for the Notification event that
                            caused this function to execute.
  @param[in] Ppi            Pointer to the PPI data associated with this function.

  @retval EFI_STATUS        Always return EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
FspSiliconInitDoneNotify (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDesc,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS     Status;
  EFI_BOOT_MODE  BootMode;
  UINTN          MchBaseAddress;

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  // Enable PAM regions for AP wakeup vector (resume)
  // - CPU is finalised by PiSmmCpuDxeSmm, not FSP. So, it's safe here?
  // TODO/TEST: coreboot does this unconditionally, vendor FWs may not (test resume). Should we?
  // - It is certainly interesting that only PAM0, PAM5 and PAM6 are defined for KabylakeSiliconPkg.
  // - Also note that 0xA0000-0xFFFFF is marked "reserved" in FSP HOB - this does not mean
  //   that the memory is unusable, perhaps this is precisely because it will contain
  //   the AP wakeup vector.
  if (BootMode == BOOT_ON_S3_RESUME) {
    MchBaseAddress = PCI_LIB_ADDRESS (0, 0, 0, 0);
    PciWrite8 (MchBaseAddress + R_SA_PAM0, 0x30);
    PciWrite8 (MchBaseAddress + (R_SA_PAM0 + 1), 0x33);
    PciWrite8 (MchBaseAddress + (R_SA_PAM0 + 2), 0x33);
    PciWrite8 (MchBaseAddress + (R_SA_PAM0 + 3), 0x33);
    PciWrite8 (MchBaseAddress + (R_SA_PAM0 + 4), 0x33);
    PciWrite8 (MchBaseAddress + R_SA_PAM5, 0x33);
    PciWrite8 (MchBaseAddress + R_SA_PAM6, 0x33);
  }

  //
  // Install EFI_PEI_MM_ACCESS_PPI for S3 resume case
  //
  Status = PeiInstallSmmAccessPpi ();
  ASSERT_EFI_ERROR (Status);

  //
  // Install EFI_PEI_MM_CONTROL_PPI for S3 resume case
  //
  Status = PeiInstallSmmControlPpi ();
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_PEI_NOTIFY_DESCRIPTOR  mFspSiliconInitDoneNotifyDesc = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gFspSiliconInitDonePpiGuid,
  FspSiliconInitDoneNotify
};

/**
  S3 PEI module entry point

  @param[in]  FileHandle           Not used.
  @param[in]  PeiServices          General purpose services available to every PEIM.

  @retval     EFI_SUCCESS          The function completes successfully
  @retval     EFI_OUT_OF_RESOURCES Insufficient resources to create database
**/
EFI_STATUS
EFIAPI
S3PeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status;

  Status = PeiServicesNotifyPpi (&mFspSiliconInitDoneNotifyDesc);
  ASSERT_EFI_ERROR (Status);

  return Status;
}
