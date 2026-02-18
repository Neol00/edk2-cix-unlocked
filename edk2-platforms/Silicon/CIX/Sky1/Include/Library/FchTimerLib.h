/** @file
  FCH timer Lib to control FCH timer controller.

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef FCH_TIMER_LIB_H__
#define FCH_TIMER_LIB_H__

#define  FCH_TIMER_PWM_CLOCK  25000000

#define FCH_TIMER_0_DP2_VARY_BL  0
#define FCH_TIMER_1_PWM0         1
#define FCH_TIMER_2_PWM1         2
#define FCH_TIMER_3_FAN_OUT0     3
#define FCH_TIMER_4_FAN_OUT1     4
#define FCH_TIMER_5_FAN_OUT2     5

VOID
EFIAPI
FchTimerEnablePwm (
  IN  UINT8  Timer
  );

VOID
EFIAPI
FchTimerDisablePwm (
  IN  UINT8  Timer
  );

EFI_STATUS
EFIAPI
FchTimerGetPwmPeriod (
  IN UINT8   Timer,
  IN UINT32  *Period
  );

VOID
EFIAPI
FchTimerSetPwmPeriod (
  IN UINT8   Timer,
  IN UINT32  Period
  );

EFI_STATUS
EFIAPI
FchTimerGetPwmHighTime (
  IN UINT8   Timer,
  IN UINT32  *HighTime
  );

VOID
EFIAPI
FchTimerSetPwmHighTime (
  IN UINT8   Timer,
  IN UINT32  HighTime
  );

EFI_STATUS
EFIAPI
FchTimerInitPwm (
  IN UINT8   Timer,
  IN UINT32  Freq,
  IN UINT8   Duty
  );

EFI_STATUS
EFIAPI
FchTimerGetPwmDuty (
  IN UINT8  Timer,
  IN UINT8  *Duty
  );

VOID
EFIAPI
FchTimerSetPwmDuty (
  IN UINT8  Timer,
  IN UINT8  Duty
  );

#endif // FCH_TIMER_LIB_H__
