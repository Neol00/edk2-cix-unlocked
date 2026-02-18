/** @file

  @copyright
  Copyright 2012 - 2019 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/UbaCfgDb.h>
#include <Library/UbaUsbOcUpdateLib.h>

EFI_STATUS
PlatformGetUsbOcMappings (
  IN OUT   USB_OVERCURRENT_PIN   **Usb20OverCurrentMappings,
  IN OUT   USB_OVERCURRENT_PIN   **Usb30OverCurrentMappings,
  IN OUT   USB2_PHY_PARAMETERS   **Usb20AfeParams
  )
{
  EFI_STATUS                            Status;
  UBA_CONFIG_DATABASE_PROTOCOL          *UbaConfigProtocol = NULL;
  PLATFORM_USBOC_UPDATE_TABLE           UsbOcUpdateTable;
  UINTN                                 TableSize;

  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &UbaConfigProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  TableSize = sizeof(UsbOcUpdateTable);
  Status = UbaConfigProtocol->GetData (
                                UbaConfigProtocol,
                                &gDxePlatformUbaOcConfigDataGuid,
                                &UsbOcUpdateTable,
                                &TableSize
                                );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (UsbOcUpdateTable.Signature == PLATFORM_USBOC_UPDATE_SIGNATURE);
  ASSERT (UsbOcUpdateTable.Version == PLATFORM_USBOC_UPDATE_VERSION);

  UsbOcUpdateTable.CallUsbOcUpdate ( Usb20OverCurrentMappings,
                                     Usb30OverCurrentMappings,
                                     Usb20AfeParams
                                   );

  return Status;
}
