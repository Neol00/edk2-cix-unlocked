/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef CSU_PM_IPC_LIB
#define CSU_PM_IPC_LIB

#include <Base.h>
#include <Uefi.h>

EFI_STATUS
CsuPmMsgClockCountDecrase (
  IN UINT32  ClockId
  );

EFI_STATUS
CsuPmMsgGetFirmwareVersion (
  );

EFI_STATUS
CsuPmMsgRegisterS3ScritTable (
  IN UINT32  AddressL32,
  IN UINT32  AddressH32,
  IN UINT32  CrcL32,
  IN UINT32  CrcH32
  );

EFI_STATUS
CsuPmIpcCommandGetPayload (
  OUT UINT32  **Payload
  );

EFI_STATUS
CsuPmIpcCommandExecute (
  IN     UINT32  MsgId,
  IN OUT UINT32  *PayloadLength,
  OUT    UINT32  **ReturnValues OPTIONAL
  );

#endif // CSU_PM_IPC_LIB
