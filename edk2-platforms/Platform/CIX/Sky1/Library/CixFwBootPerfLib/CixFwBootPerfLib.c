/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
  Copyright 2024 Cix Technology Group Co., Ltd.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/CixFwBootPerfLib.h>
#include <Library/ShMemLib.h>

UINT64
TimeStampGet (
  )
{
  // CHAR8   Buffer[32];
  UINT64  Ticker, TimeStamp;

  // UINT64 Second, Remainder, MicroSecond;

  Ticker    = GetPerformanceCounter ();
  TimeStamp = GetTimeInNanoSecond (Ticker);
  return TimeStamp;
  // Second      = TimeStamp/(1000*1000*1000);
  // Remainder   = TimeStamp%(1000*1000*1000);
  // MicroSecond = Remainder/(1000);

  // AsciiSPrint (Buffer, 13, "[%02lld.%06lld] ", Second, MicroSecond);
  // SerialPortWrite ((UINT8 *)Buffer, AsciiStrLen (Buffer));
  // Index++;
}

static void
set_fw_name (
  enum FW_BOOT_PHASE  phase,
  char                *fw_name
  )
{
  switch (phase) {
    case BROM_PHASE:
      CopyMem (fw_name, "BOOTROM", sizeof ("BOOTROM"));
      break;
    case SE_PHASE:
      CopyMem (fw_name, "CSU_SE", sizeof ("CSU_SE"));
      break;
    case PM_PHASE:
      CopyMem (fw_name, "CSU_PM", sizeof ("CSU_PM"));
      break;
    case PBL_PHASE:
      CopyMem (fw_name, "PBL", sizeof ("PBL"));
      break;
    case TFA_PHASE:
      CopyMem (fw_name, "TFA", sizeof ("TFA"));
      break;
    case TEE_PHASE:
      CopyMem (fw_name, "TEE", sizeof ("TEE"));
      break;
    case BLOADER_PHASE:
      CopyMem (fw_name, "BLOADER", sizeof ("BLOADER"));
      break;
    case GRUB_PHASE:
      CopyMem (fw_name, "GRUB", sizeof ("GRUB"));
      break;
    default:
      break;
  }
}

void
cix_set_boot_phase (
  enum FW_BOOT_PHASE  phase,
  enum RECORD_POINT   point
  )
{
  struct fw_boot_phase_point  *g_boot_perf_base;
  UINT32                      SmemSize;

  g_boot_perf_base = SmemGetAddr (SMEM_FW_BOOT_PERF, &SmemSize);
  if (!g_boot_perf_base) {
    return;
  }

  switch (point) {
    case RECORD_START:
      set_fw_name (phase, g_boot_perf_base[phase].fw_name);
      g_boot_perf_base[phase].start = TimeStampGet () / 1000000;
      break;
    case RECORD_END:
      g_boot_perf_base[phase].end = TimeStampGet () / 1000000;
      break;
    default:
      break;
  }
}

UINT64
cix_get_boot_phase (
  enum FW_BOOT_PHASE  phase,
  enum RECORD_POINT   point
  )
{
  struct fw_boot_phase_point  *g_boot_perf_base;
  UINT32                      SmemSize;

  g_boot_perf_base = SmemGetAddr (SMEM_FW_BOOT_PERF, &SmemSize);
  if (!g_boot_perf_base) {
    return 0;
  }

  switch (point) {
    case RECORD_START:
      return g_boot_perf_base[phase].start;

    case RECORD_END:
      return g_boot_perf_base[phase].end;

    default:
      break;
  }

  return 0;
}
