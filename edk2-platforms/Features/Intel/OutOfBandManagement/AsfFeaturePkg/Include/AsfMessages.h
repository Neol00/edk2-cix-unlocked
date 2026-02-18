/** @file
  Asf message format define.

  Copyright (c) 1985 - 2022, AMI. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Revision Reference:
  Format defined in Asf 2.0 Specification.
**/

#ifndef __ASF_MESSAGES_H__
#define __ASF_MESSAGES_H__

#include <Base.h>
#include <IndustryStandard/Asf.h>

#define MESSAGE_ERROR_LEVEL_PROGRESS           BIT0
#define MESSAGE_ERROR_LEVEL_ERROR              BIT1
#define MESSAGE_ERROR_LEVEL_SYSTEM_MANAGEMENT  BIT2

#pragma pack(push,1)

/**
  This message causes the alert-sending device to transmit a single,
  un-retransmitted PET frame. If the alert-sending device is either temporarily
  unable to handle the message or unable to send the requested PET frame
  because the device's transport media is down, the device must NACK the message
  according to SMBUS_2.0 definitions; otherwise, the device sends the
  single-frame transmission.
**/
typedef struct {
  UINT8    Command;         ///< Message Command.
  UINT8    ByteCount;       ///< Length of the data in bytes.
  UINT8    SubCommand;      ///< SubCommand No Retransmit.
  UINT8    Version;         ///< Version Number.
  UINT8    EventSensorType; ///< Event Sensor Type.
  UINT8    EventType;       ///< Event Type.
  UINT8    EventOffset;     ///< Event Offset.
  UINT8    EventSourceType; ///< Describes the originator of the event.
  UINT8    EventSeverity;   ///< The severity of the event
  UINT8    SensorDevice;    ///< The Sensor Device that caused the event
  UINT8    SensorNumber;    ///< Identify a given instance of a sensor relative to the Sensor Device.
  UINT8    Entity;          ///< Indicates the platform device or subsystem associated with the event.
  UINT8    EntityInstance;  ///< Identifies which unique device is associated with the event.
  UINT8    EventData1;
  UINT8    EventData2;
  UINT8    EventData3;
  UINT8    EventData4;
  UINT8    EventData5;
} ASF_MSG_NORETRANSMIT;

/**
  This is the ASF START WatchDog Timer Data structure.

**/
typedef struct {
  UINT8    Command;
  UINT8    ByteCount;
  UINT8    SubCommand;
  UINT8    Version;
} ASF_STOP_WATCHDOG;

/**
  This is the ASF Message Type structure.

**/
typedef enum {
  MsgHddInit,
  MsgApInit,
  MsgUserInitSetup,
  MsgUsbResourceConfig,
  MsgPciResourceConfig,
  MsgVideoInit,
  MsgKbcInit,
  MsgKbcTest,
  MsgMotherBoardInit,
  MsgNoVideo,
  MsgKbdFailure,
  MsgHddFailure,
  MsgChassisIntrusion,
  MsgNoBootMedia
} ASF_MESSAGE_TYPE;

/**
  This is the Message Data Hub Map Structure.

**/
typedef struct {
  ASF_MESSAGE_TYPE         MessageType;
  EFI_STATUS_CODE_VALUE    StatusCode;
} MESSAGE_DATA_HUB_MAP;

/**
  This is the ASF System Firmware Event Structure.

**/
typedef struct {
  ASF_MESSAGE_TYPE        Type;
  ASF_MSG_NORETRANSMIT    Message;
} ASF_MESSAGE;

#pragma pack(pop)

#endif //__ASF_MESSAGES_H__
