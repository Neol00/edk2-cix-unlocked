/** @file

  @copyright
  Copyright 1999 - 2020 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _ACPI_PLATFORM_H_
#define _ACPI_PLATFORM_H_

//
// Statements that include other header files
//
#include <PiDxe.h>
#include <PchAccess.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/SerialPortConsoleRedirectionTable.h>
#include <IndustryStandard/DmaRemappingReportingTable.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>
#include <IndustryStandard/Pci.h>
#include <Library/AcpiPlatformLib.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/IioUds.h>
#include <Protocol/DmaRemap.h>
#include <Protocol/PciIo.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/SerialIo.h>
  #include <Protocol/MpService.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/GlobalNvsArea.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Guid/PlatformInfo.h>
#include <Guid/SetupVariable.h>
#include <PchSetupVariable.h>
#include <Guid/SocketVariable.h>
#include <Guid/HobList.h>
#include <Guid/MemoryMapData.h>
#include <Protocol/PlatformType.h>
#include <Protocol/CpuCsrAccess.h>
#include <PpmPolicyPeiDxeCommon.h>
#include <Acpi/Mcfg.h>
#include <Acpi/Hpet.h>
#include <Acpi/Srat.h>
#include <Acpi/Slit.h>
#include <Acpi/Migt.h>
#include <Acpi/Msct.h>
#include <Acpi/Bdat.h>
#include <Acpi/Nfit.h>
#include <Acpi/Pcat.h>
#include "Platform.h"
#include <AcpiVtd.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>
#include <Library/PchInfoLib.h>
#include <Library/ReportStatusCodeLib.h>

#include <SystemBoard.h>
#include <PchAccess.h>
#include <UncoreCommonIncludes.h>

#include <SystemInfoVar.h>
#include <Register/Cpuid.h>
#include <Library/PlatformStatusCodes.h>
#include <Protocol/DynamicSiLibraryProtocol.h>
#include <Protocol/DynamicSiLibraryProtocol2.h>
#include <Protocol/DmaRemap.h>

#define RTC_ADDRESS_REGISTER_D                      13


/**
  Entry point for Acpi platform driver.

  @param ImageHandle  -  A handle for the image that is initializing this driver.
  @param SystemTable  -  A pointer to the EFI system table.

  @retval EFI_SUCCESS           -  Driver initialized successfully.
  @retval EFI_LOAD_ERROR        -  Failed to Initialize or has been loaded.
  @retval EFI_OUT_OF_RESOURCES  -  Could not allocate needed resources.
**/
EFI_STATUS
EFIAPI
AcpiPlatformEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );

EFI_STATUS
LocateSupportProtocol (
  IN   EFI_GUID       *Protocol,
  IN   EFI_GUID       gEfiAcpiMultiTableStorageGuid,
  OUT  VOID           **Instance,
  IN   UINT32         Type
  );

VOID
AcpiVtdIntRemappingEnable (
  VOID
  );

EFI_STATUS
AcpiVtdTablesInstall (
  VOID
  );

#endif // _ACPI_PLATFORM_H_
