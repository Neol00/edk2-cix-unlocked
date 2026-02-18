/** @file
  Asf protocol define.

  Copyright (c) 1985 - 2022, AMI. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef __ASF_PROTOCOL_H__
#define __ASF_PROTOCOL_H__

#include <IndustryStandard/SmBus.h>

#define ASF_PROTOCOL_GUID \
  { \
    0xdec89827, 0x8a7e, 0x45e0, { 0xbc, 0xb5, 0xd5, 0x3b, 0x46, 0x14, 0xad, 0xb8 } \
  }

#pragma pack(push, 1)

/**
  This is the ASF Boot Option data structure.

**/
typedef struct {
  UINT8     SubCommand;
  UINT8     Version;
  UINT32    IanaId;
  UINT8     SpecialCommand;
  UINT16    SpecCmdParameter;
  UINT16    BootOptionBit;
  UINT16    OemParameter;
} ASF_BOOT_OPTION;

/**
  This is the ASF PUSH EVENT Structure.

**/
typedef EFI_STATUS (EFIAPI *ASF_PUSH_EVENT)(
                                            IN UINT8 Command,
                                            IN UINTN Length,
                                            IN UINT8 *ASFEvent
                                            );

/**
  This is the AMI ASF Protocol Structure.

**/
typedef struct {
  ASF_PUSH_EVENT     PushEvent;
  ASF_BOOT_OPTION    *BootOption;
} ASF_PROTOCOL;

#pragma pack(pop)

extern EFI_GUID  gAsfProtocolGuid;

#endif //__ASF_PROTOCOL_H__
