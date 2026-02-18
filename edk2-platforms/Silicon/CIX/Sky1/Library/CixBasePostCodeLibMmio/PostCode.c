/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
  Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/CixPostCodeLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseLib.h>
#include <Library/CixFwBootPerfLib.h>

typedef struct {
  POST_CODE_KEY    Key;
  UINT32           PostCode;
  CHAR16           *StrPostCode;
} POST_CODE_KEY_TO_VAL;

POST_CODE_KEY_TO_VAL  PostCodeMapTable[] =
{
  // 0xE000 ~ 0xEFFF
  // {0xE001~0xE0FF used by sec},
  { PrePiStart,                         0xE006, L"PrePiStart"                         },
  { PrePiEnd,                           0xE007, L"PrePiEnd"                           },
  { DxeMainStart,                       0xE100, L"DxeMainStart"                       },
  { DxeCoreDispatcherStart,             0xE101, L"DxeCoreDispatcherStart"             },
  { SerialDxeStart,                     0xE109, L"SerialDxeStart"                     },
  { SerialDxeEnd,                       0xE10A, L"SerialDxeEnd"                       },
  { ArmGicDxeStart,                     0xE10B, L"ArmGicDxeStart"                     },
  { ArmGicDxeEnd,                       0xE10C, L"ArmGicDxeEnd"                       },
  { ArmCpuDxeStart,                     0xE10D, L"ArmCpuDxeStart"                     },
  { ArmCpuDxeEnd,                       0xE10E, L"ArmCpuDxeEnd"                       },
  { ArmTimerDxeStart,                   0xE10F, L"ArmTimerDxeStart"                   },
  { ArmTimerDxeEnd,                     0xE110, L"ArmTimerDxeEnd"                     },
  { BdsDxeStart,                        0xE111, L"BdsDxeStart"                        },
  { BdsDxeEnd,                          0xE112, L"BdsDxeEnd"                          },
  { ConSplitterDxeStart,                0xE113, L"ConSplitterDxeStart"                },
  { ConSplitterDxeEnd,                  0xE114, L"ConSplitterDxeEnd"                  },
  { PlatformDxeStart,                   0xE115, L"PlatformDxeStart"                   },
  { PlatformDxeEnd,                     0xE116, L"PlatformDxeEnd"                     },
  { ConfigParamsManageDxeStart,         0xE117, L"ConfigParamsManageDxeStart"         },
  { ConfigParamsManageDxeEnd,           0xE118, L"ConfigParamsManageDxeEnd"           },
  { PlatformConfigParamsManageDxeStart, 0xE119, L"PlatformConfigParamsManageDxeStart" },
  { PlatformConfigParamsManageDxeEnd,   0xE11A, L"PlatformConfigParamsManageDxeEnd"   },
  { GpioInterruptDxeStart,              0xE281, L"GpioInterruptDxeStart"              },
  { GpioInterruptDxeEnd,                0xE2C0, L"GpioInterruptDxeEnd"                },
  { PcieInitDxeStart,                   0xE2C1, L"PcieInitDxeStart"                   },
  { PcieInitDxeEnd,                     0xE300, L"PcieInitDxeEnd"                     },
  { UsbStart,                           0xE301, L"USBSTART"                           },
  { CdnspDxeStart,                      0xE302, L"CdnspDxeStart"                      },
  { CdnspDxeEnd,                        0xE303, L"CdnspDxeEnd"                        },
  { UsbDeviceControlDxeStart,           0xE304, L"UsbDeviceControlStart"              },
  { UsbDeviceControlDxeEnd,             0xE305, L"UsbDeviceControlEnd"                },
  { UsbEnd,                             0xE340, L"USBEND"                             },
  { GmacInitDxeStart,                   0xE381, L"GmacInitDxeStart"                   },
  { GmacInitDxeEnd,                     0xE3C0, L"GmacInitDxeEnd"                     },
  { I2cInitDxeStart,                    0xE201, L"I2cInitDxeStart"                    },
  { I2cInitDxeEnd,                      0xE240, L"I2cInitDxeEnd"                      },
  { SpiInitDxeStart,                    0xE241, L"SpiInitDxeStart"                    },
  { SpiInitDxeEnd,                      0xE280, L"SpiInitDxeEnd"                      },
  { XspiInitDxeStart,                   0xE3C1, L"XspiInitDxeStart"                   },
  { XspiInitDxeEnd,                     0xE400, L"XspiInitDxeEnd"                     },
  { HdaInitDxeStart,                    0xE401, L"HdaInitDxeStart"                    },
  { HdaInitDxeEnd,                      0xE440, L"HdaInitDxeEnd"                      },
  { PdDxeStart,                         0xE6C1, L"PdDxeStart"                         },
  { PdDxeEnd,                           0xE700, L"PdDxeEnd"                           },
  { DxeCoreDispatcherEnd,               0xE1FE, L"DxeCoreDispatcherEnd"               },
  { DxeMainEnd,                         0xE1FF, L"DxeMainEnd"                         },
  // boot manager       0xE541 ~0xE580  64
  { BdsStart,                           0xE541, L"BdsStart"                           },
  { BootLogo,                           0xE543, L"BootLogo"                           },
  { BMAfterConsole,                     0xE550, L"BmAfterConsole"                     },
  { DxeReadyToBoot,                     0xE580, L"DxeReadyToBoot"                     },

  { 0,                                  0,      NULL                                  }
};

POST_CODE_KEY_TO_VAL  *CheckPointStatusCodes[] =
{
  PostCodeMapTable
};

UINT32
FindAndMapByteCode (
  POST_CODE_KEY_TO_VAL  *Map,
  POST_CODE_KEY         Value
  )
{
  while (Map->Key != 0) {
    if (Map->Key == Value) {
      return Map->PostCode;
    }

    Map++;
  }

  return Value;
}

CHAR16 *
FindStrPostCode (
  POST_CODE_KEY_TO_VAL  *Map,
  POST_CODE_KEY         Value
  )
{
  while (Map->Key != 0) {
    if (Map->Key == Value) {
      return Map->StrPostCode;
    }

    Map++;
  }

  return NULL;
}

EFI_STATUS
SerialOutput (
  IN CHAR8  *String
  )
{
  UINT8  *Buffer;
  UINTN  Count = 0;

  if (String == NULL) {
    return EFI_SUCCESS;
  }

  // first make data useable
  Buffer = (UINT8 *)String;

  // Lets count the size of the string
  while (*(Buffer + Count)) {
    if ((*(Buffer + Count) == '\r') && (*(Buffer + Count + 1) == '\n')) {
      Count += 2;
    } else if (*(Buffer + Count) == '\n') {
      // we need to add \r before \n so if we found \n send all that before it
      if (Count) {
        SerialPortWrite (Buffer, Count);                        // replace "\n" with "\r\n" and send 2 symbols
      }

      SerialPortWrite ((UINT8 *)"\r\n", 2);
      Buffer =  Buffer + Count + 1; // init Buffer with nex position after \n
      Count  = 0;                   // and reset Counter
    } else {
      Count++;
    }
  }

  if (Count) {
    SerialPortWrite (Buffer, Count);
  }

  return EFI_SUCCESS;
}

VOID
SerialCheckpoint (
  UINT16  Checkpoint
  )
{
  char  s[32];

  AsciiSPrint (s, sizeof (s), "[UEFI] %x\n", Checkpoint);
  SerialOutput (s);
}

VOID
SerialStrCheckpoint (
  UINT32  PostCodeVal,
  CHAR16  *pStrPostCode
  )
{
  char  s[100];

  if (pStrPostCode == NULL) {
    AsciiSPrint (s, sizeof (s), "[UEFI] %x\n", PostCodeVal);
  } else {
    AsciiSPrint (s, sizeof (s), "[UEFI] %x %s\n", PostCodeVal, pStrPostCode);
  }

  SerialOutput (s);
 #ifdef CONFIG_RLOG_ENABLE
  rlog_printf (LOGLEVEL_INFO, s);
 #endif
}

VOID
TimeStampPrint (
  )
{
  CHAR8   Buffer[32];
  UINT64  Ticker, TimeStamp, Second, Remainder, MicroSecond;
  UINT64  TimeStampStart = 0;

  TimeStampStart = cix_get_boot_phase (BLOADER_PHASE, RECORD_START)*1000000;
  Ticker         = GetPerformanceCounter ();
  TimeStamp      = GetTimeInNanoSecond (Ticker);
  TimeStamp      = TimeStamp - TimeStampStart;
  Second         = TimeStamp/(1000*1000*1000);
  Remainder      = TimeStamp%(1000*1000*1000);
  MicroSecond    = Remainder/(1000*1000);

  AsciiSPrint (Buffer, 13, "[%02d.%03d] ", Second, MicroSecond);
  SerialPortWrite ((UINT8 *)Buffer, AsciiStrLen (Buffer));
  // Index++;
}

UINT32
EFIAPI
PostCode (
  IN POST_CODE_KEY  Value
  )
{
  UINT32  PostCodeVal;
  CHAR16  *pStrPostCode;

  PostCodeVal  = FindAndMapByteCode (CheckPointStatusCodes[0], Value);
  pStrPostCode = FindStrPostCode (CheckPointStatusCodes[0], Value);

  // SerialCheckpoint(PostCodeVal);
  TimeStampPrint (); // Print timestamp
  SerialStrCheckpoint (PostCodeVal, pStrPostCode);
  MmioWrite32 (FixedPcdGet32 (PcdPostCodeApAddr), PostCodeVal<<8);
  // MmioWrite32 (FixedPcdGet32 (PcdPostCodeSeAddr), PostCodeVal);

  return PostCodeVal;
}

BOOLEAN
EFIAPI
PostCodeEnabled (
  VOID
  )
{
  return (BOOLEAN)((PcdGet8 (PcdPostCodePropertyMask) & POST_CODE_PROPERTY_POST_CODE_ENABLED) != 0);
}
