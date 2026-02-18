#/** @file
#
#  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
#
#**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

EFI_STATUS
EFIAPI
SampleDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  // Before your function execute  , enable the Watchdog Timer for  the 10 seconds period
  gBS->SetWatchdogTimer (10, 0x0000, 0x00, NULL);

  // Your function start
  // Status = YourFunction ( );
  // Your function end

  // Clear the Watchdog Timer after function execute
  gBS->SetWatchdogTimer (0x0000, 0x0000, 0x0000, NULL);

  return Status;
}
