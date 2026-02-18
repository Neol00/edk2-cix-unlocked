/** @file
  Asf Pei Initialization Driver.

  Follow Asf spec to send progress or error message to Smbus device

  Copyright (c) 1985 - 2022, AMI. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <AsfMessages.h>
#include <Base.h>
#include <Ppi/ReportStatusCodeHandler.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/Smbus2.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Pi/PiStatusCode.h>

/**
  This Event Notify processes the ASF request at Memory Initial Completed.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] NotifyDescriptor     The notification structure this PEIM registered on install.
  @param[in] Ppi                  The memory discovered PPI.  Not used.

  @retval EFI_SUCCESS             Succeeds.
  @retval EFI_UNSUPPORTED         Push Event error.

**/
EFI_STATUS
EFIAPI
MsgMemoryInitCompleted (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mMemoryDiscoveredNotifyDes = {
  EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gEfiPeiMemoryDiscoveredPpiGuid,
  MsgMemoryInitCompleted
};

ASF_MSG_NORETRANSMIT  mAsfSystemStateWorking = \
{
  ASFMSG_COMMAND_SYSTEM_STATE,
  0x3,    // ByteCount
  ASFMSG_SUBCOMMAND_SET_SYSTEM_STATE,
  ASFMSG_VERSION_NUMBER_10,
  ASFMSG_SYSTEM_STATE_S0
};

ASF_MSG_NORETRANSMIT  mMsgStopTimer =
{
  ASFMSG_COMMAND_MANAGEMENT_CONTROL,
  0x2,    // ByteCount
  ASFMSG_SUBCOMMAND_STOP_WATCH_DOG,
  ASFMSG_VERSION_NUMBER_10
};

// 3.1.5.3 System Firmware Progress Events
ASF_MSG_NORETRANSMIT  mMsgBiosPresent =
{
  ASFMSG_COMMAND_MESSAGING,
  0xd,    // ByteCount
  ASFMSG_SUBCOMMAND_NO_RETRANSMIT,
  ASFMSG_VERSION_NUMBER_10,
  ASFMSG_EVENT_SENSOR_TYPE_ENTITY_PRESENCE,
  ASFMSG_EVENT_TYPE_SENSOR_SPECIFIC,
  ASFMSG_EVENT_OFFSET_ENTITY_PRESENT,
  ASFMSG_EVENT_SOURCE_TYPE_ASF10,
  ASFMSG_EVENT_SEVERITY_NON_CRITICAL,
  ASFMSG_SENSOR_DEVICE_UNSPECIFIED,
  ASFMSG_SENSOR_NUMBER_UNSPECIFIED,
  ASFMSG_ENTITY_BIOS,
  ASFMSG_ENTITY_INSTANCE_UNSPECIFIED,
  ASFMSG_EVENT_DATA1,
  ASFMSG_EVENT_DATA_UNSPECIFIED
};

// Starting memory initialization and test.
ASF_MSG_NORETRANSMIT  mMsgMemoryInit =
{
  ASFMSG_COMMAND_MESSAGING,
  0xd,    // ByteCount
  ASFMSG_SUBCOMMAND_NO_RETRANSMIT,
  ASFMSG_VERSION_NUMBER_10,
  ASFMSG_EVENT_SENSOR_TYPE_FW_ERROR_PROGRESS,
  ASFMSG_EVENT_TYPE_SENSOR_SPECIFIC,
  ASFMSG_EVENT_OFFSET_SYS_FW_PROGRESS_ENTRY,
  ASFMSG_EVENT_SOURCE_TYPE_ASF10,
  ASFMSG_EVENT_SEVERITY_NON_CRITICAL,
  ASFMSG_SENSOR_DEVICE_UNSPECIFIED,
  ASFMSG_SENSOR_NUMBER_UNSPECIFIED,
  ASFMSG_ENTITY_MEMORY_DEVICE,
  ASFMSG_ENTITY_INSTANCE_UNSPECIFIED,
  ASFMSG_EVENT_DATA1,
  ASFMSG_EVENT_DATA_MEMORY_INITIALIZATION
};

// Memory initialized and tested.
ASF_MSG_NORETRANSMIT  mMsgMemoryInitialized =
{
  ASFMSG_COMMAND_MESSAGING,
  0xd,    // ByteCount
  ASFMSG_SUBCOMMAND_NO_RETRANSMIT,
  ASFMSG_VERSION_NUMBER_10,
  ASFMSG_EVENT_SENSOR_TYPE_FW_ERROR_PROGRESS,
  ASFMSG_EVENT_TYPE_SENSOR_SPECIFIC,
  ASFMSG_EVENT_OFFSET_SYS_FW_PROGRESS_EXIT,
  ASFMSG_EVENT_SOURCE_TYPE_ASF10,
  ASFMSG_EVENT_SEVERITY_NON_CRITICAL,
  ASFMSG_SENSOR_DEVICE_UNSPECIFIED,
  ASFMSG_SENSOR_NUMBER_UNSPECIFIED,
  ASFMSG_ENTITY_MEMORY_DEVICE,
  ASFMSG_ENTITY_INSTANCE_UNSPECIFIED,
  ASFMSG_EVENT_DATA1,
  ASFMSG_EVENT_DATA_MEMORY_INITIALIZATION
};

ASF_MSG_NORETRANSMIT  mAsfmsgCacheInit =
{
  ASFMSG_COMMAND_MESSAGING,
  0xd,    // ByteCount
  ASFMSG_SUBCOMMAND_NO_RETRANSMIT,
  ASFMSG_VERSION_NUMBER_10,
  ASFMSG_EVENT_SENSOR_TYPE_FW_ERROR_PROGRESS,
  ASFMSG_EVENT_TYPE_SENSOR_SPECIFIC,
  ASFMSG_EVENT_OFFSET_SYS_FW_PROGRESS_ENTRY,
  ASFMSG_EVENT_SOURCE_TYPE_ASF10,
  ASFMSG_EVENT_SEVERITY_MONITOR,
  ASFMSG_SENSOR_DEVICE_UNSPECIFIED,
  ASFMSG_SENSOR_NUMBER_UNSPECIFIED,
  ASFMSG_ENTITY_PROCESSOR,
  ASFMSG_ENTITY_INSTANCE_UNSPECIFIED,
  ASFMSG_EVENT_DATA1,
  ASFMSG_EVENT_DATA_CACHE_INITIALIZATION
};

ASF_MSG_NORETRANSMIT  mAsfmsgMemoryMissing =
{
  ASFMSG_COMMAND_MESSAGING,
  0xd,    // ByteCount
  ASFMSG_SUBCOMMAND_NO_RETRANSMIT,
  ASFMSG_VERSION_NUMBER_10,
  ASFMSG_EVENT_SENSOR_TYPE_FW_ERROR_PROGRESS,
  ASFMSG_EVENT_TYPE_SENSOR_SPECIFIC,
  ASFMSG_EVENT_OFFSET_SYS_FW_ERROR,
  ASFMSG_EVENT_SOURCE_TYPE_ASF10,
  ASFMSG_EVENT_SEVERITY_NON_CRITICAL,
  ASFMSG_SENSOR_DEVICE_UNSPECIFIED,
  ASFMSG_SENSOR_NUMBER_UNSPECIFIED,
  ASFMSG_ENTITY_MEMORY_DEVICE,
  ASFMSG_ENTITY_INSTANCE_UNSPECIFIED,
  ASFMSG_EVENT_DATA1,
  ASFMSG_EVENT_DATA_NO_MEMORY
};

/**
  This function pushes the PEI System Firmware Progress Events.

  @param[in] SmBus               Pointer to the SmBus PPI.
  @param[in] FixedTargetAddress  Device address
  @param[in] MessageErrorLevel   Progress or error or system management message Type.
  @param[in] MessageBuffer       Pointer to the Event Data Buffer.

**/
VOID
EFIAPI
AsfPushProgressMessage (
  IN EFI_PEI_SMBUS2_PPI        *SmBus,
  IN EFI_SMBUS_DEVICE_ADDRESS  FixedTargetAddress,
  IN UINT32                    MessageErrorLevel,
  IN UINT8                     *MessageBuffer
  )
{
  EFI_STATUS  Status;
  UINTN       Length;

  if (MessageErrorLevel & PcdGet32 (PcdAsfMessageErrorLevel)) {
    Length = ((ASF_MSG_NORETRANSMIT *)MessageBuffer)->ByteCount;
    Status = SmBus->Execute (
                      SmBus,
                      FixedTargetAddress,
                      ((ASF_MSG_NORETRANSMIT *)MessageBuffer)->Command,
                      EfiSmbusWriteBlock,
                      TRUE,
                      &Length,
                      (UINT8 *)(MessageBuffer+2)
                      );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Push alert message fail, status = %r\n", Status));
    }
  }

  return;
}

/**
  This callback registered by Report Status Code Ppi for Memory Missing PET.

  @param[in] PeiServices        General purpose services available to every PEIM.
  @param[in] Type               Indicates the type of status code being reported.
  @param[in] Value              Describes the current status of a hardware or software entity.
                                This included information about the class and subclass that is
                                used to classify the entity as well as an operation.
  @param[in] Instance           The enumeration of a hardware or software entity within the system.
                                Valid instance numbers start with 1.
  @param[in] CallerId           This optional parameter may be used to identify the caller.
                                This parameter allows the status code driver to apply different
                                rules to different callers.
  @param[in] Data               This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS           Always return EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
AsfPeiStatusCodeCallBack (
  IN  EFI_PEI_SERVICES       **PeiServices,
  IN  EFI_STATUS_CODE_TYPE   Type,
  IN  EFI_STATUS_CODE_VALUE  Value,
  IN  UINT32                 Instance,
  IN  EFI_GUID               *CallerId,
  IN  EFI_STATUS_CODE_DATA   *Data
  )
{
  EFI_STATUS                Status;
  EFI_PEI_SMBUS2_PPI        *SmBus;
  EFI_SMBUS_DEVICE_ADDRESS  FixedTargetAddress;

  FixedTargetAddress.SmbusDeviceAddress = PcdGet8 (PcdSmbusSlaveAddressForDashLan) >> 1;
  if (FixedTargetAddress.SmbusDeviceAddress == 0) {
    return EFI_SUCCESS;
  }

  Status = PeiServicesLocatePpi (
             &gEfiPeiSmbus2PpiGuid,
             0,
             NULL,
             (VOID **)&SmBus
             );
  if ( EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  if ((Type & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE) {
    if ((Value == (EFI_SOFTWARE_PEI_CORE | EFI_SW_PEI_CORE_EC_MEMORY_NOT_INSTALLED)) ||
        (Value == (EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_NONE_DETECTED)))
    {
      // Error message - Memory Missing.
      AsfPushProgressMessage (SmBus, FixedTargetAddress, MESSAGE_ERROR_LEVEL_ERROR, (UINT8 *)&mAsfmsgMemoryMissing);
    }
  }

  if (((Type & EFI_STATUS_CODE_TYPE_MASK) == EFI_PROGRESS_CODE) &&
      (Value == (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_PC_CACHE_INIT)))
  {
    // Progress message - Cache initialization.
    AsfPushProgressMessage (SmBus, FixedTargetAddress, MESSAGE_ERROR_LEVEL_PROGRESS, (UINT8 *)&mAsfmsgCacheInit);
  }

  return EFI_SUCCESS;
}

/**
  This send memory initialized message after memory discovered.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] NotifyDescriptor     The notification structure this PEIM registered on install.
  @param[in] Ppi                  The memory discovered PPI.

  @retval EFI_SUCCESS             Always return EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
MsgMemoryInitCompleted (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                Status;
  EFI_PEI_SMBUS2_PPI        *SmBus;
  EFI_BOOT_MODE             BootMode;
  EFI_SMBUS_DEVICE_ADDRESS  FixedTargetAddress;

  FixedTargetAddress.SmbusDeviceAddress = PcdGet8 (PcdSmbusSlaveAddressForDashLan) >> 1;
  if (FixedTargetAddress.SmbusDeviceAddress == 0) {
    return EFI_SUCCESS;
  }

  Status = PeiServicesLocatePpi (
             &gEfiPeiSmbus2PpiGuid,
             0,
             NULL,
             (VOID **)&SmBus
             );
  if ( EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  // Progress message - Completed memory initialization and test.
  AsfPushProgressMessage (SmBus, FixedTargetAddress, MESSAGE_ERROR_LEVEL_PROGRESS, (UINT8 *)&mMsgMemoryInitialized);

  // Get Boot Path.
  Status = PeiServicesGetBootMode (&BootMode);
  if (!EFI_ERROR (Status) && (BootMode == BOOT_ON_S3_RESUME)) {
    // Push System State Working if S3 resuming.
    AsfPushProgressMessage (
      SmBus,
      FixedTargetAddress,
      MESSAGE_ERROR_LEVEL_SYSTEM_MANAGEMENT,
      (UINT8 *)&mAsfSystemStateWorking
      );
  }

  return EFI_SUCCESS;
}

/**
  Asf PEI module entry point

  @param[in]  FileHandle           FileHandle  Handle of the file being invoked.
  @param[in]  PeiServices          Describes the list of possible PEI Services.

  @retval     EFI_SUCCESS          The PEIM initialized successfully.

**/
EFI_STATUS
EFIAPI
AsfPeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                Status;
  EFI_PEI_SMBUS2_PPI        *SmBus;
  EFI_PEI_RSC_HANDLER_PPI   *RscHndrPpi;
  EFI_SMBUS_DEVICE_ADDRESS  FixedTargetAddress;

  FixedTargetAddress.SmbusDeviceAddress = PcdGet8 (PcdSmbusSlaveAddressForDashLan) >> 1;
  if (FixedTargetAddress.SmbusDeviceAddress == 0) {
    return EFI_UNSUPPORTED;
  }

  Status = PeiServicesLocatePpi (
             &gEfiPeiSmbus2PpiGuid,
             0,
             NULL,
             (VOID **)&SmBus
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // If the managed client's firmware supports a system boot-failure watchdog timer,
  // the firmware issues the Stop Watchdog Timer command to stop the timer that is
  // automatically started by the alert-sending device at power-on reset.
  //
  AsfPushProgressMessage (SmBus, FixedTargetAddress, MESSAGE_ERROR_LEVEL_SYSTEM_MANAGEMENT, (UINT8 *)&mMsgStopTimer);

  // Progress message - BIOS Present.
  AsfPushProgressMessage (SmBus, FixedTargetAddress, MESSAGE_ERROR_LEVEL_PROGRESS, (UINT8 *)&mMsgBiosPresent);

  // Progress message - Started memory initialization and test.
  AsfPushProgressMessage (SmBus, FixedTargetAddress, MESSAGE_ERROR_LEVEL_PROGRESS, (UINT8 *)&mMsgMemoryInit);

  PeiServicesNotifyPpi (&mMemoryDiscoveredNotifyDes);

  Status = PeiServicesLocatePpi (
             &gEfiPeiRscHandlerPpiGuid,
             0,
             NULL,
             (VOID **)&RscHndrPpi
             );
  if (!EFI_ERROR (Status)) {
    RscHndrPpi->Register ((EFI_PEI_RSC_HANDLER_CALLBACK)AsfPeiStatusCodeCallBack);
  }

  return EFI_SUCCESS;
}
