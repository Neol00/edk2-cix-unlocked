#/** @file
#
#  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
#
#**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/HwHarvestLib.h>
#include <Library/CixPostCodeLib.h>
#include <Protocol/WatchdogTimer.h>

#define GLOBAL_WDT_DEFAULT_TIME 20 //seconds

VOID
EFIAPI
WatchdogTimerTestHandler (
  IN UINT64  Time
  )
{
  TimeStampPrint();
  DebugPrint (DEBUG_INFO, "Global Watchdog Timer Expired: %ld\nResetting...\n", Time);
}

VOID
EFIAPI
GlobalWatchdogCloseNotify (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                        Status = EFI_SUCCESS;
  EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *WDT   = NULL;
  Status = gBS->LocateProtocol (
                                &gEfiWatchdogTimerArchProtocolGuid,
                                NULL,
                                (VOID **)&WDT
                                );
  if (!EFI_ERROR (Status)) {
      WDT->SetTimerPeriod (WDT, 0);
  }

}


static EFI_EVENT  WdtReadyToBootEvent;

EFI_STATUS
EFIAPI
GlobalWatchdogDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                        Status = EFI_SUCCESS;
  EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *WDT   = NULL;

#ifndef NDEBUG
  DebugPrint (DEBUG_INFO, "Global Watchdog not work if debug mode.\n");
  return Status;
#endif


  if(!IsApWatchdogEnable()){
    DebugPrint (DEBUG_INFO, "Glocal Watchdog is disabled.\n");
    return Status;
  }


  Status = gBS->LocateProtocol (
                                &gEfiWatchdogTimerArchProtocolGuid,
                                NULL,
                                (VOID **)&WDT
                                );

  if (Status != EFI_SUCCESS) {
    DebugPrint (DEBUG_INFO, "Glocal Watchdog set failed.\n");
    return Status;
  }

  Status = WDT->RegisterHandler (
                                 WDT,
                                 WatchdogTimerTestHandler
                                 );
  if (Status != EFI_SUCCESS) {
    DebugPrint (DEBUG_INFO, "Glocal Watchdog set failed.\n");
    return Status;
  }

  /* Set watchdog timer to 3 seconds */
  Status = WDT->SetTimerPeriod (WDT, GLOBAL_WDT_DEFAULT_TIME*10000000);
  if (Status != EFI_SUCCESS) {
    DebugPrint (DEBUG_INFO, "Glocal Watchdog set failed.\n");
    return Status;
  }
  TimeStampPrint();
  DebugPrint (DEBUG_INFO, "Glocal Watchdog %d(s) set success\n",GLOBAL_WDT_DEFAULT_TIME);


  //ready to boot disable global watchdog
  Status = gBS->CreateEventEx (
                               EVT_NOTIFY_SIGNAL,
                               TPL_CALLBACK,
                               GlobalWatchdogCloseNotify,
                               NULL,
                               &gEfiEventReadyToBootGuid,
                               &WdtReadyToBootEvent
                               );

  return Status;
}
