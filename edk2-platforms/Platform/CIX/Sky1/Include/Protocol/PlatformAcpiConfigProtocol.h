/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved

**/
#ifndef __PLATFORM_ACPI_CONFIG_PROTOCOL_H__
#define __PLATFORM_ACPI_CONFIG_PROTOCOL_H__
#include <Uefi.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/AcpiTable.h>

#define PLATFORM_ACPI_CONFIG_PROTOCOL_GUID \
    {0x1B5EE0D9, 0x1145, 0x42B9, {0x93, 0x33, 0xF9, 0x8B, 0x3D, 0x60, 0xF7, 0x03}}
#define PLATFORM_ACPI_CONFIG_PROTOCOL_SIGNATURE  SIGNATURE_32 ('P', 'A', 'C', 'P')
#define PLATFORM_ACPI_CONFIG_PROTOCOL_REVISION   0x1

typedef struct _PLATFORM_ACPI_CONFIG_PROTOCOL PLATFORM_ACPI_CONFIG_PROTOCOL;

typedef
  EFI_STATUS
(EFIAPI *GET_ACPI_TABLE_BY_SIGNATURE)(
  IN   PLATFORM_ACPI_CONFIG_PROTOCOL  *This,
  IN      UINT32                        Signature,
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER   **Table,
  IN OUT  UINTN                         *Handle
  );

typedef
  EFI_STATUS
(EFIAPI *UPDATE_NAME_ASL_CODE)(
  IN   PLATFORM_ACPI_CONFIG_PROTOCOL  *This,
  IN     UINT32                        Signature,
  IN     VOID                          *Buffer,
  IN     UINTN                         Length
  );

struct _PLATFORM_ACPI_CONFIG_PROTOCOL {
  UINT32                         Signature;
  UINT32                         Revision;
  UINT32                         *pAcpiRamAddress;
  UINT32                         AcpiRamSize;
  EFI_ACPI_SDT_PROTOCOL          *pAcpiSdtProtocol;
  EFI_ACPI_TABLE_PROTOCOL        *pAcpiTableProtocol;
  GET_ACPI_TABLE_BY_SIGNATURE    GetAcpiTableBySignature;
  UPDATE_NAME_ASL_CODE           UpdateNameAslCode;
};

extern EFI_GUID  gPlatformAcpiConfigProtocolGuid;

#endif
