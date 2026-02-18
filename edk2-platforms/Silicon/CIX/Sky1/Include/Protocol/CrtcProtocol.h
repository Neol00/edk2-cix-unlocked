/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved. *
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef CRTC_PROTOCOL_H_
#define CRTC_PROTOCOL_H_

#include <Library/DisplayLib.h>

#define CIX_CRTC_PROTOCOL_GUID   \
    {0xC128406A, 0x99D9, 0x11EC, {0x99, 0x27, 0xF4, 0x2A, 0x7D, 0xCB, 0x92, 0x5D}}

typedef struct CIX_CRTC_PROTOCOL CIX_CRTC_PROTOCOL;

typedef struct {
  INT32      ID;
  BOOLEAN    Enable;
  UINT32     PlaneMask;
  UINT32     PrimaryPlane;
  INT32      OutputType;
} VPS_CONFIG;

typedef
EFI_STATUS
(EFIAPI *CIX_CRTC_PREINIT)(
  IN CIX_CRTC_PROTOCOL      *This,
  IN OUT DISPLAY_STATE           *DisplayState
  );

typedef
EFI_STATUS
(EFIAPI *CIX_CRTC_INIT)(
  IN CIX_CRTC_PROTOCOL      *This,
  IN OUT DISPLAY_STATE           *DisplayState
  );

typedef
EFI_STATUS
(EFIAPI *CIX_CRTC_DEINIT)(
  IN CIX_CRTC_PROTOCOL      *This,
  IN OUT DISPLAY_STATE           *DisplayState
  );

typedef
EFI_STATUS
(EFIAPI *CIX_CRTC_SET_PLANE)(
  IN CIX_CRTC_PROTOCOL      *This,
  IN OUT DISPLAY_STATE           *DisplayState
  );

typedef
EFI_STATUS
(EFIAPI *CIX_CRTC_PREPARE)(
  IN CIX_CRTC_PROTOCOL      *This,
  IN OUT DISPLAY_STATE           *DisplayState
  );

typedef
EFI_STATUS
(EFIAPI *CIX_CRTC_ENABLE)(
  IN CIX_CRTC_PROTOCOL      *This,
  IN OUT DISPLAY_STATE           *DisplayState
  );

typedef
EFI_STATUS
(EFIAPI *CIX_CRTC_DISABLE)(
  IN CIX_CRTC_PROTOCOL      *This,
  IN OUT DISPLAY_STATE           *DisplayState
  );

typedef
EFI_STATUS
(EFIAPI *CIX_CRTC_UNPREPARE)(
  IN CIX_CRTC_PROTOCOL      *This,
  IN OUT DISPLAY_STATE           *DisplayState
  );

struct CIX_CRTC_PROTOCOL {
  VOID                  *Private;
  CIX_CRTC_PREINIT      Preinit;
  CIX_CRTC_INIT         Init;
  CIX_CRTC_DEINIT       Deinit;
  CIX_CRTC_SET_PLANE    SetPlane;
  CIX_CRTC_PREPARE      Prepare;
  CIX_CRTC_ENABLE       Enable;
  CIX_CRTC_DISABLE      Disable;
  CIX_CRTC_UNPREPARE    Unprepare;
  DRM_DISPLAY_MODE      ActiveMode;
  VPS_CONFIG            Vps[4];
  BOOLEAN               Hpd;
  BOOLEAN               Active;
  BOOLEAN               AssignPlane;
};

extern EFI_GUID  gCixCrtcProtocolGuid;

#endif
