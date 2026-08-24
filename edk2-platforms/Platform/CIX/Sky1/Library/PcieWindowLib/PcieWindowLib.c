/** @file PcieWindowLib.c

  Runtime-selectable sizing of the PCIe 64-bit (above-4G) MMIO apertures.

  Copyright (c) 2025, Radxa Orion O6 custom firmware.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PciSegmentLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PcieWindowLib.h>
#include <Library/ConfigParamsDataBlockLib.h>
#include <Protocol/ConfigParamsManageProtocol.h>
#include <IndustryStandard/Pci.h>
#include <PlatformSetupVar.h>

//
// The five fixed layouts.  Indexed by root bridge number; bridge 0 is the x8
// slot, which is the one a discrete GPU goes in.
//
// Every layout keeps each root bridge at its stock base address and changes
// only the size of bridge 0's window.  This is not a style choice: on this SoC
// the decode base of a root complex is fixed in hardware.  Relocating bridge 0
// to 0x10_00000000 (bridge 2's stock base) was tried and fails completely --
// the card behind the x8 slot never comes up, and the resulting allocation
// failure takes the integrated graphics down with it.  Growing bridge 0's
// window upward past the top of the stock aperture does work, and is measured
// good to at least 0x27_FFFFFFFF.
//
// Nothing else claims 0x1C_00000000 upward; DRAM-high does not start until
// 0x80_00000000, so the growth room is real and the other four ports are never
// disturbed.
//
STATIC CONST PCIE_MEM64_WINDOW  mLayouts[PCIE_WINDOW_MODE_COUNT][PCIE_MAX_ROOTBRIDGE] = {
  //
  // PCIE_WINDOW_MODE_AUTO: never used as a layout, resolved before lookup.
  //
  { { 0, 0 } },

  //
  // PCIE_WINDOW_MODE_STOCK: the original CIX layout, 16 GB each.  Verified
  // working, but a 16 GB BAR exactly fills bridge 0's window and so cannot be
  // placed.
  //
  {
    { 0x1800000000ULL, 0x400000000ULL },  // RB0 x8, ends at 0x1B_FFFFFFFF
    { 0x1400000000ULL, 0x400000000ULL },  // RB1 x4
    { 0x1000000000ULL, 0x400000000ULL },  // RB2 x2
    { 0x0C00000000ULL, 0x400000000ULL },  // RB3 x1
    { 0x0800000000ULL, 0x400000000ULL }   // RB4 x1
  },

  //
  // PCIE_WINDOW_MODE_LARGE: 32 GB on the x8 slot.  The base is 32 GB aligned,
  // so a 16 GB BAR lands at 0x18_00000000 with 16 GB left over -- the smallest
  // window that houses a 16 GB-VRAM card.
  //
  {
    { 0x1800000000ULL, 0x800000000ULL },  // RB0 x8, ends at 0x1F_FFFFFFFF
    { 0x1400000000ULL, 0x400000000ULL },
    { 0x1000000000ULL, 0x400000000ULL },
    { 0x0C00000000ULL, 0x400000000ULL },
    { 0x0800000000ULL, 0x400000000ULL }
  },

  //
  // PCIE_WINDOW_MODE_HUGE: 64 GB on the x8 slot.  Verified working on hardware.
  // Houses a 32 GB BAR at 0x18_00000000 with 32 GB to spare, which covers
  // 24 GB-VRAM cards.
  //
  {
    { 0x1800000000ULL, 0x1000000000ULL }, // RB0 x8, ends at 0x27_FFFFFFFF
    { 0x1400000000ULL, 0x400000000ULL  },
    { 0x1000000000ULL, 0x400000000ULL  },
    { 0x0C00000000ULL, 0x400000000ULL  },
    { 0x0800000000ULL, 0x400000000ULL  }
  },

  //
  // PCIE_WINDOW_MODE_MAX: 128 GB on the x8 slot.  Same fixed base, but it
  // reaches 0x37_FFFFFFFF, past the highest address confirmed to decode, so it
  // stays an explicit opt-in and Auto never selects it.  A 64 GB BAR fits, at
  // the 64 GB-aligned 0x20_00000000.
  //
  {
    { 0x1800000000ULL, 0x2000000000ULL }, // RB0 x8, ends at 0x37_FFFFFFFF
    { 0x1400000000ULL, 0x400000000ULL  },
    { 0x1000000000ULL, 0x400000000ULL  },
    { 0x0C00000000ULL, 0x400000000ULL  },
    { 0x0800000000ULL, 0x400000000ULL  }
  }
};

GLOBAL_REMOVE_IF_UNREFERENCED CONST CHAR8  *mModeNames[PCIE_WINDOW_MODE_COUNT] = {
  "Auto", "Stock 16G", "Large 32G", "Huge 64G", "Max 128G"
};

/**
  Can a BarSize-aligned block of BarSize bytes be placed in this window while
  still leaving PCIE_WINDOW_HEADROOM for everything else on the port?

**/
STATIC
BOOLEAN
WindowFitsBar (
  IN CONST PCIE_MEM64_WINDOW  *Window,
  IN UINT64                   BarSize
  )
{
  UINT64  AlignedStart;
  UINT64  Leftover;

  if (BarSize == 0) {
    return TRUE;
  }

  AlignedStart = ALIGN_VALUE (Window->Base, BarSize);
  if ((AlignedStart < Window->Base) ||
      (AlignedStart - Window->Base > Window->Size) ||
      (Window->Size - (AlignedStart - Window->Base) < BarSize))
  {
    return FALSE;
  }

  //
  // Space wasted by alignment is not usable for anything large, but space past
  // the BAR is, so only the tail counts as headroom.
  //
  Leftover = Window->Size - (AlignedStart - Window->Base) - BarSize;

  return (BOOLEAN)(Leftover >= PCIE_WINDOW_HEADROOM);
}

/**
  Largest size the Resizable BAR capability of one function advertises support
  for, or 0 if the function has no such capability.

**/
STATIC
UINT64
GetFunctionMaxRebarSize (
  IN UINT8  Bus,
  IN UINT8  Device,
  IN UINT8  Function
  )
{
  UINT64  Address;
  UINT16  Offset;
  UINT32  Header;
  UINT32  Capability;
  UINT32  Control;
  UINT32  SizeMask;
  UINT8   BarCount;
  UINT8   Index;
  UINT8   Bit;
  UINT64  Largest;

  Address = PCI_SEGMENT_LIB_ADDRESS (0, Bus, Device, Function, 0);

  if (PciSegmentRead16 (Address + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
    return 0;
  }

  //
  // Walk the PCI Express extended capability list looking for Resizable BAR
  // (ID 0x0015).
  //
  Offset = 0x100;
  while ((Offset >= 0x100) && (Offset < 0x1000)) {
    Header = PciSegmentRead32 (Address + Offset);
    if ((Header == 0) || (Header == MAX_UINT32)) {
      return 0;
    }

    if ((Header & 0xFFFF) == 0x0015) {
      break;
    }

    Offset = (UINT16)((Header >> 20) & 0xFFF);
  }

  if ((Offset < 0x100) || (Offset >= 0x1000)) {
    return 0;
  }

  //
  // The BAR count lives in the first control register and covers the whole
  // capability; a value outside 1..6 means we are misreading it.
  //
  Control  = PciSegmentRead32 (Address + Offset + 8);
  BarCount = (UINT8)((Control >> 5) & 0x7);
  if ((BarCount == 0) || (BarCount > 6)) {
    return 0;
  }

  Largest = 0;
  for (Index = 0; Index < BarCount; Index++) {
    Capability = PciSegmentRead32 (Address + Offset + 4 + (Index * 8));
    Control    = PciSegmentRead32 (Address + Offset + 8 + (Index * 8));

    //
    // Capability bits 4..31 are supported sizes 1 MB << 0 .. 1 MB << 27, and
    // control bits 16..31 continue the same series at 1 MB << 28.  Anything
    // past 1 MB << 43 cannot be expressed in a UINT64 shift here and is far
    // beyond what this platform can place, so stop at the low word.
    //
    SizeMask = Capability >> 4;
    for (Bit = 0; Bit < 28; Bit++) {
      if ((SizeMask & (1u << Bit)) != 0) {
        Largest = MAX (Largest, LShiftU64 (SIZE_1MB, Bit));
      }
    }

    SizeMask = Control >> 16;
    for (Bit = 0; Bit < 16; Bit++) {
      if ((SizeMask & (1u << Bit)) != 0) {
        Largest = MAX (Largest, LShiftU64 (SIZE_1MB, 28 + Bit));
      }
    }
  }

  return Largest;
}

/**
  Largest Resizable BAR size wanted by the device behind one root port.

  PcieInitDxe has already assigned bus numbers by the time anything calls this,
  so the endpoint is reachable through ECAM at the root port's secondary bus.

**/
STATIC
UINT64
GetPortMaxRebarSize (
  IN UINT8  RootPortBus
  )
{
  UINT64  Address;
  UINT8   SecondaryBus;
  UINT8   Function;
  UINT8   HeaderType;
  UINT64  Largest;
  UINT64  Size;

  Address = PCI_SEGMENT_LIB_ADDRESS (0, RootPortBus, 0, 0, 0);

  if (PciSegmentRead16 (Address + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
    return 0;
  }

  SecondaryBus = PciSegmentRead8 (Address + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
  if ((SecondaryBus == 0) || (SecondaryBus == 0xFF)) {
    return 0;
  }

  Address = PCI_SEGMENT_LIB_ADDRESS (0, SecondaryBus, 0, 0, 0);
  if (PciSegmentRead16 (Address + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
    return 0;
  }

  //
  // A graphics card presents its display and audio functions separately and
  // only one of them carries the big BAR, so check them all.
  //
  HeaderType = PciSegmentRead8 (Address + PCI_HEADER_TYPE_OFFSET);
  Largest    = 0;

  for (Function = 0; Function < 8; Function++) {
    Size    = GetFunctionMaxRebarSize (SecondaryBus, 0, Function);
    Largest = MAX (Largest, Size);

    if ((HeaderType & HEADER_TYPE_MULTI_FUNCTION) == 0) {
      break;
    }
  }

  return Largest;
}

/**
  Read the setup selection, falling back to Auto when variable services are not
  up yet or the variable has never been written.

**/
STATIC
UINT8
GetConfiguredMode (
  VOID
  )
{
  EFI_STATUS           Status;
  UINTN                VarSize;
  PLATFORM_SETUP_DATA  SetupData;

  ZeroMem (&SetupData, sizeof (SetupData));

  VarSize = sizeof (PLATFORM_SETUP_DATA);
  Status  = gRT->GetVariable (
                   PLATFORM_SETUP_VAR,
                   &gPlatformSetupVariableGuid,
                   NULL,
                   &VarSize,
                   &SetupData
                   );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_WARN,
      "%a: %s unavailable (%r), defaulting to Auto\n",
      __FUNCTION__,
      PLATFORM_SETUP_VAR,
      Status
      ));
    return PCIE_WINDOW_MODE_AUTO;
  }

  //
  // A variable written by an earlier firmware build stops short of this field,
  // so only trust it once the stored data is known to reach that far.
  //
  if (VarSize < OFFSET_OF (PLATFORM_SETUP_DATA, PcieBarWindow) + sizeof (SetupData.PcieBarWindow)) {
    DEBUG ((DEBUG_INFO, "%a: setup variable predates this option, using Auto\n", __FUNCTION__));
    return PCIE_WINDOW_MODE_AUTO;
  }

  if (SetupData.PcieBarWindow >= PCIE_WINDOW_MODE_COUNT) {
    return PCIE_WINDOW_MODE_AUTO;
  }

  return SetupData.PcieBarWindow;
}

/**
  Pick the smallest known-safe layout that houses every port's largest
  Resizable BAR.  The experimental layouts are deliberately not reachable from
  here; a board that needs one has to be told so explicitly in setup.

**/
STATIC
UINT8
ResolveAutoMode (
  VOID
  )
{
  EFI_STATUS                         Status;
  CONFIG_PARAMS_DATA_BLOCK           *ConfigData;
  CIX_CONFIG_PARAMS_MANAGE_PROTOCOL  *ConfigManage;
  UINT64                             Wanted[PCIE_MAX_ROOTBRIDGE];
  UINTN                              Index;
  UINT8                              Mode;
  BOOLEAN                            AllFit;

  Status = gBS->LocateProtocol (&gCixConfigParamsManageProtocolGuid, NULL, (VOID **)&ConfigManage);
  if (EFI_ERROR (Status) || (ConfigManage->Data == NULL)) {
    DEBUG ((DEBUG_WARN, "%a: no link state available, staying on Stock\n", __FUNCTION__));
    return PCIE_WINDOW_MODE_STOCK;
  }

  ConfigData = ConfigManage->Data;

  for (Index = 0; Index < PCIE_MAX_ROOTBRIDGE; Index++) {
    if (ConfigData->Pcie.PcieLinkUpStatus[Index] == FALSE) {
      Wanted[Index] = 0;
      continue;
    }

    Wanted[Index] = GetPortMaxRebarSize ((UINT8)mPcieResourceAppeture[Index].BusBase);

    DEBUG ((
      DEBUG_INFO,
      "%a: RB%d largest resizable BAR 0x%lx\n",
      __FUNCTION__,
      Index,
      Wanted[Index]
      ));
  }

  //
  // Stock, Large and Huge are all confirmed good on hardware, so Auto may pick
  // any of them -- the smallest that houses every device.  Max is left out: it
  // reaches past the highest address known to decode.
  //
  for (Mode = PCIE_WINDOW_MODE_STOCK; Mode <= PCIE_WINDOW_MODE_HUGE; Mode++) {
    AllFit = TRUE;
    for (Index = 0; Index < PCIE_MAX_ROOTBRIDGE; Index++) {
      if (!WindowFitsBar (&mLayouts[Mode][Index], Wanted[Index])) {
        AllFit = FALSE;
        break;
      }
    }

    if (AllFit) {
      return Mode;
    }
  }

  DEBUG ((
    DEBUG_WARN,
    "%a: no confirmed layout houses every device; using Huge (64G). A device "
    "needing more than a 32 GB BAR requires the Max (128G) setting.\n",
    __FUNCTION__
    ));

  return PCIE_WINDOW_MODE_HUGE;
}

UINT8
EFIAPI
PcieGetMem64Windows (
  OUT PCIE_MEM64_WINDOW  *Windows
  )
{
  UINT8  Mode;
  UINTN  Index;

  ASSERT (Windows != NULL);

  Mode = GetConfiguredMode ();
  if (Mode == PCIE_WINDOW_MODE_AUTO) {
    Mode = ResolveAutoMode ();
  }

  CopyMem (Windows, mLayouts[Mode], sizeof (PCIE_MEM64_WINDOW) * PCIE_MAX_ROOTBRIDGE);

  DEBUG ((DEBUG_INFO, "%a: PCIe 64-bit window layout '%a'\n", __FUNCTION__, mModeNames[Mode]));
  for (Index = 0; Index < PCIE_MAX_ROOTBRIDGE; Index++) {
    DEBUG ((
      DEBUG_INFO,
      "  RB%d: 0x%lx..0x%lx (%ld GB)\n",
      Index,
      Windows[Index].Base,
      Windows[Index].Base + Windows[Index].Size - 1,
      RShiftU64 (Windows[Index].Size, 30)
      ));
  }

  return Mode;
}
