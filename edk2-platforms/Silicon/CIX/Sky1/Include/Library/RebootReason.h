/**

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved

**/
#ifndef __REBOOT_REASON__
#define __REBOOT_REASON__
#include <PiDxe.h>
#define REBOOT_REASON_SHIFT      0
#define SUB_REBOOT_REASON_SHIFT  8
#define REBOOT_REASON_MASK       0xFFFFFF00
#define SUB_REBOOT_REASON_MASK   0xFFFF00FF

typedef enum {
  DefaultException         = 0x01,
  WatchDogInterruptTrigger = 0x02,
  RebootReasonMax          = 0xFF
} REBOOT_REASON;

#define SET_REBOOT_REASON(Value)  SetRebootReason(Value)
#define GET_REBOOT_REASON  GetRebootReason()

void
SetRebootReason (
  unsigned int  RebootReason
  );

unsigned int
GetRebootReason (
  void
  );

EFI_STATUS
EFIAPI
LibResetSystem (
  IN EFI_RESET_TYPE  ResetType,
  IN EFI_STATUS      ResetStatus,
  IN UINTN           DataSize,
  IN CHAR16          *ResetData OPTIONAL
  );

#endif
