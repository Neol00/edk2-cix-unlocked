/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 **/

#ifndef _RA8900CE_REAL_TIME_CLOCK_LIB_H_
#define _RA8900CE_REAL_TIME_CLOCK_LIB_H_

#include <PiDxe.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/TimeBaseLib.h>
#include <Library/RealTimeClockLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Protocol/I2cMaster.h>
#include <Protocol/I2cDevicePath.h>
#include <Protocol/ConfigParamsManageProtocol.h>

#define SLAVE_ADDRESS  (FixedPcdGet8 (PcdRa8900ceI2cSlaveAddress))

#define RA8900CE_DATA_REG_OFFSET    0x00
#define RA8900CE_SEC_REG_OFFSET     0x00
#define RA8900CE_MIN_REG_OFFSET     0x01
#define RA8900CE_HOUR_REG_OFFSET    0x02
#define RA8900CE_WEEK_REG_OFFSET    0x03
#define RA8900CE_DAY_REG_OFFSET     0x04
#define RA8900CE_MONTH_REG_OFFSET   0x05
#define RA8900CE_YEAR_REG_OFFSET    0x06
#define RA8900CE_ALARM_REG_OFFSET   0x08
#define RA8900CE_EXT_REG_OFFSET     0x0D
#define RA8900CE_FLAG_REG_OFFSET    0x0E
#define RA8900CE_CTRL_REG_OFFSET    0x0F
#define RA8900CE_BACKUP_REG_OFFSET  0x18

#define RA8900CE_SECONDS_MASK   0x7F
#define RA8900CE_MINUTES_MASK   0x7F
#define RA8900CE_HOURS_MASK     0x3F
#define RA8900CE_WEEKDAYS_MASK  0x7F
#define RA8900CE_DAYS_MASK      0x3F
#define RA8900CE_MONTHS_MASK    0x1F
#define RA8900CE_YEARS_MASK     0xFF

#define EPOCH_BASE  2000

#define RA8900CE_ALARM_REG_AE  BIT7

#define RA8900CE_EXT_REG_TEST   BIT7
#define RA8900CE_EXT_REG_WADA   BIT6
#define RA8900CE_EXT_REG_USEL   BIT5
#define RA8900CE_EXT_REG_TE     BIT4
#define RA8900CE_EXT_REG_FSEL1  BIT3
#define RA8900CE_EXT_REG_FSEL0  BIT2
#define RA8900CE_EXT_REG_TSEL1  BIT1
#define RA8900CE_EXT_REG_TSEL0  BIT0

#define RA8900CE_FLAG_REG_UF    BIT5
#define RA8900CE_FLAG_REG_TF    BIT4
#define RA8900CE_FLAG_REG_AF    BIT3
#define RA8900CE_FLAG_REG_VLF   BIT1
#define RA8900CE_FLAG_REG_VDET  BIT0

#define RA8900CE_CTRL_REG_CSEL1  BIT7
#define RA8900CE_CTRL_REG_CSEL0  BIT6
#define RA8900CE_CTRL_REG_UIE    BIT5
#define RA8900CE_CTRL_REG_TIE    BIT4
#define RA8900CE_CTRL_REG_AIE    BIT3
#define RA8900CE_CTRL_REG_RESET  BIT0

#define RA8900CE_BACKUP_REG_VDETOFF  BIT3
#define RA8900CE_BACKUP_REG_SWOFF    BIT2

#pragma pack(1)
typedef struct {
  UINT8    Seconds;
  UINT8    Minutes;
  UINT8    Hours;
  UINT8    Weekdays;
  UINT8    Days;
  UINT8    Months;
  UINT8    Years;
} RTC_DATETIME;

typedef struct {
  UINT8           Reg;
  RTC_DATETIME    DateTime;
} RTC_SET_DATETIME_PACKET;
#pragma pack()

typedef struct {
  UINTN                OperationCount;
  EFI_I2C_OPERATION    SetAddressOp;
  EFI_I2C_OPERATION    GetDateTimeOp;
} RTC_GET_I2C_REQUEST;

typedef EFI_I2C_REQUEST_PACKET RTC_SET_I2C_REQUEST;

#endif
