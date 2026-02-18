/** @file
  This file defines the PCH SPI2 PPI which implements the
  Intel(R) PCH SPI Host Controller Compatibility Interface.

  This SPI Protocol differs from the PCH SPI 1 Protocol interface
  primarily by identifying SPI flash regions by GUID instead
  of numeric values.

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  Copyright (c) Microsoft Corporation.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_SPI2_PPI_H_
#define _PCH_SPI2_PPI_H_

#include <Protocol/Spi2.h>

//
// Extern the GUID for PPI users.
//
extern EFI_GUID           gPchSpi2PpiGuid;

/**
  Reuse the PCH_SPI2_PROTOCOL definitions
  This is possible because the PPI implementation does not rely on a PeiService pointer,
  as it uses EDKII Glue Lib to do IO accesses
**/
typedef PCH_SPI2_PROTOCOL PCH_SPI2_PPI;

#endif
