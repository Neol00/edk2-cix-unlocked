/** @file
  This file publishes protocol that provides additional information
  for memory related SMBIOS entries.
  NOTE: Currently only Type17 entries are used, no need for others identified.
  Only Type17 entries that represent existing DIMMs are listed.

  @copyright
  Copyright 2017 - 2018 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _PROTOCOL_SMBIOSMEMINFO_H_
#define _PROTOCOL_SMBIOSMEMINFO_H_

#define SMBIOS_MEMINFO_PROT_VERSION 1

//
// Invalid SMBIOS handle to use if Type17 entry does not exist.
//
#define SMBIOS_INVALID_HANDLE       0xFFFF

typedef struct _SMBIOS_MEM_INFO_PROTOCOL_ SMBIOS_MEM_INFO_PROTOCOL;

/**
 * Localization information for SMBIOS Type17 entries that represent DIMMs.
 */
typedef struct _SMBIOS_DIMM_INFO_ {
  INT8              Socket;       ///< Socket index (0 based)
  INT8              Imc;          ///< Intergrated memory controller in the above socket
  INT8              Channel;      ///< Channel in the above IMC
  INT8              Dimm;         ///< DIMM slot index in the above channel
  EFI_SMBIOS_HANDLE Type17Handle; ///< Type17 handle in SMBIOS table
  BOOLEAN           IsNvDimm;     ///< True if it is Non Volatile DIMM
} SMBIOS_DIMM_INFO;


/**
  This function finds SMBIOS Type17 entry for given SMBIOS handle.

  On input Info->Handle must be set.
  On output, unless error was returned, the rest of the structure is filled.

  @param[in]     This  - Pointer to the protocol
  @param[in,out] Info  - Pointer to DIMM info structure

  @return Standard status codes are returned.
**/
typedef EFI_STATUS (EFIAPI *SMBIOS_GET_DIMM_BY_HANDLE) (
  IN     SMBIOS_MEM_INFO_PROTOCOL *This,
  IN OUT SMBIOS_DIMM_INFO         *Info
  );

/**
  This function finds SMBIOS Type17 entry for given DIMM location.

  On input Info->Socket, Info->Imc, Info->Channel, Info->Dimm must be set.
  On output, unless error was returned, the rest of the structure is filled.

  @param[in]     This  - Pointer to the protocol
  @param[in,out] Info  - Pointer to DIMM info structure.

  @return Standard status codes are returned.
**/
typedef EFI_STATUS (EFIAPI *SMBIOS_GET_DIMM_BY_LOCATION) (
  IN     SMBIOS_MEM_INFO_PROTOCOL *This,
  IN OUT SMBIOS_DIMM_INFO         *Info
  );

/**
 * This protocol provides information about memory related SMBIOS entries.
 *
 * NOTE: Currently only Type17 entries are used, no need for others.
 *       Only Type17 entries that represent existing DIMMs are listed.
 */
typedef struct _SMBIOS_MEM_INFO_PROTOCOL_ {
  UINT16                      SmbiosMemInfoProtVersion;
  UINT8                       Reserved[2];
  UINT32                      SmbiosDimmNum; ///< Number of DIMM (Type17) handles in SMBIOS

  SMBIOS_GET_DIMM_BY_HANDLE   SmbiosGetDimmByHandle;
  SMBIOS_GET_DIMM_BY_LOCATION SmbiosGetDimmByLocation;
} SMBIOS_MEM_INFO_PROTOCOL;

extern EFI_GUID gSmbiosMemInfoProtocolGuid;

#endif // _PROTOCOL_SMBIOSMEMINFO_H_
