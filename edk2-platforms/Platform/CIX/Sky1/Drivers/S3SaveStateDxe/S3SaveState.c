/*
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 */
#include "InternalS3SaveState.h"
#include "InternalBootScript.h"

EFI_HANDLE                 mHandle = NULL;
S3_BOOT_SCRIPT_TABLE_DATA  *mS3BootScriptTablePtr;

CIX_S3_SAVE_STATE_PROTOCOL  mS3SaveState = {
  BootScriptWrite,
  BootScriptDump
};

/**
  This is an internal function to add a terminate node the entry, recalculate the table
  length and fill into the table.

  @return the base address of the boot script table.
 **/
UINT8 *
S3BootScriptInternalCloseTable (
  VOID
  )
{
  UINT8                         *S3TableBase;
  CIX_BOOT_SCRIPT_TERMINATE     ScriptTerminate;
  CIX_BOOT_SCRIPT_TABLE_HEADER  *ScriptTableInfo;

  S3TableBase = mS3BootScriptTablePtr->TableBase;

  if (S3TableBase == NULL) {
    //
    // the table is not exist
    //
    return S3TableBase;
  }

  //
  // Append the termination entry.
  //
  ScriptTerminate.OpCode = CIX_S3_BOOT_SCRIPT_TERMINATE_OPCODE;
  ScriptTerminate.Length = (UINT8)sizeof (CIX_BOOT_SCRIPT_TERMINATE);
  CopyMem (mS3BootScriptTablePtr->TableBase + mS3BootScriptTablePtr->TableLength, &ScriptTerminate, sizeof (CIX_BOOT_SCRIPT_TERMINATE));
  //
  // fill the table length
  //
  ScriptTableInfo                    = (CIX_BOOT_SCRIPT_TABLE_HEADER *)(mS3BootScriptTablePtr->TableBase);
  ScriptTableInfo->TableLength       = mS3BootScriptTablePtr->TableLength + sizeof (CIX_BOOT_SCRIPT_TERMINATE);
  mS3BootScriptTablePtr->TableLength = mS3BootScriptTablePtr->TableLength + sizeof (CIX_BOOT_SCRIPT_TERMINATE);
  return S3TableBase;
}

BOOLEAN
DoubleChecksum (
  VOID     *Start,
  UINT32   Length,
  UINT64   *Sum64,
  BOOLEAN  DoCheck
  )
{
  UINT32  *ptr = (UINT32 *)Start;
  UINT32  cka = 0, ckb = 0;

  // configASSERT( 0 == ((UINT32)Start & 0x03) ); // Input array must 4 bytes aligned

  for (UINT32 i = 0; i * 4 < Length; i++) {
    cka += ptr[i];
    ckb += cka;
  }

  if (DoCheck) {
    if (Sum64) {
      if (((UINT32)(*Sum64 >> 32) == cka) && ((UINT32)(*Sum64) == ckb)) {
        return TRUE;
      }
    }
  } else if (Sum64) {
    *Sum64 = ((UINT64)cka << 32) + (UINT64)ckb;
    return TRUE;
  }

  return FALSE;
}

/**
  This is the Event call back function to notify the driver the system is exit
  boot service.

  @param  Event   Pointer to this event
  @param  Context Event handler private data
 **/
VOID
EFIAPI
S3SaveStateEventCallBack (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS  Status;
  UINT64      Sum64;
  UINT32      CrcL32, CrcH32, TableBaseL32, TableBaseH32;

  S3BootScriptInternalCloseTable ();

  DoubleChecksum (mS3BootScriptTablePtr->TableBase, mS3BootScriptTablePtr->TableLength, &Sum64, FALSE);
  CrcH32       = (UINT32)(Sum64 >> 32);
  CrcL32       = (UINT32)(Sum64 & 0x0ffffffff);
  TableBaseH32 = (UINT32)((UINT64)(mS3BootScriptTablePtr->TableBase) >> 32);
  TableBaseL32 = (UINT32)((UINT64)(mS3BootScriptTablePtr->TableBase) & 0x0ffffffff);

  // Notify PM boot script table is ready
  Status = CsuPmMsgRegisterS3ScritTable (TableBaseL32, TableBaseH32, CrcL32, CrcH32);
  if (EFI_ERROR (Status)) {
    DebugPrint (DEBUG_ERROR, "Error! S3 boot script register failed.\n");
  }
}

/**
  Internal function to add memory write opcode to the table.

  @param  Marker                The variable argument list to get the opcode
                                and associated attributes.

  @retval EFI_OUT_OF_RESOURCES  Not enough resource to do operation.
  @retval EFI_SUCCESS           Opcode is added.

**/
EFI_STATUS
BootScriptWriteMemWrite (
  IN VA_LIST  Marker
  )
{
  S3_BOOT_SCRIPT_WIDTH  Width;
  UINT64                Address;
  UINT8                 Count;
  UINT8                 *Buffer;

  Width   = VA_ARG (Marker, S3_BOOT_SCRIPT_WIDTH);
  Address = VA_ARG (Marker, UINT64);
  Count   = VA_ARG (Marker, UINT8);
  Buffer  = VA_ARG (Marker, UINT8 *);

  return S3BootScriptSaveMemWrite (Width, Address, Count, Buffer);
}

/**
  Internal function to add memory read/write opcode to the table.

  @param  Marker                The variable argument list to get the opcode
                                and associated attributes.

  @retval EFI_OUT_OF_RESOURCES  Not enough resource to do operation.
  @retval EFI_SUCCESS           Opcode is added.

**/
EFI_STATUS
BootScriptWriteMemReadWrite (
  IN VA_LIST  Marker
  )
{
  S3_BOOT_SCRIPT_WIDTH  Width;
  UINT64                Address;
  UINT8                 *Data;
  UINT8                 *DataMask;

  Width    = VA_ARG (Marker, S3_BOOT_SCRIPT_WIDTH);
  Address  = VA_ARG (Marker, UINT64);
  Data     = VA_ARG (Marker, UINT8 *);
  DataMask = VA_ARG (Marker, UINT8 *);

  return S3BootScriptSaveMemReadWrite (Width, Address, Data, DataMask);
}

/**
  Adds a record into S3 boot script table.

  This function is used to store a boot script record into S3 boot
  script table. If the S3 boot script table  is nonexistent in the
  system, a new table will automatically be created and then the script record
  will be added into the new table. This function is responsible for allocating
  necessary memory for the script.

  This function has a variable parameter list. The exact parameter list depends on
  the OpCode that is passed into the function. If an unsupported OpCode or illegal
  parameter list is passed in, this function returns EFI_INVALID_PARAMETER.
  If there are not enough resources available for storing more scripts, this function returns
  EFI_OUT_OF_RESOURCES.

  @param  This                  A pointer to the EFI_S3_SAVE_STATE_PROTOCOL instance.
  @param  OpCode                The operation code (opcode) number.
  @param  ...                   Argument list that is specific to each opcode.

  @retval EFI_SUCCESS           The operation succeeded. A record was added into the
                                specified script table.
  @retval EFI_INVALID_PARAMETER The parameter is illegal or the given boot script is not supported.
                                If the opcode is unknow or not supported because of the PCD
                                Feature Flags.
  @retval EFI_OUT_OF_RESOURCES  There is insufficient memory to store the boot script.

**/
EFI_STATUS
EFIAPI
BootScriptWrite (
  IN CONST CIX_S3_SAVE_STATE_PROTOCOL  *This,
  IN       UINT8                       OpCode,
  ...
  )
{
  EFI_STATUS  Status;
  VA_LIST     Marker;

  //
  // Build script according to opcode
  //
  switch (OpCode) {
    case CIX_S3_BOOT_SCRIPT_MEM_WRITE_OPCODE:
      VA_START (Marker, OpCode);
      Status = BootScriptWriteMemWrite (Marker);
      VA_END (Marker);
      break;

    case CIX_S3_BOOT_SCRIPT_MEM_READ_WRITE_OPCODE:
      VA_START (Marker, OpCode);
      Status = BootScriptWriteMemReadWrite (Marker);
      VA_END (Marker);
      break;

    default:
      Status = EFI_INVALID_PARAMETER;
      break;
  }

  if (!EFI_ERROR (Status)) {
    mS3BootScriptTablePtr->ScriptNumber++;
  }

  return Status;
}

/**
  Dump S3 boot script infomation.
**/
EFI_STATUS
BootScriptDump (
  VOID
  )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  CIX_BOOT_SCRIPT_TABLE_HEADER    *BootScripTableHeader;
  CIX_BOOT_SCRIPT_GENERIC_HEADER  *GenericHeader;
  UINTN                           BootScriptEndAddress;
  UINT8                           BitWidth, ByteWidth;
  UINT32                          Count;
  UINT32                          i;
  UINTN                           Address;

  if (mS3BootScriptTablePtr->TableBase == 0) {
    DebugPrint (DEBUG_INFO, "Cix boot script table not initial!\n");
    return EFI_NOT_FOUND;
  }

  DebugPrint (DEBUG_INFO, "TableBase=0x%lx\tTableLength=0x%x\tScriptNumber=%d\n", mS3BootScriptTablePtr->TableBase, mS3BootScriptTablePtr->TableLength, mS3BootScriptTablePtr->ScriptNumber);
  BootScriptEndAddress = (UINTN)(mS3BootScriptTablePtr->TableBase)+mS3BootScriptTablePtr->TableLength;

  // get header
  GenericHeader        = (CIX_BOOT_SCRIPT_GENERIC_HEADER  *)(mS3BootScriptTablePtr->TableBase);
  BootScripTableHeader = (CIX_BOOT_SCRIPT_TABLE_HEADER  *)GenericHeader;
  if (BootScripTableHeader->OpCode == CIX_S3_BOOT_SCRIPT_TABLE_OPCODE) {
    DebugPrint (DEBUG_INFO, "[Table Header]\tVersion=%d\tTable Length=0x%x\n", BootScripTableHeader->Version, BootScripTableHeader->TableLength);
  } else {
    DebugPrint (DEBUG_INFO, "Error header in cix boot script table!\n");
    return EFI_NOT_FOUND;
  }

  while ((UINTN)GenericHeader+GenericHeader->Length < BootScriptEndAddress) {
    GenericHeader = (CIX_BOOT_SCRIPT_GENERIC_HEADER  *)((UINTN)GenericHeader+GenericHeader->Length);
    switch (GenericHeader->OpCode) {
      case CIX_S3_BOOT_SCRIPT_MEM_WRITE_OPCODE:
        BitWidth  = 1<<(((CIX_BOOT_SCRIPT_MEM_WRITE *)GenericHeader)->Width+3);
        ByteWidth = BitWidth/8;
        Count     = ((CIX_BOOT_SCRIPT_MEM_WRITE *)GenericHeader)->Count;
        Address   = ((CIX_BOOT_SCRIPT_MEM_WRITE *)GenericHeader)->Address;
        DebugPrint (
          DEBUG_INFO,
          "[MmioWrite%d]\tAddress=%lx\tCount=%d\tBuffer=",
          BitWidth,
          Address,
          Count
          );
        Address = (UINTN)GenericHeader+sizeof (CIX_BOOT_SCRIPT_MEM_WRITE);
        for (i = 0; i < Count; i++) {
          Address = Address+i*ByteWidth;
          switch (ByteWidth) {
            case 1:
              DebugPrint (DEBUG_INFO, "%02x\t", *(UINT8 *)Address);
              break;
            case 2:
              DebugPrint (DEBUG_INFO, "%04x\t", *(UINT16 *)Address);
              break;
            case 4:
              DebugPrint (DEBUG_INFO, "%08x\t", *(UINT32 *)Address);
              break;
            case 8:
              DebugPrint (DEBUG_INFO, "%016lx\t", *(UINT64 *)Address);
              break;
            default:
              break;
          }

          DebugPrint (DEBUG_INFO, "\n");
        }

        break;
      case CIX_S3_BOOT_SCRIPT_MEM_READ_WRITE_OPCODE:
        BitWidth  = 1<<(((CIX_BOOT_SCRIPT_MEM_READ_WRITE *)GenericHeader)->Width+3);
        ByteWidth = BitWidth/8;
        Address   = ((CIX_BOOT_SCRIPT_MEM_READ_WRITE *)GenericHeader)->Address;
        DebugPrint (DEBUG_INFO, "[RwMem%d]\tAddress=%lx\t", BitWidth, Address);
        Address = (UINTN)GenericHeader+sizeof (CIX_BOOT_SCRIPT_MEM_READ_WRITE);
        switch (ByteWidth) {
          case 1:
            DebugPrint (DEBUG_INFO, "SetValue=%02x\tResetValue=%02x\n", *(UINT8 *)Address, *(UINT8 *)(Address+1));
            break;
          case 2:
            DebugPrint (DEBUG_INFO, "SetValue=%02x\tResetValue=%02x\n", *(UINT16 *)Address, *(UINT16 *)(Address+2));
            break;
          case 4:
            DebugPrint (DEBUG_INFO, "SetValue=%08x\tResetValue=%08x\n", *(UINT32 *)Address, *(UINT32 *)(Address+4));
            break;
          case 8:
            DebugPrint (DEBUG_INFO, "SetValue=%016lx\tResetValue=%016lx\n", *(UINT64 *)Address, *(UINT64 *)(Address+8));
            break;
          default:
            break;
        }

        break;
      case CIX_S3_BOOT_SCRIPT_TERMINATE_OPCODE:
        DebugPrint (DEBUG_INFO, "[Table End]\n");
        break;
      default:
        break;
    }
  }

  return Status;
}

/**
  This routine is entry point of ScriptSave driver.

  @param  ImageHandle           Handle for this drivers loaded image protocol.
  @param  SystemTable           EFI system table.

  @retval EFI_OUT_OF_RESOURCES  No enough resource
  @retval EFI_SUCCESS           Succesfully installed the ScriptSave driver.
  @retval other                 Errors occurred.

**/
EFI_STATUS
EFIAPI
InitializeS3SaveState (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   Event = NULL;

  mS3BootScriptTablePtr = AllocateZeroPool (sizeof (S3_BOOT_SCRIPT_TABLE_DATA));
  Status                = gBS->CreateEventEx (
                                 EVT_NOTIFY_SIGNAL,
                                 TPL_CALLBACK,
                                 S3SaveStateEventCallBack,
                                 NULL,
                                 &gEfiEventExitBootServicesGuid,
                                 &Event
                                 );
  ASSERT (Event != NULL);

  Status =  gBS->InstallProtocolInterface (
                   &mHandle,
                   &gCixS3SaveStateProtocolGuid,
                   EFI_NATIVE_INTERFACE,
                   &mS3SaveState
                   );

  if (EFI_ERROR (Status)) {
    DEBUG (
      (DEBUG_ERROR,
       "%a: failed to install cix s3 save state protocol (Status == %r)\n",
       __FUNCTION__, Status)
      );
  }

  return Status;
}
