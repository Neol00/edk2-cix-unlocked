/** @file

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef ACPI_APEI_H_
#define ACPI_APEI_H_

#include <AcpiConfigNVDataStruct.h>
#include <Base.h>
#include <Guid/AcpiConfigHii.h>
#include <IndustryStandard/Acpi63.h>
#include <Library/AcpiLib.h>
#include <Library/AmpereCpuLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Platform/Ac01.h>
#include <Protocol/AcpiTable.h>

#pragma pack(1)
#define BERT_MSG_SIZE                0x2C
#define BERT_UEFI_FAILURE            5
#define BERT_DEFAULT_ERROR_SEVERITY  0x1
#define GENERIC_ERROR_DATA_REVISION  0x300

#define RAS_TYPE_2P                  0x03
#define RAS_TYPE_BERT                0x3F
#define RAS_TYPE_ERROR_MASK          0x3F
#define RAS_TYPE_PAYLOAD_MASK        0xC0
#define RAS_TYPE_PAYLOAD0            0x00
#define RAS_TYPE_PAYLOAD1            0x40
#define RAS_TYPE_PAYLOAD2            0x80
#define RAS_TYPE_PAYLOAD3            0xC0
#define RAS_TYPE_BERT_PAYLOAD3       (RAS_TYPE_BERT | RAS_TYPE_PAYLOAD3)

#define PLAT_CRASH_ITERATOR_SIZE     0x398
#define SMPRO_CRASH_SIZE             0x800
#define PMPRO_CRASH_SIZE             0x800
#define RASIP_CRASH_SIZE             0x1000
#define HEST_NUM_ENTRIES_PER_SOC     3

#define CURRENT_BERT_VERSION         0x11
#define BERT_FLASH_OFFSET            0x91B30000ULL
#define BERT_DDR_OFFSET              0x88230000ULL
#define BERT_DDR_LENGTH              0x50000

typedef struct {
  UINT8  Type;
  UINT8  SubType;
  UINT16 Instance;
  CHAR8  Msg[BERT_MSG_SIZE];
} APEI_BERT_ERROR_DATA;

typedef struct {
  APEI_BERT_ERROR_DATA Vendor;
  UINT8                BertRev;
  UINT8                S0PmproRegisters[PMPRO_CRASH_SIZE];
  UINT8                S0SmproRegisters[SMPRO_CRASH_SIZE];
  UINT8                S0RasIpRegisters[RASIP_CRASH_SIZE];
  UINT8                S1PmproRegisters[PMPRO_CRASH_SIZE];
  UINT8                S1SmproRegisters[SMPRO_CRASH_SIZE];
  UINT8                S1RasIpRegisters[RASIP_CRASH_SIZE];
  UINT8                AtfDump[PLATFORM_CPU_MAX_NUM_CORES * PLAT_CRASH_ITERATOR_SIZE];
} APEI_CRASH_DUMP_DATA;

typedef struct {
  EFI_ACPI_6_3_GENERIC_ERROR_STATUS_STRUCTURE     Ges;
  EFI_ACPI_6_3_GENERIC_ERROR_DATA_ENTRY_STRUCTURE Ged;
  APEI_CRASH_DUMP_DATA                            Bed;
} APEI_CRASH_DUMP_BERT_ERROR;
#pragma pack()

VOID
EFIAPI
CreateDefaultBertData (
  APEI_BERT_ERROR_DATA *Data
  );

VOID
EFIAPI
WrapBertErrorData (
  APEI_CRASH_DUMP_BERT_ERROR *WrappedError
  );

VOID
EFIAPI
PullBertSpinorData (
  APEI_CRASH_DUMP_DATA *BertErrorData
  );

VOID
EFIAPI
AdjustBERTRegionLen (
  UINT32 Len
  );

BOOLEAN
EFIAPI
IsBertEnabled (
  VOID
  );

VOID
EFIAPI
WriteDDRBertTable (
  APEI_CRASH_DUMP_BERT_ERROR *Data
  );

VOID
WriteSpinorDefaultBertTable (
  APEI_CRASH_DUMP_DATA *SpiRefrenceData
  );

EFI_STATUS
EFIAPI
AcpiApeiUpdate (
  VOID
  );

EFI_STATUS
EFIAPI
AcpiPopulateBert (
  VOID
  );

#endif /* ACPI_APEI_H_ */
