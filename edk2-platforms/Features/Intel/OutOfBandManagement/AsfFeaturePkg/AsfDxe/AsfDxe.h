/** @file
  Asf Dxe driver which is used for sending event record log to NIC or receiving
  boot option command from NIC and provide in Asf Dxe protocol.

  Copyright (c) 1985 - 2022, AMI. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef __ASF_DXE_H__
#define __ASF_DXE_H__

#include <Pi/PiStatusCode.h>
#include <Protocol/AsfProtocol.h>
#include <IndustryStandard/SmBus.h>
#include <Protocol/SmbusHc.h>
#include <Protocol/ReportStatusCodeHandler.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Protocol/AcpiTable.h>
#include <AsfMessages.h>

extern MESSAGE_DATA_HUB_MAP     mMsgProgressMap[];
extern MESSAGE_DATA_HUB_MAP     mMsgErrorMap[];
extern ASF_MESSAGE              mAsfMessages[];
extern UINTN                    mMsgProgressMapSize;
extern UINTN                    mMsgErrorMapSize;
extern UINTN                    mAsfMessagesSize;
extern ASF_MSG_NORETRANSMIT     mAsfSystemState;

/**
  This function pushes the DXE System Firmware Events.

  @param[in] Command      Command of System Firmware Events.
  @param[in] Length       Length of the data in bytes.
  @param[in] AsfEvent     System Firmware Events Command.

  @retval EFI_SUCCESS     Push Event successfully.
  @retval EFI_UNSUPPORTED Push Event error.
**/
EFI_STATUS
EFIAPI
AsfPushEvent  (
  IN  UINT8  Command,
  IN  UINTN  Length,
  IN  UINT8  *AsfEvent
  );

/**
  This function install the ASF acpi Table.

  @param[in]  Event     A pointer to the Event that triggered the callback.
  @param[in]  Context   A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
InstallAsfAcpiTableEvent  (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  );

#endif //__ASF_DXE_H__
