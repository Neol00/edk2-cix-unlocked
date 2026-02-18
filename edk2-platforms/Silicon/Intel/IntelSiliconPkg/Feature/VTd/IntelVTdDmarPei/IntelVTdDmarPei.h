/** @file
  The definition for DMA access Library.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __DMA_ACCESS_LIB_H__
#define __DMA_ACCESS_LIB_H__

#define VTD_64BITS_ADDRESS(Lo, Hi) (LShiftU64 (Lo, 12) | LShiftU64 (Hi, 32))

typedef struct {
  BOOLEAN                          Done;
  UINTN                            VtdUnitBaseAddress;
  UINT16                           Segment;
  UINT8                            Flags;
  VTD_VER_REG                      VerReg;
  VTD_CAP_REG                      CapReg;
  VTD_ECAP_REG                     ECapReg;
  BOOLEAN                          Is5LevelPaging;
  UINT8                            EnableQueuedInvalidation;
  UINT16                           QiDescLength;
  QI_DESC                          *QiDesc;
  UINT16                           QiFreeHead;
  UINTN                            FixedSecondLevelPagingEntry;
  UINTN                            RootEntryTable;
  UINTN                            ExtRootEntryTable;
  UINTN                            RootEntryTablePageSize;
  UINTN                            ExtRootEntryTablePageSize;
} VTD_UNIT_INFO;

typedef struct {
  EFI_ACPI_DMAR_HEADER             *AcpiDmarTable;
  UINT8                            HostAddressWidth;
  UINTN                            VTdEngineCount;
  VTD_UNIT_INFO                    *VtdUnitInfo;
} VTD_INFO;

typedef struct {
  UINTN                            DmaBufferBase;
  UINTN                            DmaBufferSize;
  UINTN                            DmaBufferCurrentTop;
  UINTN                            DmaBufferCurrentBottom;
} DMA_BUFFER_INFO;

typedef
VOID
(*PROCESS_DRHD_CALLBACK_FUNC) (
  IN OUT VOID                       *Context,
  IN     UINTN                      VTdIndex,
  IN     EFI_ACPI_DMAR_DRHD_HEADER  *DmarDrhd
  );

/**
  Enable VTd translation table protection for block DMA

  @param[in] VtdUnitBaseAddress The base address of the VTd engine.

  @retval EFI_SUCCESS           DMAR translation is enabled.
  @retval EFI_DEVICE_ERROR      DMAR translation is not enabled.
**/
EFI_STATUS
EnableVTdTranslationProtectionBlockDma (
  IN UINTN                      VtdUnitBaseAddress
  );

/**
  Enable VTd translation table protection.

  @param[in]  VTdInfo           The VTd engine context information.

  @retval EFI_SUCCESS           DMAR translation is enabled.
  @retval EFI_DEVICE_ERROR      DMAR translation is not enabled.
**/
EFI_STATUS
EnableVTdTranslationProtection (
  IN VTD_INFO                   *VTdInfo
  );

/**
  Disable VTd translation table protection.

  @param[in]  VTdInfo           The VTd engine context information.
**/
VOID
DisableVTdTranslationProtection (
  IN VTD_INFO                   *VTdInfo
  );

/**
  Parse DMAR DRHD table.

  @param[in]  AcpiDmarTable     DMAR ACPI table
  @param[in]  Callback          Callback function for handle DRHD
  @param[in]  Context           Callback function Context

  @return the VTd engine number.

**/
UINTN
ParseDmarAcpiTableDrhd (
  IN EFI_ACPI_DMAR_HEADER               *AcpiDmarTable,
  IN PROCESS_DRHD_CALLBACK_FUNC         Callback,
  IN VOID                               *Context
  );

/**
  Dump DMAR ACPI table.

  @param[in]  Dmar              DMAR ACPI table
**/
VOID
DumpAcpiDMAR (
  IN EFI_ACPI_DMAR_HEADER       *Dmar
  );

/**
  Prepare VTD cache invalidation configuration.

  @param[in]  VTdInfo           The VTd engine context information.

  @retval EFI_SUCCESS           Prepare Vtd config success
**/
EFI_STATUS
PrepareVtdCacheInvalidationConfig (
  IN VTD_INFO                   *VTdInfo
  );

/**
  Prepare VTD configuration.

  @param[in]  VTdInfo           The VTd engine context information.

  @retval EFI_SUCCESS           Prepare Vtd config success
**/
EFI_STATUS
PrepareVtdConfig (
  IN VTD_INFO                   *VTdInfo
  );

/**
  Setup VTd translation table.

  @param[in]  VTdInfo           The VTd engine context information.

  @retval EFI_SUCCESS           Setup translation table successfully.
  @retval EFI_OUT_OF_RESOURCE   Setup translation table fail.
**/
EFI_STATUS
SetupTranslationTable (
  IN VTD_INFO                   *VTdInfo
  );

/**
  Flush VTD page table and context table memory.

  This action is to make sure the IOMMU engine can get final data in memory.

  @param[in]  VTdUnitInfo       The VTd engine unit information.
  @param[in]  Base              The base address of memory to be flushed.
  @param[in]  Size              The size of memory in bytes to be flushed.
**/
VOID
FlushPageTableMemory (
  IN VTD_UNIT_INFO              *VTdUnitInfo,
  IN UINTN                      Base,
  IN UINTN                      Size
  );

/**
  Allocate zero pages.

  @param[in]  Pages             the number of pages.

  @return the page address.
  @retval NULL No resource to allocate pages.
**/
VOID *
EFIAPI
AllocateZeroPages (
  IN UINTN                      Pages
  );

/**
  Return the index of PCI data.

  @param[in]  VTdUnitInfo       The VTd engine unit information.
  @param[in]  Segment           The Segment used to identify a VTd engine.
  @param[in]  SourceId          The SourceId used to identify a VTd engine and table entry.

  @return The index of the PCI data.
  @retval (UINTN)-1  The PCI data is not found.
**/
UINTN
GetPciDataIndex (
  IN VTD_UNIT_INFO              *VTdUnitInfo,
  IN UINT16                     Segment,
  IN VTD_SOURCE_ID              SourceId
  );

extern EFI_GUID mVTdInfoGuid;
extern EFI_GUID mDmaBufferInfoGuid;

#endif
