/** @file
  SSDT Cpu Topology Table Generator.

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
  Copyright (c) 2021, Arm Limited. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Reference(s):
    - ACPI 6.3 Specification - January 2019 - s8.4 Declaring Processors
**/

#ifndef SSDT_CPU_TOPOLOGY_GENERATOR_H_
#define SSDT_CPU_TOPOLOGY_GENERATOR_H_

#include <AcpiPlatform.h>
#include <AcpiNameSpaceObjects.h>

#pragma pack(1)

// Mask for the flags that need to be checked.
#define PPTT_PROCESSOR_MASK  (                                                \
          (EFI_ACPI_6_3_PPTT_PACKAGE_PHYSICAL)          |                     \
          (EFI_ACPI_6_3_PPTT_PROCESSOR_ID_VALID << 1)   |                     \
          (EFI_ACPI_6_3_PPTT_NODE_IS_LEAF << 3))

// Mask for the cpu flags.
#define PPTT_CPU_PROCESSOR_MASK  (                                            \
          (EFI_ACPI_6_3_PPTT_PACKAGE_NOT_PHYSICAL)      |                     \
          (EFI_ACPI_6_3_PPTT_PROCESSOR_ID_VALID << 1)   |                     \
          (EFI_ACPI_6_3_PPTT_NODE_IS_LEAF << 3))

// Mask for the cluster flags.
// Even though a _UID is generated for clusters, it is simpler to use
// EFI_ACPI_6_3_PPTT_PROCESSOR_ID_INVALID and to not match the cluster id of
// the PPTT table (not sure the PPTT table is generated).
#define PPTT_CLUSTER_PROCESSOR_MASK  (                                        \
          (EFI_ACPI_6_3_PPTT_PACKAGE_NOT_PHYSICAL)      |                     \
          (EFI_ACPI_6_3_PPTT_PROCESSOR_ID_INVALID << 1) |                     \
          (EFI_ACPI_6_3_PPTT_NODE_IS_NOT_LEAF << 3))

/** LPI states are stored in the ASL namespace at '\_SB_.Lxxx',
    with xxx being the node index of the LPI state.
*/
#define SB_SCOPE         "\\_SB_"
#define SB_SCOPE_PREFIX  SB_SCOPE "."
/// Size of the SB_SCOPE_PREFIX string.
#define SB_SCOPE_PREFIX_SIZE  sizeof (SB_SCOPE_PREFIX)

/// HID for a processor device.
#define ACPI_HID_PROCESSOR_DEVICE  "ACPI0007"

/// HID for a processor container device.
#define ACPI_HID_PROCESSOR_CONTAINER_DEVICE  "ACPI0010"

/** Node names of Cpus and Clusters are 'Cxxx', and 'Lxxx' for LPI states.
    The 'xxx' is an index on 12 bits is given to node name,
    thus the limitation in the number of nodes.
*/
#define MAX_NODE_COUNT  (1 << 12)

#pragma pack()

#endif // SSDT_CPU_TOPOLOGY_GENERATOR_H_
