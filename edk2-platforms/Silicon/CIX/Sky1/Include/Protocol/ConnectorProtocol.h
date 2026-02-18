/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved. *
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef _CIX_CONNECTOR_PROTOCOL_H_
#define _CIX_CONNECTOR_PROTOCOL_H_

#include <Library/DisplayLib.h>

#define Cix_CONNECTOR_PROTOCOL_GUID   \
    {0x50439CB6, 0x9B85, 0x11EC, {0x95, 0x73, 0xF4, 0x2A, 0x7D, 0xCB, 0x92, 0x5D}}

typedef struct CIX_CONNECTOR_PROTOCOL CIX_CONNECTOR_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *CIX_CONNECTOR_PREINIT)(
  IN CIX_CONNECTOR_PROTOCOL      *This,
  IN OUT DISPLAY_STATE           *DisplayState
  );

typedef
EFI_STATUS
(EFIAPI *CIX_CONNECTOR_INIT)(
  IN CIX_CONNECTOR_PROTOCOL      *This,
  IN OUT DISPLAY_STATE           *DisplayState
  );

typedef
EFI_STATUS
(EFIAPI *CIX_CONNECTOR_DEINIT)(
  IN CIX_CONNECTOR_PROTOCOL      *This,
  IN OUT DISPLAY_STATE           *DisplayState
  );

typedef
EFI_STATUS
(EFIAPI *CIX_CONNECTOR_DETECT)(
  IN CIX_CONNECTOR_PROTOCOL      *This,
  IN OUT DISPLAY_STATE           *DisplayState
  );

typedef
EFI_STATUS
(EFIAPI *CIX_CONNECTOR_GET_TIMING)(
  IN CIX_CONNECTOR_PROTOCOL      *This,
  IN OUT DISPLAY_STATE           *DisplayState
  );

typedef
EFI_STATUS
(EFIAPI *CIX_CONNECTOR_GET_EDIE)(
  IN CIX_CONNECTOR_PROTOCOL *This,
  IN OUT DISPLAY_STATE           *DisplayState
  );

typedef
EFI_STATUS
(EFIAPI *CIX_CONNECTOR_PREPARE)(
  IN CIX_CONNECTOR_PROTOCOL      *This,
  IN OUT DISPLAY_STATE           *DisplayState
  );

typedef
EFI_STATUS
(EFIAPI *CIX_CONNECTOR_ENABLE)(
  IN CIX_CONNECTOR_PROTOCOL      *This,
  IN OUT DISPLAY_STATE           *DisplayState
  );

typedef
EFI_STATUS
(EFIAPI *CIX_CONNECTOR_DISABLE)(
  IN CIX_CONNECTOR_PROTOCOL      *This,
  IN OUT DISPLAY_STATE           *DisplayState
  );

typedef
EFI_STATUS
(EFIAPI *CIX_CONNECTOR_UNPREPARE)(
  IN CIX_CONNECTOR_PROTOCOL      *This,
  IN OUT DISPLAY_STATE           *DisplayState
  );

struct CIX_CONNECTOR_PROTOCOL {
  VOID                        *Private;
  CIX_CONNECTOR_PREINIT       Preinit;
  CIX_CONNECTOR_INIT          Init;
  CIX_CONNECTOR_DEINIT        Deinit;
  CIX_CONNECTOR_DETECT        Detect;
  CIX_CONNECTOR_GET_TIMING    GetTiming;
  CIX_CONNECTOR_GET_EDIE      GetEdid;
  CIX_CONNECTOR_PREPARE       Prepare;
  CIX_CONNECTOR_ENABLE        Enable;
  CIX_CONNECTOR_DISABLE       Disable;
  CIX_CONNECTOR_UNPREPARE     Unprepare;
};

extern EFI_GUID  gCixConnectorProtocolGuid;

#endif
