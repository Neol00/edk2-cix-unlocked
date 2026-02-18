/** @file
*
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
*  Copyright (c) 2013-2018, ARM Limited. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#include <PiDxe.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/ArmGenericTimerCounterLib.h>
#include <Library/ArmSmcLib.h>
#include <IndustryStandard/ArmStdSmc.h>
#include <Library/HwHarvestLib.h>

#include <Protocol/HardwareInterrupt2.h>
#include <Protocol/WatchdogTimer.h>
#include <Protocol/ConfigParamsManageProtocol.h>

#include "GenericWatchdog.h"

/* The number of 100ns periods (the unit of time passed to these functions)
   in a second */
#define TIME_UNITS_PER_SECOND  10000000

// Tick frequency of the generic timer basis of the generic watchdog.
STATIC UINTN  mTimerFrequencyHz = 0;

/* In cases where the compare register was set manually, information about
   how long the watchdog was asked to wait cannot be retrieved from hardware.
   It is therefore stored here. 0 means the timer is not running. */
STATIC UINT64  mNumTimerTicks = 0;

STATIC EFI_HARDWARE_INTERRUPT2_PROTOCOL  *mInterruptProtocol;
STATIC EFI_WATCHDOG_TIMER_NOTIFY         mWatchdogNotify;

STATIC
EFI_STATUS
EFIAPI
ResetSystemEarly (
  )
{
  ARM_SMC_ARGS  ArmSmcArgs;

      // Send a PSCI 0.2 SYSTEM_RESET command
  ArmSmcArgs.Arg0 = ARM_SMC_ID_PSCI_SYSTEM_RESET;


  ArmCallSmc (&ArmSmcArgs);

  // We should never be here
  DEBUG ((DEBUG_ERROR, "%a: PSCI Reset failed\n", __FUNCTION__));
  CpuDeadLoop ();
  return EFI_UNSUPPORTED;
}

STATIC
VOID
WatchdogWriteOffsetRegister (
  UINT64  Value
  )
{
  MmioWrite32 (GENERIC_WDOG_OFFSET_REG, Value & MAX_UINT32);
  MmioWrite32 (GENERIC_WDOG_OFFSET_REG_HIGH, (Value >> 32) & MAX_UINT32);
}


VOID
WatchdogWriteCompareRegister (
  UINT64  Value
  )
{
  MmioWrite32 (GENERIC_WDOG_COMPARE_VALUE_REG_LOW, Value & MAX_UINT32);
  MmioWrite32 (GENERIC_WDOG_COMPARE_VALUE_REG_HIGH, (Value >> 32) & MAX_UINT32);
}

STATIC
VOID
WatchdogEnable (
  VOID
  )
{
  MmioWrite32 (GENERIC_WDOG_CONTROL_STATUS_REG, GENERIC_WDOG_ENABLED);
}

STATIC
VOID
WatchdogDisable (
  VOID
  )
{
  MmioWrite32 (GENERIC_WDOG_CONTROL_STATUS_REG, GENERIC_WDOG_DISABLED);
  MmioWrite32 (GENERIC_WDOG_OFFSET_REG, 0);
  MmioWrite32 (GENERIC_WDOG_OFFSET_REG_HIGH, 0);
}

/** On exiting boot services we must make sure the Watchdog Timer
    is stopped.
**/
STATIC
VOID
EFIAPI
WatchdogExitBootServicesEvent (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  WatchdogDisable ();
  mNumTimerTicks = 0;
}

/* This function is called when the watchdog's first signal (WS0) goes high.
   It uses the ResetSystem Runtime Service to reset the board.
*/
STATIC
VOID
EFIAPI
WatchdogInterruptHandler (
  IN  HARDWARE_INTERRUPT_SOURCE  Source,
  IN  EFI_SYSTEM_CONTEXT         SystemContext
  )
{
  // STATIC CONST CHAR16  ResetString[] = L"The generic watchdog timer ran out.";
  UINT64               TimerPeriod;

  WatchdogDisable ();

  mInterruptProtocol->EndOfInterrupt (mInterruptProtocol, Source);

  //
  // The notify function should be called with the elapsed number of ticks
  // since the watchdog was armed, which should exceed the timer period.
  // We don't actually know the elapsed number of ticks, so let's return
  // the timer period plus 1.
  //
  if (mWatchdogNotify != NULL) {
    TimerPeriod = ((TIME_UNITS_PER_SECOND / mTimerFrequencyHz) * mNumTimerTicks);
    mWatchdogNotify (TimerPeriod + 1);
  }
#ifdef REBOOT_REASON_ENABLE
  SET_REBOOT_REASON(WatchDogInterruptTrigger);
#endif

  ResetSystemEarly();

  // If we got here then the reset didn't work
  ASSERT (FALSE);
}

/**
  This function registers the handler NotifyFunction so it is called every time
  the watchdog timer expires.  It also passes the amount of time since the last
  handler call to the NotifyFunction.
  If NotifyFunction is not NULL and a handler is not already registered,
  then the new handler is registered and EFI_SUCCESS is returned.
  If NotifyFunction is NULL, and a handler is already registered,
  then that handler is unregistered.
  If an attempt is made to register a handler when a handler is already
  registered, then EFI_ALREADY_STARTED is returned.
  If an attempt is made to unregister a handler when a handler is not
  registered, then EFI_INVALID_PARAMETER is returned.

  @param  This             The EFI_TIMER_ARCH_PROTOCOL instance.
  @param  NotifyFunction   The function to call when a timer interrupt fires.
                           This function executes at TPL_HIGH_LEVEL. The DXE
                           Core will register a handler for the timer interrupt,
                           so it can know how much time has passed. This
                           information is used to signal timer based events.
                           NULL will unregister the handler.

  @retval EFI_UNSUPPORTED       The code does not support NotifyFunction.

**/
STATIC
EFI_STATUS
EFIAPI
WatchdogRegisterHandler (
  IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
  IN EFI_WATCHDOG_TIMER_NOTIFY         NotifyFunction
  )
{
  if ((mWatchdogNotify == NULL) && (NotifyFunction == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((mWatchdogNotify != NULL) && (NotifyFunction != NULL)) {
    return EFI_ALREADY_STARTED;
  }

  mWatchdogNotify = NotifyFunction;
  return EFI_SUCCESS;
}

/**
  This function sets the amount of time to wait before firing the watchdog
  timer to TimerPeriod 100ns units.  If TimerPeriod is 0, then the watchdog
  timer is disabled.

  @param  This             The EFI_WATCHDOG_TIMER_ARCH_PROTOCOL instance.
  @param  TimerPeriod      The amount of time in 100ns units to wait before
                           the watchdog timer is fired. If TimerPeriod is zero,
                           then the watchdog timer is disabled.

  @retval EFI_SUCCESS           The watchdog timer has been programmed to fire
                                in TimerPeriod 100ns units.

**/
STATIC
EFI_STATUS
EFIAPI
WatchdogSetTimerPeriod (
  IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
  IN UINT64                            TimerPeriod          // In 100ns units
  )
{
  // UINTN  SystemCount;

  // if TimerPeriod is 0, this is a request to stop the watchdog.
  if (TimerPeriod == 0) {
    mNumTimerTicks = 0;
    WatchdogDisable ();
    return EFI_SUCCESS;
  }

  // Work out how many timer ticks will equate to TimerPeriod
  mNumTimerTicks = (mTimerFrequencyHz * TimerPeriod) / TIME_UNITS_PER_SECOND;


  WatchdogWriteOffsetRegister (mNumTimerTicks);
  WatchdogEnable ();


  return EFI_SUCCESS;
}

/**
  This function retrieves the period of timer interrupts in 100ns units,
  returns that value in TimerPeriod, and returns EFI_SUCCESS.  If TimerPeriod
  is NULL, then EFI_INVALID_PARAMETER is returned.  If a TimerPeriod of 0 is
  returned, then the timer is currently disabled.

  @param  This             The EFI_TIMER_ARCH_PROTOCOL instance.
  @param  TimerPeriod      A pointer to the timer period to retrieve in
                           100ns units. If 0 is returned, then the timer is
                           currently disabled.


  @retval EFI_SUCCESS           The timer period was returned in TimerPeriod.
  @retval EFI_INVALID_PARAMETER TimerPeriod is NULL.

**/
STATIC
EFI_STATUS
EFIAPI
WatchdogGetTimerPeriod (
  IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
  OUT UINT64                           *TimerPeriod
  )
{
  if (TimerPeriod == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *TimerPeriod = ((TIME_UNITS_PER_SECOND / mTimerFrequencyHz) * mNumTimerTicks);

  return EFI_SUCCESS;
}

VOID
EFIAPI
GlobalWatchdogStatusSwitch(
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS Status;
  CONFIG_PARAMS_DATA_BLOCK           *ConfigData = NULL;
  CIX_CONFIG_PARAMS_MANAGE_PROTOCOL  *ConfigManage;

  // DebugPrint (DEBUG_ERROR, " %a enter\n", __FUNCTION__);
  Status = gBS->LocateProtocol (&gCixConfigParamsManageProtocolGuid, NULL, (VOID **)&ConfigManage);
  if (!EFI_ERROR (Status)) {
    ConfigData = ConfigManage->Data;
  } else {
    DebugPrint (DEBUG_ERROR, "%a LocateProtocol gCixConfigParamsManageProtocolGuid failed: %r\n", __FUNCTION__, Status);
  }

  if ((!ConfigData->S5.SocWatchdogTimer)||(!IsApWatchdogEnable ())) {
    // clear fuse

    // disable watchdog
    WatchdogDisable ();
    // unregister the watchdog handler
    Status = gBS->LocateProtocol (
                  &gHardwareInterrupt2ProtocolGuid,
                  NULL,
                  (VOID **)&mInterruptProtocol
                  );
    if (!EFI_ERROR (Status)) {
      mInterruptProtocol->RegisterInterruptSource (
                        mInterruptProtocol,
                        FixedPcdGet32 (PcdGenericWatchdogEl2IntrNum),
                        NULL
                        );
    }



  }
}

void
RegisterGlobalWatchdogCallback()
{
  EFI_STATUS  Status;
  EFI_EVENT   Event;
  VOID        *Registration;


  Status = gBS->CreateEvent (
                             EVT_NOTIFY_SIGNAL,
                             TPL_CALLBACK,
                             GlobalWatchdogStatusSwitch,
                             NULL,
                             &Event
                             );

  ASSERT_EFI_ERROR (Status);

  Status = gBS->RegisterProtocolNotify (
                                        &gCixSetupVarInitCompleteProtocolGuid,
                                        Event,
                                        &Registration
                                        );
}

/**
  Interface structure for the Watchdog Architectural Protocol.

  @par Protocol Description:
  This protocol provides a service to set the amount of time to wait
  before firing the watchdog timer, and it also provides a service to
  register a handler that is invoked when the watchdog timer fires.

  @par When the watchdog timer fires, control will be passed to a handler
  if one has been registered.  If no handler has been registered,
  or the registered handler returns, then the system will be
  reset by calling the Runtime Service ResetSystem().

  @param RegisterHandler
  Registers a handler that will be called each time the
  watchdogtimer interrupt fires.  TimerPeriod defines the minimum
  time between timer interrupts, so TimerPeriod will also
  be the minimum time between calls to the registered
  handler.
  NOTE: If the watchdog resets the system in hardware, then
        this function will not have any chance of executing.

  @param SetTimerPeriod
  Sets the period of the timer interrupt in 100ns units.
  This function is optional, and may return EFI_UNSUPPORTED.
  If this function is supported, then the timer period will
  be rounded up to the nearest supported timer period.

  @param GetTimerPeriod
  Retrieves the period of the timer interrupt in 100ns units.

**/
STATIC EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  mWatchdogTimer = {
  WatchdogRegisterHandler,
  WatchdogSetTimerPeriod,
  WatchdogGetTimerPeriod
};

STATIC EFI_EVENT  mEfiExitBootServicesEvent;

EFI_STATUS
EFIAPI
GenericWatchdogEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;

  Status = gBS->LocateProtocol (
                  &gHardwareInterrupt2ProtocolGuid,
                  NULL,
                  (VOID **)&mInterruptProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  /* Make sure the Watchdog Timer Architectural Protocol has not been installed
     in the system yet.
     This will avoid conflicts with the universal watchdog */
  ASSERT_PROTOCOL_ALREADY_INSTALLED (NULL, &gEfiWatchdogTimerArchProtocolGuid);

  mTimerFrequencyHz = ArmGenericTimerGetTimerFreq ();
  ASSERT (mTimerFrequencyHz != 0);

  // Install interrupt handler
  Status = mInterruptProtocol->RegisterInterruptSource (
                                 mInterruptProtocol,
                                 FixedPcdGet32 (PcdGenericWatchdogEl2IntrNum),
                                 WatchdogInterruptHandler
                                 );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = mInterruptProtocol->SetTriggerType (
                                 mInterruptProtocol,
                                 FixedPcdGet32 (PcdGenericWatchdogEl2IntrNum),
                                 EFI_HARDWARE_INTERRUPT2_TRIGGER_EDGE_RISING
                                 );
  if (EFI_ERROR (Status)) {
    goto UnregisterHandler;
  }

  // Install the Timer Architectural Protocol onto a new handle
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiWatchdogTimerArchProtocolGuid,
                  &mWatchdogTimer,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    goto UnregisterHandler;
  }

  // Register for an ExitBootServicesEvent
  Status = gBS->CreateEvent (
                  EVT_SIGNAL_EXIT_BOOT_SERVICES,
                  TPL_NOTIFY,
                  WatchdogExitBootServicesEvent,
                  NULL,
                  &mEfiExitBootServicesEvent
                  );
  ASSERT_EFI_ERROR (Status);

  mNumTimerTicks = 0;
  WatchdogDisable ();

  RegisterGlobalWatchdogCallback();

  return EFI_SUCCESS;

UnregisterHandler:
  // Unregister the handler
  mInterruptProtocol->RegisterInterruptSource (
                        mInterruptProtocol,
                        FixedPcdGet32 (PcdGenericWatchdogEl2IntrNum),
                        NULL
                        );
  return Status;
}
