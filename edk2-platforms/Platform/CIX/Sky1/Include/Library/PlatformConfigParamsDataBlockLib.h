/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef __PLATFORM_CONFIG_PARAMS_DATA_BLOCK_LIB_H__
#define __PLATFORM_CONFIG_PARAMS_DATA_BLOCK_LIB_H__

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#define  MAX_PARAMS_NAME_STRING_SIZE    50
#define  MAX_PARAMS_OPTION_STRING_SIZE  120
#define  MAX_PARAMS_OPTION_NUM          10

#define  BOARD_ID_INFO_FLAGS_VIRTUAL_SKU_ID  BIT0
#define  BOARD_ID_INFO_FLAGS_RESERVED_0      BIT1  // to be implemented for future function
#define  BOARD_ID_INFO_FLAGS_RESERVED_1      BIT2  // to be implemented for future function
#define  BOARD_ID_INFO_FLAGS_RESERVED_2      BIT3  // to be implemented for future function

#pragma pack(1)

typedef union {
  UINT16    Value;
  struct {
    UINT16    Sku    : 3;
    UINT16    Memory : 3;
    UINT16    Pmic   : 2;
    UINT16    Rev    : 2;
    UINT16    Rsvd   : 2;
    UINT16    Flag   : 4;
  } Id;
} PLATFORM_BOARD_ID_INFO;

typedef struct _PLATFORM_CONFIG_PARAMS_DATA_BLOCK {
  // Platform relevant
  BOOLEAN    RtcVoltDrop;
  UINT16     BoardId;

  // OS boot relevant
  UINT8      SystemTableSelect;
  UINT8      DtbMenuEntry;
  // Onboard device power control
  UINT8      GfxPower;
  UINT8      TouchPanelPower;
  UINT8      TpmPower;
  UINT8      WwanPower;
  UINT8      PcieX2SlotPower;
  UINT8      FingerPrintPower;
  UINT8      WlanPower;
  UINT8      M2SsdPower;
  UINT8      OnBoardLanPower;
  UINT8      IspCamera0Power;
  UINT8      IspCamera1Power;
  UINT8      IspCamera2Power;
  UINT8      IspCamera3Power;
  UINT8      StateAfterG3;
  UINT8      RtcWakeup;
  UINT8      LightSensorCtrl;
  UINT8      SpcrEnable;
  UINT8      GopDisplayPort;
  UINT8      TypecMode[4];
  UINT8      EcFanMode;
} PLATFORM_CONFIG_PARAMS_DATA_BLOCK;

typedef struct _PLATFORM_CONFIG_PARAMS_DATA_ENTRY {
  UINT32    Id;
  UINTN     Offset;
  UINT16    Size;
  UINT16    Type;
  CHAR16    Name[MAX_PARAMS_NAME_STRING_SIZE];
  CHAR16    Option[MAX_PARAMS_OPTION_STRING_SIZE];
} PLATFORM_CONFIG_PARAMS_DATA_ENTRY;

typedef struct _PLATFORM_CONFIG_PARAMS_DATA_OPTIONS {
  UINT64    Value;
  CHAR16    String[MAX_PARAMS_OPTION_STRING_SIZE];
} PLATFORM_CONFIG_PARAMS_DATA_OPTIONS;

#pragma pack()

#define  PLATFORM_PARAMS_DATA_OFFSET_SIZE(Params)  ((UINTN)&(((PLATFORM_CONFIG_PARAMS_DATA_BLOCK *)0)->Params)),   \
                                             ((UINT16)sizeof((((PLATFORM_CONFIG_PARAMS_DATA_BLOCK *)0)->Params)))

#define  PARAMS_DATA_BOOLEAN_TYPE       0x0000
#define  PARAMS_DATA_MULTI_OPTION_TYPE  0x0001
#define  PARAMS_DATA_INTEGER_TYPE       0x0002
#define  PARAMS_DATA_STRING_TYPE        0x0003

#define  PARAMS_DATA_PLATFORM_ID_GROUP  0x00800000

#define  PARAMS_DATA_RTC_VOLT_DROP_ID        (PARAMS_DATA_PLATFORM_ID_GROUP | 0x0000)
#define  PARAMS_DATA_BOARD_ID_ID             (PARAMS_DATA_PLATFORM_ID_GROUP | 0x0001)
#define  PARAMS_DATA_SYSTEM_TABLE_SELECT_ID  (PARAMS_DATA_PLATFORM_ID_GROUP | 0x0002)
#define  PARAMS_DATA_DTB_MENU_ENTRY_ID       (PARAMS_DATA_PLATFORM_ID_GROUP | 0x0003)
#define  PARAMS_DATA_GFX_PWR_ID              (PARAMS_DATA_PLATFORM_ID_GROUP | 0x0004)
#define  PARAMS_DATA_TP_PWR_ID               (PARAMS_DATA_PLATFORM_ID_GROUP | 0x0005)
#define  PARAMS_DATA_TPM_PWR_ID              (PARAMS_DATA_PLATFORM_ID_GROUP | 0x0006)
#define  PARAMS_DATA_WWAN_PWR_ID             (PARAMS_DATA_PLATFORM_ID_GROUP | 0x0007)
#define  PARAMS_DATA_X2_SLOT_PWR_ID          (PARAMS_DATA_PLATFORM_ID_GROUP | 0x0008)
#define  PARAMS_DATA_FP_PWR_ID               (PARAMS_DATA_PLATFORM_ID_GROUP | 0x0009)
#define  PARAMS_DATA_WLAN_PWR_ID             (PARAMS_DATA_PLATFORM_ID_GROUP | 0x000A)
#define  PARAMS_DATA_M2SSD_PWR_ID            (PARAMS_DATA_PLATFORM_ID_GROUP | 0x000B)
#define  PARAMS_DATA_ONBOARD_LAN_PWR_ID      (PARAMS_DATA_PLATFORM_ID_GROUP | 0x000C)
#define  PARAMS_DATA_ISP_CAM0_PWR_ID         (PARAMS_DATA_PLATFORM_ID_GROUP | 0x000D)
#define  PARAMS_DATA_ISP_CAM1_PWR_ID         (PARAMS_DATA_PLATFORM_ID_GROUP | 0x000E)
#define  PARAMS_DATA_ISP_CAM2_PWR_ID         (PARAMS_DATA_PLATFORM_ID_GROUP | 0x000F)
#define  PARAMS_DATA_ISP_CAM3_PWR_ID         (PARAMS_DATA_PLATFORM_ID_GROUP | 0x0010)
#define  PARAMS_DATA_STATE_AFTER_G3_ID       (PARAMS_DATA_PLATFORM_ID_GROUP | 0x0011)
#define  PARAMS_DATA_RTC_WAKE_UP_ID          (PARAMS_DATA_PLATFORM_ID_GROUP | 0x0012)
#define  PARAMS_DATA_LIGHT_SENSOR_CTRL_ID    (PARAMS_DATA_PLATFORM_ID_GROUP | 0x0013)
#define  PARAMS_DATA_SPCR_ENABLE_ID          (PARAMS_DATA_PLATFORM_ID_GROUP | 0x0014)
#define  PARAMS_DATA_GOP_DISPAY_PORT_ID      (PARAMS_DATA_PLATFORM_ID_GROUP | 0x0015)
#define  PARAMS_DATA_TYPEC0_MODE_ID          (PARAMS_DATA_PLATFORM_ID_GROUP | 0x0016)
#define  PARAMS_DATA_TYPEC1_MODE_ID          (PARAMS_DATA_PLATFORM_ID_GROUP | 0x0017)
#define  PARAMS_DATA_TYPEC2_MODE_ID          (PARAMS_DATA_PLATFORM_ID_GROUP | 0x0018)
#define  PARAMS_DATA_TYPEC3_MODE_ID          (PARAMS_DATA_PLATFORM_ID_GROUP | 0x0019)
#define  PARAMS_DATA_EC_FAN_MODE_ID          (PARAMS_DATA_PLATFORM_ID_GROUP | 0x001B)
extern PLATFORM_CONFIG_PARAMS_DATA_BLOCK  mPlatformConfigParamsDataBlock;
extern PLATFORM_CONFIG_PARAMS_DATA_ENTRY  mPlatformConfigDataEntryTable[];
extern UINT32                             mPlatformConfigDataEntryNum;

EFI_STATUS
EFIAPI
ParsePlatformConfigDataOption (
  IN  CHAR16                               *String,
  OUT UINT32                               *Num,
  OUT PLATFORM_CONFIG_PARAMS_DATA_OPTIONS  *Options
  );

#endif
