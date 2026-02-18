/** @file
  Heterogeneous Memory Attribute Table (HMAT)

  Copyright (c) 2021, ARM Limited. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <IndustryStandard/Acpi.h>
#include <Library/AcpiLib.h>
#include <Library/ArmLib.h>
#include "N1SdpAcpiHeader.h"

//
// Heterogeneous Memory Attribute Table
//
#pragma pack (1)

typedef struct {
  EFI_ACPI_6_3_HMAT_STRUCTURE_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO LatencyStruct;
  UINT32 InitiatorProximityDomainList[1];
  UINT32 TargetProximityDomainList[2];
  UINT16 LatencyEntry[1][2];
} EFI_ACPI_6_3_HMAT_SYSTEM_LOCALITY_LATENCY_STRUCTURE;

typedef struct {
  EFI_ACPI_6_3_HETEROGENEOUS_MEMORY_ATTRIBUTE_TABLE_HEADER       Header;
  EFI_ACPI_6_3_HMAT_STRUCTURE_MEMORY_PROXIMITY_DOMAIN_ATTRIBUTES Memory[2];
  EFI_ACPI_6_3_HMAT_SYSTEM_LOCALITY_LATENCY_STRUCTURE            LatencyInfo;
} EFI_ACPI_HETEROGENEOUS_MEMORY_ATTRIBUTE_TABLE;

#pragma pack ()

EFI_ACPI_HETEROGENEOUS_MEMORY_ATTRIBUTE_TABLE Hmat = {
  // Header
  {
    ARM_ACPI_HEADER (
      EFI_ACPI_6_3_HETEROGENEOUS_MEMORY_ATTRIBUTE_TABLE_SIGNATURE,
      EFI_ACPI_HETEROGENEOUS_MEMORY_ATTRIBUTE_TABLE,
      EFI_ACPI_6_3_HETEROGENEOUS_MEMORY_ATTRIBUTE_TABLE_REVISION
    ),
    {0x00, 0x00, 0x00, 0x00},
  },

  // Memory Attribute Structure
  {
    {
      EFI_ACPI_6_3_HMAT_TYPE_MEMORY_PROXIMITY_DOMAIN_ATTRIBUTES, // Type
      {0x00, 0x00}, // Reserved
      40, // Length
      {.InitiatorProximityDomainValid = 1}, // Flags
      {0x00, 0x00}, // Reserved1
      0, // InitiatorProximityDomain
      0, // MemoryProximityDomain
      { 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
      }, // Reserved2
    },
    {
      EFI_ACPI_6_3_HMAT_TYPE_MEMORY_PROXIMITY_DOMAIN_ATTRIBUTES, // Type
      {0x00, 0x00}, // Reserved
      40, // Length
      {.InitiatorProximityDomainValid = 1}, // Flags
      {0x00, 0x00}, // Reserved1
      0, // InitiatorProximityDomain
      1, // MemoryProximityDomain
      { 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
      }, // Reserved2
    },
  },

  // System Locality Latency Structure (LatencyInfo)
  {
    // LatencyStruct
    {
      EFI_ACPI_6_3_HMAT_TYPE_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO, // Type
      {0x00, 0x00}, // Reserved
      sizeof (EFI_ACPI_6_3_HMAT_SYSTEM_LOCALITY_LATENCY_STRUCTURE), // Length
      {.MemoryHierarchy = 0}, // Flags
      0, // DataType - Access latency
      {0x00, 0x00}, // Reserved1
      1, // NumberOfInitiatorProximityDomains
      2, // NumberOfTargetProximityDomains
      {0x00, 0x00, 0x00, 0x00}, // Reserved2
      1000, // EntryBaseUnit - 1000ps = 1ns
    },
    // InitiatorProximityDomainList
    { 0 },
    // TargetProximityDomainList
    { 0, 1 },
    // LatencyEntry
    {
      {119, 200},
    },
  },
};
