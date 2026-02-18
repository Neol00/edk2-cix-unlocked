/**

  @file I2s.h
  @author Black.Li (black.li@cixtech.com)


  *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved. *
  *
  *  SPDX-License-Identifier: BSD-2-Clause-Patent
  *
**/

#ifndef __I2S_MASTER_H__
#define __I2S_MASTER_H__

#include <Uefi.h>
// f845b0dc-3ab3-11ef-87d4-c3f6ce87ef83
#define EFI_I2S_MASTER_PROTOCOL_GUID  { 0xf845b0dc, 0x3ab3, 0x11ef, { 0x87, 0xd4, 0xc3, 0xf6, 0xce, 0x87, 0xef, 0x83 }}

typedef struct _EFI_I2S_MASTER_PROTOCOL EFI_I2S_MASTER_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_I2S_MASTER_CONFIG)(
  IN CONST EFI_I2S_MASTER_PROTOCOL   *This
  );

typedef
EFI_STATUS
(EFIAPI *EFI_I2S_MASTER_PROTOCOL_RESET)(
  IN CONST EFI_I2S_MASTER_PROTOCOL   *This
  );

typedef
EFI_STATUS
(EFIAPI *EFI_I2S_MASTER_INIT)(
  IN CONST EFI_I2S_MASTER_PROTOCOL   *This
  );

typedef
EFI_STATUS
(EFIAPI *EFI_I2S_MASTER_PROTOCOL_TRANSMIT)(
  IN CONST EFI_I2S_MASTER_PROTOCOL *This,
  IN UINTN                         Channel,
  IN VOID                          *Data,
  IN UINTN                         Size
  );

typedef
EFI_STATUS
(EFIAPI *EFI_I2S_MASTER_PROTOCOL_RECEIVE)(
  IN CONST EFI_I2S_MASTER_PROTOCOL *This,
  IN UINTN                         Channel,
  IN VOID                          *Data,
  IN UINTN                         ReceiveTimeInMicroSeconds
);

struct _EFI_I2S_MASTER_PROTOCOL {
  ///
  /// Set the clock frequency for the I2S bus.
  ///
  EFI_I2S_MASTER_INIT                 Init;
  EFI_I2S_MASTER_CONFIG               Config;
  ///
  /// Reset the I2S host controller.
  ///
  EFI_I2S_MASTER_PROTOCOL_RESET       Reset;
  /// Transmit Data To DataOut
  EFI_I2S_MASTER_PROTOCOL_TRANSMIT    Transmit;
  /// Receive Data From DataIn
  EFI_I2S_MASTER_PROTOCOL_RECEIVE     Receive;
  UINTN                               BaseAddress;
  VOID*                               ConfigData;
};

extern EFI_GUID  gEfiI2sMasterProtocolGuid;

#endif //  __I2S_MASTER_H__
