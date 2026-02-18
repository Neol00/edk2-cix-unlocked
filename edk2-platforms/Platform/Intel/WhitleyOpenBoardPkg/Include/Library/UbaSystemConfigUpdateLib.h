/** @file
  UBA System Config Update Library Header File.

  @copyright
  Copyright 2017 - 2021 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _UBA_SYSTEM_CONFIG_UPDATE_LIB_H_
#define _UBA_SYSTEM_CONFIG_UPDATE_LIB_H_

#include <Base.h>
#include <Uefi.h>

#include <Guid/SetupVariable.h>

#define SYSTEM_CONFIG_UPDATE_SIGNATURE  SIGNATURE_32 ('S', 'C', 'O', 'N')
#define SYSTEM_CONFIG_UPDATE_VERSION    01

typedef
VOID
(*IIO_DEFAULT_CONFIG_UPDATE_CALLBACK) (
  IN  SYSTEM_CONFIGURATION       *Default
  );

typedef struct
{
  UINT32                                    Signature;
  UINT32                                    Version;
  IIO_DEFAULT_CONFIG_UPDATE_CALLBACK        CallUpdateIioConfig;
} SYSTEM_CONFIG_UPDATE_DATA;

EFI_STATUS
UpdateIioDefaultConfig (
  IN  SYSTEM_CONFIGURATION       *Default
  );

#endif  //_UBA_SYSTEM_CONFIG_UPDATE_LIB_H_
