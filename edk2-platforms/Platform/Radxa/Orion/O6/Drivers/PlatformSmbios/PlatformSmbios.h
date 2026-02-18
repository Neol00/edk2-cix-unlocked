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

#define PLATFORM_SMBIOS_TABLE_HOOK  AddSmbiosType0,AddSmbiosType1,AddSmbiosType2,AddSmbiosType3,AddSmbiosType4,AddSmbiosType7,AddSmbiosType32
#define PLATFORM_SMBIOS_TABLE_NAME  "AddSmbiosType0","AddSmbiosType1","AddSmbiosType2","AddSmbiosType3","AddSmbiosType4","AddSmbiosType7","AddSmbiosType32"

typedef EFI_STATUS (ADD_PLATFORM_SMBIOS_TABLE)(
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  );

// SMBIOS tables often reference each other using
// fixed constants, define a list of these constants
// for our hardcoded tables
typedef enum {
  SMBIOS_HANDLE_ENCLOSURE = 0x1000,
  SMBIOS_HANDLE_CLUSTER1,
  SMBIOS_HANDLE_L1I_CACHE,
  SMBIOS_HANDLE_L1D_CACHE,
  SMBIOS_HANDLE_L2_CACHE,
  SMBIOS_HANDLE_L3_CACHE,
  SMBIOS_HANDLE_L4_CACHE,
  SMBIOS_HANDLE_PHYSICAL_MEMORY,
  SMBIOS_HANDLE_MEMORY_DEVICE0000,  // Chip 0 Bank 0
  SMBIOS_HANDLE_MEMORY_DEVICE0001,  // Chip 0 Bank 1
  SMBIOS_HANDLE_MEMORY_DEVICE0100,  // Chip 1 Bank 0
  SMBIOS_HANDLE_MEMORY_DEVICE0101,  // Chip 1 Bank 1
  SMBIOS_HANDLE_MEMORY_DEVICE0200,  // Chip 2 Bank 0
  SMBIOS_HANDLE_MEMORY_DEVICE0201,  // Chip 2 Bank 1
  SMBIOS_HANDLE_MEMORY_DEVICE0300,  // Chip 3 Bank 0
  SMBIOS_HANDLE_MEMORY_DEVICE0301,  // Chip 3 Bank 1
  SMBIOS_HANDLE_BASE_BOARD,
  SMBIOS_HANDLE_CHASSIS,
} SMBIOS_REFRENCE_HANDLES;

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

EFI_STATUS
AddSmbiosType4 (
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  );

EFI_STATUS
AddSmbiosType7 (
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  );

EFI_STATUS
AddSmbiosType32 (
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  );
#endif
