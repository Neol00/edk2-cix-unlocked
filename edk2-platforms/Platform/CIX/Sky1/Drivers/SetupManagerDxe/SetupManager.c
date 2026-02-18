/**
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "SetupManager.h"
#include <Library/CpuInfoLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <PlatformSetupVar.h>
#include <Guid/NetworkStackSetup.h>
#include <Guid/ConsolePrefFormSet.h>

SETUP_MANAGER_CALLBACK_DATA  gSetupManagerPrivate = {
  SETUP_MANAGER_CALLBACK_DATA_SIGNATURE,
  NULL,
  NULL,
  {
    SetupManagerExtractConfig,
    SetupManagerRouteConfig,
    SetupManagerCallback
  }
};

EFI_GUID  mSetupManagerGuid = SETUP_MANAGER_FORMSET_GUID;

typedef struct {
  CHAR8    MonthNameStr[4]; // example "Jan", Compiler build date, month
  CHAR8    DigitStr[3];     // example "01", Smbios date format, month
} MonthStringDig;

STATIC MonthStringDig  MonthMatch[12] = {
  { "Jan", "01" },
  { "Feb", "02" },
  { "Mar", "03" },
  { "Apr", "04" },
  { "May", "05" },
  { "Jun", "06" },
  { "Jul", "07" },
  { "Aug", "08" },
  { "Sep", "09" },
  { "Oct", "10" },
  { "Nov", "11" },
  { "Dec", "12" }
};

HII_VENDOR_DEVICE_PATH  mSetupManagerHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8)(sizeof (VENDOR_DEVICE_PATH)),
        (UINT8)((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    SETUP_MANAGER_FORMSET_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8)(END_DEVICE_PATH_LENGTH),
      (UINT8)((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

/**
  This function allows a caller to extract the current configuration for one
  or more named elements from the target driver.


  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Request         A null-terminated Unicode string in <ConfigRequest> format.
  @param Progress        On return, points to a character in the Request string.
                         Points to the string's null terminator if request was successful.
                         Points to the most recent '&' before the first failing name/value
                         pair (or the beginning of the string if the failure is in the
                         first name/value pair) if the request was not successful.
  @param Results         A null-terminated Unicode string in <ConfigAltResp> format which
                         has all values filled in for the names in the Request string.
                         String to be allocated by the called function.

  @retval  EFI_SUCCESS            The Results is filled with the requested values.
  @retval  EFI_OUT_OF_RESOURCES   Not enough memory to store the results.
  @retval  EFI_INVALID_PARAMETER  Request is illegal syntax, or unknown name.
  @retval  EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
SetupManagerExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  CONST EFI_STRING                      Request,
  OUT EFI_STRING                            *Progress,
  OUT EFI_STRING                            *Results
  )
{
  if ((Progress == NULL) || (Results == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Request;
  return EFI_NOT_FOUND;
}

/**
  This function processes the results of changes in configuration.

  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Configuration   A null-terminated Unicode string in <ConfigResp> format.
  @param Progress        A pointer to a string filled in with the offset of the most
                         recent '&' before the first failing name/value pair (or the
                         beginning of the string if the failure is in the first
                         name/value pair) or the terminating NULL if all was successful.

  @retval  EFI_SUCCESS            The Results is processed successfully.
  @retval  EFI_INVALID_PARAMETER  Configuration is NULL.
  @retval  EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
SetupManagerRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  CONST EFI_STRING                      Configuration,
  OUT EFI_STRING                            *Progress
  )
{
  if ((Configuration == NULL) || (Progress == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Configuration;

  return EFI_NOT_FOUND;
}

/**
  This function is invoked if user selected a interactive opcode from Device Manager's
  Formset. If user set VBIOS, the new value is saved to EFI variable.

  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Action          Specifies the type of action taken by the browser.
  @param QuestionId      A unique value which is sent to the original exporting driver
                         so that it can identify the type of data to expect.
  @param Type            The type of value for the question.
  @param Value           A pointer to the data being sent to the original exporting driver.
  @param ActionRequest   On return, points to the action requested by the callback function.

  @retval  EFI_SUCCESS           The callback successfully handled the action.
  @retval  EFI_INVALID_PARAMETER The setup browser call this function with invalid parameters.

**/
EFI_STATUS
EFIAPI
SetupManagerCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  EFI_BROWSER_ACTION                    Action,
  IN  EFI_QUESTION_ID                       QuestionId,
  IN  UINT8                                 Type,
  IN  EFI_IFR_TYPE_VALUE                    *Value,
  OUT EFI_BROWSER_ACTION_REQUEST            *ActionRequest
  )
{
  if ((Action != EFI_BROWSER_ACTION_CHANGING) && (Action != EFI_BROWSER_ACTION_SUBMITTED)) {
    return EFI_UNSUPPORTED;
  }

  if ((Value == NULL) || (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Action == EFI_BROWSER_ACTION_CHANGING) {
    switch (QuestionId) {
      case QUESTION_RESET_ALL_DEFAULTS:
      {
        EFI_INPUT_KEY  Key;

        do {
          CreatePopUp (
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
            &Key,
            L"",
            L"Reset all BIOS settings to factory defaults?",
            L"Press ENTER to confirm, ESC to cancel.",
            L"",
            NULL
            );
        } while ((Key.ScanCode != SCAN_ESC) && (Key.UnicodeChar != CHAR_CARRIAGE_RETURN));

        if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
          gRT->SetVariable (PLATFORM_SETUP_VAR, &gPlatformSetupVariableGuid, 0, 0, NULL);
          gRT->SetVariable (NETWORK_STACK_VAR, &gEfiNetworkStackSetupGuid, 0, 0, NULL);
          gRT->SetVariable (L"ConsolePref", &gConsolePrefFormSetGuid, 0, 0, NULL);
          gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
        }
        break;
      }

      case QUESTION_CLEAR_NVRAM:
      {
        EFI_INPUT_KEY  Key;
        EFI_STATUS     VarStatus;
        CHAR16         VarName[256];
        EFI_GUID       VarGuid;
        UINTN          VarNameSize;
        UINTN          MaxEntries;
        UINTN          EntryCount;

        typedef struct {
          CHAR16    Name[256];
          EFI_GUID  Guid;
        } VAR_ENTRY;

        VAR_ENTRY  *Entries;
        UINTN      Index;

        do {
          CreatePopUp (
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
            &Key,
            L"",
            L"Clear ALL stored NVRAM variables?",
            L"This is equivalent to a fresh BIOS flash.",
            L"Press ENTER to confirm, ESC to cancel.",
            L"",
            NULL
            );
        } while ((Key.ScanCode != SCAN_ESC) && (Key.UnicodeChar != CHAR_CARRIAGE_RETURN));

        if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
          //
          // Enumerate all variables first, then delete them.
          // Cannot delete during enumeration as it invalidates the iterator.
          //
          MaxEntries = 512;
          Entries = AllocateZeroPool (MaxEntries * sizeof (VAR_ENTRY));
          if (Entries == NULL) {
            break;
          }

          EntryCount = 0;
          VarName[0] = L'\0';
          ZeroMem (&VarGuid, sizeof (EFI_GUID));

          while (TRUE) {
            VarNameSize = sizeof (VarName);
            VarStatus = gRT->GetNextVariableName (&VarNameSize, VarName, &VarGuid);
            if (EFI_ERROR (VarStatus)) {
              break;
            }
            if (EntryCount < MaxEntries) {
              StrnCpyS (Entries[EntryCount].Name, 256, VarName, 255);
              CopyGuid (&Entries[EntryCount].Guid, &VarGuid);
              EntryCount++;
            }
          }

          for (Index = 0; Index < EntryCount; Index++) {
            gRT->SetVariable (Entries[Index].Name, &Entries[Index].Guid, 0, 0, NULL);
          }

          FreePool (Entries);
          gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
        }
        break;
      }

      default:
        break;
    }
  }

  return EFI_SUCCESS;
}

/**
  Extract device path for given HII handle and class guid.

  @param Handle          The HII handle.

  @retval  NULL          Fail to get the device path string.
  @return  PathString    Get the device path string.

**/
CHAR16 *
DmExtractDevicePathFromHiiHandle (
  IN      EFI_HII_HANDLE  Handle
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  DriverHandle;

  ASSERT (Handle != NULL);

  if (Handle == NULL) {
    return NULL;
  }

  Status = gHiiDatabase->GetPackageListHandle (gHiiDatabase, Handle, &DriverHandle);
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  //
  // Get device path string.
  //
  return ConvertDevicePathToText (DevicePathFromHandle (DriverHandle), FALSE, FALSE);
}

/**
  Dynamic create Hii information for Device Manager.

  @param   NextShowFormId     The FormId which need to be show.

**/
VOID
CreateSetupManagerForm (
  IN EFI_FORM_ID  NextShowFormId
  )
{
  UINTN               Index;
  EFI_STRING          String;
  EFI_STRING_ID       Token;
  EFI_STRING_ID       TokenHelp;
  EFI_HII_HANDLE      *HiiHandles;
  EFI_HII_HANDLE      HiiHandle;
  EFI_GUID            FormSetGuid;
  VOID                *StartOpCodeHandle;
  VOID                *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL  *StartLabel;
  EFI_IFR_GUID_LABEL  *EndLabel;
  BOOLEAN             AddNetworkMenu;
  UINTN               AddItemCount;
  // UINTN               NewStringLen;
  // EFI_STRING          NewStringTitle;
  CHAR16            *DevicePathStr;
  EFI_STRING_ID     DevicePathId;
  EFI_IFR_FORM_SET  *Buffer;
  UINTN             BufferSize;
  UINT8             ClassGuidNum;
  EFI_GUID          *ClassGuid;
  UINTN             TempSize;
  UINT8             *Ptr;
  EFI_STATUS        Status;

  TempSize   = 0;
  BufferSize = 0;
  Buffer     = NULL;

  HiiHandle      = gSetupManagerPrivate.HiiHandle;
  AddNetworkMenu = FALSE;
  AddItemCount   = 0;
  //
  // Allocate space for creation of UpdateData Buffer
  //
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (EndOpCodeHandle != NULL);

  //
  // Create Hii Extend Label OpCode as the start opcode
  //
  StartLabel               = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = LABEL_PRIVATE_FORM_LIST;

  //
  // Create Hii Extend Label OpCode as the end opcode
  //
  EndLabel               = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_PRIVATE_FORM_END;

  //
  // Get all the Hii handles
  //
  HiiHandles = HiiGetHiiHandles (NULL);
  ASSERT (HiiHandles != NULL);

  //
  // Search for formset of each class type
  //
  for (Index = 0; HiiHandles[Index] != NULL; Index++) {
    Status = HiiGetFormSetFromHiiHandle (HiiHandles[Index], &Buffer, &BufferSize);
    if (EFI_ERROR (Status)) {
      continue;
    }

    Ptr = (UINT8 *)Buffer;
    while (TempSize < BufferSize) {
      TempSize += ((EFI_IFR_OP_HEADER *)Ptr)->Length;
      if (((EFI_IFR_OP_HEADER *)Ptr)->Length <= OFFSET_OF (EFI_IFR_FORM_SET, Flags)) {
        Ptr += ((EFI_IFR_OP_HEADER *)Ptr)->Length;
        continue;
      }

      ClassGuidNum = (UINT8)(((EFI_IFR_FORM_SET *)Ptr)->Flags & 0x3);
      ClassGuid    = (EFI_GUID *)(VOID *)(Ptr + sizeof (EFI_IFR_FORM_SET));
      while (ClassGuidNum-- > 0) {
        if (CompareGuid (&gCixPrivateSetupFormsetGuid, ClassGuid) == 0) {
          ClassGuid++;
          continue;
        }

        String = HiiGetString (HiiHandles[Index], ((EFI_IFR_FORM_SET *)Ptr)->FormSetTitle, NULL);
        if (String == NULL) {
          String = HiiGetString (HiiHandle, STRING_TOKEN (STR_MISSING_STRING), NULL);
          ASSERT (String != NULL);
        }

        Token = HiiSetString (HiiHandle, 0, String, NULL);
        FreePool (String);

        String = HiiGetString (HiiHandles[Index], ((EFI_IFR_FORM_SET *)Ptr)->Help, NULL);
        if (String == NULL) {
          String = HiiGetString (HiiHandle, STRING_TOKEN (STR_MISSING_STRING), NULL);
          ASSERT (String != NULL);
        }

        TokenHelp = HiiSetString (HiiHandle, 0, String, NULL);
        FreePool (String);

        CopyMem (&FormSetGuid, &((EFI_IFR_FORM_SET *)Ptr)->Guid, sizeof (EFI_GUID));

        if (NextShowFormId == SETUP_MANAGER_FORM_ID) {
          DevicePathStr = DmExtractDevicePathFromHiiHandle (HiiHandles[Index]);
          DevicePathId  = 0;
          if (DevicePathStr != NULL) {
            DevicePathId =  HiiSetString (HiiHandle, 0, DevicePathStr, NULL);
            FreePool (DevicePathStr);
          }

          HiiCreateGotoExOpCode (
            StartOpCodeHandle,
            0,
            Token,
            TokenHelp,
            0,
            (EFI_QUESTION_ID)(Index + DEVICE_KEY_OFFSET),
            0,
            &FormSetGuid,
            DevicePathId
            );
        }

        break;
      }

      Ptr += ((EFI_IFR_OP_HEADER *)Ptr)->Length;
    }

    FreePool (Buffer);
    Buffer     = NULL;
    TempSize   = 0;
    BufferSize = 0;
  }

  HiiUpdateForm (
    HiiHandle,
    &mSetupManagerGuid,
    NextShowFormId,
    StartOpCodeHandle,
    EndOpCodeHandle
    );

  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);
  FreePool (HiiHandles);
}

VOID
AsciiToUnicode (
  IN    CHAR8   *AsciiString,
  IN    CHAR16  *UnicodeString
  )
{
  UINT8  Index;

  Index = 0;
  while (AsciiString[Index] != 0) {
    UnicodeString[Index] = (CHAR16)AsciiString[Index];
    Index++;
  }
}

STATIC
VOID
ConstructBuildDate (
  OUT CHAR8  *DateBuf
  )
{
  UINTN  i;

  // GCC __DATE__ format is "Feb  2 1996"
  // If the day of the month is less than 10, it is padded with a space on the left
  CHAR8  *BuildDate = __DATE__;

  // SMBIOS spec date string: MM/DD/YYYY
  CHAR8  SmbiosDateStr[sizeof (RELEASE_DATE_TEMPLATE)] = { 0 };

  SmbiosDateStr[sizeof (RELEASE_DATE_TEMPLATE) - 1] = '\0';

  SmbiosDateStr[2] = '/';
  SmbiosDateStr[5] = '/';

  // Month
  for (i = 0; i < sizeof (MonthMatch) / sizeof (MonthMatch[0]); i++) {
    if (AsciiStrnCmp (&BuildDate[0], MonthMatch[i].MonthNameStr, AsciiStrLen (MonthMatch[i].MonthNameStr)) == 0) {
      CopyMem (&SmbiosDateStr[0], MonthMatch[i].DigitStr, AsciiStrLen (MonthMatch[i].DigitStr));
      break;
    }
  }

  // Day
  CopyMem (&SmbiosDateStr[3], &BuildDate[4], 2);
  if (BuildDate[4] == ' ') {
    // day is less then 10, SAPCE filed by compiler, SMBIOS requires 0
    SmbiosDateStr[3] = '0';
  }

  // Year
  CopyMem (&SmbiosDateStr[6], &BuildDate[7], 4);

  CopyMem (DateBuf, SmbiosDateStr, AsciiStrLen (RELEASE_DATE_TEMPLATE));
}

VOID
InitializeHardwareInfo (
  )
{
  EFI_STATUS             Status;
  EFI_HII_HANDLE         HiiHandle;
  BOARD_ID_INFO          BoardID;
  CIX_SOC_INFO_PROTOCOL  *pCixSocInfoProtocol = NULL;
  EC_PLATFORM_PROTOCOL   *pEcPlatformProtocol = NULL;
  EC_RESPONSE            EcResponse;
  CHAR8                  DateBuf[11]   = { 0 };
  CHAR16                 NewString[11] = { 0 };

  Status = gBS->LocateProtocol (&gCixEcPlatformProtocolGuid, NULL, (VOID **)&pEcPlatformProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Locate Protocol failed for %g\n", &gCixEcPlatformProtocolGuid));
    return;
  }

  Status = pEcPlatformProtocol->Transfer (pEcPlatformProtocol, EC_COMMAND_GET_BOARD_ID, NULL, &EcResponse);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Get Board ID failed\n"));
    return;
  }

  Status = gBS->LocateProtocol (&gCixSocInfoProtocolGuid, NULL, (VOID **)&pCixSocInfoProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Locate Protocol failed for %g\n", &gCixSocInfoProtocolGuid));
    return;
  }

  HiiHandle = gSetupManagerPrivate.HiiHandle;

  ZeroMem (DateBuf, sizeof (DateBuf));
  ZeroMem (NewString, sizeof (NewString));
  AsciiSPrint ((CHAR8 *)DateBuf, sizeof (DateBuf), "%dGB", pCixSocInfoProtocol->MemInfo->TotalSize/1024);
  AsciiToUnicode (DateBuf, NewString);
  HiiSetString (HiiHandle, STRING_TOKEN (STR_MEMORY_SIZE_VALUE), NewString, NULL);

  ZeroMem (DateBuf, sizeof (DateBuf));
  ZeroMem (NewString, sizeof (NewString));
  AsciiSPrint ((CHAR8 *)DateBuf, sizeof (DateBuf), "%d MT/s", pCixSocInfoProtocol->MemInfo->MaxFreq * 2);
  AsciiToUnicode (DateBuf, NewString);
  HiiSetString (HiiHandle, STRING_TOKEN (STR_MEMORY_FREQ_VALUE), NewString, NULL);

  {
    UINTN  CpuFreqHz = GetMaxCpuFreq ();
    UINTN  CpuFreqMHz = CpuFreqHz / 1000000;
    ZeroMem (DateBuf, sizeof (DateBuf));
    ZeroMem (NewString, sizeof (NewString));
    AsciiSPrint ((CHAR8 *)DateBuf, sizeof (DateBuf), "%d MHz", CpuFreqMHz);
    AsciiToUnicode (DateBuf, NewString);
    HiiSetString (HiiHandle, STRING_TOKEN (STR_CPU_SPEED_VALUE), NewString, NULL);
  }

  BoardID.Bits.PcbSku = EcResponse.BoardId.Id.Sku + (EcResponse.BoardId.Id.SkuExt << 3);
  switch (BoardID.Bits.PcbSku) {
    case 0:
      HiiSetString (HiiHandle, STRING_TOKEN (STR_PCB_SKU_VALUE), L"MERAK", NULL);
      break;
    case 1:
      HiiSetString (HiiHandle, STRING_TOKEN (STR_PCB_SKU_VALUE), L"MIZAR", NULL);
      break;
    case 2:
      HiiSetString (HiiHandle, STRING_TOKEN (STR_PCB_SKU_VALUE), L"MERAK DAP", NULL);
      break;
    case 3:
      HiiSetString (HiiHandle, STRING_TOKEN (STR_PCB_SKU_VALUE), L"PHECDA SLT", NULL);
      break;
    case 4:
      HiiSetString (HiiHandle, STRING_TOKEN (STR_PCB_SKU_VALUE), L"PHECDA", NULL);
      break;
    case 5:
      HiiSetString (HiiHandle, STRING_TOKEN (STR_PCB_SKU_VALUE), L"MEGREZ", NULL);
      break;
    case 6:
      HiiSetString (HiiHandle, STRING_TOKEN (STR_PCB_SKU_VALUE), L"PHECDA DAP", NULL);
      break;
    default:
      HiiSetString (HiiHandle, STRING_TOKEN (STR_PCB_SKU_VALUE), L"Undefined", NULL);
      break;
  }

  BoardID.Bits.MemType = EcResponse.BoardId.Id.Memory + (EcResponse.BoardId.Id.MemExt << 3);
  switch (BoardID.Bits.MemType) {
    case 0:
      HiiSetString (HiiHandle, STRING_TOKEN (STR_MEMORY_TYPE_VALUE), L"4GB Samsung LP5 315b", NULL);
      break;
    case 1:
      HiiSetString (HiiHandle, STRING_TOKEN (STR_MEMORY_TYPE_VALUE), L"4GB Hynix LP4x 200b", NULL);
      break;
    case 3:
      HiiSetString (HiiHandle, STRING_TOKEN (STR_MEMORY_TYPE_VALUE), L"8GB Samsung LP5 315b", NULL);
      break;
    case 4:
      HiiSetString (HiiHandle, STRING_TOKEN (STR_MEMORY_TYPE_VALUE), L"8GB Samsung LP5 441b", NULL);
      break;
    default:
      HiiSetString (HiiHandle, STRING_TOKEN (STR_MEMORY_TYPE_VALUE), L"Undefined", NULL);
      break;
  }

  BoardID.Bits.BoardRev = EcResponse.BoardId.Id.Rev;
  switch (BoardID.Bits.BoardRev) {
    case 0:
      HiiSetString (HiiHandle, STRING_TOKEN (STR_BOARD_REV_VALUE), L"Rev A", NULL);
      break;
    case 1:
      HiiSetString (HiiHandle, STRING_TOKEN (STR_BOARD_REV_VALUE), L"Rev B", NULL);
      break;
    case 2:
      HiiSetString (HiiHandle, STRING_TOKEN (STR_BOARD_REV_VALUE), L"Rev C", NULL);
      break;
    case 3:
      HiiSetString (HiiHandle, STRING_TOKEN (STR_BOARD_REV_VALUE), L"Rev D", NULL);
      break;
    default:
      break;
  }

  Status = pEcPlatformProtocol->Transfer (pEcPlatformProtocol, EC_COMMAND_GET_PD_VERSION, NULL, &EcResponse);
  if (!EFI_ERROR (Status)) {
    ZeroMem (DateBuf, sizeof (DateBuf));
    ZeroMem (NewString, sizeof (NewString));
    AsciiSPrint (
      (CHAR8 *)DateBuf,
      sizeof (DateBuf),
      "%d.%d %d.%d",
      EcResponse.PdVer.Pd2Ver & 0xFF,
      (EcResponse.PdVer.Pd2Ver & 0xFF00) >> 8,
      EcResponse.PdVer.Pd1Ver & 0xFF,
      (EcResponse.PdVer.Pd1Ver & 0xFF00) >> 8
      );
    AsciiToUnicode (DateBuf, NewString);
    HiiSetString (HiiHandle, STRING_TOKEN (STR_PD_VER_VALUE), NewString, NULL);
  }

  Status = pEcPlatformProtocol->Transfer (pEcPlatformProtocol, EC_COMMAND_GET_PMIC_VERSION, NULL, &EcResponse);
  if (!EFI_ERROR (Status)) {
    ZeroMem (DateBuf, sizeof (DateBuf));
    ZeroMem (NewString, sizeof (NewString));
    AsciiSPrint ((CHAR8 *)DateBuf, sizeof (DateBuf), "%d %d %d", EcResponse.PmicVer.Pmic3Ver, EcResponse.PmicVer.Pmic2Ver, EcResponse.PmicVer.Pmic1Ver);
    AsciiToUnicode (DateBuf, NewString);
    HiiSetString (HiiHandle, STRING_TOKEN (STR_PMIC_VER_VALUE), NewString, NULL);
  }
}

VOID
EFIAPI
GetPlatformInfo (
  )
{
  CHAR8           *ReleaseDateBuf = NULL;
  CHAR16          *NewString;
  UINTN           StrSize;
  EFI_HII_HANDLE  HiiHandle;

  HiiHandle = gSetupManagerPrivate.HiiHandle;

  NewString = (CHAR16 *)FixedPcdGetPtr (PcdFirmwareVersionString);
  HiiSetString (HiiHandle, STRING_TOKEN (STR_BIOS_VERSION_VALUE), NewString, NULL);

  StrSize        = AsciiStrSize (RELEASE_DATE_TEMPLATE);
  ReleaseDateBuf = AllocateZeroPool (StrSize);

  if (ReleaseDateBuf != NULL) {
    ConstructBuildDate (ReleaseDateBuf);
    NewString = AllocateZeroPool (StrSize*sizeof (CHAR16));
    if (NewString != NULL) {
      AsciiStrToUnicodeStrS (ReleaseDateBuf, NewString, StrSize);
      HiiSetString (HiiHandle, STRING_TOKEN (STR_BIOS_RELEASE_DATE_VALUE), NewString, NULL);
      FreePool (NewString);
    }

    FreePool (ReleaseDateBuf);
  }

  // NewString = L"1.5GHz";
  // HiiSetString (HiiHandle, STRING_TOKEN (STR_CPU_SPEED_VALUE), NewString, NULL);
  // NewString = L"4GB";
  // HiiSetString (HiiHandle, STRING_TOKEN (STR_MEMORY_SIZE_VALUE), NewString, NULL);
  // NewString = L"2000MHz";
  // HiiSetString (HiiHandle, STRING_TOKEN (STR_MEMORY_FREQ_VALUE), NewString, NULL);
  InitializeHardwareInfo ();
}

VOID
EFIAPI
GetFirmwareVersionInfo (
  )
{
  EFI_STATUS               Status;
  EFI_HII_HANDLE           HiiHandle;
  CIX_FW_VERSION_PROTOCOL  *pFwVerProtocol;
  CHAR16                   *pFwVerBuff;
  UINT32                   FwVerSize;

  HiiHandle = gSetupManagerPrivate.HiiHandle;
  Status    = gBS->LocateProtocol (
                     &gCixFwVersionProtocolGuid,
                     NULL,
                     (VOID **)&pFwVerProtocol
                     );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to locate CIX_FW_VERSION_PROTOCOL\n"));
    return;
  }

  Status = pFwVerProtocol->GetFwVersion (FwVerSE, &pFwVerBuff, &FwVerSize);
  if (!EFI_ERROR (Status)) {
    HiiSetString (HiiHandle, STRING_TOKEN (STR_SE_VER_VALUE), pFwVerBuff, NULL);
  } else {
    HiiSetString (HiiHandle, STRING_TOKEN (STR_SE_VER_VALUE), L"N/A", NULL);
  }

  Status = pFwVerProtocol->GetFwVersion (FwVerPBL, &pFwVerBuff, &FwVerSize);
  if (!EFI_ERROR (Status)) {
    HiiSetString (HiiHandle, STRING_TOKEN (STR_PBL_VER_VALUE), pFwVerBuff, NULL);
  } else {
    HiiSetString (HiiHandle, STRING_TOKEN (STR_PBL_VER_VALUE), L"N/A", NULL);
  }

  Status = pFwVerProtocol->GetFwVersion (FwVerATF, &pFwVerBuff, &FwVerSize);
  if (!EFI_ERROR (Status)) {
    HiiSetString (HiiHandle, STRING_TOKEN (STR_ATF_VER_VALUE), pFwVerBuff, NULL);
  } else {
    HiiSetString (HiiHandle, STRING_TOKEN (STR_ATF_VER_VALUE), L"N/A", NULL);
  }

  Status = pFwVerProtocol->GetFwVersion (FwVerPM, &pFwVerBuff, &FwVerSize);
  if (!EFI_ERROR (Status)) {
    HiiSetString (HiiHandle, STRING_TOKEN (STR_PM_VER_VALUE), pFwVerBuff, NULL);
  } else {
    HiiSetString (HiiHandle, STRING_TOKEN (STR_PM_VER_VALUE), L"N/A", NULL);
  }

  Status = pFwVerProtocol->GetFwVersion (FwVerTEE, &pFwVerBuff, &FwVerSize);
  if (!EFI_ERROR (Status)) {
    HiiSetString (HiiHandle, STRING_TOKEN (STR_TEE_VER_VALUE), pFwVerBuff, NULL);
  } else {
    HiiSetString (HiiHandle, STRING_TOKEN (STR_TEE_VER_VALUE), L"N/A", NULL);
  }

  Status = pFwVerProtocol->GetFwVersion (FwVerUEFI, &pFwVerBuff, &FwVerSize);
  if (!EFI_ERROR (Status)) {
    HiiSetString (HiiHandle, STRING_TOKEN (STR_UEFI_VER_VALUE), pFwVerBuff, NULL);
  } else {
    HiiSetString (HiiHandle, STRING_TOKEN (STR_UEFI_VER_VALUE), L"N/A", NULL);
  }

  Status = pFwVerProtocol->GetFwVersion (FwVerEC, &pFwVerBuff, &FwVerSize);
  if (!EFI_ERROR (Status)) {
    HiiSetString (HiiHandle, STRING_TOKEN (STR_EC_VER_VALUE), pFwVerBuff, NULL);
  } else {
    HiiSetString (HiiHandle, STRING_TOKEN (STR_EC_VER_VALUE), L"N/A", NULL);
  }
}

/**
  Install Boot Manager Menu driver.

  @param ImageHandle     The image handle.
  @param SystemTable     The system table.

  @retval  EFI_SUCEESS  Install Boot manager menu success.
  @retval  Other        Return error status.

**/
EFI_STATUS
EFIAPI
SetupManagerDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  gSetupManagerPrivate.DriverHandle = NULL;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &gSetupManagerPrivate.DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mSetupManagerHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &gSetupManagerPrivate.ConfigAccess,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Publish our HII data.
  //
  gSetupManagerPrivate.HiiHandle = HiiAddPackages (
                                     &mSetupManagerGuid,
                                     gSetupManagerPrivate.DriverHandle,
                                     SetupManagerVfrBin,
                                     SetupManagerDxeStrings,
                                     NULL
                                     );
  ASSERT (gSetupManagerPrivate.HiiHandle != NULL);

  //
  // The device manager form contains a page listing all the network
  // controllers in the system. This list can only be populated if all
  // handles have been connected, so do it here.
  //
  // EfiBootManagerConnectAll ();

  //
  // Update boot manager page
  //
  CreateSetupManagerForm (SETUP_MANAGER_FORM_ID);

  GetPlatformInfo ();
  GetFirmwareVersionInfo ();

  return EFI_SUCCESS;
}
