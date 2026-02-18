/** @file
  This file contains definitions of PCH Info HOB.

@copyright
  Copyright 2018 Intel Corporation.

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _PCH_INFO_HOB_H_
#define _PCH_INFO_HOB_H_

#include <Library/PchPcieRpLib.h>

extern EFI_GUID gPchInfoHobGuid;

#define PCH_INFO_HOB_REVISION  2

#pragma pack (push,1)
/**
  This structure is used to provide the information of PCH controller.

  <b>Revision 1</b>:
  - Initial version.
  <b>Revision 2</b>:
  - Add CridSupport, CridOrgRid, and CridNewRid.
**/
typedef struct {
  /**
    This member specifies the revision of the PCH Info HOB. This field is used
    to indicate backwards compatible changes to the protocol. Platform code that
    consumes this protocol must read the correct revision value to correctly interpret
    the content of the protocol fields.
  **/
  UINT8        Revision;

  /**
    Publish Hpet BDF and IoApic BDF information for VTD.
  **/
  UINT32       HpetBusNum    :  8;
  UINT32       HpetDevNum    :  5;
  UINT32       HpetFuncNum   :  3;
  UINT32       IoApicBusNum  :  8;
  UINT32       IoApicDevNum  :  5;
  UINT32       IoApicFuncNum :  3;
} PCH_INFO_HOB;

#pragma pack (pop)

#endif // _PCH_INFO_HOB_H_
