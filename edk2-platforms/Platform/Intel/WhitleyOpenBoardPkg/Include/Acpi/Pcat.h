/** @file
  Pcat.h

  @copyright
  Copyright 2014 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _PCAT_DEFS_H_
#define _PCAT_DEFS_H_

#include <Uefi/UefiBaseType.h>

/// @brief PCAT Signature to put in table header
#define NVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE_SIGNATURE  SIGNATURE_32('P', 'C', 'A', 'T')

#define NVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE_REVISION   0x11

#define MAX_PCAT_SIZE                                     0x200

typedef struct {
  UINT32                      Signature;      // 'PCAT' should be the signature for this table
  UINT32                      Length;         // Length in bytes for the entire table
  UINT8                       Revision;       // Revision # of this table, initial is '1'
  UINT8                       Checksum;       // Entire Table Checksum must sum to 0
  UINT8                       OemID[6];       // OemID
  UINT8                       OemTblID[8];    // Should be Manufacturer's Model #
  UINT32                      OemRevision;    // Oem Revision of for Supplied OEM Table ID
  UINT32                      CreatorID;      // Vendor ID of the utility that is creating this table
  UINT32                      CreatorRev;     // Revision of utility that is creating this table
  UINT32                      Reserved;       // Alignement for size modulo 8 = 0
  UINT8                       PCATTables[MAX_PCAT_SIZE];
} NVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE, *PNVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE;

/// @brief Layout of SSKU Attribute Extension Table header
typedef struct {
  UINT16       Type;                          // Type for Mgmt Info Struct, type should be 3
  UINT16       Length;                        // Length in Bytes for Entire SSKU Attribute Extension Table size.
  UINT16       Rsvd;
  UINT16       VendorID;                      // Vendor ID
  EFI_GUID     Guid;                          // Intel defined SSKU Attribute Extension Table. guid: F93032E5-B045-40ef-91C8-F02B06AD948D
} NVDIMM_SSKU_ATTR_EXT_TABLE, *PNVDIMM_SSKU_ATTR_EXT_TABLE;

#endif //_PCAT_DEFS_H_
