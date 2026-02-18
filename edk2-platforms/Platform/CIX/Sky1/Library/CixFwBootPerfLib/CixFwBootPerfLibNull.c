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

void
cix_set_boot_phase (
  enum FW_BOOT_PHASE  phase,
  enum RECORD_POINT   point
  )
{

}

UINT64
cix_get_boot_phase (
  enum FW_BOOT_PHASE  phase,
  enum RECORD_POINT   point
  )
{
  return 0;
}