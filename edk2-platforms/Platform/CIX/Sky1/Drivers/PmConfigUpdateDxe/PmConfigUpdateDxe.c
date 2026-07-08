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

  Entry 1 of CPU/DSU domains (2-7) is the safe sustained entry and is
  never patched from NVRAM — it always keeps its stock safe value.

  @param[in,out]  DomainOpp    Pointer to domain OPP config in pm_config
  @param[in]      SetupFreq    Array of 13 frequency values (MHz), 0=stock
  @param[in]      SetupVolt    Array of 13 voltage values (mV), 0=stock
  @param[in]      DomainIdx    Domain index (DVFS_ELEMENT_IDX_*)

  @retval TRUE   At least one entry was modified
  @retval FALSE  No modifications made
**/
STATIC
BOOLEAN
PatchOppEntries (
  IN OUT DOMAIN_OPP_CONFIG_T  *DomainOpp,
  IN     UINT16               *SetupFreq,
  IN     UINT16               *SetupVolt,
  IN     UINT8                 DomainIdx
  )
{
  UINT16   NvSlot;
  UINT16   OppIdx;
  UINT16   NumVisible;
  BOOLEAN  Modified;
  UINT32   NewFreq;
  UINT32   NewVolt;
  UINT32   NewLevel;
  BOOLEAN  HasHidden;

  Modified = FALSE;

  //
  // All domains except CI700 (10) have a hidden safe sustained entry at OPP[0].
  // NVRAM uses contiguous slots: slot 0 → OPP[1], slot 1 → OPP[2], ...
  // CI700 has only a single fixed entry and is not user-configurable.
  //
  HasHidden = (DomainIdx != 10);
  NumVisible = DomainOpp->Size;
  if (HasHidden && NumVisible > 0) {
    NumVisible--;   // one entry is hidden
  }
  if (NumVisible > DOMAIN_MAX_OPP_ENTRIES) {
    NumVisible = DOMAIN_MAX_OPP_ENTRIES;
  }

  for (NvSlot = 0; NvSlot < NumVisible; NvSlot++) {
    //
    // Map NVRAM slot to OPP table index.
    // For domains with a hidden entry at OPP[0]: slot N → OPP[N+1].
    //
    if (HasHidden) {
      OppIdx = NvSlot + 1;
    } else {
      OppIdx = NvSlot;
    }

    //
    // Bound by the real OppTable[] array size, not just DomainOpp->Size:
    // Size comes from flash and, if it were >= DOMAIN_MAX_OPP_ENTRIES with a
    // hidden entry, OppIdx (= NvSlot + 1) could reach DOMAIN_MAX_OPP_ENTRIES
    // and write one DVFS_OPP_T past the array into the next domain.
    //
    if ((OppIdx >= DomainOpp->Size) || (OppIdx >= DOMAIN_MAX_OPP_ENTRIES)) {
      break;
    }

    if (SetupFreq[NvSlot] != 0) {
      //
      // GPU domains (GPU_CORE=0, GPU_TOP=1) store clock rate in the
      // Frequency field (kHz).  All other domains (CPU, DSU, NPU, VPU,
      // MMHUB) store it in Level (MHz) and leave Frequency at 0 — the
      // SCP firmware uses Level for those domains and ignores Frequency.
      // Writing a non-zero Frequency for non-GPU domains causes the SCP
      // to compute the wrong clock, so we keep Frequency = 0 and also
      // repair any value that a previous boot may have written incorrectly.
      //
      if (DomainIdx <= DVFS_ELEMENT_IDX_GPU_TOP) {
        //
        // GPU: update Frequency (kHz) and keep Level (MHz) in sync.
        //
        NewFreq  = (UINT32)SetupFreq[NvSlot] * 1000U;
        NewLevel = (UINT32)SetupFreq[NvSlot];
        if (DomainOpp->OppTable[OppIdx].Frequency != NewFreq) {
          DomainOpp->OppTable[OppIdx].Frequency = NewFreq;
          Modified = TRUE;
        }
        if (DomainOpp->OppTable[OppIdx].Level != NewLevel) {
          DomainOpp->OppTable[OppIdx].Level = NewLevel;
          Modified = TRUE;
        }
      } else {
        //
        // Non-GPU: update Level (MHz), ensure Frequency is 0.
        //
        NewLevel = (UINT32)SetupFreq[NvSlot];
        if (DomainOpp->OppTable[OppIdx].Level != NewLevel) {
          DomainOpp->OppTable[OppIdx].Level = NewLevel;
          Modified = TRUE;
        }
        if (DomainOpp->OppTable[OppIdx].Frequency != 0) {
          DomainOpp->OppTable[OppIdx].Frequency = 0;
          Modified = TRUE;
        }
      }
    }

    if (SetupVolt[NvSlot] != 0) {
      NewVolt = (UINT32)SetupVolt[NvSlot];
      if (DomainOpp->OppTable[OppIdx].Voltage != NewVolt) {
        DomainOpp->OppTable[OppIdx].Voltage = NewVolt;
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

//
// CPU cluster CPPC patching — maps OPP domain index to the DesiredPerf
// hardware register address that uniquely identifies each cluster's _CPC
// package in the compiled DSDT AML.
//
typedef struct {
  UINT8   DvfsIdx;      // DVFS_ELEMENT_IDX_*
  UINT32  DesiredReg;   // CORE_*_DESIRED_PERF_REG
  UINT8   CoreCount;    // number of cores sharing this OPP
} CPPC_CLUSTER_MAP;

// 5-cluster map matching MADT UID order (mCoreIterOrder = [10,11,8,9,4,5,6,7,0,1,2,3]):
//   UIDs 0-1:  BIG G1 (boot cluster), DesiredPerf reg = BIG G1 (0x0659009C)
//   UIDs 2-3:  BIG G0,                DesiredPerf reg = BIG G0 (0x06590098)
//   UIDs 4-5:  MID G0,                DesiredPerf reg = MID G0 (0x065900A0)
//   UIDs 6-7:  MID G1,                DesiredPerf reg = MID G1 (0x065900A4)
//   UIDs 8-11: LITTLE,                DesiredPerf reg = LITTLE (0x06590094)
STATIC CONST CPPC_CLUSTER_MAP  mClusterMap[] = {
  { DVFS_ELEMENT_IDX_BIG_G1,  0x0659009C, 2 },  // BIG G1 (boot), UIDs 0-1
  { DVFS_ELEMENT_IDX_BIG_G0,  0x06590098, 2 },  // BIG G0,        UIDs 2-3
  { DVFS_ELEMENT_IDX_MID_G0,  0x065900A0, 2 },  // MID G0,        UIDs 4-5
  { DVFS_ELEMENT_IDX_MID_G1,  0x065900A4, 2 },  // MID G1,        UIDs 6-7
  { DVFS_ELEMENT_IDX_LITTLE,  0x06590094, 4 },  // LITTLE,         UIDs 8-11
};

#define CLUSTER_MAP_COUNT  (sizeof (mClusterMap) / sizeof (mClusterMap[0]))

/**
  Find the maximum OPP frequency (in MHz) for a given DVFS domain.

  GPU domains (indices 0-1) store clock rate in the Frequency field (kHz).
  All other domains (CPU, DSU, NPU, VPU, ...) store it in the Level field
  (MHz) and leave Frequency at 0.

  @param[in]  OppConfig  Pointer to the OPP config for this domain
  @param[in]  DvfsIdx    DVFS_ELEMENT_IDX_* for this domain
  @retval     Maximum frequency in MHz, or 0 if not found
**/
STATIC
UINT32
GetMaxOppFreqMhz (
  IN DOMAIN_OPP_CONFIG_T  *OppConfig,
  IN UINT8                 DvfsIdx
  )
{
  UINT32  MaxFreq;
  UINT16  I;

  MaxFreq = 0;
  for (I = 0; I < OppConfig->Size && I < DOMAIN_MAX_OPP_ENTRIES; I++) {
    UINT32  Freq;
    if (DvfsIdx <= DVFS_ELEMENT_IDX_GPU_TOP) {
      //
      // GPU: clock rate in Frequency field (kHz) → convert to MHz
      //
      Freq = OppConfig->OppTable[I].Frequency / 1000;
    } else {
      //
      // CPU/DSU/etc.: clock rate in Level field (already MHz)
      //
      Freq = OppConfig->OppTable[I].Level;
    }
    if (Freq > MaxFreq) {
      MaxFreq = Freq;
    }
  }

  return MaxFreq;
}

//
// SSTP patch table: maps AML Named integer name to EDP rail index.
// These Named integers are defined in Dsdt-Thermal.asl and hold the
// sustainable power (mW) for each thermal zone.  PwrCap == 0 means
// "disabled / no hardware limit" — skip patching so the ACPI default
// (which already matches the stock pmic_config.h value) is preserved.
//
typedef struct {
  CHAR8  Name[4];   // AML NameSeg (4 chars, not NUL-terminated)
  UINT8  EdpRail;   // DPM_EDP_* index into PmicConfig.EdpCfg[]
} SSTP_PATCH_ENTRY;

STATIC CONST SSTP_PATCH_ENTRY  mSstpMap[] = {
  { {'S','B','0','P'}, DPM_EDP_CPU_GB0 },  // TZB0 — Big G0
  { {'S','B','1','P'}, DPM_EDP_CPU_GB1 },  // TZB1 — Big G1
  { {'S','M','0','P'}, DPM_EDP_CPU_GM0 },  // TZM0 — Mid G0
  { {'S','M','1','P'}, DPM_EDP_CPU_GM1 },  // TZM1 — Mid G1
  { {'S','G','P','P'}, DPM_EDP_GPU     },  // TZGT — GPU
};

#define SSTP_MAP_COUNT  (sizeof (mSstpMap) / sizeof (mSstpMap[0]))

/**
  Patch a single AML Named integer (Name op + 4-char NameSeg) in the DSDT.

  Searches the AML buffer for the pattern:
    0x08  <Name[0]> <Name[1]> <Name[2]> <Name[3]>  <opcode>  <value bytes>
  and overwrites the value bytes in-place.  Handles Byte (0x0A), Word (0x0B)
  and DWord (0x0C) integer encodings; the new value must fit within the
  existing encoding width or the patch is skipped.

  @param[in,out]  Buf       DSDT AML buffer
  @param[in]      Len       Buffer length in bytes
  @param[in]      Name4     Pointer to 4-byte AML NameSeg (not NUL-terminated)
  @param[in]      NewValue  New integer value to write

  @retval TRUE   Value was patched
  @retval FALSE  Pattern not found or value does not fit encoding
**/
STATIC
BOOLEAN
PatchNamedIntInDsdt (
  IN OUT UINT8        *Buf,
  IN     UINT32        Len,
  IN     CONST CHAR8  *Name4,
  IN     UINT32        NewValue
  )
{
  UINTN  Pos;
  UINT8  Opcode;

  for (Pos = 0; Pos + 6 < (UINTN)Len; Pos++) {
    //
    // Look for NameOp (0x08) followed by the 4-byte NameSeg.
    //
    if (Buf[Pos]   != 0x08)            { continue; }
    if (Buf[Pos+1] != (UINT8)Name4[0]) { continue; }
    if (Buf[Pos+2] != (UINT8)Name4[1]) { continue; }
    if (Buf[Pos+3] != (UINT8)Name4[2]) { continue; }
    if (Buf[Pos+4] != (UINT8)Name4[3]) { continue; }

    Opcode = Buf[Pos+5];

    if (Opcode == 0x0A) {
      // ByteConst: 1-byte value, max 255
      if (NewValue > 0xFF || Pos + 6 >= Len) {
        return FALSE;
      }
      Buf[Pos+6] = (UINT8)NewValue;
      return TRUE;
    }

    if (Opcode == 0x0B) {
      // WordConst: 2-byte little-endian value, max 65535
      if (NewValue > 0xFFFF || Pos + 7 >= Len) {
        return FALSE;
      }
      Buf[Pos+6] = (UINT8)(NewValue);
      Buf[Pos+7] = (UINT8)(NewValue >> 8);
      return TRUE;
    }

    if (Opcode == 0x0C) {
      // DWordConst: 4-byte little-endian value
      if (Pos + 9 >= Len) {
        return FALSE;
      }
      Buf[Pos+6] = (UINT8)(NewValue);
      Buf[Pos+7] = (UINT8)(NewValue >> 8);
      Buf[Pos+8] = (UINT8)(NewValue >> 16);
      Buf[Pos+9] = (UINT8)(NewValue >> 24);
      return TRUE;
    }

    // Found the name but unrecognised opcode — keep searching in case of
    // a false positive (e.g. the same byte sequence inside a string).
  }

  return FALSE;
}

/**
  Patch the SSTP Named integers in the DSDT to reflect the effective EDP
  power caps from the (already patched) pm_config binary.

  For each thermal zone's SSTP variable, reads the corresponding
  EdpCfg[Rail].PwrCap from pm_config.  If PwrCap is zero (domain disabled /
  no hardware limit), the ACPI default (which already matches the stock
  pmic_config.h value) is preserved.

  @param[in]  PmCrc  Pointer to the validated pm_config binary
**/
STATIC
VOID
PatchSstpInDsdt (
  IN PM_EXPORT_CONFIG_CRC_T  *PmCrc
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
  UINTN                   Entry;
  UINT8                   Rail;
  UINT32                  PwrCap;
  BOOLEAN                 AnyPatched;

  Status = gBS->LocateProtocol (&gEfiAcpiSdtProtocolGuid, NULL, (VOID **)&AcpiSdt);
  if (EFI_ERROR (Status)) {
    return;
  }

  for (I = 0; I < ACPI_MAX_TABLES; I++) {
    Status = AcpiSdt->GetAcpiTable (I, &Table, &TableVersion, &TableKey);
    if (EFI_ERROR (Status)) {
      break;
    }

    if (Table->Signature != SIGNATURE_32 ('D','S','D','T')) {
      continue;
    }

    Buf        = (UINT8 *)Table;
    Len        = Table->Length;
    AnyPatched = FALSE;

    for (Entry = 0; Entry < SSTP_MAP_COUNT; Entry++) {
      Rail   = mSstpMap[Entry].EdpRail;
      PwrCap = PmCrc->Config.PmicConfig.EdpCfg[Rail].PwrCap;

      //
      // PwrCap == 0 means "no hardware limit" — skip so the ACPI default
      // (already set to the stock pmic_config.h value) is preserved.
      //
      if (PwrCap == 0 || PwrCap > 65000) {
        DEBUG ((DEBUG_INFO, "[PmConfigUpdate] SSTP %c%c%c%c: PwrCap=%u, skipping\n",
                mSstpMap[Entry].Name[0], mSstpMap[Entry].Name[1],
                mSstpMap[Entry].Name[2], mSstpMap[Entry].Name[3], PwrCap));
        continue;
      }

      if (PatchNamedIntInDsdt (Buf, Len, mSstpMap[Entry].Name, PwrCap)) {
        AnyPatched = TRUE;
        DEBUG ((DEBUG_INFO, "[PmConfigUpdate] SSTP %c%c%c%c: patched to %u mW\n",
                mSstpMap[Entry].Name[0], mSstpMap[Entry].Name[1],
                mSstpMap[Entry].Name[2], mSstpMap[Entry].Name[3], PwrCap));
      } else {
        DEBUG ((DEBUG_WARN, "[PmConfigUpdate] SSTP %c%c%c%c: not found in DSDT\n",
                mSstpMap[Entry].Name[0], mSstpMap[Entry].Name[1],
                mSstpMap[Entry].Name[2], mSstpMap[Entry].Name[3]));
      }
    }

    if (AnyPatched) {
      AcpiFixChecksum (Table);
    }

    break;  // only one DSDT
  }
}

/**
  Patch CPPC HighestPerf and NominalPerf in ACPI tables for all CPU clusters
  based on actual OPP table maximum frequencies.

  The _CPC packages are generated dynamically in the CPU topology SSDT by
  the SsdtCpuTopologyGenerator.  This function searches both SSDTs and
  the DSDT for each cluster's unique DesiredPerf register address within
  ACPI Generic Register Descriptors in _CPC packages.  The _CPC package
  layout places HighestPerf and NominalPerf as the 3rd and 4th elements.
  In compiled AML these are integer constants preceding the first
  ResourceTemplate (GuaranteedPerf register).

  We locate the DesiredPerf register descriptor (the 8th _CPC element),
  then scan backwards to find and patch the HighestPerf/NominalPerf values.

  @param[in]  PmCrc  Pointer to the pm_config binary (validated)
**/
STATIC
VOID
PatchCppcInAcpi (
  IN PM_EXPORT_CONFIG_CRC_T  *PmCrc
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
  UINTN                   Pos;
  UINTN                   Cluster;
  UINT32                  MaxFreq;
  UINT32                  DesiredReg;
  UINT8                   DesiredRegLe[4];
  UINTN                   Patched;
  UINTN                   Expected;
  UINTN                   Back;
  UINTN                   IntCount;
  UINTN                   IntPos[6];
  UINTN                   Scan;
  UINTN                   Idx;
  UINT32                  NumEntries;
  UINT32                  Revision;
  UINT8                   Prefix;

  Status = gBS->LocateProtocol (&gEfiAcpiSdtProtocolGuid, NULL, (VOID **)&AcpiSdt);
  if (EFI_ERROR (Status)) {
    return;
  }

  for (I = 0; I < ACPI_MAX_TABLES; I++) {
    Status = AcpiSdt->GetAcpiTable (I, &Table, &TableVersion, &TableKey);
    if (EFI_ERROR (Status)) {
      break;
    }

    //
    // _CPC packages live in the dynamically generated SSDT (CPU topology),
    // not in the DSDT.  Search both so the patch works regardless of where
    // future code places the _CPC objects.
    //
    if (Table->Signature != SIGNATURE_32 ('S','S','D','T') &&
        Table->Signature != SIGNATURE_32 ('D','S','D','T')) {
      continue;
    }

    Buf = (UINT8 *)Table;
    Len = Table->Length;

    for (Cluster = 0; Cluster < CLUSTER_MAP_COUNT; Cluster++) {
      MaxFreq = GetMaxOppFreqMhz (
                  &PmCrc->Config.OppConfig.Opps[mClusterMap[Cluster].DvfsIdx],
                  mClusterMap[Cluster].DvfsIdx
                  );
      if (MaxFreq == 0) {
        continue;
      }

      DesiredReg = mClusterMap[Cluster].DesiredReg;
      DesiredRegLe[0] = (UINT8)(DesiredReg);
      DesiredRegLe[1] = (UINT8)(DesiredReg >> 8);
      DesiredRegLe[2] = (UINT8)(DesiredReg >> 16);
      DesiredRegLe[3] = (UINT8)(DesiredReg >> 24);

      //
      // Count expected _CPC instances (one per core in this cluster)
      //
      Expected = mClusterMap[Cluster].CoreCount;
      Patched  = 0;

      for (Pos = 0; Pos + 15 < Len; Pos++) {
        //
        // Look for Generic Register Descriptor (0x82) containing our address.
        // Format: 0x82 0x0C 0x00 AddrSpaceId BitWidth BitOffset AccessSize Addr[8]
        // The DesiredPerf register is SystemMemory(0x00), BitWidth=32(0x20), BitOffset=0, AccessSize=3
        //
        if (Buf[Pos] != 0x82 || Buf[Pos+1] != 0x0C || Buf[Pos+2] != 0x00) {
          continue;
        }
        if (Buf[Pos+3] != 0x00 || Buf[Pos+4] != 0x20 || Buf[Pos+5] != 0x00 || Buf[Pos+6] != 0x03) {
          continue;
        }
        if (CompareMem (&Buf[Pos+7], DesiredRegLe, 4) != 0) {
          continue;
        }

        //
        // Found the DesiredPerf register descriptor at Pos.
        // Now scan backwards to find the integer constants that form
        // HighestPerf(3rd), NominalPerf(4th), LowestNonlinear(5th), LowestPerf(6th).
        // Before the DesiredPerf register there are several ResourceTemplate buffers
        // (GuaranteedPerf register) and the 4 integer performance values and
        // NumEntries(23) + Revision(3).
        //
        // Strategy: scan backwards up to ~200 bytes looking for integer opcodes.
        // Collect the last 6 integers found (NumEntries, Revision, Highest,
        // Nominal, LowestNonlinear, LowestPerf).
        // Patch integers [2] (Highest) and [3] (Nominal).
        //
        IntCount = 0;
        Back = Pos;
        if (Back > 200) {
          Back = Pos - 200;
        } else {
          Back = 0;
        }

        for (Scan = Back; Scan < Pos && IntCount < 6; Scan++) {
          if (Buf[Scan] == AML_DWORD_PREFIX && Scan + 4 < Pos) {
            IntPos[IntCount++] = Scan + 1;  // offset of the 4-byte value
            Scan += 4;  // skip past the DWORD
          } else if (Buf[Scan] == AML_WORD_PREFIX && Scan + 2 < Pos) {
            IntPos[IntCount++] = Scan + 1;  // offset of the 2-byte value
            Scan += 2;
          } else if (Buf[Scan] == AML_BYTE_PREFIX) {
            IntPos[IntCount++] = Scan + 1;
            Scan += 1;
          }
        }

        //
        // We need at least 4 integers: NumEntries, Revision, Highest, Nominal
        // Patch Highest (index 2) and Nominal (index 3)
        //
        if (IntCount >= 4) {
          //
          // Verify NumEntries = 23 and Revision = 3 to confirm we found _CPC
          //
          NumEntries = 0;
          Revision = 0;

          // Read NumEntries (could be byte/word/dword encoded)
          if (Buf[IntPos[0] - 1] == AML_BYTE_PREFIX) {
            NumEntries = Buf[IntPos[0]];
          } else if (Buf[IntPos[0] - 1] == AML_WORD_PREFIX) {
            NumEntries = *(UINT16 *)&Buf[IntPos[0]];
          } else {
            NumEntries = *(UINT32 *)&Buf[IntPos[0]];
          }

          if (Buf[IntPos[1] - 1] == AML_BYTE_PREFIX) {
            Revision = Buf[IntPos[1]];
          } else if (Buf[IntPos[1] - 1] == AML_WORD_PREFIX) {
            Revision = *(UINT16 *)&Buf[IntPos[1]];
          } else {
            Revision = *(UINT32 *)&Buf[IntPos[1]];
          }

          if (NumEntries != 23 || Revision != 3) {
            continue;  // not a _CPC package
          }

          //
          // Patch HighestPerf (index 2) and NominalPerf (index 3)
          // The value encoding determines how we write:
          // BytePrefix (0x0A): 1 byte, max 255
          // WordPrefix (0x0B): 2 bytes, max 65535
          // DWordPrefix (0x0C): 4 bytes
          //
          // Frequencies up to 4500 MHz fit in a WORD. The existing values
          // are compiled with the same encoding, so we can safely overwrite
          // in-place as long as the new value fits in the same encoding size.
          //
          for (Idx = 2; Idx <= 3; Idx++) {
            Prefix = Buf[IntPos[Idx] - 1];

            if (Prefix == AML_WORD_PREFIX && MaxFreq <= 0xFFFF) {
              *(UINT16 *)&Buf[IntPos[Idx]] = (UINT16)MaxFreq;
            } else if (Prefix == AML_DWORD_PREFIX) {
              *(UINT32 *)&Buf[IntPos[Idx]] = MaxFreq;
            } else if (Prefix == AML_BYTE_PREFIX && MaxFreq <= 0xFF) {
              Buf[IntPos[Idx]] = (UINT8)MaxFreq;
            }
            // If encoding is too small for the value, skip (shouldn't happen
            // since existing value is 4500 which requires at least WORD).
          }

          Patched++;
        }
      }

      DEBUG ((DEBUG_INFO, "[PmConfigUpdate] CPPC cluster %d: patched %d/%d cores, max=%u MHz\n",
              (UINT32)Cluster, (UINT32)Patched, (UINT32)Expected, MaxFreq));
    }

    AcpiFixChecksum (Table);
    // continue scanning — _CPC may be in any SSDT or the DSDT
  }
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

  //
  // Patch CPPC HighestPerf/NominalPerf in the CPU topology SSDT
  //
  PatchCppcInAcpi (PmCrc);

  //
  // Patch SSTP Named integers in thermal zones with actual EDP power caps
  //
  PatchSstpInDsdt (PmCrc);

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
            &SetupVar->PmOppVolt[Domain * 13],
            Domain
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

  //
  // Patch SoC voltage offset (delta_mV on DPM_EDP_SOC rail)
  //
  if (SetupVar->PmSocVoltageOffset != 0) {
    INT32   SocDeltaMv;
    UINT16  SocOffsetMag;

    //
    // EdpCfg[].DeltaMv is a signed 10-bit field (range -512..+511).  The setup
    // variable is a UINT16 and OS-writable, so an out-of-range magnitude would
    // be truncated when stored, the read-back would never equal the requested
    // value, "Changed" would latch TRUE every boot, and the driver would write
    // + cold-reset on every boot (soft-brick).  Clamp the magnitude so the
    // value always round-trips through the field.
    //
    SocOffsetMag = SetupVar->PmSocVoltageOffset;
    if (SocOffsetMag > 511) {
      DEBUG ((
        DEBUG_WARN,
        "[PmConfigUpdate] SoC voltage offset %d mV exceeds 511, clamping\n",
        SocOffsetMag
        ));
      SocOffsetMag = 511;
    }

    SocDeltaMv = (INT32)SocOffsetMag;
    if (SetupVar->PmSocVoltagePolarity == 1) {
      SocDeltaMv = -SocDeltaMv;
    }

    if (Cfg->PmicConfig.PmicScheme.Fields.Valid != PM_CONFIG_VALID ||
        Cfg->PmicConfig.PmicScheme.Fields.RawData != CONFIG_EDP_CFG_CUSTOM) {
      Cfg->PmicConfig.PmicScheme.Fields.Valid   = PM_CONFIG_VALID;
      Cfg->PmicConfig.PmicScheme.Fields.RawData = CONFIG_EDP_CFG_CUSTOM;
      Changed = TRUE;
    }

    if (Cfg->PmicConfig.EdpCfg[DPM_EDP_SOC].DeltaMv != SocDeltaMv) {
      DEBUG ((DEBUG_INFO, "[PmConfigUpdate] Patched SoC delta_mV: %d -> %d mV\n",
              Cfg->PmicConfig.EdpCfg[DPM_EDP_SOC].DeltaMv, SocDeltaMv));
      Cfg->PmicConfig.EdpCfg[DPM_EDP_SOC].DeltaMv = SocDeltaMv;
      Changed = TRUE;
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
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[PmConfigUpdate] FirmwareRawEntryUpdate WRITE failed: %r\n", Status));
    FreePool (PmBuf);
    return FALSE;
  }

  //
  // Read-back verify BEFORE resetting.  FirmwareRawEntryUpdate returns a
  // UINT16 status code zero-extended into EFI_STATUS, so the EFI_ERROR() check
  // above can never see a device-level write failure (the error bit, 63, is
  // always clear).  If the write silently failed, the flash still holds the
  // old values, "Changed" would be TRUE again next boot, and we would
  // cold-reset on every boot.  Re-read the region and only reset if the flash
  // now matches what we intended to write.
  //
  {
    UINT8  *VerifyBuf;

    VerifyBuf = AllocateZeroPool (PM_CONFIG_BIN_SIZE);
    if (VerifyBuf != NULL) {
      FwUpdate->FirmwareRawEntryUpdate (
                  FIRMWARE_TYPE_PM_CONF,
                  VerifyBuf,
                  PM_CONFIG_BIN_SIZE,
                  ENTRY_READ,
                  NULL
                  );
      if (CompareMem (VerifyBuf, PmBuf, PM_CONFIG_BIN_SIZE) != 0) {
        DEBUG ((
          DEBUG_ERROR,
          "[PmConfigUpdate] pm_config read-back mismatch after write; "
          "NOT resetting to avoid a reset loop\n"
          ));
        FreePool (VerifyBuf);
        FreePool (PmBuf);
        return FALSE;
      }

      FreePool (VerifyBuf);
    }

    //
    // If the verify buffer could not be allocated we fall through and reset
    // as before rather than skip a legitimate update.
    //
  }

  FreePool (PmBuf);

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
  Entry point.Performs SPI flash pm_config patching immediately (early DXE)
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
  //
  // Pre-zero: if a stored variable from an older BIOS is shorter than this
  // build's struct, GetVariable succeeds with a short VarSize and would leave
  // the tail (PmOppFreq/PmOppVolt/PmTdp/PmSocVoltageOffset) as stack garbage,
  // which PatchSpiFlashPmConfig would then write into the SPI pm_config as
  // real OPP/voltage values.  Zeroing first makes any missing field read as
  // 0 ("stock", no patch).
  //
  ZeroMem (&SetupVar, sizeof (SetupVar));
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
