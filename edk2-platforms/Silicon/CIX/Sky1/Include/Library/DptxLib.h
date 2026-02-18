/**
 * @file DptxLib.c
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
 * @brief
 *
 *
 * SPDX-License-Identifier: BSD-2-Clause-Patent
 *
**/

#ifndef __DPTX_LIB_H__
#define __DPTX_LIB_H__

#include <Protocol/ConnectorProtocol.h>
#include <Library/DisplayLib.h>
#include <Library/Edid.h>
#include <Library/drm_dp.h>
#include <Library/drm_dp_helper.h>
#include <Protocol/ResetId.h>
#include <Protocol/ClockId.h>

#define DP_TIMEOUT_LOOP_COUNT  200
#define MAX_CR_LOOP            5
#define MAX_EQ_LOOP            5

/* I2C EDID Chip ID, Slave Address */
#define I2C_EDID_DEVICE_ADDR    0x50
#define I2C_E_EDID_DEVICE_ADDR  0x30

#define EDID_BLOCK_LENGTH    0x80
#define EDID_HEADER_PATTERN  0x00
#define EDID_EXTENSION_FLAG  0x7e
#define EDID_CHECKSUM        0x7f

/* DP_MAX_LANE_COUNT */
#define DPCD_ENHANCED_FRAME_CAP(x)  (((x) >> 7) & 0x1)
#define DPCD_MAX_LANE_COUNT(x)      ((x) & 0x1f)

/* DP_LANE_COUNT_SET */
#define DPCD_LANE_COUNT_SET(x)  ((x) & 0x1f)

/* DP_TRAINING_LANE0_SET */
#define DPCD_PRE_EMPHASIS_SET(x)   (((x) & 0x3) << 3)
#define DPCD_PRE_EMPHASIS_GET(x)   (((x) >> 3) & 0x3)
#define DPCD_VOLTAGE_SWING_SET(x)  (((x) & 0x3) << 0)
#define DPCD_VOLTAGE_SWING_GET(x)  (((x) >> 0) & 0x3)

enum LinkLaneCountType {
  LANE_COUNT1 = 1,
  LANE_COUNT2 = 2,
  LANE_COUNT4 = 4
};

enum LinkTrainingState {
  START,
  CLOCK_RECOVERY,
  EQUALIZER_TRAINING,
  FINISHED,
  FAILED
};

enum VoltageSwingLevel {
  VOLTAGE_LEVEL_0,
  VOLTAGE_LEVEL_1,
  VOLTAGE_LEVEL_2,
  VOLTAGE_LEVEL_3,
};

enum PreEmphasisLevel {
  PRE_EMPHASIS_LEVEL_0,
  PRE_EMPHASIS_LEVEL_1,
  PRE_EMPHASIS_LEVEL_2,
  PRE_EMPHASIS_LEVEL_3,
};

enum ColorSpace {
  COLOR_RGB,
  COLOR_YCBCR422,
  COLOR_YCBCR444
};

enum ColorDepth {
  COLOR_8,
  COLOR_10,
  COLOR_12
};

enum ColorCoefficient {
  COLOR_YCBCR601,
  COLOR_YCBCR709
};

enum DynamicRange {
  VESA,
  CEA
};

struct VideoInfo {
  char                      *Name;

  BOOLEAN                   H_Sync_Polarity;
  BOOLEAN                   V_Sync_Polarity;
  BOOLEAN                   Interlaced;

  enum ColorSpace           ColorSpace;
  enum DynamicRange         DynamicRange;
  enum ColorCoefficient     Ycbcr_Coeff;
  enum ColorDepth           ColorDepth;

  UINTN                     MaxLinkRate;
  enum LinkLaneCountType    MaxLaneCount;

  UINTN                     pclock;
  UINT8                     misc0;
  UINT8                     misc1;
  UINT8                     bpp;
};

struct LinkTrain {
  UINTN                     Eq_Loop;
  UINTN                     Cr_Loop[4];

  UINT8                     Link_Rate;
  UINT8                     LaneCount;
  UINT8                     TrainingLane[4];
  BOOLEAN                   SSC;

  enum LinkTrainingState    LtState;
};

struct DptxDevice {
  INT32                      ID;
  UINT32                     Base;
  BOOLEAN                    Force_Hpd;
  struct VideoInfo           *VideoInfo;
  struct LinkTrain           LinkTrain;

  struct DRM_DSIPALY_MODE    *MODE;
  struct VideoMode           *Timing;
  UINT8                      Enhanced;
  UINT8                      MaxLinkRate;
  UINT8                      SinkLaneCount; /* from dpcd */
  UINT8                      LaneCount;     /* from phy  */
  struct drm_dp_aux          aux;
  UINT32                     State;
  CIX_RESET_PROTOCOL         *mCixResetProtocol;
  CIX_CLOCK_PROTOCOL         *mCixClockProtocol;

  UINT8                      Dpcd[16];
  CONST VOID                 *Private;
  BOOLEAN                    TimingFix;     /* if timing is fixed */
  UINT32                     Delay;         /* us, delay After Training */
  BOOLEAN                    eDPPanel;      /* eDP panel is mounted */
};

#endif
