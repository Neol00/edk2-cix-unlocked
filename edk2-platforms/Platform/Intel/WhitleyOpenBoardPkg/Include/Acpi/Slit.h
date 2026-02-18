/** @file
  This file describes the contents of the ACPI System Locality Information
  Table (SLIT).  Some additional ACPI 3.0 values are defined in Acpi3_0.h.
  All changes to the Slit contents should be done in this file.

  @copyright
  Copyright 1999 - 2019 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _SLIT_H_
#define _SLIT_H_

#include "MaxSocket.h"

//
// SLIT Definitions, see TBD specification for details.
//

#define EFI_ACPI_OEM_SLIT_REVISION  0x00000001
//
// SLIT Revision (defined in spec)
//
#define EFI_ACPI_SLIT_PMEM_NODES_SOCKET_MAX_CNT 8  // Max number of PMEM nodes per socket
#define EFI_ACPI_SLIT_NODES_SOCKET_MAX_CNT      4  // Max number of SNC nodes
#define EFI_ACPI_SLIT_DOMAINS_NODES_MAX_CNT     2  // Max number of Domins per SNC node (1LM domain and 2LM domain)

#define EFI_ACPI_SLIT_NODES_MAX_CNT \
  (MAX_SOCKET * ((EFI_ACPI_SLIT_NODES_SOCKET_MAX_CNT * EFI_ACPI_SLIT_DOMAINS_NODES_MAX_CNT) \
  + EFI_ACPI_SLIT_PMEM_NODES_SOCKET_MAX_CNT))

#define EFI_ACPI_SYSTEM_LOCALITIES_ENTRY_COUNT \
  (EFI_ACPI_SLIT_NODES_MAX_CNT * EFI_ACPI_SLIT_NODES_MAX_CNT)

#define EFI_ACPI_SLIT_PMEM_INFO_CNT \
  (MAX_SOCKET * EFI_ACPI_SLIT_PMEM_NODES_SOCKET_MAX_CNT)

#define PMEM_INVALID_SOCKET         0xFF

#define PMEM_ZERO_HOP               10
#define PMEM_ONE_ONE                17
#define PMEM_ONE_HOP                28
#define PMEM_TWO_HOP                38

#define ZERO_HOP                    10
#define ZERO_ONE                    11
#define ZERO_TWO                    12
#define ZERO_THREE                  13
#define ONE_HOP                     20
#define ONE_ONE                     21
#define ONE_TWO                     22
#define TWO_HOP                     30
#define THREE_HOP                   40
#define DISTANT_NODE_4S_EP          2
#define DISTANT_NODE_4S_EP_COD      (DISTANT_NODE_4S_EP * 2)

typedef struct {
  UINT8 Socket;
  UINT8 Pmem;
  UINT8 Valid;
} EFI_ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE_PMEM_INFO;

typedef struct {
  UINT8   Entry;
} ACPI_SYSTEM_LOCALITIES_STRUCTURE;

typedef struct {
  EFI_ACPI_6_2_SYSTEM_LOCALITY_DISTANCE_INFORMATION_TABLE_HEADER    Header;
  ACPI_SYSTEM_LOCALITIES_STRUCTURE                                  NumSlit[0];

} ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE;


#endif
