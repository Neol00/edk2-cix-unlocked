#/** @file PciHostBridgeLib.c
#
#  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
#
#**/

#include <PiDxe.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/PciHostBridgeLib.h>
#include <Protocol/PciHostBridgeResourceAllocation.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Library/PlatformPcieLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ConfigParamsDataBlockLib.h>
#include <Protocol/ConfigParamsManageProtocol.h>

#define EFI_PCI_SUPPORT  (EFI_PCI_ATTRIBUTE_ISA_MOTHERBOARD_IO |  \
                           EFI_PCI_ATTRIBUTE_IDE_SECONDARY_IO | \
                           EFI_PCI_ATTRIBUTE_IDE_PRIMARY_IO | \
                           EFI_PCI_ATTRIBUTE_ISA_IO_16  | \
                           EFI_PCI_ATTRIBUTE_VGA_MEMORY | \
                           EFI_PCI_ATTRIBUTE_VGA_IO_16  | \
                           EFI_PCI_ATTRIBUTE_VGA_PALETTE_IO_16)

#define EFI_PCI_ATTRIBUTE  EFI_PCI_SUPPORT

#pragma pack(1)
typedef struct {
  ACPI_HID_DEVICE_PATH        AcpiDevicePath;
  EFI_DEVICE_PATH_PROTOCOL    EndDevicePath;
} EFI_PCI_ROOT_BRIDGE_DEVICE_PATH;
#pragma pack ()

STATIC EFI_PCI_ROOT_BRIDGE_DEVICE_PATH  mEfiPciRootBridgeDevicePath = {
  {
    {
      ACPI_DEVICE_PATH,
      ACPI_DP,
      {
        (UINT8)(sizeof (ACPI_HID_DEVICE_PATH)),
        (UINT8)((sizeof (ACPI_HID_DEVICE_PATH)) >> 8)
      }
    },
    EISA_PNP_ID (0x0A03), // PCI
    0
  },  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      END_DEVICE_PATH_LENGTH,
      0
    }
  }
};

STATIC PCI_ROOT_BRIDGE  mRootBridgeTemplate = {
  0,                                // Segment
  EFI_PCI_SUPPORT,                  // Supports
  EFI_PCI_ATTRIBUTE,                // Attributes
  TRUE,                             // DmaAbove4G
  FALSE,                            // NoExtendedConfigSpace
  FALSE,                            // ResourceAssigned
  EFI_PCI_HOST_BRIDGE_MEM64_DECODE, // AllocationAttributes
  {
    // Bus
    0,
    0
  },{
    // Io
    0,
    0,
    0
  },{
    // Mem
    MAX_UINT64,
    0,
    0
  },{
    // MemAbove4G
    MAX_UINT64,
    0,
    0
  },{
    // PMem
    MAX_UINT64,
    0,
    0
  },{
    // PMemAbove4G
    MAX_UINT64,
    0,
    0
  },
  (EFI_DEVICE_PATH_PROTOCOL *)&mEfiPciRootBridgeDevicePath
};

EFI_STATUS
ConstructRootBridge (
  PCI_ROOT_BRIDGE                    *Bridge,
  PCI_ROOT_BRIDGE_RESOURCE_APPETURE  *Appeture,
  UINT8                              RootPortIndex
  )
{
  EFI_PCI_ROOT_BRIDGE_DEVICE_PATH  *DevicePath;

  DEBUG ((DEBUG_ERROR, "Construct Root Bridge resource\n"));

  CopyMem (Bridge, &mRootBridgeTemplate, sizeof *Bridge);
  Bridge->Segment   = Appeture->Segment;
  Bridge->Bus.Base  = Appeture->BusBase;
  Bridge->Bus.Limit = Appeture->BusLimit;

  if (Appeture->IoBase) {
    Bridge->Io.Base  = Appeture->IoBase;
    Bridge->Io.Limit = Appeture->IoBase + Appeture->IoSize - 1;
  } else {
    Bridge->Io.Base  = 0;
    Bridge->Io.Limit = 0xFFFF;
  }

  if (Appeture->Mem) {
    Bridge->Mem.Base  = Appeture->Mem;
    Bridge->Mem.Limit = Appeture->Mem + Appeture->MemSize - 1;
  } else {
    Bridge->Mem.Base  = MAX_UINT64;
    Bridge->Mem.Limit = 0;
  }

  if (Appeture->MemAbove4G) {
    Bridge->MemAbove4G.Base  = Appeture->MemAbove4G;
    Bridge->MemAbove4G.Limit = Appeture->MemAbove4G + Appeture->MemAbove4GSize - 1;
  } else {
    Bridge->MemAbove4G.Base  = MAX_UINT64;
    Bridge->MemAbove4G.Limit = 0;
  }

  if (Appeture->PMem) {
    Bridge->PMem.Base  = Appeture->PMem;
    Bridge->PMem.Limit = Appeture->PMem + Appeture->PMemSize - 1;
  } else {
    Bridge->PMem.Base  = MAX_UINT64;
    Bridge->PMem.Limit = 0;
  }

  if (Appeture->PMemAbove4G) {
    Bridge->PMemAbove4G.Base  = Appeture->PMemAbove4G;
    Bridge->PMemAbove4G.Limit = Appeture->PMemAbove4G + Appeture->PMemAbove4GSize -1;
  } else {
    Bridge->PMemAbove4G.Base  = MAX_UINT64;
    Bridge->PMemAbove4G.Limit = 0;
  }

  DevicePath = AllocateCopyPool (sizeof mEfiPciRootBridgeDevicePath, &mEfiPciRootBridgeDevicePath);
  if (DevicePath == NULL) {
    DEBUG ((DEBUG_ERROR, "[%a]:[%dL] AllocatePool failed!\n", __FUNCTION__, __LINE__));
    return EFI_OUT_OF_RESOURCES;
  }

  DevicePath->AcpiDevicePath.UID = RootPortIndex;// Root Port Number
  Bridge->DevicePath             = (EFI_DEVICE_PATH_PROTOCOL *)DevicePath;
  return EFI_SUCCESS;
}

/**
  Return all the root bridge instances in an array.

  @param Count  Return the count of root bridge instances.

  @return All the root bridge instances in an array.

**/
PCI_ROOT_BRIDGE *
EFIAPI
PciHostBridgeGetRootBridges (
  OUT UINTN  *Count
  )
{
  EFI_STATUS                         Status;
  UINTN                              Loop;
  UINTN                              RootBridgeCount = 0;
  PCI_ROOT_BRIDGE                    *Bridges;
  CONFIG_PARAMS_DATA_BLOCK           *ConfigData = NULL;
  CIX_CONFIG_PARAMS_MANAGE_PROTOCOL  *ConfigManage;

  // Set default value to 0 in case we got any error.
  *Count = 0;

  RootBridgeCount = PCIE_MAX_ROOTBRIDGE;

  Bridges = AllocatePool (RootBridgeCount * sizeof *Bridges);
  if (Bridges == NULL) {
    DEBUG ((DEBUG_ERROR, "[%a:%d] - AllocatePool failed!\n", __FUNCTION__, __LINE__));
    return NULL;
  }

  Status = gBS->LocateProtocol (&gCixConfigParamsManageProtocolGuid, NULL, (VOID **)&ConfigManage);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Could not locate CIX Config Params Manage Protocol\n"));
    FreePool (Bridges);
    return NULL;
  }

  ConfigData = ConfigManage->Data;

  if (ConfigData == NULL) {
    DEBUG ((EFI_D_ERROR, "[%a]ConfigData is NULL\n", __FUNCTION__));
    FreePool (Bridges);
    return NULL;
  }

  for (Loop = 0; Loop < PCIE_MAX_ROOTBRIDGE; Loop++) {
    if (ConfigData->Pcie.PcieLinkUpStatus[Loop] == FALSE) {
      continue;
    }

    Status = ConstructRootBridge (&Bridges[*Count], &mPcieResourceAppeture[Loop], Loop);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[%a:%d] - ConstructRootBridge failed!\n", __FUNCTION__, __LINE__));
      continue;
    }

    (*Count)++;
  }

  if (*Count == 0) {
    FreePool (Bridges);
    return NULL;
  }

  return Bridges;
}

/**
  Free the root bridge instances array returned from PciHostBridgeGetRootBridges().

  @param Bridges The root bridge instances array.
  @param Count   The count of the array.
**/
VOID
EFIAPI
PciHostBridgeFreeRootBridges (
  PCI_ROOT_BRIDGE  *Bridges,
  UINTN            Count
  )
{
}

GLOBAL_REMOVE_IF_UNREFERENCED CHAR16  *mPciHostBridgeLibAcpiAddressSpaceTypeStr[] = {
  L"Mem", L"I/O", L"Bus"
};

/**
  Inform the platform that the resource conflict happens.

  @param HostBridgeHandle Handle of the Host Bridge.
  @param Configuration    Pointer to PCI I/O and PCI memory resource
                          descriptors. The Configuration contains the resources
                          for all the root bridges. The resource for each root
                          bridge is terminated with END descriptor and an
                          additional END is appended indicating the end of the
                          entire resources. The resource descriptor field
                          values follow the description in
                          EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL
                          .SubmitResources().

**/
VOID
EFIAPI
PciHostBridgeResourceConflict (
  EFI_HANDLE  HostBridgeHandle,
  VOID        *Configuration
  )
{
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR  *Descriptor;
  UINTN                              RootBridgeIndex;

  DEBUG ((DEBUG_ERROR, "PciHostBridge: Resource conflict happens!\n"));

  RootBridgeIndex = 0;
  Descriptor      = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Configuration;

  while (Descriptor->Desc == ACPI_ADDRESS_SPACE_DESCRIPTOR) {
    DEBUG ((DEBUG_ERROR, "RootBridge[%d]:\n", RootBridgeIndex++));

    for ( ; Descriptor->Desc == ACPI_ADDRESS_SPACE_DESCRIPTOR; Descriptor++) {
      ASSERT (Descriptor->ResType < sizeof (mPciHostBridgeLibAcpiAddressSpaceTypeStr) / sizeof (mPciHostBridgeLibAcpiAddressSpaceTypeStr[0]));

      DEBUG (
        (
         DEBUG_ERROR,
         " %s: Length/Alignment = 0x%lx / 0x%lx\n",
         mPciHostBridgeLibAcpiAddressSpaceTypeStr[Descriptor->ResType],
         Descriptor->AddrLen,
         Descriptor->AddrRangeMax
        )
        );

      if (Descriptor->ResType == ACPI_ADDRESS_SPACE_TYPE_MEM) {
        DEBUG (
          (DEBUG_ERROR, "     Granularity/SpecificFlag = %ld / %02x%s\n",
           Descriptor->AddrSpaceGranularity, Descriptor->SpecificFlag,
           ((Descriptor->SpecificFlag &
             EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_CACHEABLE_PREFETCHABLE
             ) != 0) ? L" (Prefetchable)" : L""
          )
          );
      }
    }

    //
    // Skip the END descriptor for root bridge
    //
    ASSERT (Descriptor->Desc == ACPI_END_TAG_DESCRIPTOR);
    Descriptor = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)(
                                                       (EFI_ACPI_END_TAG_DESCRIPTOR *)Descriptor + 1
                                                       );
  }

  return;
}
