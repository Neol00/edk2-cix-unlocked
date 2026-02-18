/** @file

  @copyright
  Copyright 2017 - 2018 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _ACPI_PLATFORM_UTILS_H_
#define _ACPI_PLATFORM_UTILS_H_

extern EFI_GUID gEfiAcpiTableStorageGuid;


/**
  Function checks if ACPI Platform Protocol is ready to install

  @param None

  @retval TRUE  ACPI Platform Protocol can be installed,
          FALSE ACPI Platform Protocol not ready yet

**/
BOOLEAN
IsAcpiPlatformProtocolReadyForInstall (
  VOID
  );

/**
  Function checks if ACPI Platform Protocol is already installed

  @param None

  @retval TRUE  ACPI Platform Protocol is installed,
          FALSE ACPI Platform Protocol not installed yet

**/
BOOLEAN
IsAcpiPlatformProtocolInstalled (
  VOID
  );

/**
  Locate the first instance of a protocol.  If the protocol requested is an
  FV protocol, then it will return the first FV that contains the ACPI table
  storage file.

  @param[in]  Protocol            - The protocol to find.
  @param[in]  EfiAcpiStorageGuid  - EFI ACPI tables storage guid
  @param[out] Instance            - Return pointer to the first instance of the protocol.
  @param[in]  Type                - The type of protocol to locate.

  @retval EFI_SUCCESS           -  The function completed successfully.
  @retval EFI_NOT_FOUND         -  The protocol could not be located.
  @retval EFI_OUT_OF_RESOURCES  -  There are not enough resources to find the protocol.

**/
EFI_STATUS
LocateSupportProtocol (
  IN   EFI_GUID       *Protocol,
  IN   EFI_GUID       EfiAcpiStorageGuid,
  OUT  VOID           **Instance,
  IN   UINT32         Type
  );

#endif // _ACPI_PLATFORM_UTILS_H_
