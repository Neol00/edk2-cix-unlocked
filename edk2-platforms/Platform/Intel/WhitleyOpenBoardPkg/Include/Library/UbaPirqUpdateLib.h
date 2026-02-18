/** @file
  UBA PirqData Update Library Header File.

  @copyright
  Copyright 2008 - 2014 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _UBA_PIRQ_UPDATE_LIB_H
#define _UBA_PIRQ_UPDATE_LIB_H

#include <Base.h>
#include <Uefi.h>

#include <PlatPirqData.h>

#define PLATFORM_PIRQ_UPDATE_SIGNATURE  SIGNATURE_32 ('P', 'I', 'R', 'Q')
#define PLATFORM_PIRQ_UPDATE_VERSION    0x01


// {4C1F48A5-C976-4d90-9F03-8E9B1C327FCF}
#define   PLATFORM_PIRQ_CONFIG_DATA_GUID \
{ 0x4c1f48a5, 0xc976, 0x4d90, { 0x9f, 0x3, 0x8e, 0x9b, 0x1c, 0x32, 0x7f, 0xcf } }


typedef struct {
  UINT32                  Signature;
  UINT32                  Version;

  PLATFORM_PIRQ_DATA      *PirqDataPtr;

} PLATFORM_PIRQ_UPDATE_TABLE;

EFI_STATUS
PlatformGetPirqDataPointer (
  IN  PLATFORM_PIRQ_DATA                **PirqData
);

STATIC  EFI_GUID gPlatformPirqConfigDataGuid = PLATFORM_PIRQ_CONFIG_DATA_GUID;

#endif //_UBA_PIRQ_UPDATE_LIB_H
