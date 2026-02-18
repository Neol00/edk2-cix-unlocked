/** @file

  @copyright
  Copyright 2013 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include <Library/UbaOpromUpdateLib.h>

#include <Protocol/UbaCfgDb.h>


BOOLEAN
PlatformCheckPcieRootPort (
  IN  UINTN                 Bus,
  IN  UINT32                PcieSlotOpromBitMap
  )
{
  EFI_STATUS                              Status;

  UBA_CONFIG_DATABASE_PROTOCOL            *UbaConfigProtocol = NULL;
  UINTN                                   DataLength = 0;
  PLATFORM_OPTION_ROM_UPDATE_DATA         OptionRomUpdateTable;

  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &UbaConfigProtocol
                  );
  if (EFI_ERROR (Status)) {
    return TRUE;
  }

  DataLength  = sizeof (OptionRomUpdateTable);
  Status = UbaConfigProtocol->GetData (
                                    UbaConfigProtocol,
                                    &gPlatformOptionRomUpdateConfigDataGuid,
                                    &OptionRomUpdateTable,
                                    &DataLength
                                    );
  if (EFI_ERROR (Status)) {
    return TRUE;
  }

  ASSERT (OptionRomUpdateTable.Signature == PLATFORM_OPTION_ROM_UPDATE_SIGNATURE);
  ASSERT (OptionRomUpdateTable.Version == PLATFORM_OPTION_ROM_UPDATE_VERSION);

  return OptionRomUpdateTable.CallCheckRootPort (Bus, PcieSlotOpromBitMap);
}

EFI_STATUS
PlatformGetOptionRomTable (
  IN  PC_PCI_OPTION_ROM_TABLE         **OptionRomTable
  )
{
  EFI_STATUS                              Status;

  UBA_CONFIG_DATABASE_PROTOCOL            *UbaConfigProtocol = NULL;
  UINTN                                   DataLength = 0;
  PLATFORM_OPTION_ROM_UPDATE_DATA         OptionRomUpdateTable;

  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &UbaConfigProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DataLength  = sizeof (OptionRomUpdateTable);
  Status = UbaConfigProtocol->GetData (
                                    UbaConfigProtocol,
                                    &gPlatformOptionRomUpdateConfigDataGuid,
                                    &OptionRomUpdateTable,
                                    &DataLength
                                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (OptionRomUpdateTable.Signature == PLATFORM_OPTION_ROM_UPDATE_SIGNATURE);
  ASSERT (OptionRomUpdateTable.Version == PLATFORM_OPTION_ROM_UPDATE_VERSION);

  if (OptionRomUpdateTable.GetOptionRomTable == NULL) {
    return EFI_NOT_FOUND;
  }

  return OptionRomUpdateTable.GetOptionRomTable (OptionRomTable);
}

EFI_STATUS
PlatformGetNicSetupConfigTable (
  IN  NIC_SETUP_CONFIGURATION_STUCT     **NicSetupConfigTable,
  IN  UINTN                             *NumOfConfig
  )
{
  EFI_STATUS                              Status;

  UBA_CONFIG_DATABASE_PROTOCOL            *UbaConfigProtocol = NULL;
  UINTN                                   DataLength = 0;
  PLATFORM_OPTION_ROM_UPDATE_DATA         OptionRomUpdateTable;

  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &UbaConfigProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DataLength  = sizeof (OptionRomUpdateTable);
  Status = UbaConfigProtocol->GetData (
                                    UbaConfigProtocol,
                                    &gPlatformOptionRomUpdateConfigDataGuid,
                                    &OptionRomUpdateTable,
                                    &DataLength
                                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (OptionRomUpdateTable.Signature == PLATFORM_OPTION_ROM_UPDATE_SIGNATURE);
  ASSERT (OptionRomUpdateTable.Version == PLATFORM_OPTION_ROM_UPDATE_VERSION);

  if (OptionRomUpdateTable.GetNicSetupConfigTable == NULL) {
    return EFI_NOT_FOUND;
  }

  return OptionRomUpdateTable.GetNicSetupConfigTable (NicSetupConfigTable, NumOfConfig);
}

EFI_STATUS
PlatformGetNicCapabilityTable (
  IN  NIC_OPTIONROM_CAPBILITY_STRUCT    **NicCapabilityTable,
  IN  UINTN                             *NumOfNicCapTable
  )
{
  EFI_STATUS                              Status;

  UBA_CONFIG_DATABASE_PROTOCOL            *UbaConfigProtocol = NULL;
  UINTN                                   DataLength = 0;
  PLATFORM_OPTION_ROM_UPDATE_DATA         OptionRomUpdateTable;

  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &UbaConfigProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DataLength  = sizeof (OptionRomUpdateTable);
  Status = UbaConfigProtocol->GetData (
                                    UbaConfigProtocol,
                                    &gPlatformOptionRomUpdateConfigDataGuid,
                                    &OptionRomUpdateTable,
                                    &DataLength
                                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (OptionRomUpdateTable.Signature == PLATFORM_OPTION_ROM_UPDATE_SIGNATURE);
  ASSERT (OptionRomUpdateTable.Version == PLATFORM_OPTION_ROM_UPDATE_VERSION);

  if (OptionRomUpdateTable.GetNicCapabilityTable == NULL) {
    return EFI_NOT_FOUND;
  }

  return OptionRomUpdateTable.GetNicCapabilityTable (NicCapabilityTable, NumOfNicCapTable);
}

EFI_STATUS
PlatformSetupPcieSlotNumber (
  OUT  UINT8                   *PcieSlotItemCtrl
  )
{
  EFI_STATUS                              Status;

  UBA_CONFIG_DATABASE_PROTOCOL            *UbaConfigProtocol = NULL;
  UINTN                                   DataLength = 0;
  PLATFORM_OPTION_ROM_UPDATE_DATA         OptionRomUpdateTable;

  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &UbaConfigProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DataLength  = sizeof (OptionRomUpdateTable);
  Status = UbaConfigProtocol->GetData (
                                    UbaConfigProtocol,
                                    &gPlatformOptionRomUpdateConfigDataGuid,
                                    &OptionRomUpdateTable,
                                    &DataLength
                                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (OptionRomUpdateTable.Signature == PLATFORM_OPTION_ROM_UPDATE_SIGNATURE);
  ASSERT (OptionRomUpdateTable.Version == PLATFORM_OPTION_ROM_UPDATE_VERSION);

  return OptionRomUpdateTable.SetupSlotNumber (PcieSlotItemCtrl);
}
