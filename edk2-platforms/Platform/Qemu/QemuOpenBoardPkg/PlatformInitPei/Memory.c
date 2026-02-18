/** @file Memory.c
  Memory probing and installation

  Copyright (c) 2022 Theo Jehl All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PlatformInit.h>
#include <Library/DebugLib.h>
#include <Library/QemuOpenFwCfgLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/HobLib.h>
#include <IndustryStandard/E820.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>

/**
  Return the memory size below 4GB.

  @return Size of memory below 4GB, in bytes.
**/
UINT32
EFIAPI
GetMemoryBelow4Gb (
  VOID
  )
{
  EFI_E820_ENTRY64  E820Entry;
  QEMU_FW_CFG_FILE  FwCfgFile;
  UINT32            Processed;
  UINT64            Size;
  EFI_STATUS        Status;

  Status = QemuFwCfgIsPresent ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = QemuFwCfgFindFile ("etc/e820", &FwCfgFile);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Size = 0;
  QemuFwCfgSelectItem (FwCfgFile.Select);
  for (Processed = 0; Processed < FwCfgFile.Size / sizeof (EFI_E820_ENTRY); Processed++) {
    QemuFwCfgReadBytes (sizeof (EFI_E820_ENTRY), &E820Entry);
    if (E820Entry.Type != EfiAcpiAddressRangeMemory) {
      continue;
    }

    if (E820Entry.BaseAddr + E820Entry.Length < SIZE_4GB) {
      Size += E820Entry.Length;
    } else {
      ASSERT (Size == (UINT32)Size);
      return (UINT32) Size;
    }
  }

  ASSERT (Size == (UINT32)Size);
  return (UINT32) Size;
}

/**
  Reserve an MMIO region.

  @param[in] Start  Start of the MMIO region.
  @param[in] Length Length of the MMIO region.
**/
STATIC
VOID
ReserveMmioRegion (
  EFI_PHYSICAL_ADDRESS  Start,
  UINT64                Length
  )
{
  EFI_RESOURCE_TYPE            ResourceType;
  EFI_RESOURCE_ATTRIBUTE_TYPE  ResourceAttributes;

  ResourceAttributes = EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE | EFI_RESOURCE_ATTRIBUTE_TESTED;
  ResourceType       = EFI_RESOURCE_MEMORY_MAPPED_IO;

  BuildResourceDescriptorHob (
    ResourceType,
    ResourceAttributes,
    Start,
    Length
    );
}

/**
  Install EFI memory by probing QEMU FW CFG devices for valid E820 entries.
  It also reserves space for MMIO regions such as VGA, BIOS and APIC.

  @param[in] PeiServices      PEI Services pointer.

  @retval EFI_SUCCESS     Memory initialization succeded.
  @retval EFI_UNSUPPORTED Installation failed (etc/e820 file was not found).
  @retval EFI_NOT_FOUND   QEMU FW CFG device is not present.
**/
EFI_STATUS
EFIAPI
InstallMemory (
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  EFI_STATUS                   Status;
  CONST EFI_PEI_SERVICES       **PeiServicesTable;
  EFI_E820_ENTRY64             E820Entry;
  EFI_E820_ENTRY64             LargestE820Entry;
  QEMU_FW_CFG_FILE             FwCfgFile;
  UINT32                       Processed;
  BOOLEAN                      ValidMemory;
  EFI_RESOURCE_TYPE            ResourceType;
  EFI_RESOURCE_ATTRIBUTE_TYPE  ResourceAttributes;
  UINT32                       MemoryBelow4G;
  UINT32                       RequiredBySmm;

  Status = QemuFwCfgIsPresent ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "QEMU fw_cfg device is not present\n"));
    return EFI_NOT_FOUND;
  } else {
    DEBUG ((DEBUG_INFO, "QEMU fw_cfg device is present\n"));
  }

  Status = QemuFwCfgFindFile ("etc/e820", &FwCfgFile);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "etc/e820 was not found \n"));
    return EFI_UNSUPPORTED;
  }

  MemoryBelow4G = GetMemoryBelow4Gb ();

  LargestE820Entry.Length = 0;
  QemuFwCfgSelectItem (FwCfgFile.Select);
  for (Processed = 0; Processed < FwCfgFile.Size / sizeof (EFI_E820_ENTRY); Processed++) {
    QemuFwCfgReadBytes (sizeof (EFI_E820_ENTRY), &E820Entry);

    ValidMemory        = E820Entry.Type == EfiAcpiAddressRangeMemory;
    ResourceType       = EFI_RESOURCE_MEMORY_RESERVED;
    ResourceAttributes = EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE | EFI_RESOURCE_ATTRIBUTE_TESTED;

    if (ValidMemory) {
      if (FeaturePcdGet (PcdSmmSmramRequire) && (E820Entry.BaseAddr + E820Entry.Length == MemoryBelow4G)) {
        RequiredBySmm = PcdGet16 (PcdQ35TsegMbytes) * SIZE_1MB;
        if (E820Entry.Length < RequiredBySmm) {
          DEBUG ((
            DEBUG_ERROR,
            "Error: There's not enough memory below TOLUD for SMM (%lx < %x)\n",
            E820Entry.Length,
            RequiredBySmm
            ));
        }

        E820Entry.Length -= RequiredBySmm;
        DEBUG ((
          DEBUG_INFO,
          "SMM is enabled! Stealing [%lx, %lx](%u MiB) for SMRAM...\n",
          E820Entry.BaseAddr + E820Entry.Length,
          E820Entry.BaseAddr + E820Entry.Length + RequiredBySmm - 1,
          PcdGet16 (PcdQ35TsegMbytes)
          ));
      }

      ResourceType       = EFI_RESOURCE_SYSTEM_MEMORY;
      ResourceAttributes = EFI_RESOURCE_ATTRIBUTE_PRESENT |
                           EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
                           EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
                           EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
                           EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
                           EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE |
                           EFI_RESOURCE_ATTRIBUTE_TESTED;

      //
      // Lets handle the lower 1MB in a special way
      //

      if (E820Entry.BaseAddr == 0) {
        //
        // 0 - 0xa0000 is system memory, everything above that up to 1MB is not
        // Note that we check if we actually have 1MB
        //

        BuildResourceDescriptorHob (
          ResourceType,
          ResourceAttributes,
          0,
          MIN (0xa0000, E820Entry.Length)
          );

        E820Entry.BaseAddr += BASE_1MB;
        E820Entry.Length   -= MIN (BASE_1MB, E820Entry.Length);
      }

      //
      // Note that we can only check if this is the largest entry after reserving everything we have to reserve
      //

      if ((E820Entry.Length > LargestE820Entry.Length) && (E820Entry.BaseAddr + E820Entry.Length <= SIZE_4GB)) {
        CopyMem (&LargestE820Entry, &E820Entry, sizeof (EFI_E820_ENTRY64));
        DEBUG ((
          DEBUG_INFO,
          "New largest entry for PEI: BaseAddress %lx, Size %lx\n",
          LargestE820Entry.BaseAddr,
          LargestE820Entry.Length
          ));
      }
    }

    BuildResourceDescriptorHob (
      ResourceType,
      ResourceAttributes,
      E820Entry.BaseAddr,
      E820Entry.Length
      );

    DEBUG ((
      DEBUG_INFO,
      "Processed E820 entry [%lx, %lx] with type %x\n",
      E820Entry.BaseAddr,
      E820Entry.BaseAddr + E820Entry.Length - 1,
      E820Entry.Type
      ));
  }

  ASSERT (LargestE820Entry.Length != 0);
  DEBUG ((
    DEBUG_INFO,
    "Largest memory chunk found: [%lx, %lx]\n",
    LargestE820Entry.BaseAddr,
    LargestE820Entry.BaseAddr + LargestE820Entry.Length - 1
    ));

  PeiServicesTable = GetPeiServicesTablePointer ();

  Status = (*PeiServices)->InstallPeiMemory (PeiServicesTable, LargestE820Entry.BaseAddr, LargestE820Entry.Length);

  ASSERT_EFI_ERROR (Status);

  //
  //  Reserve architectural PC MMIO regions
  //  VGA space + BIOS shadow mapping
  //

  ReserveMmioRegion (0xa0000, 0x100000 - 0xa0000);

  //
  // IO APIC and LAPIC space
  //

  ReserveMmioRegion (0xfec00000, 0xff000000 - 0xfec00000);
  return Status;
}
