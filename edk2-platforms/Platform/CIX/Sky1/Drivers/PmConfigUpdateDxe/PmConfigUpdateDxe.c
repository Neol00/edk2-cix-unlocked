/** @file
  PM Config Update DXE Driver.

  Reads PlatformSetupVar PM config fields and patches:
    1. The 4KB pm_config binary on SPI flash (per-OPP-entry freq/voltage, TDP)
    2. ACPI DSDT gpu-microvolt property to reflect effective GPU voltage.

  PM config is always enabled. The setup variable contains stock OPP defaults
  so that "Reset to Defaults" restores the original pm_config.
  The setup variable contains per-OPP-entry frequency (MHz) and voltage (mV)
  for all 12 DVFS domains (up to 13 entries each), plus per-rail TDP power
  caps. A value of 0 means "use stock default".

  SPI flash patching runs in the driver entry point (early DXE, before the
  boot menu) so that a cold reset — when needed — happens instantly without
  waiting for the boot timeout.  ACPI DSDT patching is deferred to a
  ReadyToBoot callback since ACPI tables are not yet available at DXE entry.

  Targets the v2.1 pm_config binary format where CRC fields are at the END
  of the structure (after Config + padding), not before.

  Copyright 2025 Radxa Computer (Shenzhen) Co., Ltd. All Rights Reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "PmConfigUpdateDxe.h"
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <IndustryStandard/AcpiAml.h>

//
// Maximum number of ACPI tables to iterate when searching
//
#define ACPI_MAX_TABLES  256

//
// Module-global: TRUE when the entry point detected custom PM is enabled.
// The ReadyToBoot callback uses this to decide whether to patch the DSDT.
//
STATIC BOOLEAN  mPmConfigEnabled = FALSE;

//
// Helper functions to access CRC fields at the end of the binary buffer.
// v2.1 layout: header(16) + config + padding + crc1(4) + crc2(4)
//
STATIC
UINT32 *
GetCrc1Ptr (
  IN UINT8  *PmBuf
  )
{
  return (UINT32 *)(PmBuf + PM_CRC1_OFFSET);
}

STATIC
UINT32 *
GetCrc2Ptr (
  IN UINT8  *PmBuf
  )
{
  return (UINT32 *)(PmBuf + PM_CRC2_OFFSET);
}

/**
  Fletcher-like double checksum algorithm matching csupm_bin_config.c.
  Operates on 4-byte aligned data.

  @param[in]  Start     Pointer to data start
  @param[in]  Length    Data length in bytes (must be 4-byte aligned)
  @param[out] Crc1      First CRC value (cka)
  @param[out] Crc2      Second CRC value (ckb)
**/
STATIC
VOID
DoubleCheckSum (
  IN  VOID    *Start,
  IN  UINT32  Length,
  OUT UINT32  *Crc1,
  OUT UINT32  *Crc2
  )
{
  UINT32  *Ptr = (UINT32 *)Start;
  UINT32  Cka  = 0;
  UINT32  Ckb  = 0;
  UINT32  I;

  for (I = 0; I * 4 < Length; I++) {
    Cka += Ptr[I];
    Ckb += Cka;
  }

  *Crc1 = Cka;
  *Crc2 = Ckb;
}

/**
  Verify the double checksum of a pm_config binary.

  The reference tool (csupm_bin_config.c) computes the checksum over
  sizeof(pm_export_config_crc_t) - 8, i.e. the entire struct EXCLUDING
  the crc1/crc2 fields at the end. The length is PM_CRC1_OFFSET bytes.

  @param[in]  PmBuf     Pointer to full pm_config buffer

  @retval TRUE   Checksum matches
  @retval FALSE  Checksum mismatch
**/
STATIC
BOOLEAN
VerifyCheckSum (
  IN  UINT8   *PmBuf
  )
{
  UINT32  SavedCrc1;
  UINT32  SavedCrc2;
  UINT32  ComputedCrc1;
  UINT32  ComputedCrc2;

  SavedCrc1 = *GetCrc1Ptr (PmBuf);
  SavedCrc2 = *GetCrc2Ptr (PmBuf);

  //
  // Checksum covers everything BEFORE the CRC fields (PM_CRC1_OFFSET bytes).
  // This matches: double_check_sum(&g_config, sizeof(g_config) - 8, ...)
  //
  DoubleCheckSum (PmBuf, PM_CRC1_OFFSET, &ComputedCrc1, &ComputedCrc2);

  return (ComputedCrc1 == SavedCrc1) && (ComputedCrc2 == SavedCrc2);
}

/**
  Patch individual OPP entries for a given domain.

  For each OPP entry (up to Size entries), if the setup variable specifies
  a non-zero frequency or voltage, that value replaces the stock value.
  Only sets Modified when a value actually changes (important for avoiding
  unnecessary cold resets).
  Frequency is stored in MHz in the setup var but kHz in the OPP table.

  @param[in,out]  DomainOpp    Pointer to domain OPP config in pm_config
  @param[in]      SetupFreq    Array of 13 frequency values (MHz), 0=stock
  @param[in]      SetupVolt    Array of 13 voltage values (mV), 0=stock

  @retval TRUE   At least one entry was modified
  @retval FALSE  No modifications made
**/
STATIC
BOOLEAN
PatchOppEntries (
  IN OUT DOMAIN_OPP_CONFIG_T  *DomainOpp,
  IN     UINT16               *SetupFreq,
  IN     UINT16               *SetupVolt
  )
{
  UINT16   I;
  UINT16   Size;
  BOOLEAN  Modified;
  UINT32   NewFreq;
  UINT32   NewVolt;
  UINT32   NewLevel;

  Modified = FALSE;
  Size = DomainOpp->Size;
  if (Size > DOMAIN_MAX_OPP_ENTRIES) {
    Size = DOMAIN_MAX_OPP_ENTRIES;
  }

  for (I = 0; I < Size; I++) {
    if (SetupFreq[I] != 0) {
      NewFreq = (UINT32)SetupFreq[I] * 1000U;
      if (DomainOpp->OppTable[I].Frequency != NewFreq) {
        DomainOpp->OppTable[I].Frequency = NewFreq;
        Modified = TRUE;
      }
      //
      // Also update Level to match frequency for CPU domains where
      // Level == frequency in MHz. For GPU domains Level represents
      // shader core count, so we leave it unchanged when it differs
      // significantly from frequency.
      //
      if (DomainOpp->OppTable[I].Level >= 400 &&
          DomainOpp->OppTable[I].Level <= 5000) {
        NewLevel = (UINT32)SetupFreq[I];
        if (DomainOpp->OppTable[I].Level != NewLevel) {
          DomainOpp->OppTable[I].Level = NewLevel;
          Modified = TRUE;
        }
      }
    }

    if (SetupVolt[I] != 0) {
      NewVolt = (UINT32)SetupVolt[I];
      if (DomainOpp->OppTable[I].Voltage != NewVolt) {
        DomainOpp->OppTable[I].Voltage = NewVolt;
        Modified = TRUE;
      }
    }
  }

  return Modified;
}

/**
  Recompute the ACPI table checksum after in-place AML modifications.

  @param[in,out]  Table  Pointer to ACPI table header
**/
STATIC
VOID
AcpiFixChecksum (
  IN OUT EFI_ACPI_SDT_HEADER  *Table
  )
{
  UINT8   *Buf;
  UINT32  Len;
  UINT8   Sum;
  UINT32  I;

  Buf = (UINT8 *)Table;
  Len = Table->Length;
  Table->Checksum = 0;
  Sum = 0;
  for (I = 0; I < Len; I++) {
    Sum = (UINT8)(Sum + Buf[I]);
  }
  Table->Checksum = (UINT8)(0 - Sum);
}

/**
  Patch the gpu-microvolt property in the DSDT to reflect the actual
  maximum GPU voltage after applying user-configured voltage changes.

  @param[in]  NewMicrovolt  New voltage value in microvolts (uV)
**/
STATIC
VOID
PatchGpuMicrovoltInDsdt (
  IN UINT32  NewMicrovolt
  )
{
  EFI_STATUS              Status;
  EFI_ACPI_SDT_PROTOCOL   *AcpiSdt;
  EFI_ACPI_SDT_HEADER     *Table;
  EFI_ACPI_TABLE_VERSION  TableVersion;
  UINTN                   TableKey;
  UINTN                   I;
  UINT8                   *Buf;
  UINT32                  Len;
  UINTN                   SearchLen;
  UINTN                   Pos;
  UINTN                   Off;
  UINT8                   LeadByte;
  STATIC CONST CHAR8      GpuMvStr[] = "gpu-microvolt";

  Status = gBS->LocateProtocol (&gEfiAcpiSdtProtocolGuid, NULL, (VOID **)&AcpiSdt);
  if (EFI_ERROR (Status)) {
    return;
  }

  SearchLen = AsciiStrLen (GpuMvStr);

  for (I = 0; I < ACPI_MAX_TABLES; I++) {
    Status = AcpiSdt->GetAcpiTable (I, &Table, &TableVersion, &TableKey);
    if (EFI_ERROR (Status)) {
      break;
    }

    if (Table->Signature != SIGNATURE_32 ('D','S','D','T')) {
      continue;
    }

    Buf = (UINT8 *)Table;
    Len = Table->Length;

    for (Pos = 0; Pos + SearchLen + 10 < Len; Pos++) {
      if (CompareMem (&Buf[Pos], GpuMvStr, SearchLen) != 0 || Buf[Pos + SearchLen] != 0x00) {
        continue;
      }

      Off = Pos + SearchLen + 1;

      if (Off >= Len || Buf[Off] != AML_PACKAGE_OP) {
        continue;
      }

      Off++;  // skip PackageOp

      if (Off >= Len) {
        continue;
      }

      LeadByte = Buf[Off];
      if ((LeadByte & 0xC0) == 0x00) {
        Off += 1;
      } else if ((LeadByte & 0xC0) == 0x40) {
        Off += 2;
      } else if ((LeadByte & 0xC0) == 0x80) {
        Off += 3;
      } else {
        Off += 4;
      }

      Off++;  // skip NumElements byte

      if (Off >= Len || Buf[Off] != AML_DWORD_PREFIX) {
        continue;
      }

      Off++;  // skip DWordPrefix

      if (Off + sizeof (UINT32) > Len) {
        continue;
      }

      CopyMem (&Buf[Off], &NewMicrovolt, sizeof (UINT32));
      AcpiFixChecksum (Table);
      DEBUG ((DEBUG_INFO, "[PmConfigUpdate] gpu-microvolt patched to %u uV\n", NewMicrovolt));
      return;
    }

    break;  // only one DSDT
  }

  DEBUG ((DEBUG_WARN, "[PmConfigUpdate] gpu-microvolt not found in DSDT\n"));
}

/**
  ReadyToBoot callback.  Patches the ACPI DSDT gpu-microvolt property
  to reflect the effective maximum GPU voltage from the (already patched)
  pm_config on SPI flash.  This runs late because ACPI tables are not
  guaranteed to be installed during early DXE.

  @param[in]  Event     Event handle
  @param[in]  Context   Not used
**/
STATIC
VOID
EFIAPI
PmConfigReadyToBootCallback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS              Status;
  CIX_FW_UPDATE_PROTOCOL  *FwUpdate;
  UINT8                   *PmBuf;
  PM_EXPORT_CONFIG_CRC_T  *PmCrc;
  DOMAIN_OPP_CONFIG_T     *GpuOpp;
  UINT32                  MaxVoltMv;
  UINT16                  I;

  gBS->CloseEvent (Event);

  if (!mPmConfigEnabled) {
    return;
  }

  //
  // Read the current pm_config from SPI flash to find GPU voltage
  //
  Status = gBS->LocateProtocol (&gCixFirmwareUpdateProtocolGuid, NULL, (VOID **)&FwUpdate);
  if (EFI_ERROR (Status)) {
    return;
  }

  PmBuf = AllocateZeroPool (PM_CONFIG_BIN_SIZE);
  if (PmBuf == NULL) {
    return;
  }

  Status = FwUpdate->FirmwareRawEntryUpdate (
                       FIRMWARE_TYPE_PM_CONF,
                       PmBuf,
                       PM_CONFIG_BIN_SIZE,
                       ENTRY_READ,
                       NULL
                       );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  PmCrc = (PM_EXPORT_CONFIG_CRC_T *)PmBuf;
  if (PmCrc->Signature != PM_CONFIG_SIGNATURE) {
    goto Done;
  }

  GpuOpp    = &PmCrc->Config.OppConfig.Opps[DVFS_ELEMENT_IDX_GPU_CORE];
  MaxVoltMv = 0;

  for (I = 0; I < GpuOpp->Size && I < DOMAIN_MAX_OPP_ENTRIES; I++) {
    if (GpuOpp->OppTable[I].Voltage > MaxVoltMv) {
      MaxVoltMv = GpuOpp->OppTable[I].Voltage;
    }
  }

  if (MaxVoltMv > 0 && MaxVoltMv <= 4294) {
    PatchGpuMicrovoltInDsdt (MaxVoltMv * 1000);
  }

Done:
  FreePool (PmBuf);
}

/**
  Check setup variables against SPI flash pm_config.  If any OPP or TDP
  values differ, patch the binary, recompute the CRC, write it back, and
  trigger a cold reset so SCP picks up the change immediately.

  Runs from the driver entry point (early DXE) so the reset happens before
  the boot menu is shown — the user never gets a chance to enter BIOS setup
  during the transient reset.

  @retval TRUE   A cold reset was triggered (caller should not continue)
  @retval FALSE  No changes, or an error occurred — continue normal boot
**/
STATIC
BOOLEAN
PatchSpiFlashPmConfig (
  IN PLATFORM_SETUP_DATA  *SetupVar
  )
{
  EFI_STATUS              Status;
  CIX_FW_UPDATE_PROTOCOL  *FwUpdate;
  UINT8                   *PmBuf;
  PM_EXPORT_CONFIG_CRC_T  *PmCrc;
  PM_EXPORT_CONFIG_T      *Cfg;
  BOOLEAN                 Changed;
  UINT32                  Crc1;
  UINT32                  Crc2;
  UINT8                   Domain;
  UINT8                   Rail;
  BOOLEAN                 AnyTdp;

  //
  // Locate firmware update protocol
  //
  Status = gBS->LocateProtocol (&gCixFirmwareUpdateProtocolGuid, NULL, (VOID **)&FwUpdate);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[PmConfigUpdate] LocateProtocol FwUpdate failed: %r\n", Status));
    return FALSE;
  }

  //
  // Allocate buffer and read current pm_config from SPI flash
  //
  PmBuf = AllocateZeroPool (PM_CONFIG_BIN_SIZE);
  if (PmBuf == NULL) {
    DEBUG ((DEBUG_ERROR, "[PmConfigUpdate] AllocateZeroPool failed\n"));
    return FALSE;
  }

  //
  // FirmwareRawEntryUpdate returns UINT16 but the original CIX drivers
  // store it in EFI_STATUS and use EFI_ERROR() to check.  For ENTRY_READ
  // the low byte (RetVal) is uninitialized in the CIX implementation,
  // so the only reliable check is EFI_ERROR() which tests the high bit.
  //
  Status = FwUpdate->FirmwareRawEntryUpdate (
                       FIRMWARE_TYPE_PM_CONF,
                       PmBuf,
                       PM_CONFIG_BIN_SIZE,
                       ENTRY_READ,
                       NULL
                       );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[PmConfigUpdate] FirmwareRawEntryUpdate READ failed: %r\n", Status));
    FreePool (PmBuf);
    return FALSE;
  }

  //
  // Validate signature (at offset 12 in v2.1: after VersionMajor, VersionMinor, Timestamp)
  //
  PmCrc = (PM_EXPORT_CONFIG_CRC_T *)PmBuf;
  if (PmCrc->Signature != PM_CONFIG_SIGNATURE) {
    DEBUG ((DEBUG_ERROR, "[PmConfigUpdate] Invalid signature: 0x%08X (expected 0x%08X)\n",
            PmCrc->Signature, PM_CONFIG_SIGNATURE));
    FreePool (PmBuf);
    return FALSE;
  }

  DEBUG ((DEBUG_INFO, "[PmConfigUpdate] pm_config v%d.%d, signature OK\n",
          PmCrc->VersionMajor, PmCrc->VersionMinor));

  //
  // Verify CRC (v2.1: crc1/crc2 are at the END of the buffer)
  //
  if (!VerifyCheckSum (PmBuf)) {
    DEBUG ((DEBUG_WARN, "[PmConfigUpdate] CRC mismatch, proceeding anyway\n"));
  }

  Cfg     = &PmCrc->Config;
  Changed = FALSE;

  //
  // Patch OPP tables: per-entry frequency and voltage for all 12 domains
  //
  for (Domain = 0; Domain < DVFS_ELEMENT_IDX_COUNT; Domain++) {
    if (Domain < DOMAIN_MAX_COUNT) {
      if (PatchOppEntries (
            &Cfg->OppConfig.Opps[Domain],
            &SetupVar->PmOppFreq[Domain * 13],
            &SetupVar->PmOppVolt[Domain * 13]
            ))
      {
        DEBUG ((DEBUG_INFO, "[PmConfigUpdate] Patched OPP domain %d\n", Domain));
        Changed = TRUE;
      }
    }
  }

  //
  // Patch EDP/TDP power caps
  //
  AnyTdp = FALSE;
  for (Rail = 0; Rail < DPM_EDP_MAX; Rail++) {
    if (SetupVar->PmTdp[Rail] != 0) {
      AnyTdp = TRUE;
      break;
    }
  }

  if (AnyTdp) {
    if (Cfg->PmicConfig.PmicScheme.Fields.Valid != PM_CONFIG_VALID ||
        Cfg->PmicConfig.PmicScheme.Fields.RawData != CONFIG_EDP_CFG_CUSTOM) {
      Cfg->PmicConfig.PmicScheme.Fields.Valid   = PM_CONFIG_VALID;
      Cfg->PmicConfig.PmicScheme.Fields.RawData = CONFIG_EDP_CFG_CUSTOM;
      Changed = TRUE;
    }

    for (Rail = 0; Rail < DPM_EDP_MAX; Rail++) {
      if (SetupVar->PmTdp[Rail] != 0 &&
          Cfg->PmicConfig.EdpCfg[Rail].PwrCap != SetupVar->PmTdp[Rail]) {
        Cfg->PmicConfig.EdpCfg[Rail].PwrCap = SetupVar->PmTdp[Rail];
        DEBUG ((DEBUG_INFO, "[PmConfigUpdate] Patched TDP rail %d = %d mW\n",
                Rail, SetupVar->PmTdp[Rail]));
        Changed = TRUE;
      }
    }
  }

  if (!Changed) {
    DEBUG ((DEBUG_INFO, "[PmConfigUpdate] No changes needed\n"));
    FreePool (PmBuf);
    return FALSE;
  }

  //
  // Recompute CRC over everything before the CRC fields, then store.
  // Matches: double_check_sum(&g_config, sizeof(g_config) - 8, ...)
  //
  DoubleCheckSum (PmBuf, PM_CRC1_OFFSET, &Crc1, &Crc2);
  *GetCrc1Ptr (PmBuf) = Crc1;
  *GetCrc2Ptr (PmBuf) = Crc2;

  DEBUG ((DEBUG_INFO, "[PmConfigUpdate] Writing patched pm_config to SPI flash\n"));
  Status = FwUpdate->FirmwareRawEntryUpdate (
                       FIRMWARE_TYPE_PM_CONF,
                       PmBuf,
                       PM_CONFIG_BIN_SIZE,
                       ENTRY_WRITE,
                       NULL
                       );
  FreePool (PmBuf);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[PmConfigUpdate] FirmwareRawEntryUpdate WRITE failed: %r\n", Status));
    return FALSE;
  }

  //
  // Cold reset so SCP re-reads the patched pm_config from SPI flash.
  // On the next boot the driver will see the values already match and
  // skip the write, so no reset loop occurs.
  //
  DEBUG ((DEBUG_INFO, "[PmConfigUpdate] pm_config updated, cold resetting...\n"));
  gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  CpuDeadLoop ();  // should not reach here
  return TRUE;
}

/**
  Entry point.  Performs SPI flash pm_config patching immediately (early DXE)
  so any needed cold reset happens before the boot menu.  Registers a
  ReadyToBoot callback for ACPI DSDT gpu-microvolt patching.

  @param[in]  ImageHandle   Image handle
  @param[in]  SystemTable   System table pointer

  @retval EFI_SUCCESS       Completed successfully
**/
EFI_STATUS
EFIAPI
PmConfigUpdateDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS           Status;
  EFI_EVENT            Event;
  PLATFORM_SETUP_DATA  SetupVar;
  UINTN                VarSize;

  //
  // Read setup variable
  //
  VarSize = sizeof (PLATFORM_SETUP_DATA);
  Status  = gRT->GetVariable (
                   PLATFORM_SETUP_VAR,
                   &gPlatformSetupVariableGuid,
                   NULL,
                   &VarSize,
                   &SetupVar
                   );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[PmConfigUpdate] GetVariable failed: %r\n", Status));
    return Status;
  }

  mPmConfigEnabled = TRUE;

  //
  // Patch SPI flash immediately.  If values changed this triggers a cold
  // reset and does not return — the user never sees the boot menu.
  //
  PatchSpiFlashPmConfig (&SetupVar);

  //
  // If we get here, no reset was needed.  Register a ReadyToBoot callback
  // to patch the ACPI DSDT gpu-microvolt (ACPI tables are not available yet).
  //
  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             PmConfigReadyToBootCallback,
             NULL,
             &Event
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[PmConfigUpdate] CreateEventReadyToBoot failed: %r\n", Status));
  }

  return EFI_SUCCESS;
}
