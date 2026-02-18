/** @file
  UBA FPK configuration library header

  @copyright
  Copyright 2016 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _UBA_FPK_CONFIG_LIB_H
#define _UBA_FPK_CONFIG_LIB_H

#include <Base.h>
#include <Uefi.h>
#include <Library/GpioLib.h>

#define PLATFORM_FPK_CONFIG_SIGNATURE  SIGNATURE_32 ('P', 'F', 'P', 'K')
#define PLATFORM_FPK_CONFIG_VERSION    01

// {38F0930C-E7FB-49CC-B88E-D9909EB65D77}
#define PLATFORM_FPK_CONFIG_DATA_GUID \
{ 0x38f0930c, 0xe7fb, 0x49cc, { 0xb8, 0x8e, 0xd9, 0x90, 0x9e, 0xb6, 0x5d, 0x77 } }

typedef enum {
  PortMappedToFunc0,
  PortMappedToFunc1,
  PortMappedToFunc2,
  PortMappedToFunc3,
  PortNotMapped
} PLATFORM_FPK_PORT_MAP;

typedef struct _PLATFORM_FPK_CONFIG_STRUCT {
  UINT32                            Signature;
  UINT32                            Version;
  PLATFORM_FPK_PORT_MAP             *PortToFuncMapPtr;
  UINTN                             PortToFuncMapSize;
  GPIO_PAD                          PciDisNGpioPad;
  GPIO_PAD                          LanDisNGpioPad;
} PLATFORM_FPK_CONFIG_STRUCT;

/**
  Retrieves FPK config struct from UBA database

  @retval EFI_SUCCESS           Config struct is retrieved.
  @retval EFI_NOT_FOUND         UBA protocol, platform or data not found.
  @retval EFI_INVALID_PARAMETER If PlatformFpkConfigStruct is NULL.
**/
EFI_STATUS
FpkConfigGetConfigStruct (
  OUT PLATFORM_FPK_CONFIG_STRUCT *PlatformFpkConfigStruct
  );

STATIC EFI_GUID gPlatformFpkConfigDataGuid = PLATFORM_FPK_CONFIG_DATA_GUID;

#endif // !_UBA_FPK_CONFIG_LIB_H
