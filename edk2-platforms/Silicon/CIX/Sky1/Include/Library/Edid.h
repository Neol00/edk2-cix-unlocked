/** @file Edid.h
 * @brief Helper routine and corresponding data struct used by USB DisplayLink Driver.
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
 * Reads and parses the EDID, checks if a requested video mode is in the supplied EDID
 *
 * Copyright (c) 2018-2019, DisplayLink (UK) Ltd. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause-Patent
 *
**/

#ifndef __EDID_H__
#define __EDID_H__

#define EDID_HEADER_SIZE                          ((UINTN)8)
#define EDID_NUMBER_OF_ESTABLISHED_TIMINGS_BYTES  ((UINTN)3)
#define EDID_NUMBER_OF_STANDARD_TIMINGS           ((UINTN)8)
#define EDID_NUMBER_OF_DETAILED_TIMINGS           ((UINTN)4)

// Requests to read values from the firmware
#define EDID_BLOCK_SIZE                           128
#define EDID_DETAILED_TIMING_INVALID_PIXEL_CLOCK  ((UINT16)(0x64))

typedef struct {
  UINT16    HRes;
  UINT16    VRes;
  UINT16    Refresh;
} EDID_TIMING;

struct VideoMode {
  UINT16    PixelClock;
  UINT16    HActive;
  UINT16    HBlanking;
  UINT16    HSyncOffset; // Horizontal Front Porch
  UINT16    HSyncWidth;
  UINT16    VActive;
  UINT16    VBlanking;
  UINT16    VSyncOffset; // Vertical Front Porch
  UINT16    VSyncWidth;
  // End of Edid Detailed Timing Descriptor

  UINT16    Flags /*ModeFlags*/;
  UINT16    InsetLeft;
  UINT16    InsetTop;
  UINT16    InsetRight;
  UINT16    InsetBottom;
  UINT8     Vic;
};

EFI_STATUS
EdidGetSupportedVideoMode (
  UINT32                  ModeNumber,
  CONST VOID              *EDID,
  UINT32                  EdidSize,
  CONST struct VideoMode  **VideoMode
  );

EFI_STATUS
EdidGetSupportedVideoModeWithFallback (
  UINT32                  ModeNumber,
  CONST VOID              *EDID,
  UINT32                  EdidSize,
  CONST struct VideoMode  **VideoMode
  );

UINT32
EdidGetNumSupportedModesInEdid (
  CONST VOID  *EDID,
  UINT32      EdidSize
  );

UINT32
VideoModeGetNumSupportedVideoModes (
  );

CONST struct VideoMode *
VideoModeGetSupportedVideoMode (
  UINT32  Index
  );

UINT32
EdidGetPanelID (
  IN CONST VOID  *EDID,
  IN UINT32      EdidSize
  );

// EDID Detailed timings section - Features
enum EdidDetailedTimingsFeatures {
  EdidDetailedTimingsFeaturesInterlaced             = 0x80,
  EdidDetailedTimingsFeaturesStereoModeMask         = 0x60,
  EdidDetailedTimingsFeaturesSyncSchemeMask         = 0x18,
  EdidDetailedTimingsFeaturesHorizontalSyncPositive = 0x02,
  EdidDetailedTimingsFeaturesVerticalSyncPositive   = 0x04,
};

// NR-110497-TC-7ZM Section 4.3.3 0x22 Set Video Mode - Flags
enum VideoModeFlags {
  VideoModeFlagsInterlaced             = 0x0001,
  VideoModeFlagsHorizontalSyncInverted = 0x0100,
  VideoModeFlagsVerticalSyncInverted   = 0x0200,
};

struct __attribute__ ((__packed__)) StandardTimingIdentification {
  UINT8    HorizontalActivePixels;     // X resolution, from 256->2288 in increments of 8 pixels
  UINT8    ImageAspectRatioAndrefresh; // Bits 7,6 Aspect ratio - 0=16:10 1=4:3 2=5:4 3=16:9
  // Bits 5,0 Refresh rate Range 60->1213Hz
};

struct __attribute__ ((__packed__)) DetailedTimingIdentification {
  UINT16    PixelClock;               // wPixelClock in VideoMode struct
  UINT8     HActiveLo;                // wHActive
  UINT8     HBlankingLo;              // wHBlanking
  UINT8     HActiveHiBlankingHi;
  UINT8     VActiveLo;                // wVActive
  UINT8     VBlankingLo;              // wVBlanking
  UINT8     VActiveHiBlankingHi;
  UINT8     HSyncOffsetLo;            // wHSyncOffset, front porch
  UINT8     HSyncWidthLo;             // wHSyncWidth
  UINT8     VSyncOffsetLoSyncWidthLo;
  UINT8     HSyncOffsetHiHSyncWidthHiVSyncOffsetHiSyncWidthHi;
  UINT8     HImageSizeHi;
  UINT8     VImageSizeHi;
  UINT8     HImageSizeLoVImageSizeLo;
  UINT8     HBorder;
  UINT8     VBorder;
  UINT8     Features;
};

struct __attribute__ ((__packed__)) Edid {
  UINT8                                  Header[EDID_HEADER_SIZE]; // EDID header "00 FF FF FF FF FF FF 00"
  UINT8                                  MFGID[2];                 // EISA 3-character ID
  UINT16                                 ProductCode;              // Vendor assigned code
  UINT32                                 SerialNumber;             // 32-bit serial number
  UINT8                                  WeekOfManufacture;        // Week number
  UINT8                                  YearOfManufacture;        // Year
  UINT8                                  EdidVersion;              // EDID Structure Version
  UINT8                                  EdidRevision;             // EDID Structure Revision
  UINT8                                  VideoInputDefinition;
  UINT8                                  MaxHorizontalImageSize; // cm
  UINT8                                  MaxVerticalImageSize;   // cm
  UINT8                                  DisplayTransferCharacteristic;
  UINT8                                  FeatureSupport;
  UINT8                                  RedGreenLowBits;  // Rx1 Rx0 Ry1 Ry0 Gx1 Gx0 Gy1Gy0
  UINT8                                  BlueWhiteLowBits; // Bx1 Bx0 By1 By0 Wx1 Wx0 Wy1 Wy0
  UINT8                                  RedX;             // Red-x Bits 9 - 2
  UINT8                                  RedY;             // Red-y Bits 9 - 2
  UINT8                                  GreenX;           // Green-x Bits 9 - 2
  UINT8                                  GreenY;           // Green-y Bits 9 - 2
  UINT8                                  BlueX;            // Blue-x Bits 9 - 2
  UINT8                                  BlueY;            // Blue-y Bits 9 - 2
  UINT8                                  WhiteX;           // White-x Bits 9 - 2
  UINT8                                  WhiteY;           // White-x Bits 9 - 2
  UINT8                                  EstablishedTimings[EDID_NUMBER_OF_ESTABLISHED_TIMINGS_BYTES];
  struct StandardTimingIdentification    standardTimingIdentifications[EDID_NUMBER_OF_STANDARD_TIMINGS];
  struct DetailedTimingIdentification    detailedTimingDescriptions[EDID_NUMBER_OF_DETAILED_TIMINGS];
  UINT8                                  ExtensionFlag; // Number of (optional) 128-byte EDID extension blocks to follow
  UINT8                                  Checksum;
};

#endif // EDID_H
