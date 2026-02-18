/** @file
*  OemMiscLib.c
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
*
*  Copyright (c) 2024, Cix Technology Group Co., Ltd. All Rights Reserved.
*  Copyright (c) 2022, Ampere Computing LLC. All rights reserved.
*  Copyright (c) 2021, NUVIA Inc. All rights reserved.
*  Copyright (c) 2018, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2018, Linaro Limited. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#include <Uefi.h>
#include <Guid/ZeroGuid.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HiiLib.h>
#include <Library/OemMiscLib.h>
#include <Library/PcdLib.h>
#include <Library/CpuInfoLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <PlatformSetupVar.h>
#include <Protocol/SocInfoProtocol.h>

#define LITTLE_CORE_L1_CACHE_SIZE  32
#define CPU_MAX_SPEED              0
#define CPU_CURRENT_SPEED          0

#define CACHE_SOCKETED_SHIFT        3
#define CACHE_LOCATION_SHIFT        5
#define CACHE_ENABLED_SHIFT         7
#define CACHE_OPERATION_MODE_SHIFT  8

typedef enum {
  CacheModeWriteThrough = 0,  ///< Cache is write-through
  CacheModeWriteBack,         ///< Cache is write-back
  CacheModeVariesWithAddress, ///< Cache mode varies by address
  CacheModeUnknown,           ///< Cache mode is unknown
  CacheModeMax
} CACHE_OPERATION_MODE;

typedef enum {
  CacheLocationInternal = 0, ///< Cache is internal to the processor
  CacheLocationExternal,     ///< Cache is external to the processor
  CacheLocationReserved,     ///< Reserved
  CacheLocationUnknown,      ///< Cache location is unknown
  CacheLocationMax
} CACHE_LOCATION;

/** Gets the CPU frequency of the specified processor.

  @param ProcessorIndex Index of the processor to get the frequency for.

  @return               CPU frequency in Hz
**/
UINTN
EFIAPI
OemGetCpuFreq (
  IN UINT8  ProcessorIndex
  )
{
  return GetCurrentCpuFreq ();
}

/** Gets information about the specified processor and stores it in
    the structures provided.

  @param ProcessorIndex  Index of the processor to get the information for.
  @param ProcessorStatus Processor status.
  @param ProcessorCharacteristics Processor characteritics.
  @param MiscProcessorData        Miscellaneous processor information.

  @return  TRUE on success, FALSE on failure.
**/
BOOLEAN
EFIAPI
OemGetProcessorInformation (
  IN UINTN                               ProcessorIndex,
  IN OUT PROCESSOR_STATUS_DATA           *ProcessorStatus,
  IN OUT PROCESSOR_CHARACTERISTIC_FLAGS  *ProcessorCharacteristics,
  IN OUT OEM_MISC_PROCESSOR_DATA         *MiscProcessorData
  )
{
  EFI_STATUS           Status;
  UINT8                CpuCoreNum, EnabledCoreNum;
  UINT32               SetupCpuIndex;
  UINTN                VarSize;
  PLATFORM_SETUP_DATA  PlatformSetupVar;

  ProcessorStatus->Bits.CpuStatus       = 1;   // CPU enabled
  ProcessorStatus->Bits.Reserved1       = 0;
  ProcessorStatus->Bits.SocketPopulated = 1;
  ProcessorStatus->Bits.Reserved2       = 0;

  ProcessorCharacteristics->ProcessorReserved1              = 0;
  ProcessorCharacteristics->ProcessorUnknown                = 0;
  ProcessorCharacteristics->Processor64BitCapable           = 1;
  ProcessorCharacteristics->ProcessorMultiCore              = 1;
  ProcessorCharacteristics->ProcessorHardwareThread         = 0;
  ProcessorCharacteristics->ProcessorExecuteProtection      = 1;
  ProcessorCharacteristics->ProcessorEnhancedVirtualization = 0;
  ProcessorCharacteristics->ProcessorPowerPerformanceCtrl   = 1;
  ProcessorCharacteristics->Processor128BitCapable          = 0;
  ProcessorCharacteristics->ProcessorArm64SocId             = 0;
  ProcessorCharacteristics->ProcessorReserved2              = 0;
  GetValidCpuCoreNum (&CpuCoreNum);

  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VAR,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &PlatformSetupVar
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Get platform setup variable Fail! %r\n", Status));
    return FALSE;
  }

  EnabledCoreNum = 0;
  for (SetupCpuIndex = 0; SetupCpuIndex < CpuCoreNum; SetupCpuIndex++) {
    if (PlatformSetupVar.CpuCoreEnable[SetupCpuIndex] == 1) {
      EnabledCoreNum++;
    }
  }

  MiscProcessorData->Voltage      = 0;
  MiscProcessorData->CurrentSpeed = (UINT16)(ROUND_DIVISION (GetCurrentCpuFreq (), 1000000));
  MiscProcessorData->MaxSpeed     = (UINT16)(ROUND_DIVISION (GetMaxCpuFreq (), 1000000));
  MiscProcessorData->CoreCount    = CpuCoreNum;
  MiscProcessorData->ThreadCount  = CpuCoreNum;
  MiscProcessorData->CoresEnabled = EnabledCoreNum;
  return TRUE;
}

/** Gets information about the cache at the specified cache level.

  @param ProcessorIndex The processor to get information for.
  @param CacheLevel The cache level to get information for.
  @param DataCache  Whether the cache is a data cache.
  @param UnifiedCache Whether the cache is a unified cache.
  @param SmbiosCacheTable The SMBIOS Type7 cache information structure.

  @return TRUE on success, FALSE on failure.
**/
BOOLEAN
EFIAPI
OemGetCacheInformation (
  IN UINT8                   ProcessorIndex,
  IN UINT8                   CacheLevel,
  IN BOOLEAN                 DataCache,
  IN BOOLEAN                 UnifiedCache,
  IN OUT SMBIOS_TABLE_TYPE7  *SmbiosCacheTable
  )
{
  EFI_STATUS           Status;
  UINT8                CpuBootCoreId;
  UINT8                VBCoreNum = 0;
  UINT8                VLCoreNum = 0;
  UINT8                BCoreNum  = 0;
  UINT8                LCoreNum  = 0;
  UINT8                CpuCoreNum;
  UINT16               CacheSize, MaximumCacheSize;
  UINT32               CpuCoreMask;
  UINT32               MaxCpuCoreNum;
  UINT32               i;
  UINT32               SetupCpuIndex = 0;
  UINTN                VarSize;
  PLATFORM_SETUP_DATA  PlatformSetupVar;

  GetCpuBootCoreId (&CpuBootCoreId);
  if (CpuBootCoreId > 3) {
    VBCoreNum++;
    BCoreNum++;
    Status = gRT->GetVariable (
                    PLATFORM_SETUP_VAR,
                    &gPlatformSetupVariableGuid,
                    NULL,
                    &VarSize,
                    &PlatformSetupVar
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Get platform setup variable Fail! %r\n", Status));
      return FALSE;
    }

    GetValidCpuCoreNum (&CpuCoreNum);
    GetCpuCoreMask (&CpuCoreMask, &MaxCpuCoreNum);
    CpuCoreMask = CpuCoreMask|(1<<CpuBootCoreId);
    for (i = 0; i < MaxCpuCoreNum; i++) {
      if (((CpuCoreMask>>i)&BIT0) == 1) {
        continue;
      }

      if (i < 4) {
        LCoreNum++;
      } else {
        BCoreNum++;
      }

      SetupCpuIndex++;
      if (PlatformSetupVar.CpuCoreEnable[SetupCpuIndex] == 0) {
        continue;
      }

      if (i < 4) {
        VLCoreNum++;
      } else {
        VBCoreNum++;
      }
    }

    if (CacheLevel == 1) {
      MaximumCacheSize = BCoreNum*SmbiosCacheTable->MaximumCacheSize+LCoreNum*LITTLE_CORE_L1_CACHE_SIZE;
      CacheSize        = VBCoreNum*SmbiosCacheTable->MaximumCacheSize+VLCoreNum*LITTLE_CORE_L1_CACHE_SIZE;
    } else if (CacheLevel == 2) {
      MaximumCacheSize = BCoreNum*SmbiosCacheTable->MaximumCacheSize;
      CacheSize        = VBCoreNum*SmbiosCacheTable->MaximumCacheSize;
    } else {
      MaximumCacheSize = SmbiosCacheTable->MaximumCacheSize;
      CacheSize        = SmbiosCacheTable->MaximumCacheSize;
    }

    SmbiosCacheTable->MaximumCacheSize  = MaximumCacheSize;
    SmbiosCacheTable->InstalledSize     = CacheSize;
    SmbiosCacheTable->MaximumCacheSize2 = MaximumCacheSize;
    SmbiosCacheTable->InstalledSize2    = CacheSize;
  }

  SmbiosCacheTable->CacheConfiguration = (CacheModeVariesWithAddress << CACHE_OPERATION_MODE_SHIFT) |
                                         (1 << CACHE_ENABLED_SHIFT) |
                                         (CacheLocationInternal << CACHE_LOCATION_SHIFT) |
                                         (0 << CACHE_SOCKETED_SHIFT) |
                                         (CacheLevel - 1);
  return TRUE;
}

/** Gets the maximum number of processors supported by the platform.

  @return The maximum number of processors.
**/
UINT8
EFIAPI
OemGetMaxProcessors (
  VOID
  )
{
  return 1;
}

/** Gets the type of chassis for the system.

  @retval The type of the chassis.
**/
MISC_CHASSIS_TYPE
EFIAPI
OemGetChassisType (
  VOID
  )
{
  ASSERT (FALSE);
  return MiscChassisTypeUnknown;
}

/** Returns whether the specified processor is present or not.

  @param ProcessIndex The processor index to check.

  @return TRUE is the processor is present, FALSE otherwise.
**/
BOOLEAN
EFIAPI
OemIsProcessorPresent (
  IN UINTN  ProcessorIndex
  )
{
  return TRUE;
}

EFI_STATUS
GetSerialNumStr (
  IN OUT CHAR16  **StrSerialNum
  )
{
  EFI_STATUS             Status;
  CIX_SOC_INFO_PROTOCOL  *pSocInfoProtocol;
  UINT32                 *pSocInfo;
  UINT32                 SocInfoSize;
  UINTN                  BufferSize;

  Status = gBS->LocateProtocol (
                  &gCixSocInfoProtocolGuid,
                  NULL,
                  (VOID **)&pSocInfoProtocol
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: soc info protocol not found\n", __FUNCTION__));
    return EFI_NOT_FOUND;
  }

  Status = pSocInfoProtocol->GetSocInfo (SerialNum, &pSocInfo, &SocInfoSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: GetSerialNum failed\n", __FUNCTION__));
    return EFI_NOT_FOUND;
  }

  BufferSize    = (SocInfoSize*2+1)*sizeof (CHAR16);
  *StrSerialNum = AllocateZeroPool (BufferSize);
  UnicodeSPrint (*StrSerialNum, BufferSize, L"%08x%08x", pSocInfo[0], pSocInfo[1]);
  return EFI_SUCCESS;
}

/** Updates the HII string for the specified field.

  @param HiiHandle     The HII handle.
  @param TokenToUpdate The string to update.
  @param Field         The field to get information about.
**/
VOID
EFIAPI
OemUpdateSmbiosInfo (
  IN EFI_HII_HANDLE                    HiiHandle,
  IN EFI_STRING_ID                     TokenToUpdate,
  IN OEM_MISC_SMBIOS_HII_STRING_FIELD  Field
  )
{
  EFI_STATUS  Status;
  CHAR16      *String;

  switch (Field) {
    case ProcessorSerialNumType04:
      Status = GetSerialNumStr (&String);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Get processor serial number Fail! %r\n", Status));
        return;
      }

      break;
    default:
      String = NULL;
      break;
  }

  if (String != NULL) {
    HiiSetString (HiiHandle, TokenToUpdate, String, NULL);
  }
}

/** Fetches the Type 32 boot information status.

  @return Boot status.
**/
MISC_BOOT_INFORMATION_STATUS_DATA_TYPE
EFIAPI
OemGetBootStatus (
  VOID
  )
{
  ASSERT (FALSE);
  return BootInformationStatusNoError;
}

/** Fetches the chassis status when it was last booted.

 @return Chassis status.
**/
MISC_CHASSIS_STATE
EFIAPI
OemGetChassisBootupState (
  VOID
  )
{
  ASSERT (FALSE);
  return ChassisStateSafe;
}

/** Fetches the chassis power supply/supplies status when last booted.

 @return Chassis power supply/supplies status.
**/
MISC_CHASSIS_STATE
EFIAPI
OemGetChassisPowerSupplyState (
  VOID
  )
{
  ASSERT (FALSE);
  return ChassisStateSafe;
}

/** Fetches the chassis thermal status when last booted.

 @return Chassis thermal status.
**/
MISC_CHASSIS_STATE
EFIAPI
OemGetChassisThermalState (
  VOID
  )
{
  ASSERT (FALSE);
  return ChassisStateSafe;
}

/** Fetches the chassis security status when last booted.

 @return Chassis security status.
**/
MISC_CHASSIS_SECURITY_STATE
EFIAPI
OemGetChassisSecurityStatus (
  VOID
  )
{
  ASSERT (FALSE);
  return ChassisSecurityStatusNone;
}

/** Fetches the chassis height in RMUs (Rack Mount Units).

  @return The height of the chassis.
**/
UINT8
EFIAPI
OemGetChassisHeight (
  VOID
  )
{
  ASSERT (FALSE);
  return 1U;
}

/** Fetches the number of power cords.

  @return The number of power cords.
**/
UINT8
EFIAPI
OemGetChassisNumPowerCords (
  VOID
  )
{
  ASSERT (FALSE);
  return 1;
}

/**
  Fetches the system UUID.

  @param[out] SystemUuid     The pointer to the buffer to store the System UUID.

**/
VOID
EFIAPI
OemGetSystemUuid (
  OUT GUID  *SystemUuid
  )
{
  ASSERT (FALSE);
  CopyGuid (SystemUuid, &gZeroGuid);
}

/** Fetches the BIOS release.

  @return The BIOS release.
**/
UINT16
EFIAPI
OemGetBiosRelease (
  VOID
  )
{
  ASSERT (FALSE);
  return PcdGet16 (PcdSystemBiosRelease);
}

/** Fetches the embedded controller firmware release.

  @return The embedded controller firmware release.
**/
UINT16
EFIAPI
OemGetEmbeddedControllerFirmwareRelease (
  VOID
  )
{
  ASSERT (FALSE);
  return PcdGet16 (PcdEmbeddedControllerFirmwareRelease);
}
