/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
  Copyright (c) 2017, Linaro, Ltd. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/Ra8900ceRealTimeClockLib.h>
#include <Library/I2cLib.h>
#include <Library/DxeServicesTableLib.h>

STATIC I2C_HOST_DESCRIPTOR  *mHost;
STATIC EFI_HANDLE           mI2cMasterHandle;
STATIC EFI_EVENT            mRtcVirtualAddrChangeEvent;
STATIC EFI_EVENT            mRtcExitBootServicesEvent;
STATIC EFI_EVENT            mRtcMemoryInitEvent;

/**
  Returns the current time and date information, and the time-keeping
  capabilities of the hardware platform.

  @param  Time                  A pointer to storage to receive a snapshot of
                                the current time.
  @param  Capabilities          An optional pointer to a buffer to receive the
                                real time clock device's capabilities.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_INVALID_PARAMETER Time is NULL.
  @retval EFI_DEVICE_ERROR      The time could not be retrieved due to hardware
                                error.

**/
EFI_STATUS
EFIAPI
LibGetTime (
  OUT EFI_TIME               *Time,
  OUT EFI_TIME_CAPABILITIES  *Capabilities
  )
{
  RTC_GET_I2C_REQUEST  Op;
  RTC_DATETIME         DateTime;
  EFI_STATUS           Status;
  UINT8                Reg;

  if (Time == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Reg = RA8900CE_DATA_REG_OFFSET;

  Op.OperationCount = 2;

  Op.SetAddressOp.Flags         = 0;
  Op.SetAddressOp.LengthInBytes = 1;
  Op.SetAddressOp.Buffer        = &Reg;

  Op.GetDateTimeOp.Flags         = I2C_FLAG_READ;
  Op.GetDateTimeOp.LengthInBytes = sizeof (RTC_DATETIME);
  Op.GetDateTimeOp.Buffer        = (VOID *)&DateTime;

  Status = I2cMasterXfer (mHost, SLAVE_ADDRESS, (VOID *)&Op);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  Time->Nanosecond = 0;
  Time->Second     = BcdToDecimal8 (DateTime.Seconds & RA8900CE_SECONDS_MASK);
  Time->Minute     = BcdToDecimal8 (DateTime.Minutes & RA8900CE_MINUTES_MASK);
  Time->Hour       = BcdToDecimal8 (DateTime.Hours & RA8900CE_HOURS_MASK);
  Time->Day        = BcdToDecimal8 (DateTime.Days & RA8900CE_DAYS_MASK);
  Time->Month      = BcdToDecimal8 (DateTime.Months & RA8900CE_MONTHS_MASK);
  Time->Year       = BcdToDecimal8 (DateTime.Years) + EPOCH_BASE;

  if (Capabilities != NULL) {
    Capabilities->Resolution = 1;
    Capabilities->Accuracy   = 0;
    Capabilities->SetsToZero = TRUE;
  }

  return EFI_SUCCESS;
}

/**
  Sets the current local time and date information.

  @param  Time                  A pointer to the current time.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_INVALID_PARAMETER A time field is out of range.
  @retval EFI_DEVICE_ERROR      The time could not be set due due to hardware
                                error.

**/
EFI_STATUS
EFIAPI
LibSetTime (
  IN EFI_TIME  *Time
  )
{
  RTC_SET_I2C_REQUEST      Op;
  RTC_SET_DATETIME_PACKET  Packet;
  EFI_STATUS               Status;

  Packet.DateTime.Seconds  = DecimalToBcd8 (Time->Second);
  Packet.DateTime.Minutes  = DecimalToBcd8 (Time->Minute);
  Packet.DateTime.Hours    = DecimalToBcd8 (Time->Hour);
  Packet.DateTime.Days     = DecimalToBcd8 (Time->Day);
  Packet.DateTime.Weekdays = BIT0;
  Packet.DateTime.Months   = DecimalToBcd8 (Time->Month);
  Packet.DateTime.Years    = DecimalToBcd8 (Time->Year % 100);
  if (Time->Year >= EPOCH_BASE + 100) {
    return EFI_DEVICE_ERROR;
  }

  Packet.Reg = RA8900CE_DATA_REG_OFFSET;

  Op.OperationCount             = 1;
  Op.Operation[0].Flags         = 0;
  Op.Operation[0].LengthInBytes = sizeof (RTC_SET_DATETIME_PACKET);
  Op.Operation[0].Buffer        = (VOID *)&Packet;

  Status = I2cMasterXfer (mHost, SLAVE_ADDRESS, (VOID *)&Op);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Returns the current wakeup alarm clock setting.

  @param  Enabled               Indicates if the alarm is currently enabled or
                                disabled.
  @param  Pending               Indicates if the alarm signal is pending and
                                requires acknowledgement.
  @param  Time                  The current alarm setting.

  @retval EFI_SUCCESS           The alarm settings were returned.
  @retval EFI_INVALID_PARAMETER Any parameter is NULL.
  @retval EFI_DEVICE_ERROR      The wakeup time could not be retrieved due to a
                                hardware error.
  @retval EFI_UNSUPPORTED       A wakeup timer is not supported on this
                                platform.

**/
EFI_STATUS
EFIAPI
LibGetWakeupTime (
  OUT BOOLEAN   *Enabled,
  OUT BOOLEAN   *Pending,
  OUT EFI_TIME  *Time
  )
{
  EFI_STATUS           Status;
  UINT8                Buffer[4];
  RTC_GET_I2C_REQUEST  GetOp;
  RTC_SET_I2C_REQUEST  SetOp;

  if ((Time == NULL) || (Pending == NULL) || (Enabled == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Buffer[0] = RA8900CE_FLAG_REG_OFFSET;

  GetOp.OperationCount = 2;

  GetOp.SetAddressOp.Flags         = 0;
  GetOp.SetAddressOp.LengthInBytes = 1;
  GetOp.SetAddressOp.Buffer        = &Buffer[0];

  GetOp.GetDateTimeOp.Flags         = I2C_FLAG_READ;
  GetOp.GetDateTimeOp.LengthInBytes = 2;
  GetOp.GetDateTimeOp.Buffer        = (VOID *)&Buffer[1];

  Status = I2cMasterXfer (mHost, SLAVE_ADDRESS, (VOID *)&GetOp);
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "%a: flag register 0x%x, control register 0x%x\n", __FUNCTION__, Buffer[1], Buffer[2]));
    if (Buffer[1] & RA8900CE_FLAG_REG_AF) {
      *Pending = TRUE;
    } else {
      *Pending = FALSE;
    }

    if (Buffer[2] & RA8900CE_CTRL_REG_AIE) {
      *Enabled = TRUE;
    } else {
      *Enabled = FALSE;
    }

    if (*Pending) {
      Buffer[1] &= ~RA8900CE_FLAG_REG_AF;

      SetOp.OperationCount             = 1;
      SetOp.Operation[0].Flags         = 0;
      SetOp.Operation[0].LengthInBytes = 2;
      SetOp.Operation[0].Buffer        = (VOID *)&Buffer[0];

      Status = I2cMasterXfer (mHost, SLAVE_ADDRESS, (VOID *)&SetOp);
    }
  } else {
    DEBUG ((DEBUG_ERROR, "%a: fail to get wake enable bit, status %r\n", __FUNCTION__, Status));
    return Status;
  }

  Buffer[0] = RA8900CE_ALARM_REG_OFFSET;

  GetOp.OperationCount = 2;

  GetOp.SetAddressOp.Flags         = 0;
  GetOp.SetAddressOp.LengthInBytes = 1;
  GetOp.SetAddressOp.Buffer        = &Buffer[0];

  GetOp.GetDateTimeOp.Flags         = I2C_FLAG_READ;
  GetOp.GetDateTimeOp.LengthInBytes = 3;
  GetOp.GetDateTimeOp.Buffer        = (VOID *)&Buffer[1];

  Status = I2cMasterXfer (mHost, SLAVE_ADDRESS, (VOID *)&GetOp);
  if (!EFI_ERROR (Status)) {
    // Ra8900 just support week hour day and minute, so we init time
    // zero Time
    if (IsZeroBuffer (ZeroMem (Time, sizeof (EFI_TIME)), sizeof (EFI_TIME)) == FALSE) {
      DEBUG ((DEBUG_INFO, "Time can't reset\n"));
    }

    DEBUG ((DEBUG_INFO, "%a: alarm at day %x %x:%x\n", __FUNCTION__, Buffer[3], Buffer[2], Buffer[1]));
    if (!(Buffer[1] & RA8900CE_ALARM_REG_AE)) {
      Time->Minute = BcdToDecimal8 (Buffer[1]) & RA8900CE_MINUTES_MASK;
    }

    if (!(Buffer[2] & RA8900CE_ALARM_REG_AE)) {
      Time->Hour = BcdToDecimal8 (Buffer[2]) & RA8900CE_HOURS_MASK;
    }

    if (!(Buffer[3] & RA8900CE_ALARM_REG_AE)) {
      Time->Day = BcdToDecimal8 (Buffer[3]) & RA8900CE_DAYS_MASK;
    }

    Buffer[0] = RA8900CE_MONTH_REG_OFFSET;

    GetOp.OperationCount = 2;

    GetOp.SetAddressOp.Flags         = 0;
    GetOp.SetAddressOp.LengthInBytes = 1;
    GetOp.SetAddressOp.Buffer        = &Buffer[0];

    GetOp.GetDateTimeOp.Flags         = I2C_FLAG_READ;
    GetOp.GetDateTimeOp.LengthInBytes = 2;
    GetOp.GetDateTimeOp.Buffer        = (VOID *)&Buffer[1];

    Status = I2cMasterXfer (mHost, SLAVE_ADDRESS, (VOID *)&GetOp);
    if (!EFI_ERROR (Status)) {
      Time->Month = BcdToDecimal8 (Buffer[1] & RA8900CE_MONTHS_MASK);
      Time->Year  = BcdToDecimal8 (Buffer[2] & RA8900CE_YEARS_MASK);
    } else {
      DEBUG ((DEBUG_INFO, "Get RTC month and year error!\n"));
      return EFI_DEVICE_ERROR;
    }
  }

  return Status;
}

/**
  Sets the system wakeup alarm clock time.

  @param  Enabled               Enable or disable the wakeup alarm.
  @param  Time                  If Enable is TRUE, the time to set the wakeup
                                alarm for.

  @retval EFI_SUCCESS           If Enable is TRUE, then the wakeup alarm was
                                enabled. If Enable is FALSE, then the wakeup
                                alarm was disabled.
  @retval EFI_INVALID_PARAMETER A time field is out of range.
  @retval EFI_DEVICE_ERROR      The wakeup time could not be set due to a
                                hardware error.
  @retval EFI_UNSUPPORTED       A wakeup timer is not supported on this
                                platform.

**/
EFI_STATUS
EFIAPI
LibSetWakeupTime (
  IN BOOLEAN    Enabled,
  OUT EFI_TIME  *Time
  )
{
  EFI_STATUS           Status;
  RTC_SET_I2C_REQUEST  SetOp;
  RTC_GET_I2C_REQUEST  GetOp;
  UINT8                Buffer[4];

  if (Enabled != FALSE) {
    if ((Time == NULL) || !IsTimeValid (Time)) {
      return EFI_INVALID_PARAMETER;
    }

    // set alarm time
    Buffer[0] = RA8900CE_ALARM_REG_OFFSET;
    Buffer[1] = DecimalToBcd8 (Time->Minute) & ~RA8900CE_ALARM_REG_AE;
    Buffer[2] = DecimalToBcd8 (Time->Hour) & ~RA8900CE_ALARM_REG_AE;
    Buffer[3] = DecimalToBcd8 (Time->Day) & ~RA8900CE_ALARM_REG_AE;

    DEBUG ((DEBUG_INFO, "%a: alarm at day %x %x:%x\n", __FUNCTION__, Buffer[3], Buffer[2], Buffer[1]));
    SetOp.OperationCount             = 1;
    SetOp.Operation[0].Flags         = 0;
    SetOp.Operation[0].LengthInBytes = 4;
    SetOp.Operation[0].Buffer        = (VOID *)&Buffer[0];

    Status = I2cMasterXfer (mHost, SLAVE_ADDRESS, (VOID *)&SetOp);
    if (EFI_ERROR (Status)) {
      return EFI_DEVICE_ERROR;
    }

    // Ra8900 just support week hour day and minute, so we init time
    if (IsZeroBuffer (ZeroMem (Time, sizeof (EFI_TIME)), sizeof (EFI_TIME)) == FALSE) {
      DEBUG ((DEBUG_INFO, "Time can't reset\n"));
    } else {
      // Set Time:
      // We use realtime as Time->Year and Time->Monty,
      // and we use set time as Day, Hour and Minute.
      // Then the other filed in Time is zero.
      Time->Minute = BcdToDecimal8 (Buffer[1] & RA8900CE_MINUTES_MASK);
      Time->Hour   = BcdToDecimal8 (Buffer[2] & RA8900CE_HOURS_MASK);
      Time->Day    = BcdToDecimal8 (Buffer[3] & RA8900CE_DAYS_MASK);

      // Get realtime as Time->Year and Time->Month
      Buffer[0]            = RA8900CE_MONTH_REG_OFFSET;
      GetOp.OperationCount = 2;

      GetOp.SetAddressOp.Flags         = 0;
      GetOp.SetAddressOp.LengthInBytes = 1;
      GetOp.SetAddressOp.Buffer        = &Buffer[0];

      GetOp.GetDateTimeOp.Flags         = I2C_FLAG_READ;
      GetOp.GetDateTimeOp.LengthInBytes = 2;
      GetOp.GetDateTimeOp.Buffer        = (VOID *)&Buffer[1];

      Status = I2cMasterXfer (mHost, SLAVE_ADDRESS, (VOID *)&GetOp);
      if (!EFI_ERROR (Status)) {
        Time->Month = BcdToDecimal8 (Buffer[1] & RA8900CE_MONTHS_MASK);
        Time->Year  = BcdToDecimal8 (Buffer[2] & RA8900CE_YEARS_MASK);
      } else {
        DEBUG ((DEBUG_INFO, "Get RTC month and year error!\n"));
        return EFI_DEVICE_ERROR;
      }
    }

    // Set WADA
    Buffer[0] = RA8900CE_EXT_REG_OFFSET;

    GetOp.OperationCount = 2;

    GetOp.SetAddressOp.Flags         = 0;
    GetOp.SetAddressOp.LengthInBytes = 1;
    GetOp.SetAddressOp.Buffer        = &Buffer[0];

    GetOp.GetDateTimeOp.Flags         = I2C_FLAG_READ;
    GetOp.GetDateTimeOp.LengthInBytes = 1;
    GetOp.GetDateTimeOp.Buffer        = (VOID *)&Buffer[1];

    Status = I2cMasterXfer (mHost, SLAVE_ADDRESS, (VOID *)&GetOp);
    if (!EFI_ERROR (Status)) {
      Buffer[1] |= RA8900CE_EXT_REG_WADA;

      SetOp.OperationCount             = 1;
      SetOp.Operation[0].Flags         = 0;
      SetOp.Operation[0].LengthInBytes = 2;
      SetOp.Operation[0].Buffer        = (VOID *)&Buffer[0];

      Status = I2cMasterXfer (mHost, SLAVE_ADDRESS, (VOID *)&SetOp);
    }
  }

  // Set AIE
  Buffer[0] = RA8900CE_CTRL_REG_OFFSET;

  GetOp.OperationCount = 2;

  GetOp.SetAddressOp.Flags         = 0;
  GetOp.SetAddressOp.LengthInBytes = 1;
  GetOp.SetAddressOp.Buffer        = &Buffer[0];

  GetOp.GetDateTimeOp.Flags         = I2C_FLAG_READ;
  GetOp.GetDateTimeOp.LengthInBytes = 1;
  GetOp.GetDateTimeOp.Buffer        = (VOID *)&Buffer[1];

  Status = I2cMasterXfer (mHost, SLAVE_ADDRESS, (VOID *)&GetOp);

  if (!EFI_ERROR (Status)) {
    if (Enabled) {
      Buffer[1] |= RA8900CE_CTRL_REG_AIE;
    } else {
      Buffer[1] &= ~RA8900CE_CTRL_REG_AIE;
    }

    SetOp.OperationCount             = 1;
    SetOp.Operation[0].Flags         = 0;
    SetOp.Operation[0].LengthInBytes = 2;
    SetOp.Operation[0].Buffer        = (VOID *)&Buffer[0];

    Status = I2cMasterXfer (mHost, SLAVE_ADDRESS, (VOID *)&SetOp);
  }

  return Status;
}

VOID
EFIAPI
I2cMemoryInitEventNotify (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS  Status;
  UINTN       RuntimeMmioRegionBase;
  UINTN       RuntimeMmioRegionSize;

  if (Context == NULL) {
    return;
  }

  // to cover I2C host MMIO space
  RuntimeMmioRegionBase = ((I2C_HOST_DESCRIPTOR *)Context)->MemBase;
  RuntimeMmioRegionSize = SIZE_4KB;

  Status = gDS->AddMemorySpace (
                  EfiGcdMemoryTypeMemoryMappedIo,
                  RuntimeMmioRegionBase,
                  RuntimeMmioRegionSize,
                  EFI_MEMORY_UC | EFI_MEMORY_RUNTIME
                  );
  if (EFI_ERROR (Status)) {
    DebugPrint (DEBUG_ERROR, "%a: fail to add memory space base 0x%x, size 0x%x, status %r\n", __FUNCTION__, RuntimeMmioRegionBase, RuntimeMmioRegionSize, Status);
    ASSERT_EFI_ERROR (Status);
  }

  DEBUG ((DEBUG_INFO, "%a: add memory space base 0x%x, length 0x%x\n", __FUNCTION__, RuntimeMmioRegionBase, RuntimeMmioRegionSize));

  Status = gDS->SetMemorySpaceAttributes (
                  RuntimeMmioRegionBase,
                  RuntimeMmioRegionSize,
                  EFI_MEMORY_UC | EFI_MEMORY_RUNTIME
                  );
  if (EFI_ERROR (Status)) {
    DebugPrint (DEBUG_ERROR, "%a: fail to set memory attributes base 0x%x, size 0x%x, status %r\n", __FUNCTION__, RuntimeMmioRegionBase, RuntimeMmioRegionSize, Status);
    gDS->RemoveMemorySpace (
           RuntimeMmioRegionBase,
           RuntimeMmioRegionSize
           );
    ASSERT_EFI_ERROR (Status);
  }

  DEBUG ((DEBUG_INFO, "%a: set memory space attribute 0x%llx\n", __FUNCTION__, EFI_MEMORY_UC | EFI_MEMORY_RUNTIME));
}

STATIC
EFI_STATUS
RtcDeviceInitialize (
  IN  VOID
  )
{
  EFI_STATUS                         Status;
  UINTN                              BufferSize;
  EFI_I2C_MASTER_PROTOCOL            *I2cMaster;
  UINTN                              BusFrequency = 100000; // 100KHz by default
  RTC_SET_I2C_REQUEST                SetOp;
  RTC_GET_I2C_REQUEST                GetOp;
  UINT8                              InitSeq[4];
  EFI_DEVICE_PATH_PROTOCOL           *DevicePath;
  CONFIG_PARAMS_DATA_BLOCK           *ConfigData;
  CIX_CONFIG_PARAMS_MANAGE_PROTOCOL  *ConfigManage;

  //
  // Find the handle that marks the controller
  // that will provide the I2C master protocol.
  //
  BufferSize = sizeof (EFI_HANDLE);
  Status     = gBS->LocateHandle (
                      ByProtocol,
                      &gRa8900ceRealTimeClockLibI2cMasterProtocolGuid,
                      NULL,
                      &BufferSize,
                      &mI2cMasterHandle
                      );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->HandleProtocol (
                  mI2cMasterHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **)&DevicePath
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->LocateProtocol (
                    &gCixConfigParamsManageProtocolGuid,
                    NULL,
                    (VOID **)&ConfigManage
                    );

    if (!EFI_ERROR (Status)) {
      ConfigData   = ConfigManage->Data;
      BusFrequency = ConfigData->Fch.I2c[((I2C_DEVICE_PATH *)DevicePath)->Bus].BusFreq;
    } else {
      DEBUG ((DEBUG_ERROR, "%a: config parameters is invalid %r\n", __FUNCTION__, Status));
      return Status;
    }

    mHost          = AllocateRuntimeZeroPool (sizeof (I2C_HOST_DESCRIPTOR));
    mHost->MemBase = I2cGetMemBase (((I2C_DEVICE_PATH *)DevicePath)->Bus);
  } else {
    DEBUG ((DEBUG_ERROR, "%a: device path is invalid %r\n", __FUNCTION__, Status));
    return Status;
  }

  Status = gBS->OpenProtocol (
                  mI2cMasterHandle,
                  &gEfiI2cMasterProtocolGuid,
                  (VOID **)&I2cMaster,
                  gImageHandle,
                  NULL,
                  EFI_OPEN_PROTOCOL_EXCLUSIVE
                  );
  ASSERT_EFI_ERROR (Status);

  Status = I2cMaster->Reset (I2cMaster);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: I2C Master Reset failed - %r\n", __FUNCTION__, Status));
    return Status;
  }

  Status = I2cMaster->SetBusFrequency (I2cMaster, &BusFrequency);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: I2C Master Set Bus Frequency failed - %r\n", __FUNCTION__, Status));
    return Status;
  }

  // Initialize Ra8900ce
  InitSeq[0] = RA8900CE_EXT_REG_OFFSET;

  GetOp.OperationCount = 2;

  GetOp.SetAddressOp.Flags         = 0;
  GetOp.SetAddressOp.LengthInBytes = 1;
  GetOp.SetAddressOp.Buffer        = &InitSeq[0];

  GetOp.GetDateTimeOp.Flags         = I2C_FLAG_READ;
  GetOp.GetDateTimeOp.LengthInBytes = 3;
  GetOp.GetDateTimeOp.Buffer        = (VOID *)&InitSeq[1];

  Status = I2cMasterXfer (mHost, SLAVE_ADDRESS, (VOID *)&GetOp);
  if (!EFI_ERROR (Status)) {
    InitSeq[1] &= ~(RA8900CE_EXT_REG_TE | RA8900CE_EXT_REG_TEST);                           // TE, TEST set 0
    InitSeq[2] &= ~(RA8900CE_FLAG_REG_VDET | RA8900CE_FLAG_REG_AF | RA8900CE_FLAG_REG_VLF); // AF, VDET, VLF set 0
    InitSeq[3] &= ~(RA8900CE_CTRL_REG_TIE | RA8900CE_CTRL_REG_UIE);                         // AIE, TIE, UIE set 0

    SetOp.OperationCount             = 1;
    SetOp.Operation[0].Flags         = 0;
    SetOp.Operation[0].LengthInBytes = 4;
    SetOp.Operation[0].Buffer        = (VOID *)&InitSeq[0];

    Status = I2cMasterXfer (mHost, SLAVE_ADDRESS, (VOID *)&SetOp);
  }

  return Status;
}

/**
  Fixup internal data so that EFI can be call in virtual mode.
  Call the passed in Child Notify event and convert any pointers in
  lib to virtual mode.

  @param[in]    Event   The Event that is being processed
  @param[in]    Context Event Context
**/
VOID
EFIAPI
LibRtcVirtualNotifyEvent (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EfiConvertPointer (0x0, (VOID **)&mHost->MemBase);
  EfiConvertPointer (0x0, (VOID **)&mHost);
}

VOID
EFIAPI
LibRtcExitBootServicesNotifyEvent (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  mI2cLibAtRuntime = TRUE;
}

/**
  Library entry point

  @param  ImageHandle           Handle that identifies the loaded image.
  @param  SystemTable           System Table for this image.

  @retval EFI_SUCCESS           The operation completed successfully.

**/
EFI_STATUS
EFIAPI
LibRtcInitialize (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RtcDeviceInitialize ();

  EfiCreateProtocolNotifyEvent (
    &gEfiCpuArchProtocolGuid,
    TPL_CALLBACK,
    I2cMemoryInitEventNotify,
    mHost,
    &mRtcMemoryInitEvent
    );

  Status = gBS->CreateEvent (
                  EVT_SIGNAL_EXIT_BOOT_SERVICES,
                  TPL_CALLBACK,
                  LibRtcExitBootServicesNotifyEvent,
                  NULL,
                  &mRtcExitBootServicesEvent
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register for the virtual address change event
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  LibRtcVirtualNotifyEvent,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &mRtcVirtualAddrChangeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
