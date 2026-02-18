/** @file
  UBA smbios Update Library Header File.

  @copyright
  Copyright 2012 - 2015 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _UBA_SMBIOS_UPDATE_LIB_
#define _UBA_SMBIOS_UPDATE_LIB_

#include <Base.h>
#include <Uefi.h>
#include <IndustryStandard/SmBios.h>


typedef enum
{
  SmbiosDelayUpdate,
  SmbiosNormalUpdate,
} SmbiosUpdateType;


#define PLATFORM_SMBIOS_UPDATE_SIGNATURE  SIGNATURE_32 ('P', 'S', 'M', 'B')
#define PLATFORM_SMBIOS_UPDATE_VERSION    01

// {AAC6CAFD-42C6-440a-B958-9FD4C84B50EA}
STATIC EFI_GUID  gPlatformSmbiosConfigDataGuid =
{ 0xaac6cafd, 0x42c6, 0x440a, { 0xb9, 0x58, 0x9f, 0xd4, 0xc8, 0x4b, 0x50, 0xea } };


/**
  Callback function for SMBIOS dynamic update.

  @param Smbios                   The SMBIOS data buffer pointer.
  @param BufferSize               The SMBIOS data buffer size allocated for you.
  @param Instance                 Instance number for this type.

  @retval EFI_INVALID_PARAMETER   Check your register data if the call return this error.
  @retval EFI_NOT_FOUND           The data process have error occur.
  @retval EFI_SUCCESS             Data have been updated successfully.

**/
typedef
EFI_STATUS
(*SMBIOS_UPDATE_CALLBACK) (
  VOID
);

typedef struct {
  UINT32                  Signature;
  UINT32                  Version;

  UINT32                  PlatformType;
  SmbiosUpdateType        UpdateType;       // DelayUpdate or NormalUpdate
  SMBIOS_UPDATE_CALLBACK  CallUpdate;
} SMBIOS_UPDATE_DATA;

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
);

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
);

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
);

/**
  Add a new SMBIOS structure into SMBIOS database.

  @param Smbios                   SMBIOS structure data buffer pointer.

  @retval EFI_NOT_FOUND           SMBIOS protocol not installed.
  @retval EFI_SUCCESS             Add data structure successfully.

**/
EFI_STATUS
PlatformSmbiosAddNew (
  IN      SMBIOS_STRUCTURE_POINTER    SmbiosPtr
);

/**
  Get the number of instance of SMBIOS type structure in SMBIOS database.
  return 0 means no instance for this type now.

  @param Type                     SMBIOS type.

  @retval Count                   Number of instance.

**/
UINTN
PlatformSmbiosGetInstanceCount (
  IN      UINT8                       Type
);

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
);

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
);

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
);

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
);

/**
  Remove all the instance of specific SMBIOS type in SMBIOS database.

  @param Type                     SMBIOS type.

  @retval EFI_NOT_FOUND           SMBIOS protocol not installed.
  @retval EFI_SUCCESS             Remove data successfully.

**/
EFI_STATUS
PlatformSmbiosRemoveAll (
  IN      UINT8                       Type
);

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
);

/**
  Function provide to DXE driver, which initial the dynamic update.

  @param NULL

  @retval EFI_NOT_FOUND           Required protocol not found.
  @retval EFI_SUCCESS             Init successfully.

**/
EFI_STATUS
PlatformInitSmbiosUpdate (
  VOID
);

#endif //_UBA_SMBIOS_UPDATE_LIB_
