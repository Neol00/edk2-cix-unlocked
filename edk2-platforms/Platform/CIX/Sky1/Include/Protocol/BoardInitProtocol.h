#/** @file
#
#  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
#
#**/

#ifndef _BOARD_INIT_PROTOCOL_H_
#define _BOARD_INIT_PROTOCOL_H_

typedef enum {
  VPU,
  GMAC,
  USB,
  PCIE,
  NPU,
  DPU,
  GPU
} COMPONENT;

typedef
  EFI_STATUS
(EFIAPI *GET_COMPONENT_SUPPORT)(
                                IN  COMPONENT  Type,
                                OUT UINT32    *Data
                                );

typedef struct {
  CHAR16                   ProductName[16];
  GET_COMPONENT_SUPPORT    GetComponentSupport;
} BOARD_INIT_PROTOCOL;

extern EFI_GUID  gCixBoardInitProtocolGuid;
#endif
