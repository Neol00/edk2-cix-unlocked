/*
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#ifndef __PLATFORM_SMBIOS__
#define __PLATFORM_SMBIOS__

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>

#define PLATFORM_SMBIOS_TABLE_HOOK  AddSmbiosType0,AddSmbiosType1,AddSmbiosType2,AddSmbiosType3
#define PLATFORM_SMBIOS_TABLE_NAME  "AddSmbiosType0","AddSmbiosType1","AddSmbiosType2","AddSmbiosType3"

typedef EFI_STATUS (ADD_PLATFORM_SMBIOS_TABLE)(
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  );

// SMBIOS tables often reference each other using
// fixed constants, define a list of these constants
// for our hardcoded tables
enum SMBIOS_REFRENCE_HANDLES {
  SMBIOS_HANDLE_MOTHERBOARD = 0x1000,
  SMBIOS_HANDLE_CHASSIS,
  SMBIOS_HANDLE_CLUSTER
};

EFI_STATUS
AddSmbiosType0 (
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  );

EFI_STATUS
AddSmbiosType1 (
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  );

EFI_STATUS
AddSmbiosType2 (
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  );

EFI_STATUS
AddSmbiosType3 (
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  );

#endif
