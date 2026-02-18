/** @file
  Parsing NVPARAM board settings for bifurcation programming.

  NVPARAM board settings is spec-ed within Firmware Interface Requirement.
  Bifurcation devmap is programmed before at SCP following the rule

  Root Complex Type-A devmap settings (RP == Root Port)
  -----------------------------------------
  | RP0   | RP1  | RP2  | RP3  | Devmap   |
  | (x16) | (x4) | (x8) | (x4) | (output) |
  -------------------------------------------
  |  Y    |  N   |  N   |  N   | 0        |
  |  Y    |  N   |  Y   |  N   | 1        |
  |  Y    |  N   |  Y   |  Y   | 2        |
  |  Y    |  Y   |  Y   |  Y   | 3        |
  -----------------------------------------

  Root Complex Type-B LOW (aka RCBxA) devmap settings (RP == Root Port)
  ----------------------------------------
  | RP0  | RP1  | RP2  | RP3  | Devmap   |
  | (x8) | (x2) | (x4) | (x3) | (output) |
  ----------------------------------------
  |  Y   |  N   |  N   |  N   | 0        |
  |  Y   |  N   |  Y   |  N   | 1        |
  |  Y   |  N   |  Y   |  Y   | 2        |
  |  Y   |  Y   |  Y   |  Y   | 3        |
  ----------------------------------------

  Root Complex Type-B HIGH (aka RCBxB) devmap settings (RP == Root Port)
  ----------------------------------------
  | RP4  | RP5  | RP6  | RP7  | Devmap   |
  | (x8) | (x2) | (x4) | (x3) | (output) |
  ----------------------------------------
  |  Y   |  N   |  N   |  N   | 0        |
  |  Y   |  N   |  Y   |  N   | 1        |
  |  Y   |  N   |  Y   |  Y   | 2        |
  |  Y   |  Y   |  Y   |  Y   | 3        |
  ----------------------------------------

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>

#include <Guid/PlatformInfoHob.h>
#include <Guid/RootComplexInfoHob.h>
#include <Library/AmpereCpuLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/NVParamLib.h>
#include <NVParamDef.h>

#include "RootComplexNVParam.h"

STATIC
BOOLEAN
IsEmptyRC (
  AC01_ROOT_COMPLEX *RootComplex
  )
{
  UINT8 Idx;

  for (Idx = PcieController0; Idx < MaxPcieController; Idx++) {
    if (RootComplex->Pcie[Idx].Active) {
      return FALSE;
    }
  }

  return TRUE;
}

VOID
SetRootComplexBifurcation (
  IN AC01_ROOT_COMPLEX *RootComplex,
  IN UINT8             RPStart,
  IN DEV_MAP_MODE      DevMap
  )
{
  UINT8 MaxWidth;

  if (RPStart != PcieController0 && RPStart != PcieController4) {
    return;
  }

  if (RootComplex->Type != RootComplexTypeB && RPStart == PcieController4) {
    return;
  }

  if (RootComplex->Type == RootComplexTypeA && RootComplex->Pcie[RPStart].MaxWidth == LINK_WIDTH_X16) {
    RootComplex->Pcie[RPStart + 1].MaxWidth = LINK_WIDTH_X4;
    RootComplex->Pcie[RPStart + 2].MaxWidth = LINK_WIDTH_X8;
    RootComplex->Pcie[RPStart + 3].MaxWidth = LINK_WIDTH_X4;
  }

  if (RootComplex->Type == RootComplexTypeB && RootComplex->Pcie[RPStart].MaxWidth == LINK_WIDTH_X8) {
    RootComplex->Pcie[RPStart + 1].MaxWidth = LINK_WIDTH_X2;
    RootComplex->Pcie[RPStart + 2].MaxWidth = LINK_WIDTH_X4;
    RootComplex->Pcie[RPStart + 3].MaxWidth = LINK_WIDTH_X2;
  }

  switch (DevMap) {
  case DevMapMode2:
    MaxWidth = (RootComplex->Type == RootComplexTypeA) ? LINK_WIDTH_X8 : LINK_WIDTH_X4;
    RootComplex->Pcie[RPStart].MaxWidth = PCIE_GET_MAX_WIDTH (RootComplex->Pcie[RPStart], MaxWidth);
    RootComplex->Pcie[RPStart + 1].Active = FALSE;
    RootComplex->Pcie[RPStart + 2].MaxWidth = PCIE_GET_MAX_WIDTH (RootComplex->Pcie[RPStart + 2], MaxWidth);
    RootComplex->Pcie[RPStart + 2].Active = TRUE;
    RootComplex->Pcie[RPStart + 3].Active = FALSE;
    break;

  case DevMapMode3:
    MaxWidth = (RootComplex->Type == RootComplexTypeA) ? LINK_WIDTH_X8 : LINK_WIDTH_X4;
    RootComplex->Pcie[RPStart].MaxWidth = PCIE_GET_MAX_WIDTH (RootComplex->Pcie[RPStart], MaxWidth);
    RootComplex->Pcie[RPStart + 1].Active = FALSE;
    MaxWidth = (RootComplex->Type == RootComplexTypeA) ? LINK_WIDTH_X4 : LINK_WIDTH_X2;
    RootComplex->Pcie[RPStart + 2].MaxWidth = PCIE_GET_MAX_WIDTH (RootComplex->Pcie[RPStart + 2], MaxWidth);
    RootComplex->Pcie[RPStart + 2].Active = TRUE;
    RootComplex->Pcie[RPStart + 3].MaxWidth = PCIE_GET_MAX_WIDTH (RootComplex->Pcie[RPStart + 3], MaxWidth);
    RootComplex->Pcie[RPStart + 3].Active = TRUE;
    break;

  case DevMapMode4:
    MaxWidth = (RootComplex->Type == RootComplexTypeA) ? LINK_WIDTH_X4 : LINK_WIDTH_X2;
    RootComplex->Pcie[RPStart].MaxWidth = PCIE_GET_MAX_WIDTH (RootComplex->Pcie[RPStart], MaxWidth);
    RootComplex->Pcie[RPStart + 1].MaxWidth = PCIE_GET_MAX_WIDTH (RootComplex->Pcie[RPStart + 1], MaxWidth);
    RootComplex->Pcie[RPStart + 1].Active = TRUE;
    RootComplex->Pcie[RPStart + 2].MaxWidth = PCIE_GET_MAX_WIDTH (RootComplex->Pcie[RPStart + 2], MaxWidth);
    RootComplex->Pcie[RPStart + 2].Active = TRUE;
    RootComplex->Pcie[RPStart + 3].MaxWidth = PCIE_GET_MAX_WIDTH (RootComplex->Pcie[RPStart + 3], MaxWidth);
    RootComplex->Pcie[RPStart + 3].Active = TRUE;
    break;

  case DevMapMode1:
  default:
    MaxWidth = (RootComplex->Type == RootComplexTypeA) ? LINK_WIDTH_X16 : LINK_WIDTH_X8;
    RootComplex->Pcie[RPStart].MaxWidth = PCIE_GET_MAX_WIDTH (RootComplex->Pcie[RPStart], MaxWidth);
    RootComplex->Pcie[RPStart + 1].Active = FALSE;
    RootComplex->Pcie[RPStart + 2].Active = FALSE;
    RootComplex->Pcie[RPStart + 3].Active = FALSE;
    break;
  }
}

VOID
GetDefaultDevMap (
  AC01_ROOT_COMPLEX *RootComplex
  )
{
  if (RootComplex->Pcie[PcieController0].Active
      && RootComplex->Pcie[PcieController1].Active
      && RootComplex->Pcie[PcieController2].Active
      && RootComplex->Pcie[PcieController3].Active) {
    RootComplex->DefaultDevMapLow = DevMapMode4;
  } else if (RootComplex->Pcie[PcieController0].Active
             && RootComplex->Pcie[PcieController2].Active
             && RootComplex->Pcie[PcieController3].Active) {
    RootComplex->DefaultDevMapLow = DevMapMode3;
  } else if (RootComplex->Pcie[PcieController0].Active
             && RootComplex->Pcie[PcieController2].Active) {
    RootComplex->DefaultDevMapLow = DevMapMode2;
  } else {
    RootComplex->DefaultDevMapLow = DevMapMode1;
  }

  if (RootComplex->Pcie[PcieController4].Active
      && RootComplex->Pcie[PcieController5].Active
      && RootComplex->Pcie[PcieController6].Active
      && RootComplex->Pcie[PcieController7].Active) {
    RootComplex->DefaultDevMapHigh = DevMapMode4;
  } else if (RootComplex->Pcie[PcieController4].Active
             && RootComplex->Pcie[PcieController6].Active
             && RootComplex->Pcie[PcieController7].Active) {
    RootComplex->DefaultDevMapHigh = DevMapMode3;
  } else if (RootComplex->Pcie[PcieController4].Active
             && RootComplex->Pcie[PcieController6].Active) {
    RootComplex->DefaultDevMapHigh = DevMapMode2;
  } else {
    RootComplex->DefaultDevMapHigh = DevMapMode1;
  }

  if (RootComplex->DevMapLow == 0) {
    RootComplex->DevMapLow = RootComplex->DefaultDevMapLow;
  }

  if (RootComplex->Type == RootComplexTypeB && RootComplex->DevMapHigh == 0) {
    RootComplex->DevMapHigh = RootComplex->DefaultDevMapHigh;
  }

  SetRootComplexBifurcation (RootComplex, PcieController0, RootComplex->DevMapLow);
  if (RootComplex->Type == RootComplexTypeB) {
    SetRootComplexBifurcation (RootComplex, PcieController4, RootComplex->DevMapHigh);
  }
}

VOID
GetLaneAllocation (
  AC01_ROOT_COMPLEX *RootComplex
  )
{
  EFI_STATUS Status;
  INTN       RPIndex;
  NVPARAM    NvParamOffset;
  UINT32     Value, Width;

  // Retrieve lane allocation and capabilities for each controller
  if (RootComplex->Type == RootComplexTypeA) {
    NvParamOffset = (RootComplex->Socket == 0) ? NV_SI_RO_BOARD_S0_RCA0_CFG : NV_SI_RO_BOARD_S1_RCA0_CFG;
    NvParamOffset += RootComplex->ID * NV_PARAM_ENTRYSIZE;
  } else {
    //
    // There're two NVParam entries per RootComplexTypeB
    //
    NvParamOffset = (RootComplex->Socket == 0) ? NV_SI_RO_BOARD_S0_RCB0_LO_CFG : NV_SI_RO_BOARD_S1_RCB0_LO_CFG;
    NvParamOffset += (RootComplex->ID - MaxRootComplexA) * (NV_PARAM_ENTRYSIZE * 2);
  }

  Status = NVParamGet (NvParamOffset, NV_PERM_ALL, &Value);
  if (EFI_ERROR (Status)) {
    Value = 0;
  }

  for (RPIndex = 0; RPIndex < MaxPcieControllerOfRootComplexA; RPIndex++) {
    Width = (Value >> (RPIndex * BITS_PER_BYTE)) & BYTE_MASK;
    switch (Width) {
    case 1:
    case 2:
    case 3:
    case 4:
      RootComplex->Pcie[RPIndex].MaxWidth = 1 << Width;
      RootComplex->Pcie[RPIndex].MaxGen = LINK_SPEED_GEN3;
      RootComplex->Pcie[RPIndex].Active = TRUE;
      break;

    case 0:
    default:
      RootComplex->Pcie[RPIndex].MaxWidth = LINK_WIDTH_NONE;
      RootComplex->Pcie[RPIndex].MaxGen = LINK_SPEED_NONE;
      RootComplex->Pcie[RPIndex].Active = FALSE;
      break;
    }
  }

  if (RootComplex->Type == RootComplexTypeB) {
    NvParamOffset += NV_PARAM_ENTRYSIZE;
    Status = NVParamGet (NvParamOffset, NV_PERM_ALL, &Value);
    if (EFI_ERROR (Status)) {
      Value = 0;
    }

    for (RPIndex = MaxPcieControllerOfRootComplexA; RPIndex < MaxPcieController; RPIndex++) {
      Width = (Value >> ((RPIndex - MaxPcieControllerOfRootComplexA) * BITS_PER_BYTE)) & BYTE_MASK;
      switch (Width) {
      case 1:
      case 2:
      case 3:
      case 4:
        RootComplex->Pcie[RPIndex].MaxWidth = 1 << Width;
        RootComplex->Pcie[RPIndex].MaxGen = LINK_SPEED_GEN3;
        RootComplex->Pcie[RPIndex].Active = TRUE;
        break;

      case 0:
      default:
        RootComplex->Pcie[RPIndex].MaxWidth = LINK_WIDTH_NONE;
        RootComplex->Pcie[RPIndex].MaxGen = LINK_SPEED_NONE;
        RootComplex->Pcie[RPIndex].Active = FALSE;
        break;
      }
    }
  }

  // Do not proceed if no Root Port enabled
  if (IsEmptyRC (RootComplex)) {
    RootComplex->Active = FALSE;
  }
}

NVPARAM
GetGen3PresetNvParamOffset (
  AC01_ROOT_COMPLEX *RootComplex
  )
{
  NVPARAM NvParamOffset;

  if (RootComplex->Socket == 0) {
    if (RootComplex->Type == RootComplexTypeA) {
      if (RootComplex->ID < MaxRootComplexA) {
        NvParamOffset = NV_SI_RO_BOARD_S0_RCA0_TXRX_G3PRESET + RootComplex->ID * NV_PARAM_ENTRYSIZE;
      } else {
        NvParamOffset = NV_SI_RO_BOARD_S0_RCA4_TXRX_G3PRESET + (RootComplex->ID - MaxRootComplexA) * NV_PARAM_ENTRYSIZE;
      }
    } else {
      //
      // There're two NVParam entries per RootComplexTypeB
      //
      NvParamOffset = NV_SI_RO_BOARD_S0_RCB0A_TXRX_G3PRESET + (RootComplex->ID - MaxRootComplexA) * (NV_PARAM_ENTRYSIZE * 2);
    }
  } else if (RootComplex->Type == RootComplexTypeA) {
    if (RootComplex->ID < MaxRootComplexA) {
      NvParamOffset = NV_SI_RO_BOARD_S1_RCA2_TXRX_G3PRESET + (RootComplex->ID - 2) * NV_PARAM_ENTRYSIZE;
    } else {
      NvParamOffset = NV_SI_RO_BOARD_S1_RCA4_TXRX_G3PRESET + (RootComplex->ID - MaxRootComplexA) * NV_PARAM_ENTRYSIZE;
    }
  } else {
    //
    // There're two NVParam entries per RootComplexTypeB
    //
    NvParamOffset = NV_SI_RO_BOARD_S1_RCB0A_TXRX_G3PRESET + (RootComplex->ID - MaxRootComplexA) * (NV_PARAM_ENTRYSIZE * 2);
  }

  return NvParamOffset;
}

NVPARAM
GetGen4PresetNvParamOffset (
  AC01_ROOT_COMPLEX *RootComplex
  )
{
  NVPARAM NvParamOffset;

  if (RootComplex->Socket == 0) {
    if (RootComplex->Type == RootComplexTypeA) {
      if (RootComplex->ID < MaxRootComplexA) {
        NvParamOffset = NV_SI_RO_BOARD_S0_RCA0_TXRX_G4PRESET + RootComplex->ID * NV_PARAM_ENTRYSIZE;
      } else {
        NvParamOffset = NV_SI_RO_BOARD_S0_RCA4_TXRX_G4PRESET + (RootComplex->ID - MaxRootComplexA) * NV_PARAM_ENTRYSIZE;
      }
    } else {
      //
      // There're two NVParam entries per RootComplexTypeB
      //
      NvParamOffset = NV_SI_RO_BOARD_S0_RCB0A_TXRX_G4PRESET + (RootComplex->ID - MaxRootComplexA) * (NV_PARAM_ENTRYSIZE * 2);
    }
  } else if (RootComplex->Type == RootComplexTypeA) {
    if (RootComplex->ID < MaxRootComplexA) {
      NvParamOffset = NV_SI_RO_BOARD_S1_RCA2_TXRX_G4PRESET + (RootComplex->ID - 2) * NV_PARAM_ENTRYSIZE;
    } else {
      NvParamOffset = NV_SI_RO_BOARD_S1_RCA4_TXRX_G4PRESET + (RootComplex->ID - MaxRootComplexA) * NV_PARAM_ENTRYSIZE;
    }
  } else {
    //
    // There're two NVParam entries per RootComplexTypeB
    //
    NvParamOffset = NV_SI_RO_BOARD_S1_RCB0A_TXRX_G4PRESET + (RootComplex->ID - MaxRootComplexA) * (NV_PARAM_ENTRYSIZE * 2);
  }

  return NvParamOffset;
}

VOID
GetPresetSetting (
  AC01_ROOT_COMPLEX *RootComplex
  )
{
  EFI_STATUS  Status;
  INTN        Index;
  NVPARAM     NvParamOffset;
  UINT32      Value;

  // Load default value
  for (Index = 0; Index < MaxPcieControllerOfRootComplexB; Index++) {
    RootComplex->PresetGen3[Index] = PRESET_INVALID;
    RootComplex->PresetGen4[Index] = PRESET_INVALID;
  }

  NvParamOffset = GetGen3PresetNvParamOffset (RootComplex);

  Status = NVParamGet (NvParamOffset, NV_PERM_ALL, &Value);
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < MaxPcieControllerOfRootComplexA; Index++) {
      RootComplex->PresetGen3[Index] = (Value >> (Index * BITS_PER_BYTE)) & BYTE_MASK;
    }
  }

  if (RootComplex->Type == RootComplexTypeB) {
    NvParamOffset += NV_PARAM_ENTRYSIZE;
    Status = NVParamGet (NvParamOffset, NV_PERM_ALL, &Value);
    if (!EFI_ERROR (Status)) {
      for (Index = MaxPcieControllerOfRootComplexA; Index < MaxPcieController; Index++) {
        RootComplex->PresetGen3[Index] = (Value >> ((Index - MaxPcieControllerOfRootComplexA) * BITS_PER_BYTE)) & BYTE_MASK;
      }
    }
  }

  NvParamOffset = GetGen4PresetNvParamOffset (RootComplex);

  Status = NVParamGet (NvParamOffset, NV_PERM_ALL, &Value);
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < MaxPcieControllerOfRootComplexA; Index++) {
      RootComplex->PresetGen4[Index] = (Value >> (Index * BITS_PER_BYTE)) & BYTE_MASK;
    }
  }

  if (RootComplex->Type == RootComplexTypeB) {
    NvParamOffset += NV_PARAM_ENTRYSIZE;
    Status = NVParamGet (NvParamOffset, NV_PERM_ALL, &Value);
    if (!EFI_ERROR (Status)) {
      for (Index = MaxPcieControllerOfRootComplexA; Index < MaxPcieController; Index++) {
        RootComplex->PresetGen4[Index] = (Value >> ((Index - MaxPcieControllerOfRootComplexA) * BITS_PER_BYTE)) & BYTE_MASK;
      }
    }
  }
}

VOID
GetMaxSpeedGen (
  AC01_ROOT_COMPLEX *RootComplex
  )
{
  UINT8 MaxSpeedGen[MaxPcieControllerOfRootComplexA] = { LINK_SPEED_GEN4, LINK_SPEED_GEN4, LINK_SPEED_GEN4, LINK_SPEED_GEN4 };         // Bifurcation 0: RootComplexTypeA x16 / RootComplexTypeB x8
  UINT8 ErrataSpeedDevMap3[MaxPcieControllerOfRootComplexA] = { LINK_SPEED_GEN4, LINK_SPEED_GEN4, LINK_SPEED_GEN1, LINK_SPEED_GEN1 };  // Bifurcation 2: x8 x4 x4 (PCIE_ERRATA_SPEED1)
  UINT8 ErrataSpeedDevMap4[MaxPcieControllerOfRootComplexA] = { LINK_SPEED_GEN1, LINK_SPEED_GEN1, LINK_SPEED_GEN1, LINK_SPEED_GEN1 };  // Bifurcation 3: x4 x4 x4 x4 (PCIE_ERRATA_SPEED1)
  UINT8 ErrataSpeedRcb[MaxPcieControllerOfRootComplexA] = { LINK_SPEED_GEN1, LINK_SPEED_GEN1, LINK_SPEED_GEN1, LINK_SPEED_GEN1 };      // RootComplexTypeB PCIE_ERRATA_SPEED1
  UINT8 Idx;
  UINT8 *MaxGen;

  ASSERT (MaxPcieControllerOfRootComplexA == 4);
  ASSERT (MaxPcieController == 8);

  //
  // Due to hardware errata, for A0/A1*
  //  RootComplexTypeB is limited to Gen1 speed.
  //  RootComplexTypeA x16, x8 port supports up to Gen4,
  //  RootComplexTypeA x4 port only supports Gen1.
  //
  MaxGen = MaxSpeedGen;
  if (RootComplex->Type == RootComplexTypeB) {
    if (RootComplex->Flags & PCIE_ERRATA_SPEED1) {
      MaxGen = ErrataSpeedRcb;
    }
  } else {
    switch (RootComplex->DevMapLow) {
    case DevMapMode3: /* x8 x4 x4 */
      if (RootComplex->Flags & PCIE_ERRATA_SPEED1) {
        MaxGen = ErrataSpeedDevMap3;
      }
      break;

    case DevMapMode4: /* x4 x4 x4 x4 */
      if (RootComplex->Flags & PCIE_ERRATA_SPEED1) {
        MaxGen = ErrataSpeedDevMap4;
      }
      break;

    case DevMapMode2: /* x8 x8 */
    case DevMapMode1: /* x16 */
    default:
      break;
    }
  }

  for (Idx = 0; Idx < MaxPcieControllerOfRootComplexA; Idx++) {
    RootComplex->Pcie[Idx].MaxGen = RootComplex->Pcie[Idx].Active ? MaxGen[Idx] : LINK_SPEED_NONE;
  }

  if (RootComplex->Type == RootComplexTypeB) {
    for (Idx = MaxPcieControllerOfRootComplexA; Idx < MaxPcieController; Idx++) {
      RootComplex->Pcie[Idx].MaxGen = RootComplex->Pcie[Idx].Active ?
                                      MaxGen[Idx - MaxPcieControllerOfRootComplexA] : LINK_SPEED_NONE;
    }
  }
}

VOID
ParseRootComplexNVParamData (
  IN OUT AC01_ROOT_COMPLEX *RootComplex
  )
{
  PLATFORM_INFO_HOB  *PlatformHob;
  UINT32             EFuse;
  UINT8              RootComplexID;
  VOID               *Hob;

  EFuse = 0;
  Hob = GetFirstGuidHob (&gPlatformInfoHobGuid);
  if (Hob != NULL) {
    PlatformHob = (PLATFORM_INFO_HOB *)GET_GUID_HOB_DATA (Hob);
    EFuse = PlatformHob->RcDisableMask[0] | (PlatformHob->RcDisableMask[1] << AC01_PCIE_MAX_RCS_PER_SOCKET);
    DEBUG ((
      DEBUG_INFO,
      "RcDisableMask[0]: 0x%x [1]: 0x%x\n",
      PlatformHob->RcDisableMask[0],
      PlatformHob->RcDisableMask[1]
      ));

    // Update errata flags for Ampere Altra
    if ((PlatformHob->ScuProductId[0] & 0xff) == 0x01) {
      if (PlatformHob->AHBCId[0] == 0x20100
          || PlatformHob->AHBCId[0] == 0x21100
          || (IsSlaveSocketActive ()
              && (PlatformHob->AHBCId[1] == 0x20100
                  || PlatformHob->AHBCId[1] == 0x21100)))
      {
        RootComplex->Flags |= PCIE_ERRATA_SPEED1;
        DEBUG ((DEBUG_INFO, "RootComplex[%d]: Flags 0x%x\n", RootComplex->ID, RootComplex->Flags));
      }
    }
  }

  RootComplexID = RootComplex->Socket * AC01_PCIE_MAX_RCS_PER_SOCKET + RootComplex->ID;
  RootComplex->DefaultActive = !(EFuse & BIT (RootComplexID)) ? TRUE : FALSE;
  if (!IsSlaveSocketActive () && RootComplex->Socket == 1) {
    RootComplex->DefaultActive = FALSE;
  }
  RootComplex->Active = RootComplex->Active && RootComplex->DefaultActive;

  GetPresetSetting (RootComplex);
  GetLaneAllocation (RootComplex);
  GetDefaultDevMap (RootComplex);
  GetMaxSpeedGen (RootComplex);
}
