/**
#
#  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
#
#**/

#include <Base.h>
#include <Uefi.h>
#include <Library/CixS3IoLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Protocol/CixS3SaveState.h>

CIX_S3_SAVE_STATE_PROTOCOL      static  *gCixBootScriptSave = NULL;

EFI_STATUS
GetCixS3SaveStateProtocol (
  VOID
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  if (gCixBootScriptSave == NULL) {
    Status = gBS->LocateProtocol (
                    &gCixS3SaveStateProtocolGuid,
                    NULL, \
                    (VOID **)&gCixBootScriptSave
                    );
  }

  return Status;
}

VOID
EFIAPI
MmioWrite8S3 (
  IN UINTN  Address,
  IN UINT8  Value
  )
{
  EFI_STATUS  Status;
  UINT8       Count = 1;

  MmioWrite8 (Address, Value);

  Status = GetCixS3SaveStateProtocol ();
  if (!EFI_ERROR (Status)) {
    gCixBootScriptSave->Write (gCixBootScriptSave, CIX_S3_BOOT_SCRIPT_MEM_WRITE_OPCODE, S3BootScriptWidthUint8, Address, Count, &Value);
  }
}

VOID
EFIAPI
MmioWrite16S3 (
  IN UINTN   Address,
  IN UINT16  Value
  )
{
  EFI_STATUS  Status;
  UINT8       Count = 1;

  MmioWrite16 (Address, Value);

  Status = GetCixS3SaveStateProtocol ();
  if (!EFI_ERROR (Status)) {
    gCixBootScriptSave->Write (gCixBootScriptSave, CIX_S3_BOOT_SCRIPT_MEM_WRITE_OPCODE, S3BootScriptWidthUint16, Address, Count, &Value);
  }
}

VOID
EFIAPI
MmioWrite32S3 (
  IN UINTN   Address,
  IN UINT32  Value
  )
{
  EFI_STATUS  Status;
  UINT8       Count = 1;

  MmioWrite32 (Address, Value);
  Status = GetCixS3SaveStateProtocol ();
  if (!EFI_ERROR (Status)) {
    gCixBootScriptSave->Write (gCixBootScriptSave, CIX_S3_BOOT_SCRIPT_MEM_WRITE_OPCODE, S3BootScriptWidthUint32, Address, Count, &Value);
  }
}

VOID
EFIAPI
MmioWrite64S3 (
  IN UINTN   Address,
  IN UINT64  Value
  )
{
  EFI_STATUS  Status;
  UINT8       Count = 1;

  MmioWrite64 (Address, Value);
  Status = GetCixS3SaveStateProtocol ();
  if (!EFI_ERROR (Status)) {
    gCixBootScriptSave->Write (gCixBootScriptSave, CIX_S3_BOOT_SCRIPT_MEM_WRITE_OPCODE, S3BootScriptWidthUint64, Address, Count, &Value);
  }
}

VOID
EFIAPI
RwMem8S3 (
  IN UINTN  Address,
  IN UINT8  SetBit8,
  IN UINT8  ResetBit8
  )
{
  EFI_STATUS  Status;

  ResetBit8 = ~ResetBit8;
  MmioAndThenOr8 (Address, ResetBit8, SetBit8);

  Status = GetCixS3SaveStateProtocol ();
  if (!EFI_ERROR (Status)) {
    gCixBootScriptSave->Write (gCixBootScriptSave, CIX_S3_BOOT_SCRIPT_MEM_READ_WRITE_OPCODE, S3BootScriptWidthUint8, Address, &SetBit8, &ResetBit8);
  }
}

VOID
EFIAPI
RwMem16S3 (
  IN UINTN   Address,
  IN UINT16  SetBit16,
  IN UINT16  ResetBit16
  )
{
  EFI_STATUS  Status;

  ResetBit16 = ~ResetBit16;
  MmioAndThenOr16 (Address, ResetBit16, SetBit16);

  Status = GetCixS3SaveStateProtocol ();
  if (!EFI_ERROR (Status)) {
    gCixBootScriptSave->Write (gCixBootScriptSave, CIX_S3_BOOT_SCRIPT_MEM_READ_WRITE_OPCODE, S3BootScriptWidthUint16, Address, &SetBit16, &ResetBit16);
  }
}

VOID
EFIAPI
RwMem32S3 (
  IN UINTN   Address,
  IN UINT32  SetBit32,
  IN UINT32  ResetBit32
  )
{
  EFI_STATUS  Status;

  ResetBit32 = ~ResetBit32;
  MmioAndThenOr32 (Address, ResetBit32, SetBit32);

  Status = GetCixS3SaveStateProtocol ();
  if (!EFI_ERROR (Status)) {
    gCixBootScriptSave->Write (gCixBootScriptSave, CIX_S3_BOOT_SCRIPT_MEM_READ_WRITE_OPCODE, S3BootScriptWidthUint32, Address, &SetBit32, &ResetBit32);
  }
}

VOID
EFIAPI
RwMem64S3 (
  IN UINTN   Address,
  IN UINT64  SetBit64,
  IN UINT64  ResetBit64
  )
{
  EFI_STATUS  Status;

  ResetBit64 = ~ResetBit64;
  MmioAndThenOr64 (Address, ResetBit64, SetBit64);

  Status = GetCixS3SaveStateProtocol ();
  if (!EFI_ERROR (Status)) {
    gCixBootScriptSave->Write (gCixBootScriptSave, CIX_S3_BOOT_SCRIPT_MEM_READ_WRITE_OPCODE, S3BootScriptWidthUint64, Address, &SetBit64, &ResetBit64);
  }
}
