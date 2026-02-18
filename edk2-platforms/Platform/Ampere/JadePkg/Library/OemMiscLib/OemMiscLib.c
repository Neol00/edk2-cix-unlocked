/** @file
*  OemMiscLib.c
*
*  Copyright (c) 2021, Ampere Computing LLC. All rights reserved.
*  Copyright (c) 2021, NUVIA Inc. All rights reserved.
*  Copyright (c) 2018, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2018, Linaro Limited. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#include <Uefi.h>
#include <IndustryStandard/ArmCache.h>
#include <Library/AmpereCpuLib.h>
#include <Library/ArmLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HiiLib.h>
#include <Library/OemMiscLib.h>

#define MHZ_SCALE_FACTOR    1000000

UINT32
GetCacheConfig (
  IN UINT32  CacheLevel,
  IN BOOLEAN DataCache,
  IN BOOLEAN UnifiedCache
  )
{
  CSSELR_DATA Csselr;
  UINT64      Ccsidr;
  BOOLEAN     SupportWB;
  BOOLEAN     SupportWT;

  Csselr.Data = 0;
  Csselr.Bits.Level = CacheLevel - 1;
  Csselr.Bits.InD = (!DataCache && !UnifiedCache);

  Ccsidr = ReadCCSIDR (Csselr.Data);
  SupportWT = (Ccsidr & (1 << 31)) ? TRUE : FALSE;
  SupportWB = (Ccsidr & (1 << 30)) ? TRUE : FALSE;

  if (SupportWT && SupportWB) {
    return 2; // Varies with Memory Address
  }

  if (SupportWT) {
    return 0; // Write Through
  }

  if (SupportWB) {
    return 1; // Write Back
  }

  return 3; // Unknown
}

/** Gets the CPU frequency of the specified processor.

  @param ProcessorIndex Index of the processor to get the frequency for.

  @return               CPU frequency in Hz
**/
UINTN
EFIAPI
OemGetCpuFreq (
  IN UINT8 ProcessorIndex
  )
{
  return CpuGetCurrentFreq (ProcessorIndex);
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
  IN UINTN ProcessorIndex,
  IN OUT PROCESSOR_STATUS_DATA *ProcessorStatus,
  IN OUT PROCESSOR_CHARACTERISTIC_FLAGS *ProcessorCharacteristics,
  IN OUT OEM_MISC_PROCESSOR_DATA *MiscProcessorData
  )
{
  UINT16 ProcessorCount;

  ProcessorCount = GetNumberOfActiveSockets ();

  if (ProcessorIndex < ProcessorCount) {
    ProcessorStatus->Bits.CpuStatus       = 1; // CPU enabled
    ProcessorStatus->Bits.Reserved1       = 0;
    ProcessorStatus->Bits.SocketPopulated = 1;
    ProcessorStatus->Bits.Reserved2       = 0;
  } else {
    ProcessorStatus->Bits.CpuStatus       = 0; // CPU disabled
    ProcessorStatus->Bits.Reserved1       = 0;
    ProcessorStatus->Bits.SocketPopulated = 0;
    ProcessorStatus->Bits.Reserved2       = 0;
  }

  ProcessorCharacteristics->ProcessorReserved1      = 0;
  ProcessorCharacteristics->ProcessorUnknown        = 0;
  ProcessorCharacteristics->Processor64BitCapable   = 1;
  ProcessorCharacteristics->ProcessorMultiCore      = 1;
  ProcessorCharacteristics->ProcessorHardwareThread = 0;
  ProcessorCharacteristics->ProcessorExecuteProtection      = 1;
  ProcessorCharacteristics->ProcessorEnhancedVirtualization = 1;
  ProcessorCharacteristics->ProcessorPowerPerformanceCtrl   = 1;
  ProcessorCharacteristics->Processor128BitCapable = 0;
  ProcessorCharacteristics->ProcessorReserved2  = 0;

  MiscProcessorData->Voltage      = CpuGetVoltage (ProcessorIndex);
  MiscProcessorData->CurrentSpeed = (UINT16)(CpuGetCurrentFreq (ProcessorIndex) / MHZ_SCALE_FACTOR);
  MiscProcessorData->MaxSpeed     = (UINT16)(CpuGetMaxFreq (ProcessorIndex) / MHZ_SCALE_FACTOR);
  MiscProcessorData->CoreCount    = GetMaximumNumberOfCores ();
  MiscProcessorData->ThreadCount  = GetMaximumNumberOfCores ();
  MiscProcessorData->CoresEnabled = GetNumberOfActiveCoresPerSocket (ProcessorIndex);

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
  IN UINT8   ProcessorIndex,
  IN UINT8   CacheLevel,
  IN BOOLEAN DataCache,
  IN BOOLEAN UnifiedCache,
  IN OUT SMBIOS_TABLE_TYPE7 *SmbiosCacheTable
  )
{
  SmbiosCacheTable->CacheConfiguration = CacheLevel - 1;
  SmbiosCacheTable->CacheConfiguration |= (1 << 7); // Enable
  SmbiosCacheTable->CacheConfiguration |= (GetCacheConfig (CacheLevel, DataCache, UnifiedCache) << 8);

  SmbiosCacheTable->SupportedSRAMType.Unknown = 0;
  SmbiosCacheTable->SupportedSRAMType.Synchronous = 1;
  SmbiosCacheTable->CurrentSRAMType.Unknown = 0;
  SmbiosCacheTable->CurrentSRAMType.Synchronous = 1;

  if (CacheLevel == 1 && !DataCache && !UnifiedCache) {
    SmbiosCacheTable->ErrorCorrectionType = CacheErrorParity;
  } else {
    SmbiosCacheTable->ErrorCorrectionType = CacheErrorSingleBit;
  }

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
  return GetNumberOfSupportedSockets ();
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
  return MiscChassisTypeRackMountChassis;
}

/** Returns whether the specified processor is present or not.

  @param ProcessIndex The processor index to check.

  @return TRUE is the processor is present, FALSE otherwise.
**/
BOOLEAN
EFIAPI
OemIsProcessorPresent (
  IN UINTN ProcessorIndex
  )
{
  //
  // Platform only supports 2 sockets: Master and Slave.
  // The master socket is always online.
  //
  if (ProcessorIndex == 0) {
    return TRUE;
  } else if (ProcessorIndex == 1) {
    return IsSlaveSocketAvailable ();
  }

  return FALSE;
}

/** Updates the HII string for the specified field.

  @param HiiHandle     The HII handle.
  @param TokenToUpdate The string to update.
  @param Field         The field to get information about.
**/
VOID
EFIAPI
OemUpdateSmbiosInfo (
  IN EFI_HII_HANDLE HiiHandle,
  IN EFI_STRING_ID TokenToUpdate,
  IN OEM_MISC_SMBIOS_HII_STRING_FIELD Field
  )
{
  return;
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
  return 2;
}
