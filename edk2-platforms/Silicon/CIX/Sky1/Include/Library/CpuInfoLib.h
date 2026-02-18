/*
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#ifndef CPU_INFO_LIB
#define CPU_INFO_LIB

#include <Base.h>
#include <Uefi.h>

/** Get the boot core id of cpu
  @param[in,out]  CpuBootCoreId     The boot core id of cpu.
  @retval VOID
**/
VOID
EFIAPI
GetCpuBootCoreId (
  IN OUT UINT8  *CpuBootCoreId
  );

/** Get the core mask of cpu
 *
 * ex. bit 0 in CpuCoreMask coresponding to cpu core 0, etc.
 *
  @param[in,out]  CpuCoreMask     The core mask of cpu, bit index in CpuCoreMask
                                  coresponding to cpu core index.
  @param[out]     MaxCpuCoreNum      The max core number of cpu.
  @retval VOID
**/
VOID
EFIAPI
GetCpuCoreMask (
  IN OUT UINT32  *CpuCoreMask,
  OUT    UINT32  *MaxCpuCoreNum
  );

/** Get the valid cpu core number
  @param[in,out]  ValidCpuCoreNum     The valid cpu core number.
  @retval VOID
**/
VOID
EFIAPI
GetValidCpuCoreNum (
  IN OUT UINT8  *ValidCpuCoreNum
  );

/** Get Cpu share information
  @param[in,out]  CpuShareInfo     Cpu share information.
  @retval VOID
**/
VOID
EFIAPI
GetCpuShareInfo (
  IN OUT UINT32  *CpuShareInfo
  );

/** Gets the max CPU frequency.
  @return               CPU frequency in Hz
**/
UINTN
EFIAPI
GetMaxCpuFreq (
  VOID
  );

/** Gets the current CPU frequency.
  @return               CPU frequency in Hz
**/
UINTN
EFIAPI
GetCurrentCpuFreq (
  VOID
  );

#define ROUND_DIVISION(a, b)  (((a) / (b)) + ((((a) % (b)) * 2 >= (b)) ? 1 : 0))

#endif // CPU_INFO_LIB
