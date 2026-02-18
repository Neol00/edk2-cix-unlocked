/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved. *
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef __CIX_DISPLAY_H__
#define __CIX_DISPLAY_H__

#include <Uefi/UefiBaseType.h>

#define LIST_FOR_EACH_ENTRY(Pos, Head, Member)                          \
        for (Pos = BASE_CR((Head)->ForwardLink, typeof(*Pos), Member);  \
             &Pos->Member != (Head);                                    \
             Pos = BASE_CR(Pos->Member.ForwardLink, typeof(*Pos), Member))

#define __ROUND_MASK(x, y)  ((__typeof__(x))((y)-1))
#define ROUNDUP(x, y)       ((((x)-1) | __ROUND_MASK(x, y))+1)
#define ROUNDDOWN(x, y)     ((x) & ~__ROUND_MASK(x, y))
#define ALIGN(x, a)         (((x) + (a) - 1) & ~((a) - 1))

#define BIT(x)  (1 << x)

#define EDID_SIZE  128

#define DISPLAY_PORT_COUNT  5

typedef enum {
  CIX_FMT_ARGB8888 = 0,
  CIX_FMT_RGB888,
  CIX_FMT_RGB565,
  CIX_FMT_YUV420SP = 4,
  CIX_FMT_YUV422SP,
  CIX_FMT_YUV444SP,
} DATA_FORMAT;

typedef struct {
  UINT32     Mode;
  UINT32     Offset;
  UINT32     Width;
  UINT32     Height;
  UINT32     Bpp;
  CHAR8      *Memory;
  BOOLEAN    YMirror;
} LOGO_INFO;

typedef struct {
  UINT32    LeftMargin;
  UINT32    RightMargin;
  UINT32    TopMargin;
  UINT32    BottomMargin;
} OVER_SCAN;

typedef struct {
  /* Proposed mode values */
  UINT32     Clock;                             /* in kHz */
  UINT32     HDisplay;
  UINT32     HSyncStart;
  UINT32     HSyncEnd;
  UINT32     HTotal;
  UINT32     VDisplay;
  UINT32     VSyncStart;
  UINT32     VSyncEnd;
  UINT32     VTotal;
  UINT32     VRefresh;
  UINT32     VScan;
  UINT32     Flags;
  UINT32     PictureAspectRatio;
  UINT32     HSkew;
  UINT32     Type;
  /* Actual mode we give to hw */
  UINT32     CrtcClock;                          /* in KHz */
  UINT32     CrtcHDisplay;
  UINT32     CrtcHBlankStart;
  UINT32     CrtcHBblankEnd;
  UINT32     CrtcHSyncStart;
  UINT32     CrtcHSyncEnd;
  UINT32     CrtcHTotal;
  UINT32     CrtcHSkew;
  UINT32     CrtcVDisplay;
  UINT32     CrtcVBlankStart;
  UINT32     CrtcVBlankEnd;
  UINT32     CrtcVSyncStart;
  UINT32     CrtcVSyncEnd;
  UINT32     CrtcVTotal;
  BOOLEAN    Invalid;
} DRM_DISPLAY_MODE;

typedef struct {
  VOID                *Connector;
  UINT8               ConnectorID;
  OVER_SCAN           OverScan;
  DRM_DISPLAY_MODE    DisplayMode;
  VOID                *DispInfo;
  UINT8               EDID[EDID_SIZE * 4];
  UINT32              BusFormat;
  UINT8               OutputMode;
  UINT8               Type;
  UINT8               OutputFlags;
  UINT8               ColorSpace;
  UINT8               BPC;

  VOID                *Dptx;
} CONNECTOR_STATE;

typedef struct {
  VOID       *Crtc;
  VOID       *Private;
  UINT32     CrtcID;
  UINT32     Format;
  UINT32     YMirror;
  UINT32     RBSwap;
  UINT32     XVirtual;
  UINT32     SrcX;
  UINT32     SrcY;
  UINT32     SrcW;
  UINT32     SrcH;
  UINT32     CrtcX;
  UINT32     CrtcY;
  UINT32     CrtcW;
  UINT32     CrtcH;
  UINT32     Feature;
  UINT32     DMAAddress;
  BOOLEAN    YUVOverlay;
} CRTC_STATE;

typedef struct {
  LIST_ENTRY         ListHead;
  CRTC_STATE         CrtcState;
  CONNECTOR_STATE    ConnectorState;

  UINT32             ModeNumber;
  INT32              VpsConfigModeID;

  VOID               *MemoryBase;
  UINT32             MemorySize;

  BOOLEAN            IsEnable; /* SW */

  UINT8              ID;                           /* expected port ID */
  UINT8              HwStatus[DISPLAY_PORT_COUNT]; /* HW: 0, disabled; 1, enable */
  UINT8              IDMap[DISPLAY_PORT_COUNT];    /* order -> ID */

  BOOLEAN            IsForceOutput;
} DISPLAY_STATE;

EFIAPI
EFI_STATUS
DisplaySetCrtcInfo (
  OUT DRM_DISPLAY_MODE  *Mode,
  IN  UINT32            AdjustFlags
  );

EFIAPI
EFI_STATUS
DisplaySetFramebuffer (
  OUT EFI_PHYSICAL_ADDRESS  *VramBaseAddress,
  OUT UINTN                 *VramSize
  );

#endif
