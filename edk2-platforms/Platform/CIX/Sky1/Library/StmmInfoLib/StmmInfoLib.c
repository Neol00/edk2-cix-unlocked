/** @file  StmmInfoLib.c

  Copyright 2025 Cix Technology Group Co., Ltd. All Rights Reserved
  Copyright (c) 2022-2024, CIX, Ltd. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "StmmInfoLibInternal.h"
//
// Global variable declarations
//
EFI_EVENT                       mStmmInfoVirtualAddrChangeEvent;
EFI_MM_COMMUNICATION2_PROTOCOL  *mMmCommunication2       = NULL;
UINT8                           *mStmmInfoBuffer         = NULL;
UINT8                           *mStmmInfoBufferPhysical = NULL;
UINTN                           mStmmInfoBufferSize;
UINTN                           mStmmInfoBufferPayloadSize;

STATIC
EFI_STATUS
InitCommunicateBuffer (
  OUT     VOID   **DataPtr OPTIONAL,
  IN      UINTN  DataSize,
  IN      UINTN  Function
  )
{
  EFI_MM_COMMUNICATE_HEADER        *MmCommunicateHeader;
  MM_STMM_INFO_COMMUNICATE_HEADER  *MmStmmInfoCommunicateHeader;

  if (DataSize + MM_COMMUNICATE_HEADER_SIZE + MM_STMM_INFO_COMMUNICATE_HEADER_SIZE > mStmmInfoBufferSize) {
    return EFI_INVALID_PARAMETER;
  }

  MmCommunicateHeader = (EFI_MM_COMMUNICATE_HEADER *)mStmmInfoBuffer;
  CopyGuid (&MmCommunicateHeader->HeaderGuid, &gCixMmStmmInfoCommunicateGuid);
  MmCommunicateHeader->MessageLength = DataSize + MM_STMM_INFO_COMMUNICATE_HEADER_SIZE;

  MmStmmInfoCommunicateHeader           = (MM_STMM_INFO_COMMUNICATE_HEADER *)MmCommunicateHeader->Data;
  MmStmmInfoCommunicateHeader->Function = Function;
  if (DataPtr != NULL) {
    *DataPtr = MmStmmInfoCommunicateHeader->Data;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
SendCommunicateBuffer (
  IN      UINTN  DataSize
  )
{
  EFI_STATUS                       Status;
  UINTN                            CommSize;
  EFI_MM_COMMUNICATE_HEADER        *MmCommunicateHeader;
  MM_STMM_INFO_COMMUNICATE_HEADER  *MmStmmInfoCommunicateHeader;

  CommSize = DataSize + MM_COMMUNICATE_HEADER_SIZE + MM_STMM_INFO_COMMUNICATE_HEADER_SIZE;
  Status   = mMmCommunication2->Communicate (
                                  mMmCommunication2,
                                  mStmmInfoBufferPhysical,
                                  mStmmInfoBuffer,
                                  &CommSize
                                  );
  ASSERT_EFI_ERROR (Status);

  MmCommunicateHeader         = (EFI_MM_COMMUNICATE_HEADER *)mStmmInfoBuffer;
  MmStmmInfoCommunicateHeader = (MM_STMM_INFO_COMMUNICATE_HEADER *)MmCommunicateHeader->Data;
  return MmStmmInfoCommunicateHeader->ReturnStatus;
}

EFI_STATUS
EFIAPI
StmmInfoGetPayloadSize (
  OUT UINTN  *StmmInfoPayloadSize
  )
{
  EFI_STATUS                                 Status;
  MM_STMM_INFO_COMMUNICATE_GET_PAYLOAD_SIZE  *MmGetPayloadSize;
  EFI_MM_COMMUNICATE_HEADER                  *MmCommunicateHeader;
  MM_STMM_INFO_COMMUNICATE_HEADER            *MmStmmInfoCommunicateHeader;
  UINTN                                      CommSize;
  UINT8                                      *CommBuffer;

  MmGetPayloadSize = NULL;
  CommBuffer       = NULL;

  if (StmmInfoPayloadSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Init the communicate buffer. The buffer data size is:
  // MM_COMMUNICATE_HEADER_SIZE + MM_STMM_INFO_COMMUNICATE_HEADER_SIZE + sizeof (MM_STMM_INFO_COMMUNICATE_GET_PAYLOAD_SIZE);
  //
  CommSize   = MM_COMMUNICATE_HEADER_SIZE + MM_STMM_INFO_COMMUNICATE_HEADER_SIZE + sizeof (MM_STMM_INFO_COMMUNICATE_GET_PAYLOAD_SIZE);
  CommBuffer = AllocateZeroPool (CommSize);
  if (CommBuffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  MmCommunicateHeader = (EFI_MM_COMMUNICATE_HEADER *)CommBuffer;
  CopyGuid (&MmCommunicateHeader->HeaderGuid, &gCixMmStmmInfoCommunicateGuid);
  MmCommunicateHeader->MessageLength = MM_STMM_INFO_COMMUNICATE_HEADER_SIZE + sizeof (MM_STMM_INFO_COMMUNICATE_GET_PAYLOAD_SIZE);

  MmStmmInfoCommunicateHeader           = (MM_STMM_INFO_COMMUNICATE_HEADER *)MmCommunicateHeader->Data;
  MmStmmInfoCommunicateHeader->Function = MM_STMM_INFO_FUNCTION_GET_PAYLOAD_SIZE;
  MmGetPayloadSize                      = (MM_STMM_INFO_COMMUNICATE_GET_PAYLOAD_SIZE *)MmStmmInfoCommunicateHeader->Data;

  //
  // Send data to SMM.
  //
  Status = mMmCommunication2->Communicate (mMmCommunication2, CommBuffer, CommBuffer, &CommSize);
  ASSERT_EFI_ERROR (Status);

  Status = MmStmmInfoCommunicateHeader->ReturnStatus;
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Get data from SMM.
  //
  *StmmInfoPayloadSize = MmGetPayloadSize->PayloadSize;

Done:
  if (CommBuffer != NULL) {
    FreePool (CommBuffer);
  }

  return Status;
}

EFI_STATUS
EFIAPI
StmmInfoGetVersion (
  IN OUT CHAR16  **Version,
  IN OUT UINT32  *Size
  )
{
  EFI_STATUS                            Status                = EFI_SUCCESS;
  MM_STMM_INFO_COMMUNICATE_GET_VERSION  *MmStmmInfoGetVersion = NULL;
  UINTN                                 DataSize;
  UINTN                                 PayloadSize;

  DataSize = 0x80; // size of sshare buffer we defined for STMM version

  *Version = AllocateZeroPool (DataSize);

  if (*Version == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if ((DataSize > mStmmInfoBufferPayloadSize - OFFSET_OF (MM_STMM_INFO_COMMUNICATE_GET_VERSION, Data))) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Init the communicate buffer. The buffer data size is:
  // MM_COMMUNICATE_HEADER_SIZE + MM_STMM_INFO_COMMUNICATE_HEADER_SIZE + PayloadSize.
  //
  PayloadSize = OFFSET_OF (MM_STMM_INFO_COMMUNICATE_GET_VERSION, Data) + DataSize;
  Status      = InitCommunicateBuffer ((VOID **)&MmStmmInfoGetVersion, PayloadSize, MM_STMM_INFO_FUNCTION_GET_VERSION);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  ASSERT (MmStmmInfoGetVersion != NULL);
  if (MmStmmInfoGetVersion == NULL) {
    DebugPrint (DEBUG_ERROR, "%a: fail to init communication buffer\n", __FUNCTION__);
  }

  //
  // Send data to SMM.
  //
  Status = SendCommunicateBuffer (PayloadSize);

  if (Status == EFI_SUCCESS) {
    DataSize = StrLen ((CHAR16 *)MmStmmInfoGetVersion->Data) + 1;
    // Unicode string copy without the null terminator
    CopyMem (*Version, MmStmmInfoGetVersion->Data, 2 * DataSize);
    *Size = DataSize;
  }

Done:

  return Status;
}

RETURN_STATUS
EFIAPI
StmmInfoLibConstructor (
  VOID
  )
{
  EFI_STATUS  Status;

  Status = gBS->LocateProtocol (&gEfiMmCommunication2ProtocolGuid, NULL, (VOID **)&mMmCommunication2);
  ASSERT_EFI_ERROR (Status);

  //
  // Allocate memory for STMM information communicate buffer.
  //
  Status = StmmInfoGetPayloadSize (&mStmmInfoBufferPayloadSize);
  ASSERT_EFI_ERROR (Status);

  mStmmInfoBufferSize = MM_COMMUNICATE_HEADER_SIZE + MM_STMM_INFO_COMMUNICATE_HEADER_SIZE + mStmmInfoBufferPayloadSize;
  mStmmInfoBuffer     = AllocateRuntimePool (mStmmInfoBufferSize);
  ASSERT (mStmmInfoBuffer != NULL);

  //
  // Save the buffer physical address used for StMM conmunication.
  //
  mStmmInfoBufferPhysical = mStmmInfoBuffer;

  return EFI_SUCCESS;
}
