/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <PiDxe.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/ShMemLib.h>
#include <Protocol/FwVersionProtocol.h>
#include <Protocol/EcPlatformProtocol.h>
#include <Protocol/SocInfoProtocol.h>
#include <Library/CacheMaintenanceLib.h>
#include "UefiMemRecords.h"

// #ifdef DEBUG
// #undef DEBUG
// #define DEBUG(Expression) DebugPrint Expression
// #endif

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

UINTN
UnicodeToAscii (
  IN CONST CHAR16  *UStr,
  IN CONST UINTN   Length,
  OUT CHAR8        *AStr
  )
{
  UINTN  Index;

  //
  // just buffer copy, not character copy
  //
  for (Index = 0; Index < Length; Index++) {
    *AStr++ = (CHAR8)*UStr++;
  }

  return Index;
}

STATIC
EFI_STATUS
EFIAPI
FwGetEcVersion (
  IN OUT CHAR16  **FwVerBuff,
  IN OUT UINT32  *FwVerSize
  )
{
  EFI_STATUS            Status;
  EC_PLATFORM_PROTOCOL  *Ec;
  EC_RESPONSE           Response;

  Status = gBS->LocateProtocol (&gCixEcPlatformProtocolGuid, NULL, (VOID **)&Ec);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "EC platform not ready.\n"));
    return EFI_NOT_FOUND;
  }

  Status = Ec->Transfer (Ec, EC_COMMAND_GET_EC_VERSION, NULL, &Response);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "EC platform not ready.\n"));
    return EFI_NOT_FOUND;
  }

  *FwVerSize = AsciiStrLen ((CHAR8 *)Response.Version.String);
  if (*FwVerSize == 0) {
    return EFI_NOT_FOUND;
  }

  *FwVerBuff = AllocateZeroPool ((*FwVerSize + 1) * 2);

  AsciiToUnicode ((CHAR8 *)Response.Version.String, (CHAR16 *)*FwVerBuff);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
FwVerGetBoardId (
  IN OUT CHAR16  **FwVerBuff,
  IN OUT UINT32  *FwVerSize
  )
{
  EFI_STATUS            Status;
  EC_PLATFORM_PROTOCOL  *Ec;

  DEBUG ((DEBUG_INFO, "%a Entry\n", __FUNCTION__));

  Status = gBS->LocateProtocol (&gCixEcPlatformProtocolGuid, NULL, (VOID **)&Ec);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "EC platform not ready.\n"));
    return Status;
  }

  *FwVerSize = sizeof (EC_RESPONSE_BOARD_ID);
  *FwVerBuff = AllocateZeroPool (*FwVerSize);

  Status = Ec->Transfer (Ec, EC_COMMAND_GET_BOARD_ID, NULL, (EC_RESPONSE *)(*FwVerBuff));
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "EC platform not ready.\n"));
    return Status;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
GetFwVersion (
  FW_VERSION_TYPE  FwVerType,
  IN OUT CHAR16    **FwVerBuff,
  IN OUT UINT32    *FwVerSize
  )
{
  void        *FwVerAddr = NULL;
  EFI_STATUS  Status;
  UINT32      SmemSize;

  if ((FwVerBuff == NULL) || (FwVerSize == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  switch (FwVerType) {
    case FwVerSE:
      FwVerAddr = SmemGetAddr (SMEM_VER_SE, &SmemSize);
      if (FwVerAddr == NULL) {
        return EFI_NOT_FOUND;
      }

      // DEBUG ((DEBUG_INFO, "[VER] PBL:%s\n", FwVerAddr));
      break;
    case FwVerPM:
      FwVerAddr = SmemGetAddr (SMEM_VER_PM, &SmemSize);
      if (FwVerAddr == NULL) {
        return EFI_NOT_FOUND;
      }

      // DEBUG ((DEBUG_INFO, "[VER] PBL:%s\n", FwVerAddr));
      break;
    case FwVerTEE:
      FwVerAddr = SmemGetAddr (SMEM_VER_TEE, &SmemSize);
      if (FwVerAddr == NULL) {
        return EFI_NOT_FOUND;
      }

      // DEBUG ((DEBUG_INFO, "[VER] PBL:%s\n", FwVerAddr));
      break;
    case FwVerPBL:
      FwVerAddr = SmemGetAddr (SMEM_VER_PBL, &SmemSize);
      if (FwVerAddr == NULL) {
        return EFI_NOT_FOUND;
      }

      // DEBUG ((DEBUG_INFO, "[VER] PBL:%s\n", FwVerAddr));
      break;
    case FwVerATF:
      FwVerAddr = SmemGetAddr (SMEM_VER_ATF, &SmemSize);
      if (FwVerAddr == NULL) {
        return EFI_NOT_FOUND;
      }

      // DEBUG ((DEBUG_INFO, "[VER] ATF:%s\n", FwVerAddr));
      break;
    case FwVerUEFI:
      *FwVerBuff = (CHAR16 *)PcdGetPtr (PcdFirmwareVersionString);
      *FwVerSize = StrLen (*FwVerBuff);
      return EFI_SUCCESS;

    case FwVerEC:
      Status = FwGetEcVersion (FwVerBuff, FwVerSize);
      if (EFI_ERROR (Status)) {
        return EFI_NOT_FOUND;
      }

      return EFI_SUCCESS;
    case FwVerBoardId:
      Status = FwVerGetBoardId (FwVerBuff, FwVerSize);
      if (EFI_ERROR (Status)) {
        return EFI_NOT_FOUND;
      }

      return EFI_SUCCESS;

    default:
      break;
  }

  if (FwVerAddr == NULL) {
    return EFI_NOT_FOUND;
  }

  if (*(UINT32 *)FwVerAddr == 0xffffffff) {
    return EFI_NOT_FOUND;
  }

  *FwVerSize = AsciiStrLen ((CHAR8 *)FwVerAddr);
  if (*FwVerSize == 0) {
    return EFI_NOT_FOUND;
  }

  *FwVerBuff = AllocateZeroPool ((*FwVerSize + 1) * 2);

  AsciiToUnicode ((CHAR8 *)FwVerAddr, (CHAR16 *)*FwVerBuff);
  // CopyMem (*FwVerBuff, p, *FwVerSize);
  return EFI_SUCCESS;
}

GLOBAL_REMOVE_IF_UNREFERENCED CIX_FW_VERSION_PROTOCOL  CixFwVerProtocol = {
  {
    STR (FW_HEADER_SIG),
    FW_PROTOCOL_VERSION,
    STR (UEFI_FW_VERSION),
  },
  GetFwVersion,
};

STATIC
VOID
EFIAPI
SetUefiVersion (

  )
{
  void    *FwVerAddrUefi;
  UINT32  SmemSize;
  CHAR16  *FwVerUefiBuff;
  UINTN   FwVerUefiSize;

  FwVerUefiBuff = (CHAR16 *)PcdGetPtr (PcdFirmwareVersionString);
  FwVerUefiSize = StrLen (FwVerUefiBuff) * 2;

  FwVerAddrUefi = SmemGetAddr (SMEM_VER_UEFI, &SmemSize);
  ZeroMem (FwVerAddrUefi, SmemSize);

  UnicodeToAscii (FwVerUefiBuff, StrLen (FwVerUefiBuff), (CHAR8 *)FwVerAddrUefi);
  // CopyMem (FwVerAddrUefi, FwVerUefiBuff, FwVerUefiSize);

  return;
}

STATIC
VOID
EFIAPI
SetBoardInfo (
  )
{
  EFI_STATUS            Status;
  void                  *FwVerAddrBoard;
  UINT32                SmemSize;
  EC_PLATFORM_PROTOCOL  *Ec;
  EC_RESPONSE           EcResponse;
  UINT32                EcVersionSize;
  void                  *FwVerAddrEc,*VerAddrPd;

  DEBUG ((DEBUG_INFO, "%a Entry\n", __FUNCTION__));

  Status = gBS->LocateProtocol (&gCixEcPlatformProtocolGuid, NULL, (VOID **)&Ec);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "EC platform not ready.\n"));
    // Write Default Version info
    FwVerAddrBoard = SmemGetAddr (SMEM_INFO_BOARD, &SmemSize);
    MmioWrite32 ((UINTN)FwVerAddrBoard, 0xFFFFFFFF);
    FwVerAddrEc = SmemGetAddr (SMEM_VER_EC, &SmemSize);
    ZeroMem (FwVerAddrEc, SmemSize);
    CopyMem (FwVerAddrEc, "NULL", sizeof ("NULL"));
    return;
  }

  Status = Ec->Transfer (Ec, EC_COMMAND_GET_BOARD_ID, NULL, &EcResponse);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "EC platform not ready.\n"));
    return;
  }

  FwVerAddrBoard = SmemGetAddr (SMEM_INFO_BOARD, &SmemSize);
  MmioWrite32 ((UINTN)FwVerAddrBoard, EcResponse.BoardId.Value);

  Status = Ec->Transfer (Ec, EC_COMMAND_GET_EC_VERSION, NULL, &EcResponse);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "EC platform not ready.\n"));
    return;
  }

  EcVersionSize = AsciiStrLen ((CHAR8 *)EcResponse.Version.String);
  if (EcVersionSize == 0) {
    return;
  }

  FwVerAddrEc = SmemGetAddr (SMEM_VER_EC, &SmemSize);
  ZeroMem (FwVerAddrEc, SmemSize);

  // UnicodeToAscii(FwVerUefiBuff,StrLen(FwVerUefiBuff),(CHAR8 *)FwVerAddrUefi);
  CopyMem (FwVerAddrEc, (CHAR8 *)EcResponse.Version.String, EcVersionSize);

  Status = Ec->Transfer (Ec, EC_COMMAND_GET_PD_VERSION, NULL, &EcResponse);
  if (EFI_ERROR (Status)) {
    DebugPrint (DEBUG_INFO, "EC platform not ready.\n");
    return;
  }
  VerAddrPd = SmemGetAddr (SMEM_VER_PD, &SmemSize);
  if(VerAddrPd == NULL){
    return;
  }
  //DebugPrint (DEBUG_INFO, "PdVer:0x%x 0x%x.\nPdAddr:0x%x\n",EcResponse.PdVer.Pd1Ver, EcResponse.PdVer.Pd2Ver,(UINTN)VerAddrPd);
  ZeroMem (VerAddrPd, SmemSize);
  MmioWrite32 ((UINTN)VerAddrPd, EcResponse.PdVer.Pd2Ver | EcResponse.PdVer.Pd1Ver);

  return;
}

VOID
EFIAPI
UpdateBoardInfo (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  SetBoardInfo ();
}

EFI_STATUS
EFIAPI
RecordUefiMemory (
  )
{
  EFI_STATUS             Status;
  UINTN                  EfiMemoryMapSize;
  EFI_MEMORY_DESCRIPTOR  *EfiMemoryMap;
  EFI_MEMORY_DESCRIPTOR  *EfiMemoryMapEnd;
  EFI_MEMORY_DESCRIPTOR  *EfiEntry;
  EFI_MEMORY_DESCRIPTOR  EfiEntryBottom = { EfiLoaderCode, 0 };
  EFI_MEMORY_DESCRIPTOR  EfiEntryTop    = { 0 };
  UINTN                  EfiMapKey;
  UINTN                  EfiDescriptorSize;
  UINT32                 EfiDescriptorVersion;

  UINT64  BootServicesDataPage        = 0;
  UINT64  BootServicesCodePage        = 0;
  UINT64  ACPIReclaimMemoryPage       = 0;
  UINT64  ACPIMemoryNVSPage           = 0;
  UINT64  ReservedMemoryTypePage      = 0;
  UINT64  RuntimeServicesCodePage     = 0;
  UINT64  RuntimeServicesDataPage     = 0;
  UINT64  LoaderCodePage              = 0;
  UINT64  LoaderDataPage              = 0;
  UINT64  MaxMemoryTypePage           = 0;
  UINT64  ConventionalMemoryPage      = 0;
  UINT64  UnusableMemoryPage          = 0;
  UINT64  MemoryMappedIOPage          = 0;
  UINT64  MemoryMappedIOPortSpacePage = 0;
  UINT64  PalCodePage                 = 0;
  UINT64  PersistentMemoryPage        = 0;

  void                  *SmemUefiBottom;
  UINT32                SmemSize;
  MEMORY_RECORD_STRUCT  *pMemRecStruct = NULL;

  EfiMemoryMapSize = 0;
  EfiMemoryMap     = NULL;
  Status           = gBS->GetMemoryMap (
                            &EfiMemoryMapSize,
                            EfiMemoryMap,
                            &EfiMapKey,
                            &EfiDescriptorSize,
                            &EfiDescriptorVersion
                            );
  ASSERT (Status == EFI_BUFFER_TOO_SMALL);
  do {
    EfiMemoryMap = AllocatePool (EfiMemoryMapSize);
    if (EfiMemoryMap == NULL) {
      DEBUG ((EFI_D_ERROR, "ERROR!! Null Pointer returned by AllocatePool ()\n"));
      ASSERT_EFI_ERROR (EFI_OUT_OF_RESOURCES);
      return Status;
    }

    Status = gBS->GetMemoryMap (
                    &EfiMemoryMapSize,
                    EfiMemoryMap,
                    &EfiMapKey,
                    &EfiDescriptorSize,
                    &EfiDescriptorVersion
                    );
    if (EFI_ERROR (Status)) {
      FreePool (EfiMemoryMap);
    }
  } while (Status == EFI_BUFFER_TOO_SMALL);

  DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "[] EfiMemoryMapSize=0x%x EfiDescriptorSize=0x%x EfiMemoryMap=0x%x \n", EfiMemoryMapSize, EfiDescriptorSize, (UINTN)EfiMemoryMap));

  EfiMemoryMapEnd = (EFI_MEMORY_DESCRIPTOR *)((UINT8 *)EfiMemoryMap + EfiMemoryMapSize);
  EfiEntry        = EfiMemoryMap;

  DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "===========================%S============================== Start\n", L" MemMap"));

  while (EfiEntry < EfiMemoryMapEnd) {
    if (EfiEntry->Type == EfiReservedMemoryType) {
      DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "[] EfiReservedMemoryType  %3d %16lx pn %16lx \n", EfiEntry->Type, EfiEntry->PhysicalStart, EfiEntry->NumberOfPages));
      ReservedMemoryTypePage = ReservedMemoryTypePage + EfiEntry->NumberOfPages;
    } else if (EfiEntry->Type == EfiLoaderCode) {
      DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "[] EfiLoaderCode  %3d %16lx pn %16lx \n", EfiEntry->Type, EfiEntry->PhysicalStart, EfiEntry->NumberOfPages));
      LoaderCodePage = LoaderCodePage + EfiEntry->NumberOfPages;
    } else if (EfiEntry->Type == EfiLoaderData) {
      DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "[] EfiLoaderData  %3d %16lx pn %16lx \n", EfiEntry->Type, EfiEntry->PhysicalStart, EfiEntry->NumberOfPages));
      LoaderDataPage = LoaderDataPage + EfiEntry->NumberOfPages;
    } else if (EfiEntry->Type == EfiBootServicesCode) {
      DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "[] EfiBootServicesCode  %3d %16lx pn %16lx \n", EfiEntry->Type, EfiEntry->PhysicalStart, EfiEntry->NumberOfPages));
      BootServicesCodePage = BootServicesCodePage + EfiEntry->NumberOfPages;
    } else if (EfiEntry->Type == EfiBootServicesData) {
      DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "[] EfiBootServicesData  %3d %16lx pn %16lx \n", EfiEntry->Type, EfiEntry->PhysicalStart, EfiEntry->NumberOfPages));
      BootServicesDataPage = BootServicesDataPage + EfiEntry->NumberOfPages;
    } else if (EfiEntry->Type == EfiRuntimeServicesCode) {
      DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "[] EfiRuntimeServicesCode  %3d %16lx pn %16lx \n", EfiEntry->Type, EfiEntry->PhysicalStart, EfiEntry->NumberOfPages));
      RuntimeServicesCodePage = RuntimeServicesCodePage + EfiEntry->NumberOfPages;
    } else if (EfiEntry->Type == EfiRuntimeServicesData) {
      DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "[] EfiRuntimeServicesData  %3d %16lx pn %16lx \n", EfiEntry->Type, EfiEntry->PhysicalStart, EfiEntry->NumberOfPages));
      RuntimeServicesDataPage = RuntimeServicesDataPage + EfiEntry->NumberOfPages;
    } else if (EfiEntry->Type == EfiConventionalMemory) {
      DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "[] EfiConventionalMemory  %3d %16lx pn %16lx \n", EfiEntry->Type, EfiEntry->PhysicalStart, EfiEntry->NumberOfPages));
      ConventionalMemoryPage = ConventionalMemoryPage + EfiEntry->NumberOfPages;
    } else if (EfiEntry->Type == EfiUnusableMemory) {
      DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "[] EfiUnusableMemory  %3d %16lx pn %16lx \n", EfiEntry->Type, EfiEntry->PhysicalStart, EfiEntry->NumberOfPages));
      UnusableMemoryPage = UnusableMemoryPage + EfiEntry->NumberOfPages;
    } else if (EfiEntry->Type == EfiACPIReclaimMemory) {
      DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "[] EfiACPIReclaimMemory  %3d %16lx pn %16lx \n", EfiEntry->Type, EfiEntry->PhysicalStart, EfiEntry->NumberOfPages));
      ACPIReclaimMemoryPage = ACPIReclaimMemoryPage + EfiEntry->NumberOfPages;
    } else if (EfiEntry->Type == EfiACPIMemoryNVS) {
      DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "[] EfiACPIMemoryNVS  %3d %16lx pn %16lx \n", EfiEntry->Type, EfiEntry->PhysicalStart, EfiEntry->NumberOfPages));
      ACPIMemoryNVSPage = ACPIMemoryNVSPage + EfiEntry->NumberOfPages;
    } else if (EfiEntry->Type == EfiMemoryMappedIO) {
      DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "[] EfiMemoryMappedIO  %3d %16lx pn %16lx \n", EfiEntry->Type, EfiEntry->PhysicalStart, EfiEntry->NumberOfPages));
      MemoryMappedIOPage = MemoryMappedIOPage + EfiEntry->NumberOfPages;
    } else if (EfiEntry->Type == EfiMemoryMappedIOPortSpace) {
      DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "[] EfiMemoryMappedIOPortSpace  %3d %16lx pn %16lx \n", EfiEntry->Type, EfiEntry->PhysicalStart, EfiEntry->NumberOfPages));
      MemoryMappedIOPortSpacePage = MemoryMappedIOPortSpacePage + EfiEntry->NumberOfPages;
    } else if (EfiEntry->Type == EfiPalCode) {
      DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "[] EfiPalCode  %3d %16lx pn %16lx \n", EfiEntry->Type, EfiEntry->PhysicalStart, EfiEntry->NumberOfPages));
      PalCodePage = PalCodePage + EfiEntry->NumberOfPages;
    } else if (EfiEntry->Type == EfiPersistentMemory) {
      DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "[] EfiPersistentMemory  %3d %16lx pn %16lx \n", EfiEntry->Type, EfiEntry->PhysicalStart, EfiEntry->NumberOfPages));
      PersistentMemoryPage = PersistentMemoryPage + EfiEntry->NumberOfPages;
    } else if (EfiEntry->Type == EfiMaxMemoryType) {
      DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "[] EfiMaxMemoryType  %3d %16lx pn %16lx \n", EfiEntry->Type, EfiEntry->PhysicalStart, EfiEntry->NumberOfPages));
      MaxMemoryTypePage = MaxMemoryTypePage + EfiEntry->NumberOfPages;
    }

    EfiEntry = NEXT_MEMORY_DESCRIPTOR (EfiEntry, EfiDescriptorSize);
  }

  EfiEntry = EfiMemoryMap;
  while (EfiEntry < EfiMemoryMapEnd) {
    if ((EfiEntry->PhysicalStart < 0x7ffffffff) && (EfiEntry->Type == EfiLoaderCode)) {
      // DEBUG((DEBUG_ERROR | DEBUG_PAGE, "[] EfiConventionalMemory  %3d %16lx pn %16lx \n", EfiEntry->Type, EfiEntry->PhysicalStart, EfiEntry->NumberOfPages));
      ConventionalMemoryPage = ConventionalMemoryPage + EfiEntry->NumberOfPages;
      if (EfiEntry->PhysicalStart > EfiEntryBottom.PhysicalStart) {
        EfiEntryBottom.NumberOfPages = EfiEntry->NumberOfPages;
        EfiEntryBottom.PhysicalStart = EfiEntry->PhysicalStart;
      }
    }

    if ((EfiEntry->PhysicalStart < 0x7ffffffff) && (EfiEntry->PhysicalStart > EfiEntryTop.PhysicalStart)) {
      EfiEntryTop.Type          = EfiEntry->Type;
      EfiEntryTop.PhysicalStart = EfiEntry->PhysicalStart;
      EfiEntryTop.NumberOfPages = EfiEntry->NumberOfPages;
    }

    EfiEntry = NEXT_MEMORY_DESCRIPTOR (EfiEntry, EfiDescriptorSize);
  }

  SmemUefiBottom                    = SmemGetAddr (SMEM_ADDR_UEFI_MEM_BOTTOM, &SmemSize);
  pMemRecStruct                     = (MEMORY_RECORD_STRUCT *)SmemUefiBottom;
  pMemRecStruct->Signature          = MEMORY_RECORD_STRUCT_SIGNATURE;
  pMemRecStruct->EntryCount         = 2;
  pMemRecStruct->Entry[0].StartAddr = PcdGet64 (PcdFdBaseAddress);
  pMemRecStruct->Entry[0].EndAddr   = PcdGet64 (PcdFdBaseAddress) + PcdGet32 (PcdFdSize) - 1;
  pMemRecStruct->Entry[1].StartAddr = EfiEntryBottom.PhysicalStart + EfiEntryBottom.NumberOfPages*4096 - PcdGet32 (PcdMemoryTypeEfiLoaderCode)*4096;
  pMemRecStruct->Entry[1].EndAddr   = EfiEntryTop.PhysicalStart + EfiEntryTop.NumberOfPages*4096 - 1;

  DEBUG ((DEBUG_ERROR | DEBUG_PAGE, "===========================%S============================== End\n", L" MemMap"));
  WriteBackDataCacheRange ((VOID *)(UINTN)SmemUefiBottom, (UINTN)SmemSize);

  //   UINT32 *pAddrTemp = (UINT32 *)(UINTN)SmemUefiBottom;
  //   DEBUG((DEBUG_INFO,"0x83E_0200: %08x %08x %08x %08x\n", *pAddrTemp, *(pAddrTemp+1), *(pAddrTemp+2), *(pAddrTemp+3)));
  //   DEBUG((DEBUG_INFO,"0x83E_0210: %08x %08x %08x %08x\n", *(pAddrTemp+4), *(pAddrTemp+5), *(pAddrTemp+6), *(pAddrTemp+7)));
  //   DEBUG((DEBUG_INFO,"0x83E_0220: %08x %08x %08x %08x\n", *(pAddrTemp+8), *(pAddrTemp+9), *(pAddrTemp+10), *(pAddrTemp+11)));
  // }

  return Status;
}

VOID
EFIAPI
RecordUefiMemoryNotify (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  RecordUefiMemory ();
}

static EFI_EVENT  mExitBootServicesEvent;
// static EFI_EVENT  ReadyToBootEvent;
VOID
EFIAPI
RecordUefiMemoryWrapper (
  )
{
  EFI_STATUS  Status;

  // Status = gBS->CreateEventEx (
  //                              EVT_NOTIFY_SIGNAL,
  //                              TPL_CALLBACK,
  //                              ReserveUefiMemoryNotify,
  //                              NULL,
  //                              &gEfiEventReadyToBootGuid,
  //                              &ReadyToBootEvent
  //                              );
  // ASSERT_EFI_ERROR (Status);
  //
  // Register the notify function to update FPDT on ExitBootServices Event.
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  RecordUefiMemoryNotify,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &mExitBootServicesEvent
                  );
  ASSERT_EFI_ERROR (Status);
}

EFI_STATUS
EFIAPI
FwVersionDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   BoardInfoUpdateEvent;
  VOID        *Registration;

  DEBUG ((DEBUG_INFO, "%a start.\n", __FUNCTION__));

  SetUefiVersion ();
  SetBoardInfo ();

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gCixFwVersionProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &CixFwVerProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG (
      (DEBUG_ERROR,
       "%a: failed to install fw version protocol (Status == %r)\n",
       __FUNCTION__, Status)
      );
  }

  BoardInfoUpdateEvent = EfiCreateProtocolNotifyEvent (
                           &gCixEcPlatformProtocolGuid,
                           TPL_CALLBACK,
                           UpdateBoardInfo,
                           NULL,
                           &Registration
                           );
  if (BoardInfoUpdateEvent == NULL) {
    DEBUG ((DEBUG_ERROR, "%a UpdateBoardInfo Event Create Failed.  \n", __FUNCTION__));
  }

  RecordUefiMemoryWrapper ();

  return Status;
}
