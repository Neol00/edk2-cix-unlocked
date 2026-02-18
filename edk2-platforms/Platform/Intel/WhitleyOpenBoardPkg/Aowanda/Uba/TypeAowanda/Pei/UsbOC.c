/** @file

  @copyright
  Copyright 2018 - 2021 Intel Corporation. <BR>
  Copyright (c) 2021 - 2022, American Megatrends International LLC. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "PeiBoardInit.h"

#include <Library/PcdLib.h>
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

EFI_STATUS
TypeAowandaPlatformUpdateUsbOcMappings (
  IN UBA_CONFIG_DATABASE_PPI  *UbaConfigPpi
  )
{
  // #
  // # USB, see PG 104 in GZP SCH
  // #

  //  USB2      USB3      Port                            OC
  //
  // Port00:     PORT5     Back Panel                      ,OC0#
  // Port01:     PORT2     Back Panel                      ,OC0#
  // Port02:     PORT3     Back Panel                      ,OC1#
  // Port03:     PORT0     NOT USED                        ,NA
  // Port04:               BMC1.0                          ,NA
  // Port05:               INTERNAL_2X5_A                  ,OC2#
  // Port06:               INTERNAL_2X5_A                  ,OC2#
  // Port07:               NOT USED                        ,NA
  // Port08:               EUSB (AKA SSD)                  ,NA
  // Port09:               INTERNAL_TYPEA                  ,OC6#
  // Port10:     PORT1     Front Panel                     ,OC5#
  // Port11:               NOT USED                        ,NA
  // Port12:               BMC2.0                          ,NA
  // Port13:     PORT4     Front Panel                     ,OC5#

  EFI_STATUS  Status;

  Status = UbaConfigPpi->AddData (
                                  UbaConfigPpi,
                                  &gPeiPlatformUbaOcConfigDataGuid,
                                  &TypeAowandaUsbOcUpdate,
                                  sizeof (TypeAowandaUsbOcUpdate)
                                  );

  return Status;
}
