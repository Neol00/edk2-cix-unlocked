/** @file
  PCH Detection for the HDMI I2C Debug Port

Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/PciLib.h>
#include <Library/HdmiDebugPchDetectionLib.h>


//
// PCH Detection Registers
//
#define PCH_PCI_BUS                                       0
#define PCH_LPC_PCI_DEV                                   31
#define PCH_LPC_PCI_FUN                                   0
#define R_PCH_LPC_DID                                     0x02

#define V_SPT_LP_PCH_START_DEVICE_ID                      0x9D40
#define V_SPT_LP_PCH_END_DEVICE_ID                        0x9D5F
#define V_SPT_H_PCH_START_DEVICE_ID                       0xA140
#define V_SPT_H_PCH_END_DEVICE_ID                         0xA15F
#define V_KBP_H_PCH_START_DEVICE_ID                       0xA2C0
#define V_KBP_H_PCH_END_DEVICE_ID                         0xA2DF
#define V_CNL_LP_PCH_START_DEVICE_ID                      0x9D80
#define V_CNL_LP_PCH_END_DEVICE_ID                        0x9D9F
#define V_CNL_H_PCH_START_DEVICE_ID                       0xA300
#define V_CNL_H_PCH_END_DEVICE_ID                         0xA31F

#define V_KBP_H_PCH_DEVICE_ID_ES                          0xA2C0        ///< This is SKL-PCH-H in KBL-PCH-H package
#define V_KBP_H_PCH_DEVICE_ID_SVR_ES                      0xA2D0        ///< This is SKL-PCH-H in KBL-PCH-H package

/**
  Returns the type of PCH on the system

  @retval   The PCH type.
**/
PCH_TYPE
GetPchTypeInternal (
  VOID
  )
{
  PCH_TYPE  PchType;
  UINT16    DeviceId;

  PchType   = PchTypeUnknown;
  DeviceId  = PciRead16 (PCI_LIB_ADDRESS (PCH_PCI_BUS, PCH_LPC_PCI_DEV, PCH_LPC_PCI_FUN, R_PCH_LPC_DID));
  if ((DeviceId >= V_SPT_LP_PCH_START_DEVICE_ID) && (DeviceId <= V_SPT_LP_PCH_END_DEVICE_ID)) {
    PchType = PchTypeSptLp;
  } else if ((DeviceId >= V_SPT_H_PCH_START_DEVICE_ID) && (DeviceId <= V_SPT_H_PCH_END_DEVICE_ID )) {
    PchType = PchTypeSptH;
  } else if ((DeviceId >= V_KBP_H_PCH_START_DEVICE_ID) && (DeviceId <= V_KBP_H_PCH_END_DEVICE_ID)) {
    PchType = PchTypeKbpH;
    if ((DeviceId == V_KBP_H_PCH_DEVICE_ID_ES) || (DeviceId == V_KBP_H_PCH_DEVICE_ID_SVR_ES)) {
      PchType = PchTypeSptH;
    }
  } else if ((DeviceId >= V_CNL_LP_PCH_START_DEVICE_ID) && (DeviceId <= V_CNL_LP_PCH_END_DEVICE_ID)) {
    PchType = PchTypeCnlLp;
  } else if ((DeviceId >= V_CNL_H_PCH_START_DEVICE_ID) && (DeviceId <= V_CNL_H_PCH_END_DEVICE_ID)) {
    PchType = PchTypeCnlH;
  }

  return PchType;
}
