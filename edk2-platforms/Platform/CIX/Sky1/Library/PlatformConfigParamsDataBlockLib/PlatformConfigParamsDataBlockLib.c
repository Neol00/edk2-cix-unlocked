/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "PlatformConfigParamsDataBlockInternal.h"

PLATFORM_CONFIG_PARAMS_DATA_BLOCK  mPlatformConfigParamsDataBlock = {
  FALSE,      // RtcVoltDrop
  0xFFFF,     // BoardId
  FixedPcdGet8 (PcdSystemTableSelect),
  FixedPcdGet8 (PcdDtbMenuEntry),
  FixedPcdGet8 (PcdGfxPower),
  FixedPcdGet8 (PcdTouchPanelPower),
  FixedPcdGet8 (PcdTpmPower),
  FixedPcdGet8 (PcdWwanPower),
  FixedPcdGet8 (PcdPcieX2SlotPower),
  FixedPcdGet8 (PcdFingerPrintPower),
  FixedPcdGet8 (PcdWlanPower),
  FixedPcdGet8 (PcdM2SsdPower),
  FixedPcdGet8 (PcdOnBoardLanPower),
  FixedPcdGet8 (PcdIspCamera0Power),
  FixedPcdGet8 (PcdIspCamera1Power),
  FixedPcdGet8 (PcdIspCamera2Power),
  FixedPcdGet8 (PcdIspCamera3Power),
  FixedPcdGet8 (PcdStateAfterG3),
  FixedPcdGet8 (PcdRtcWakeup),
  FixedPcdGet8 (PcdLightSensorCtrl),
  FixedPcdGetBool (PcdAcpiSpcrEnable),
  0xFF, // Gop Display Selected, 0xFF: no display
  {
    0x01 // default usb only mode
  },
  FixedPcdGet8 (PcdEcDefaultFanMode)
};

PLATFORM_CONFIG_PARAMS_DATA_ENTRY  mPlatformConfigDataEntryTable[] = {
  { PARAMS_DATA_RTC_VOLT_DROP_ID,       PLATFORM_PARAMS_DATA_OFFSET_SIZE (RtcVoltDrop),       PARAMS_DATA_BOOLEAN_TYPE,      L"RTC voltage dropped",      L"0:no, 1:yes"                                             },
  { PARAMS_DATA_BOARD_ID_ID,            PLATFORM_PARAMS_DATA_OFFSET_SIZE (BoardId),           PARAMS_DATA_INTEGER_TYPE,      L"board ID",                 L"(hex)"                                                   },
  { PARAMS_DATA_SYSTEM_TABLE_SELECT_ID, PLATFORM_PARAMS_DATA_OFFSET_SIZE (SystemTableSelect), PARAMS_DATA_MULTI_OPTION_TYPE, L"system table selection",   L"0:ACPI, 1:DT"                                            },
  { PARAMS_DATA_DTB_MENU_ENTRY_ID,      PLATFORM_PARAMS_DATA_OFFSET_SIZE (DtbMenuEntry),      PARAMS_DATA_MULTI_OPTION_TYPE, L"DTB menu entry selection", L"0:table 0, 1:table 1, 2:table 2, 3:table 3"              },
  { PARAMS_DATA_GFX_PWR_ID,             PLATFORM_PARAMS_DATA_OFFSET_SIZE (GfxPower),          PARAMS_DATA_MULTI_OPTION_TYPE, L"GFX Device Power",         L"0:Disable, 1:Enable"                                     },
  { PARAMS_DATA_TP_PWR_ID,              PLATFORM_PARAMS_DATA_OFFSET_SIZE (TouchPanelPower),   PARAMS_DATA_MULTI_OPTION_TYPE, L"Touch Panel Power",        L"0:Disable, 1:Enable"                                     },
  { PARAMS_DATA_TPM_PWR_ID,             PLATFORM_PARAMS_DATA_OFFSET_SIZE (TpmPower),          PARAMS_DATA_MULTI_OPTION_TYPE, L"TPM Power",                L"0:Disable, 1:Enable"                                     },
  { PARAMS_DATA_WWAN_PWR_ID,            PLATFORM_PARAMS_DATA_OFFSET_SIZE (WwanPower),         PARAMS_DATA_MULTI_OPTION_TYPE, L"WWAN Power",               L"0:Disable, 1:Enable"                                     },
  { PARAMS_DATA_X2_SLOT_PWR_ID,         PLATFORM_PARAMS_DATA_OFFSET_SIZE (PcieX2SlotPower),   PARAMS_DATA_MULTI_OPTION_TYPE, L"Pcie X2 Slot Power",       L"0:Disable, 1:Enable"                                     },
  { PARAMS_DATA_FP_PWR_ID,              PLATFORM_PARAMS_DATA_OFFSET_SIZE (FingerPrintPower),  PARAMS_DATA_MULTI_OPTION_TYPE, L"Finger Print Power",       L"0:Disable, 1:Enable"                                     },
  { PARAMS_DATA_WLAN_PWR_ID,            PLATFORM_PARAMS_DATA_OFFSET_SIZE (WlanPower),         PARAMS_DATA_MULTI_OPTION_TYPE, L"WLAN Power",               L"0:Disable, 1:Enable"                                     },
  { PARAMS_DATA_M2SSD_PWR_ID,           PLATFORM_PARAMS_DATA_OFFSET_SIZE (M2SsdPower),        PARAMS_DATA_MULTI_OPTION_TYPE, L"M.2 SSD Power",            L"0:Disable, 1:Enable"                                     },
  { PARAMS_DATA_ONBOARD_LAN_PWR_ID,     PLATFORM_PARAMS_DATA_OFFSET_SIZE (OnBoardLanPower),   PARAMS_DATA_MULTI_OPTION_TYPE, L"On-Board Lan Power",       L"0:Disable, 1:Enable"                                     },
  { PARAMS_DATA_ISP_CAM0_PWR_ID,        PLATFORM_PARAMS_DATA_OFFSET_SIZE (IspCamera0Power),   PARAMS_DATA_MULTI_OPTION_TYPE, L"ISP camera0",              L"0:Disable, 1:Enable"                                     },
  { PARAMS_DATA_ISP_CAM1_PWR_ID,        PLATFORM_PARAMS_DATA_OFFSET_SIZE (IspCamera1Power),   PARAMS_DATA_MULTI_OPTION_TYPE, L"ISP camera1",              L"0:Disable, 1:Enable"                                     },
  { PARAMS_DATA_ISP_CAM2_PWR_ID,        PLATFORM_PARAMS_DATA_OFFSET_SIZE (IspCamera2Power),   PARAMS_DATA_MULTI_OPTION_TYPE, L"ISP camera2",              L"0:Disable, 1:Enable"                                     },
  { PARAMS_DATA_ISP_CAM3_PWR_ID,        PLATFORM_PARAMS_DATA_OFFSET_SIZE (IspCamera3Power),   PARAMS_DATA_MULTI_OPTION_TYPE, L"ISP camera3",              L"0:Disable, 1:Enable"                                     },
  { PARAMS_DATA_STATE_AFTER_G3_ID,      PLATFORM_PARAMS_DATA_OFFSET_SIZE (StateAfterG3),      PARAMS_DATA_MULTI_OPTION_TYPE, L"Restore AC Power Loss",    L"0:Power Off, 1:Power On"                                 },
  { PARAMS_DATA_RTC_WAKE_UP_ID,         PLATFORM_PARAMS_DATA_OFFSET_SIZE (RtcWakeup),         PARAMS_DATA_MULTI_OPTION_TYPE, L"RTC Wakeup",               L"0:Disable, 1:Enable"                                     },
  { PARAMS_DATA_LIGHT_SENSOR_CTRL_ID,   PLATFORM_PARAMS_DATA_OFFSET_SIZE (LightSensorCtrl),   PARAMS_DATA_MULTI_OPTION_TYPE, L"Light Sensor Control",     L"0:EC Mode, 1:AP Mode"                                    },
  { PARAMS_DATA_SPCR_ENABLE_ID,         PLATFORM_PARAMS_DATA_OFFSET_SIZE (SpcrEnable),        PARAMS_DATA_MULTI_OPTION_TYPE, L"Support SPCR",             L"0:Disable, 1:Enable"                                     },
  { PARAMS_DATA_GOP_DISPAY_PORT_ID,     PLATFORM_PARAMS_DATA_OFFSET_SIZE (GopDisplayPort),    PARAMS_DATA_MULTI_OPTION_TYPE, L"Gop Display Port",         L"0:DPU0, 1:DPU1, 2:DPU2, 3:DPU3, 4:DPU4, 0xFF:No Display" },
  { PARAMS_DATA_TYPEC0_MODE_ID,         PLATFORM_PARAMS_DATA_OFFSET_SIZE (TypecMode[0]),      PARAMS_DATA_MULTI_OPTION_TYPE, L"Typec0 Mode",              L"0:device, 1:usb, 2:dp-only, 3:dp+usb"                    },
  { PARAMS_DATA_TYPEC1_MODE_ID,         PLATFORM_PARAMS_DATA_OFFSET_SIZE (TypecMode[1]),      PARAMS_DATA_MULTI_OPTION_TYPE, L"Typec1 Mode",              L"0:device, 1:usb, 2:dp-only, 3:dp+usb"                    },
  { PARAMS_DATA_TYPEC2_MODE_ID,         PLATFORM_PARAMS_DATA_OFFSET_SIZE (TypecMode[2]),      PARAMS_DATA_MULTI_OPTION_TYPE, L"Typec2 Mode",              L"0:device, 1:usb, 2:dp-only, 3:dp+usb"                    },
  { PARAMS_DATA_TYPEC3_MODE_ID,         PLATFORM_PARAMS_DATA_OFFSET_SIZE (TypecMode[3]),      PARAMS_DATA_MULTI_OPTION_TYPE, L"Typec3 Mode",              L"0:device, 1:usb, 2:dp-only, 3:dp+usb"                    },
  { PARAMS_DATA_EC_FAN_MODE_ID,         PLATFORM_PARAMS_DATA_OFFSET_SIZE (EcFanMode),         PARAMS_DATA_MULTI_OPTION_TYPE, L"EC Fan Mode",              L"0:Auto Mode, 1:Performance mode"                         },
};

UINT32  mPlatformConfigDataEntryNum = sizeof (mPlatformConfigDataEntryTable) / sizeof (PLATFORM_CONFIG_PARAMS_DATA_ENTRY);

EFI_STATUS
EFIAPI
ParsePlatformConfigDataOption (
  IN  CHAR16                               *String,
  OUT UINT32                               *Num,
  OUT PLATFORM_CONFIG_PARAMS_DATA_OPTIONS  *Options
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  CHAR16      *Head, *Tail;
  CHAR16      *Str;
  UINT32      Index;
  UINTN       Location, Size;

  if ((String == NULL) || (Num == NULL) || (Options == NULL)) {
    Status = EFI_INVALID_PARAMETER;
  }

  Str = AllocateCopyPool (StrSize (String), String);
  if (Str == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Head = Str;
  Tail = Str;
  Size = StrSize (Str);

  Index    = 0;
  Location = 0;
  while ((Index < MAX_PARAMS_OPTION_NUM) && (Head != NULL) && (Location < Size)) {
    Head = StrStr (Tail, L":");
    if (Head == NULL) {
      FreePool (Str);
      return EFI_UNSUPPORTED;
    }

    *Head     = L'\0';
    Location += StrSize (Tail);
    if ((StrStr (Tail, L"0x") == NULL) && (StrStr (Tail, L"0X") == NULL)) {
      Options[Index].Value = StrDecimalToUint64 (Tail);
    } else {
      Options[Index].Value = StrHexToUint64 (Tail);
    }

    Tail = Head + 1;
    Head = StrStr (Tail, L",");
    if (Head == NULL) {
      Location += StrSize (Tail);
      if (Location != Size) {
        FreePool (Str);
        return EFI_UNSUPPORTED;
      }
    } else {
      *Head     = L'\0';
      Location += StrSize (Tail);
    }

    Status = StrCpyS (Options[Index].String, MAX_PARAMS_OPTION_STRING_SIZE, Tail);
    if (EFI_ERROR (Status)) {
      FreePool (Str);
      return Status;
    }

    if (Head != NULL) {
      Tail = Head + 1;
    }

    Index++;
  }

  *Num = Index;

  if ((Index == MAX_PARAMS_OPTION_NUM) && (Location < Size)) {
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}
