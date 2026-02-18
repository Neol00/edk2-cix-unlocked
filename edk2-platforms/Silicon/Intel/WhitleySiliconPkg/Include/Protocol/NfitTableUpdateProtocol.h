/** @file

  @copyright
  Copyright 2018 - 2019 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _NFIT_TABLE_UPDATE_PROTOCOL_H_
#define _NFIT_TABLE_UPDATE_PROTOCOL_H_

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gEfiNfitTableUpdateProtocolGuid;

typedef
EFI_STATUS
(EFIAPI *NFIT_TABLE_UPDATE) (
  UINT64 *NfitTablePointer
  );

typedef struct _EFI_NFIT_TABLE_UPDATE_PROTOCOL {
  NFIT_TABLE_UPDATE   UpdateAcpiTable;
} EFI_NFIT_TABLE_UPDATE_PROTOCOL;

#endif // _NFIT_TABLE_UPDATE_PROTOCOL_H_
