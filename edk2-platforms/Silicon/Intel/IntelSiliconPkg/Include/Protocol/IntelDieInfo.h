/** @file
  IntelDieInfo definition

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _DIE_INFO_PROTOCOL_H_
#define _DIE_INFO_PROTOCOL_H_

typedef struct _EDKII_INTEL_DIE_INFO_PROTOCOL  EDKII_INTEL_DIE_INFO_PROTOCOL;
typedef EDKII_INTEL_DIE_INFO_PROTOCOL  EDKII_INTEL_DIE_INFO_PPI;

extern EFI_GUID gIntelDieInfoProtocolGuid;
extern EFI_GUID gIntelDieInfoPpiGuid;

extern EFI_GUID gIntelDieInfoPchGuid;
extern EFI_GUID gIntelDieInfoSocGuid;
extern EFI_GUID gIntelDieInfoIoGuid;
extern EFI_GUID gIntelDieInfoCpuGuid;
extern EFI_GUID gIntelDieInfoGfxGuid;

#define DIE_INFO_PROTOCOL_REVISION 1

/**
  Returns pointer to constant string representing die name.
  Name is specific to die type.

  @param[in] This  Pointer to the DieInfoProtocol context structure
  @retval Pointer to the const string
**/
typedef
CONST CHAR8*
(EFIAPI *EDKII_INTEL_DIE_INFO_GET_DIE_NAME_STR) (
  IN EDKII_INTEL_DIE_INFO_PROTOCOL  *This
  );

/**
  Returns pointer to constant string representing stepping of the die.

  @param[in] This  Pointer to the DieInfoProtocol context structure
  @retval Pointer to the const string
**/
typedef
CONST CHAR8*
(EFIAPI *EDKII_INTEL_DIE_INFO_GET_STEPPING_STR) (
  IN EDKII_INTEL_DIE_INFO_PROTOCOL  *This
  );

/**
  Returns pointer to constant string representing SKU of the die.

  @param[in] This  Pointer to the DieInfoProtocol context structure
  @retval Pointer to the const string
**/
typedef
CONST CHAR8*
(EFIAPI *EDKII_INTEL_DIE_INFO_GET_SKU_STR) (
  IN EDKII_INTEL_DIE_INFO_PROTOCOL  *This
  );

/**
  Protocol/PPI definition.
  The purpose of this interface is to serve die-specific informations in a unified, generic way.
  It will be produced by silicon code per die, and can be consumed by any module that needs contained information.

  <b>Revision 1</b>:
   - Initial version.
**/
struct _EDKII_INTEL_DIE_INFO_PROTOCOL {
  UINT32                                 Revision; ///< Current protocol revision
  /**
    Type of the die that particular instance is reffering to.
  **/
  EFI_GUID                               Type;
  /**
    Index of the die in the package.
  **/
  UINT32                                 DieIndex;
  /**
    Unique ID specific to the die and the associated generation.
  **/
  UINT64                                 DieId;
  /**
    Generation and die specific stepping ID.
  **/
  UINT32                                 SteppingId;

  EDKII_INTEL_DIE_INFO_GET_DIE_NAME_STR  GetNameStr;
  EDKII_INTEL_DIE_INFO_GET_STEPPING_STR  GetSteppingStr;
  EDKII_INTEL_DIE_INFO_GET_SKU_STR       GetSkuStr;
};

#endif // _DIE_INFO_PROTOCOL_H_
