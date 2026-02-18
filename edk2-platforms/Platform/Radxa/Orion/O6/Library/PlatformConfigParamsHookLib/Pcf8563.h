/** @file
 *
 *  Copyright 2024 Radxa Computer (Shenzhen) Co., Ltd. All Rights Reserved.
 *  Copyright 2023 Cix Technology (Shanghai) Co., Ltd. All Rights Reserved.
 *  Copyright (c) 2017, Linaro, Ltd. All rights reserved.<BR>
 * 
 **/

#ifndef _PCF8563_H_
#define _PCF8563_H_

#include <Library/UefiLib.h>
#include <Protocol/I2cMaster.h>

#define SLAVE_ADDRESS                       (FixedPcdGet8 (PcdI2cSlaveAddress))

#define PCF8563_CONTROL1_REG_OFFSET         0x0
#define PCF8563_CONTROL2_REG_OFFSET         0x0
#define PCF8563_SECOND_REG_OFFSET           0x2
#define PCF8563_MINUTE_REG_OFFSET           0x3
#define PCF8563_HOUR_REG_OFFSET             0x4
#define PCF8563_DAY_REG_OFFSET              0x5
#define PCF8563_WEEKDAY_REG_OFFSET          0x6
#define PCF8563_MONTH_REG_OFFSET            0x7
#define PCF8563_YEAR_REG_OFFSET             0x8
#define PCF8563_ALARM_MINUTE_REG_OFFSET     0x9
#define PCF8563_ALARM_HOUR_REG_OFFSET       0xa
#define PCF8563_ALARM_DAY_REG_OFFSET        0xb
#define PCF8563_ALARM_WEEKDAY_REG_OFFSET    0xc
#define PCF8563_CLKOUT_REG_OFFSET           0xc
#define PCF8563_TIMER_CONTROL_REG_OFFSET    0xc
#define PCF8563_TIMER_COUNTDOWN_REG_OFFSET  0xc

#define PCF8563_DATA_REG_OFFSET             PCF8563_SECOND_REG_OFFSET

#define PCF8563_CLOCK_INVALID               0x80
#define PCF8563_SECONDS_MASK                0x7f
#define PCF8563_MINUTES_MASK                0x7f
#define PCF8563_HOURS_MASK                  0x3f
#define PCF8563_DAYS_MASK                   0x3f
#define PCF8563_WEEKDAYS_MASK               0x07
#define PCF8563_MONTHS_MASK                 0x1f
#define PCF8563_CENTURY_MASK                0x80

//
// The PCF8563 has a 'century' flag, which means it could theoretically span
// 200 years. However, it treats all years divisible by 4 as leap years,
// including the years 1900 and 2100 (which are not leap years), so the only
// sane base year is 2000 (which was a leap year).
//
#define EPOCH_BASE                          2000

#pragma pack(1)
typedef struct {
  UINT8                                     VL_seconds;
  UINT8                                     Minutes;
  UINT8                                     Hours;
  UINT8                                     Days;
  UINT8                                     Weekdays;
  UINT8                                     Century_months;
  UINT8                                     Years;
} RTC_DATETIME;

typedef struct {
  UINT8                                     Reg;
  RTC_DATETIME                              DateTime;
} RTC_SET_DATETIME_PACKET;
#pragma pack()

typedef struct {
  UINTN                                     OperationCount;
  EFI_I2C_OPERATION                         SetAddressOp;
  EFI_I2C_OPERATION                         GetDateTimeOp;
} RTC_GET_I2C_REQUEST;

typedef EFI_I2C_REQUEST_PACKET              RTC_SET_I2C_REQUEST;

#endif
