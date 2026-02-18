/** @file
  UbaSmbiosUpdateLib implementation.

  @copyright
  Copyright 2012 - 2017 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi.h>
#include <IndustryStandard/SmBios.h>

#include <Protocol/Smbios.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UbaSmbiosUpdateLib.h>

#include <Protocol/UbaCfgDb.h>
#include <Guid/EventGroup.h>

#define SMBIOS_TYPE_MAX_LENGTH    0x300

/**
  Provide the RegData and register a callback for dynamic update SMBIOS data.

  @param RegData                  Callback register data.

  @retval EFI_NOT_FOUND           Data log protocol not found.
  @retval EFI_OUT_OF_RESOURCES    Data was not logged due to lack of system resources.
  @retval EFI_SUCCESS             Data have been updated successfully.

**/
EFI_STATUS
PlatformRegisterSmbiosUpdate (
  IN  SMBIOS_UPDATE_DATA              *RegData
  )
{
  EFI_STATUS                              Status;
  STATIC  UBA_CONFIG_DATABASE_PROTOCOL    *UbaConfigProtocol = NULL;

  if (UbaConfigProtocol == NULL) {

    Status = gBS->LocateProtocol (
                    &gUbaConfigDatabaseProtocolGuid,
                    NULL,
                    (VOID **) &UbaConfigProtocol
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  RegData->Signature  = PLATFORM_SMBIOS_UPDATE_SIGNATURE;
  RegData->Version    = PLATFORM_SMBIOS_UPDATE_VERSION;

  Status = UbaConfigProtocol->AddData (
                                     UbaConfigProtocol,
                                     &gPlatformSmbiosConfigDataGuid,
                                     RegData,
                                     sizeof(SMBIOS_UPDATE_DATA)
                                     );
  return Status;
}

/**
  Update a String for a filled SMBIOS data structure, the structure must be filled
  before update string.
  This function update a string indicated by StringNumber to the tail of SMBIOS
  structure.

  @param Smbios                   SMBIOS structure data buffer pointer.
  @param BufferSize               SMBIOS structure data buffer size.
  @param StringNumber             The string index number of SMBIOS structure.
  @param String                   String want to update.

  @retval EFI_OUT_OF_RESOURCES    No enough memory for this action.
  @retval EFI_SUCCESS             String updated successfully.

**/
EFI_STATUS
PlatformSmbiosUpdateString (
  IN  OUT SMBIOS_STRUCTURE_POINTER    Smbios,
  IN      UINTN                       BufferSize,
  IN      UINTN                       StringNumber,
  IN      CHAR16                      *String
  )
{
  EFI_STATUS                          Status;
  CHAR8                               *AsciiString = NULL;

  UINTN                               InputStrLen;
  UINTN                               TargetStrLen;
  UINTN                               StrIndex;
  UINTN                               TargetStrOffset;
  CHAR8                               *StrStart;

  SMBIOS_STRUCTURE_POINTER            NewSmbiosPtr;

  UINTN                               OrigSize = 0;
  UINTN                               NewSize = 0;
  UINTN                               StringSize = 0;

  StringSize = StrSize (String);
  AsciiString = AllocateZeroPool (StringSize);
  ASSERT (AsciiString != NULL);
  if (AsciiString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  UnicodeStrToAsciiStrS (String, AsciiString, StringSize);
  InputStrLen = AsciiStrLen (AsciiString);

  Status = PlatformSmbiosGetTypeLength (Smbios, &OrigSize);

  //
  // Point to unformed string section
  //
  StrStart = (CHAR8 *) Smbios.Hdr + Smbios.Hdr->Length;
  for (StrIndex = 1, TargetStrOffset = 0; StrIndex < StringNumber; StrStart++, TargetStrOffset++) {
    //
    // A string ends in 00h
    //
    if (*StrStart == 0) {
      StrIndex++;
    }
  }

  //
  // Now we get the string target
  //
  TargetStrLen = AsciiStrLen(StrStart);
  if (InputStrLen == TargetStrLen) {
    Status = AsciiStrCpyS(StrStart, TargetStrLen + 1, AsciiString);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    return EFI_SUCCESS;
  }

  //
  // Original string buffer size is not exactly match input string length.
  // Re-allocate buffer is needed.
  //
  NewSmbiosPtr.Hdr = AllocateZeroPool (SMBIOS_TYPE_MAX_LENGTH);
  if (NewSmbiosPtr.Hdr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Copy SMBIOS structure and optional strings.
  //
  CopyMem (NewSmbiosPtr.Hdr, Smbios.Hdr, Smbios.Hdr->Length + TargetStrOffset);
  CopyMem ((CHAR8*)NewSmbiosPtr.Hdr + Smbios.Hdr->Length + TargetStrOffset, AsciiString, InputStrLen + 1);
  CopyMem ((CHAR8*)NewSmbiosPtr.Hdr + Smbios.Hdr->Length + TargetStrOffset + InputStrLen + 1,
           (CHAR8*)Smbios.Hdr + Smbios.Hdr->Length + TargetStrOffset + TargetStrLen + 1,
           OrigSize - Smbios.Hdr->Length - TargetStrOffset - TargetStrLen - 1);

  Status = PlatformSmbiosGetTypeLength (NewSmbiosPtr, &NewSize);
  CopyMem (Smbios.Hdr, NewSmbiosPtr.Hdr, NewSize);

  FreePool (NewSmbiosPtr.Hdr);
  FreePool (AsciiString);

  return EFI_SUCCESS;
}

/**
  Get SMBIOS data structure length, include the string in tail.

  @param Smbios                   SMBIOS structure data buffer pointer.
  @param TypeSize                 SMBIOS structure size.

  @retval EFI_INVALID_PARAMETER   Input paramter invalid.
  @retval EFI_SUCCESS             Caculate data structure size successfully.

**/
EFI_STATUS
PlatformSmbiosGetTypeLength (
  IN  OUT SMBIOS_STRUCTURE_POINTER    Smbios,
  IN  OUT UINTN                       *TypeSize
  )
{
  UINTN                               FullSize;
  UINTN                               StrLen;
  UINTN                               MaxLen;
  INT8*                               CharInStr;

  if (TypeSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FullSize  = Smbios.Hdr->Length;
  CharInStr = (INT8*)Smbios.Hdr + Smbios.Hdr->Length;
  *TypeSize = FullSize;
  StrLen    = 0;

  //
  // look for the two consecutive zeros, check the string limit by the way.
  //
  while (*CharInStr != 0 || *(CharInStr+1) != 0) {
    if (*CharInStr == 0) {
      *TypeSize += 1;
      CharInStr++;
    }

    MaxLen = SMBIOS_STRING_MAX_LENGTH;
    for (StrLen = 0 ; StrLen < MaxLen; StrLen++) {
      if (*(CharInStr+StrLen) == 0) {
        break;
      }
    }

    if (StrLen == MaxLen) {
      return EFI_INVALID_PARAMETER;
    }

    //
    // forward the pointer
    //
    CharInStr += StrLen;
    *TypeSize += StrLen;
  }

  //
  // count ending two zeros.
  //
  *TypeSize += 2;

  return EFI_SUCCESS;
}

/**
  Add a new SMBIOS structure into SMBIOS database.

  @param Smbios                   SMBIOS structure data buffer pointer.

  @retval EFI_NOT_FOUND           SMBIOS protocol not installed.
  @retval EFI_SUCCESS             Add data structure successfully.

**/
EFI_STATUS
PlatformSmbiosAddNew (
  IN      SMBIOS_STRUCTURE_POINTER    SmbiosPtr
  )
{
  EFI_STATUS                          Status;

  STATIC  EFI_SMBIOS_PROTOCOL         *Smbios = NULL;
  EFI_SMBIOS_HANDLE                   SmbiosHandle;

  if (Smbios == NULL) {
    Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  SmbiosHandle  = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->Add (Smbios, NULL, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*)SmbiosPtr.Hdr);

  return EFI_SUCCESS;
}

/**
  Get the number of instance of SMBIOS type structure in SMBIOS database.
  return 0 means no instance for this type now.

  @param Type                     SMBIOS type.

  @retval Count                   Number of instance.

**/
UINTN
PlatformSmbiosGetInstanceCount (
  IN      UINT8                       Type
  )
{
  EFI_STATUS                          Status;

  STATIC  EFI_SMBIOS_PROTOCOL         *Smbios = NULL;
  EFI_SMBIOS_TABLE_HEADER             *SmbiosRecord;
  EFI_SMBIOS_HANDLE                   SmbiosHandle;
  EFI_SMBIOS_TYPE                     SmbiosType;

  UINTN                               Count = 0;

  if (Smbios == NULL) {
    Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  SmbiosHandle  = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType    = Type;

  do {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosRecord, NULL);
    if (!EFI_ERROR (Status) && (SmbiosHandle != SMBIOS_HANDLE_PI_RESERVED)) {
      Count ++;
    }
  } while (!EFI_ERROR (Status) && (SmbiosHandle != SMBIOS_HANDLE_PI_RESERVED));

  return Count;
}

/**
  Get SMBIOS type data structure in SMBIOS database.

  This function give you a pointer of SMBIOS structure directly in the database, you can update
  the value in formated structure area and it's take affect immediately, but never directly or
  call PlatformSmbiosUpdateString to edit the string in this buffer,
  use PlatformSmbiosGetEditCopy->PlatformSmbiosUpdateType instead.

  One of the SmbiosPtr or Handle must be valid value.

  @param Type                     SMBIOS type.
  @param Instance                 The instance of this type.
  @param SmbiosPtr                Optional parameter, on input, pass a pointer of SMBIOS_STRUCTURE_POINTER
                                  to this function.
                                  On output, return the SMBIOS data pointer in SmbiosPtr.
  @param Handle                   Optional parameter, on input, pass a pointer of Handle.
                                  On output, return the SMBIOS data handle value

  @retval EFI_INVALID_PARAMETER   Both the SmbiosPtr and Handle is NULL.
  @retval EFI_NOT_FOUND           SMBIOS protocol not installed.
  @retval EFI_SUCCESS             Get structure data successfully.

**/
EFI_STATUS
PlatformSmbiosGetInstance (
  IN      UINT8                       Type,
  IN      UINTN                       Instance,
  IN  OUT SMBIOS_STRUCTURE_POINTER    *SmbiosPtr,
  IN  OUT UINT16                      *Handle
  )
{
  EFI_STATUS                          Status;

  STATIC  EFI_SMBIOS_PROTOCOL         *Smbios = NULL;
  EFI_SMBIOS_TABLE_HEADER             *SmbiosRecord;
  EFI_SMBIOS_HANDLE                   SmbiosHandle;
  EFI_SMBIOS_TYPE                     SmbiosType;

  UINTN                               Count = 0;

  if ((SmbiosPtr == NULL) && (Handle == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Smbios == NULL) {
    Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  SmbiosHandle  = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType    = Type;

  do {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosRecord, NULL);
    if (!EFI_ERROR (Status) && (SmbiosHandle != SMBIOS_HANDLE_PI_RESERVED)) {

      if (++Count == Instance) {

        if (SmbiosPtr != NULL) {
          (*SmbiosPtr).Hdr = (SMBIOS_STRUCTURE*)SmbiosRecord;
        }

        if (Handle != NULL) {
          *Handle = SmbiosHandle;
        }

        return EFI_SUCCESS;
      }
    }
  } while (!EFI_ERROR (Status) && (SmbiosHandle != SMBIOS_HANDLE_PI_RESERVED));

  return EFI_NOT_FOUND;
}

/**
  Get a copy of SMBIOS type structure data in SMBIOS database.
  Must allocate memory large enough first, then call this function to get the copy.

  @param Type                     SMBIOS type.
  @param Instance                 The instance of this type.
  @param SmbiosPtr                A valid buffer pointer which SMBIOS data will copy to this buffer.

  @retval EFI_NOT_FOUND           SMBIOS protocol not installed.
  @retval EFI_SUCCESS             Get structure data successfully.

**/
EFI_STATUS
PlatformSmbiosGetEditCopy (
  IN      UINT8                       Type,
  IN      UINTN                       Instance,
  IN  OUT SMBIOS_STRUCTURE_POINTER    SmbiosPtr
  )
{
  EFI_STATUS                          Status;
  SMBIOS_STRUCTURE_POINTER            NewSmbiosPtr;
  UINTN                               Size;

  Status = PlatformSmbiosGetInstance (Type, Instance, &NewSmbiosPtr, NULL);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PlatformSmbiosGetTypeLength (NewSmbiosPtr, &Size);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (SmbiosPtr.Hdr, NewSmbiosPtr.Hdr, Size);

  return EFI_SUCCESS;
}

/**
  Update a string which in SMBIOS database.
  The data structure which string belong to must installed before.

  @param Type                     SMBIOS type.
  @param Instance                 The instance of this type.
  @param StringNumber             The string number.
  @param String                   The string want to update.

  @retval EFI_NOT_FOUND           SMBIOS protocol not installed.
  @retval EFI_SUCCESS             Update data successfully.

**/
EFI_STATUS
PlatformSmbiosUpdateInstalledString (
  IN      UINT8                       Type,
  IN      UINTN                       Instance,
  IN      UINTN                       StringNumber,
  IN      CHAR16                      *String
  )
{
  EFI_STATUS                          Status;
  STATIC  EFI_SMBIOS_PROTOCOL         *Smbios = NULL;
  EFI_SMBIOS_HANDLE                   SmbiosHandle;

  CHAR8                               *AsciiStr = NULL;
  UINTN                               StringSize = 0;

  if (Smbios == NULL) {
    Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  StringSize = StrSize (String);
  AsciiStr = AllocateZeroPool (StringSize);
  UnicodeStrToAsciiStrS (String, AsciiStr, StringSize);

  Status = PlatformSmbiosGetInstance (Type, Instance, NULL, &SmbiosHandle);
  if (!EFI_ERROR (Status)) {
    Status = Smbios->UpdateString (Smbios, &SmbiosHandle, &StringNumber, AsciiStr);
  }

  FreePool (AsciiStr);

  return Status;
}

/**
  Remove a SMBIOS instance in SMBIOS database.

  @param Type                     SMBIOS type.
  @param Instance                 The instance of this type.

  @retval EFI_NOT_FOUND           SMBIOS protocol not installed.
  @retval EFI_SUCCESS             Remove data successfully.

**/
EFI_STATUS
PlatformSmbiosRemoveType (
  IN      UINT8                       Type,
  IN      UINTN                       Instance
  )
{
  EFI_STATUS                          Status;

  STATIC  EFI_SMBIOS_PROTOCOL         *Smbios = NULL;
  EFI_SMBIOS_HANDLE                   SmbiosHandle;

  if (Smbios == NULL) {
    Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  Status = PlatformSmbiosGetInstance (Type, Instance, NULL, &SmbiosHandle);
  if (!EFI_ERROR (Status)) {
    Status = Smbios->Remove (Smbios, SmbiosHandle);
  }

  return Status;
}

/**
  Remove all the instance of specific SMBIOS type in SMBIOS database.

  @param Type                     SMBIOS type.

  @retval EFI_NOT_FOUND           SMBIOS protocol not installed.
  @retval EFI_SUCCESS             Remove data successfully.

**/
EFI_STATUS
PlatformSmbiosRemoveAll (
  IN      UINT8                       Type
  )
{
  EFI_STATUS                          Status;
  UINTN                               Count;
  UINTN                               Index;

  Count = PlatformSmbiosGetInstanceCount (Type);

  for (Index = 0; Index < Count; Index++) {
    Status = PlatformSmbiosRemoveType (Type, 1);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**
  Update SMBIOS data structure in database with new structure data.

  @param Type                     SMBIOS type.
  @param Instance                 The instance of this type.
  @param SmbiosPtr                A valid buffer pointer which new SMBIOS data stored.

  @retval EFI_NOT_FOUND           SMBIOS protocol not installed.
  @retval EFI_SUCCESS             Update data successfully.

**/
EFI_STATUS
PlatformSmbiosUpdateType (
  IN      UINT8                       Type,
  IN      UINTN                       Instance,
  IN      SMBIOS_STRUCTURE_POINTER    SmbiosPtr
  )
{
  EFI_STATUS                          Status;
  STATIC  EFI_SMBIOS_PROTOCOL         *Smbios = NULL;
  EFI_SMBIOS_HANDLE                   SmbiosHandle;

  if (Smbios == NULL) {
    Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  Status = PlatformSmbiosGetInstance (Type, Instance, NULL, &SmbiosHandle);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PlatformSmbiosRemoveType (Type, Instance);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PlatformSmbiosAddNew (SmbiosPtr);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}

/**
  Implement the dynamic SMBIOS data update.

  @param UpdateType               Immediately update or delay update at BDS.

  @retval EFI_SUCCESS             Update successfully.

**/
EFI_STATUS
DispatchSmbiosDynamicUpdate (
  IN      SmbiosUpdateType            UpdateType
  )
{
  EFI_STATUS                              Status;

  UBA_CONFIG_DATABASE_PROTOCOL            *UbaConfigProtocol = NULL;
  UINTN                                   DataLength = 0;
  SMBIOS_UPDATE_DATA                      RegData;

  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &UbaConfigProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DataLength  = sizeof (SMBIOS_UPDATE_DATA);
  Status = UbaConfigProtocol->GetData (
                                    UbaConfigProtocol,
                                    &gPlatformSmbiosConfigDataGuid,
                                    &RegData,
                                    &DataLength
                                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (RegData.Signature == PLATFORM_SMBIOS_UPDATE_SIGNATURE);
  ASSERT (RegData.Version == PLATFORM_SMBIOS_UPDATE_VERSION);

  //
  // Check for updatetype match.
  //
  if ((RegData.UpdateType == UpdateType) && (RegData.CallUpdate != NULL)) {
    //
    // Invoke the callback and update every instance of this type
    //
    Status = RegData.CallUpdate ();
  }

  return EFI_SUCCESS;
}

/**
  Function provide to DXE driver, which initial the dynamic update.

  @param NULL

  @retval EFI_NOT_FOUND           Required protocol not found.
  @retval EFI_SUCCESS             Init successfully.

**/
EFI_STATUS
PlatformInitSmbiosUpdate (
  VOID
  )
{
  EFI_STATUS                        Status;

  DEBUG ((DEBUG_INFO, "UBA SMBIOS Update Library: PlatformInitSmbiosUpdate!\n"));
  Status = DispatchSmbiosDynamicUpdate (SmbiosDelayUpdate);

  return Status;
}
