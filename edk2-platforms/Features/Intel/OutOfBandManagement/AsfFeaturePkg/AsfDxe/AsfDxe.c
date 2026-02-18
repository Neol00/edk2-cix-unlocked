/** @file
  Asf Dxe driver which is used for sending event record log to NIC or receiving
  boot option command from NIC and provide in Asf Dxe protocol.

  Copyright (c) 1985 - 2022, AMI. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <AsfDxe.h>

ASF_BOOT_OPTION           gAsfBootOption = { 0, 0, 0, 0, 0, 0, 0 };
ASF_PROTOCOL              gAsfProtocol   = { AsfPushEvent, NULL };
EFI_SMBUS_DEVICE_ADDRESS  mFixedTargetAddress;
EFI_SMBUS_HC_PROTOCOL     *mSmBus = NULL;

/**
  Send message through SmBus to lan card.

  @param[in] Command      Command of System Firmware Events.
  @param[in] Length       Length of the data in bytes.
  @param[in] AsfEvent     Message data.

  @retval EFI_SUCCESS     Push Event successfully.
  @retval Others          Push Event error.
**/
EFI_STATUS
EFIAPI
AsfPushEvent  (
  IN  UINT8  Command,
  IN  UINTN  Length,
  IN  UINT8  *AsfEvent
  )
{
  EFI_STATUS  Status;

  if (mSmBus == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = mSmBus->Execute (
                     mSmBus,
                     mFixedTargetAddress,
                     Command,
                     EfiSmbusWriteBlock,
                     TRUE,
                     &Length,
                     AsfEvent
                     );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "AsfPushEvent Status = %r\n", Status));
  }

  return Status;
}

/**
  This function pushes the System Firmware State Events.

  @param[in] SystemState    System Firmware State.

**/
VOID
EFIAPI
AsfPushSystemState  (
  IN  UINT8  SystemState
  )
{
  mAsfSystemState.EventSensorType = SystemState;
  AsfPushEvent (
    mAsfSystemState.Command,
    mAsfSystemState.ByteCount,
    (UINT8 *)&(mAsfSystemState.SubCommand)
    );
  return;
}

/**
  This function processes the System Firmware Progress/Error Events.

  @param[in] MessageErrorLevel   Progress or error or system management message Type.
  @param[in] MessageType         Specific ASF message type.

**/
VOID
EFIAPI
AsfPushSystemErrorProgressEvent  (
  IN UINT32            MessageErrorLevel,
  IN ASF_MESSAGE_TYPE  MessageType
  )
{
  UINTN  i;

  if ((MessageErrorLevel & PcdGet32 (PcdAsfMessageErrorLevel)) ||
      ((gAsfBootOption.BootOptionBit & ASF_BOP_BIT_FORCE_PROGRESS_EVENT)))
  {
    for ( i = 0; i < mAsfMessagesSize; i++ ) {
      if ( mAsfMessages[i].Type == MessageType ) {
        AsfPushEvent (
          mAsfMessages[i].Message.Command,
          mAsfMessages[i].Message.ByteCount,
          (UINT8 *)&(mAsfMessages[i].Message.SubCommand)
          );
        break;
      }
    }
  }

  return;
}

/**
  Send relate progress or error message to lan card

  @param[in]  CodeType         Indicates the type of status code being reported.
  @param[in]  Value            Describes the current status of a hardware or software entity.
                               This included information about the class and subclass that is used to
                               classify the entity as well as an operation.
  @param[in]  Instance         The enumeration of a hardware or software entity within
                               the system. Valid instance numbers start with 1.
  @param[in]  CallerId         This optional parameter may be used to identify the caller.
                               This parameter allows the status code driver to apply different rules to
                               different callers.
  @param[in]  Data             This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS          Reported all the progress and error codes for Asf successfully.
**/
EFI_STATUS
EFIAPI
AsfRscHandlerCallback (
  IN EFI_STATUS_CODE_TYPE   CodeType,
  IN EFI_STATUS_CODE_VALUE  Value,
  IN UINT32                 Instance,
  IN EFI_GUID               *CallerId,
  IN EFI_STATUS_CODE_DATA   *Data
  )
{
  UINTN  Index;

  if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_PROGRESS_CODE) {
    for (Index = 0; Index < mMsgProgressMapSize; Index++) {
      if (mMsgProgressMap[Index].StatusCode == Value) {
        AsfPushSystemErrorProgressEvent (MESSAGE_ERROR_LEVEL_PROGRESS, mMsgProgressMap[Index].MessageType);
        break;
      }
    }
  }

  if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE) {
    for ( Index = 0; Index < mMsgErrorMapSize; Index++ ) {
      if ( mMsgErrorMap[Index].StatusCode == Value ) {
        AsfPushSystemErrorProgressEvent (MESSAGE_ERROR_LEVEL_ERROR, mMsgErrorMap[Index].MessageType);
        break;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  This function issues the ASF Get/Clear Boot Option command.

  @param[in]  AsfSlaveAddress  Specify the Dash lan SmBus slave address.

  @retval EFI_SUCCESS Reported all the progress and error codes for Asf successfully.
  @retval Others      Smbus Execute function return error.
**/
EFI_STATUS
EFIAPI
AsfGetBootOption (
  IN  EFI_SMBUS_DEVICE_ADDRESS  AsfSlaveAddress
  )
{
  EFI_STATUS       Status;
  UINTN            Length = sizeof (ASF_BOOT_OPTION);
  ASF_BOOT_OPTION  BootOption;

  // Initialize get boot option Buffer.
  SetMem (&BootOption, sizeof (ASF_BOOT_OPTION), 0);

  // Execute ASFMSG_CMD_CONFIG command.
  Status = mSmBus->Execute (
                     mSmBus,
                     AsfSlaveAddress,
                     ASFMSG_CMD_CONFIG,
                     EfiSmbusReadBlock,
                     TRUE,
                     &Length,
                     &BootOption
                     );
  if ( EFI_ERROR (Status)) {
    return Status;
  }

  if ( BootOption.SubCommand == ASFMSG_SUBCMD_RET_BOOT_OPTION ) {
    // copy Return Boot Option to global ASF Boot Option buffer.
    CopyMem (&gAsfBootOption, &BootOption, sizeof (ASF_BOOT_OPTION));
    gAsfProtocol.BootOption = &gAsfBootOption;
    // Execute Clear Boot Option command.
    BootOption.SubCommand = ASFMSG_SUBCMD_CLR_BOOT_OPTION;
    BootOption.Version    = 0x10;
    Length                = 2;
    mSmBus->Execute (
              mSmBus,
              AsfSlaveAddress,
              ASFMSG_CMD_CONFIG,
              EfiSmbusWriteBlock,
              TRUE,
              &Length,
              &BootOption
              );
  }

  return Status;
}

/**
  This Event Callback processes the requests at EFI Ready to Boot Event triggered.

  @param[in]  Event      A pointer to the Event that triggered the callback.
  @param[in]  Context    A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
AsfReadyToBootEvent (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  // Push System State S0 - "Working".
  AsfPushSystemState (ASFMSG_SYSTEM_STATE_S0);

  gBS->CloseEvent (Event);
  return;
}

/**
    Register callback if Acpi protocol is not ready, else install ASF acpi table directly.

**/
VOID
EFIAPI
InstallAsfAcpiTable (
  VOID
  )
{
  EFI_STATUS               Status;
  EFI_EVENT                Event;
  VOID                     *Registration;
  EFI_ACPI_TABLE_PROTOCOL  *AcpiTableProtocol;

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTableProtocol);
  if (!EFI_ERROR (Status)) {
    InstallAsfAcpiTableEvent (NULL, NULL);
  } else {
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    InstallAsfAcpiTableEvent,
                    NULL,
                    &Event
                    );

    if (!EFI_ERROR (Status)) {
      Status = gBS->RegisterProtocolNotify (
                      &gEfiAcpiTableProtocolGuid,
                      Event,
                      &Registration
                      );

      if (EFI_ERROR (Status)) {
        gBS->CloseEvent (Event);
      }
    }
  }

  return;
}

/**
  This is the standard EFI driver entry point for DXE phase of ASF.

  @param[in] ImageHandle   Image handle of the loaded driver
  @param[in] SystemTable   Pointer to the System Table

  @retval EFI_SUCCESS      This driver initial correctly
  @retval Others           This driver initial fail
**/
EFI_STATUS
EFIAPI
AsfDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                Status;
  EFI_RSC_HANDLER_PROTOCOL  *RscHandler;
  EFI_EVENT                 AsfEfiReadyToBootEvent;

  Status = gBS->LocateProtocol (&gEfiSmbusHcProtocolGuid, NULL, (VOID **)&mSmBus);
  if ( EFI_ERROR (Status)) {
    return Status;
  }

  mFixedTargetAddress.SmbusDeviceAddress = PcdGet8 (PcdSmbusSlaveAddressForDashLan) >> 1;
  if (mFixedTargetAddress.SmbusDeviceAddress == 0) {
    return EFI_UNSUPPORTED;
  }

  Status = AsfGetBootOption (mFixedTargetAddress);
  if ( EFI_ERROR (Status)) {
    return Status;
  }

  InstallAsfAcpiTable ();

  // Send mother board initialization message.
  AsfPushSystemErrorProgressEvent (MESSAGE_ERROR_LEVEL_PROGRESS, MsgMotherBoardInit);

  Status = gBS->LocateProtocol (&gEfiRscHandlerProtocolGuid, NULL, (VOID **)&RscHandler);
  if (!EFI_ERROR (Status)) {
    RscHandler->Register (AsfRscHandlerCallback, TPL_CALLBACK);
  }

  EfiCreateEventReadyToBootEx (
    TPL_CALLBACK,
    AsfReadyToBootEvent,
    NULL,
    &AsfEfiReadyToBootEvent
    );

  gBS->InstallProtocolInterface (
         &ImageHandle,
         &gAsfProtocolGuid,
         EFI_NATIVE_INTERFACE,
         &gAsfProtocol
         );

  return EFI_SUCCESS;
}
