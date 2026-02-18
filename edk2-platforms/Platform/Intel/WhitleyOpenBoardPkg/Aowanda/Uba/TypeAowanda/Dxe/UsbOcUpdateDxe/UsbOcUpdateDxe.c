/** @file

  @copyright
  Copyright 2018 - 2021 Intel Corporation. <BR>
  Copyright (c) 2021 - 2022, American Megatrends International LLC. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "UsbOcUpdateDxe.h"

#include <Library/UbaUsbOcUpdateLib.h>
#include <PchLimits.h>
#include <ConfigBlock/UsbConfig.h>
#include <ConfigBlock/Usb2PhyConfig.h>

USB_OVERCURRENT_PIN  TypeAowandaUsb20OverCurrentMappings[PCH_MAX_USB2_PORTS] = {
  UsbOverCurrentPin0,                              // Port01: USB 2.0 CONNECTOR
  UsbOverCurrentPinSkip,                           // Port02: NC
  UsbOverCurrentPinSkip,                           // Port03: NC
  UsbOverCurrentPinSkip,                           // Port04: NC
  UsbOverCurrentPinSkip,                           // Port05: NC
  UsbOverCurrentPinSkip,                           // Port06: NC
  UsbOverCurrentPinSkip,                           // Port07: TO BMC
  UsbOverCurrentPinSkip,                           // Port08: NC
  UsbOverCurrentPinSkip,                           // Port09: NC
  UsbOverCurrentPinSkip,                           // Port10: OCP3.0 SLOT
  UsbOverCurrentPinSkip,                           // Port11: NC
  UsbOverCurrentPinSkip,                           // Port12: NC
  UsbOverCurrentPinSkip,                           // Port13: NC
  UsbOverCurrentPinSkip,                           // Port14: NC
  UsbOverCurrentPinSkip,                           // Port15: NC
  UsbOverCurrentPinSkip                            // Port16: NC
};

USB_OVERCURRENT_PIN  TypeAowandaUsb30OverCurrentMappings[PCH_MAX_USB3_PORTS] = {
  UsbOverCurrentPinSkip,                            // Port01: NC
  UsbOverCurrentPinSkip,                            // Port02: NC
  UsbOverCurrentPinSkip,                            // Port03: NC
  UsbOverCurrentPinSkip,                            // Port04: NC
  UsbOverCurrentPinSkip,                            // Port05: NC
  UsbOverCurrentPinSkip,                            // Port06: NC
  UsbOverCurrentPinSkip,                            // Port07: NC
  UsbOverCurrentPinSkip,                            // Port08: NC
  UsbOverCurrentPinSkip,                            // Port09: NC
  UsbOverCurrentPinSkip                             // Port10: NC
};

USB2_PHY_PARAMETERS  TypeAowandaUsb20AfeParams[PCH_H_XHCI_MAX_USB2_PHYSICAL_PORTS] = {
  { 3, 0, 3, 1 },                       // PP0
  { 5, 0, 3, 1 },                       // PP1
  { 3, 0, 3, 1 },                       // PP2
  { 0, 5, 1, 1 },                       // PP3
  { 3, 0, 3, 1 },                       // PP4
  { 3, 0, 3, 1 },                       // PP5
  { 3, 0, 3, 1 },                       // PP6
  { 3, 0, 3, 1 },                       // PP7
  { 2, 2, 1, 0 },                       // PP8
  { 6, 0, 2, 1 },                       // PP9
  { 2, 2, 1, 0 },                       // PP10
  { 6, 0, 2, 1 },                       // PP11
  { 0, 5, 1, 1 },                       // PP12
  { 7, 0, 2, 1 },                       // PP13
};

EFI_STATUS
TypeAowandaPlatformUsbOcUpdateCallback (
  IN OUT   USB_OVERCURRENT_PIN  **Usb20OverCurrentMappings,
  IN OUT   USB_OVERCURRENT_PIN  **Usb30OverCurrentMappings,
  IN OUT   USB2_PHY_PARAMETERS  **Usb20AfeParams
  )
{
  *Usb20OverCurrentMappings = &TypeAowandaUsb20OverCurrentMappings[0];
  *Usb30OverCurrentMappings = &TypeAowandaUsb30OverCurrentMappings[0];

  *Usb20AfeParams = TypeAowandaUsb20AfeParams;
  return EFI_SUCCESS;
}

PLATFORM_USBOC_UPDATE_TABLE  TypeAowandaUsbOcUpdate =
{
  PLATFORM_USBOC_UPDATE_SIGNATURE,
  PLATFORM_USBOC_UPDATE_VERSION,
  TypeAowandaPlatformUsbOcUpdateCallback
};

/**
  The Driver Entry Point.

  The function is the driver Entry point.

  @param ImageHandle   A handle for the image that is initializing this driver
  @param SystemTable   A pointer to the EFI system table

  @retval EFI_SUCCESS:              Driver initialized successfully
  @retval EFI_LOAD_ERROR:           Failed to Initialize or has been loaded
  @retval EFI_OUT_OF_RESOURCES      Could not allocate needed resources

**/
EFI_STATUS
EFIAPI
UsbOcUpdateEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                    Status;
  UBA_CONFIG_DATABASE_PROTOCOL  *UbaConfigProtocol = NULL;

  DEBUG ((DEBUG_INFO, "UBA:UsbOcUpdate-TypeAowanda\n"));
  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = UbaConfigProtocol->AddData (
                                     UbaConfigProtocol,
                                     &gDxePlatformUbaOcConfigDataGuid,
                                     &TypeAowandaUsbOcUpdate,
                                     sizeof(TypeAowandaUsbOcUpdate)
                                     );

  return Status;
}
