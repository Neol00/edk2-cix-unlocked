/** @file
  ACPI Platform Driver Hooks

  @copyright
  Copyright 1996 - 2020 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Library/UbaPlatLib.h>
#include <Library/PlatformSpecificAcpiTableLib.h>
#include <Protocol/DynamicSiLibraryProtocol.h>
#include <Protocol/DynamicSiLibraryProtocol2.h>

extern BIOS_ACPI_PARAM      *mAcpiParameter;

EFI_PLATFORM_INFO           *mPlatformInfo;
EFI_CPU_CSR_ACCESS_PROTOCOL *mCpuCsrAccess;
EFI_ACPI_TABLE_PROTOCOL     *mAcpiTable;
extern EFI_IIO_UDS_PROTOCOL *mIioUds;
CPU_CSR_ACCESS_VAR          *mCpuCsrAccessVarPtr;


UINT32                      mNumOfBitShift;
BOOLEAN                     mX2ApicEnabled;

SYSTEM_MEMORY_MAP_HOB      *mSystemMemoryMap;

SOCKET_MEMORY_CONFIGURATION   mSocketMemoryConfiguration;
SOCKET_MP_LINK_CONFIGURATION  mSocketMpLinkConfiguration;
SOCKET_IIO_CONFIGURATION      mSocketIioConfiguration;
SOCKET_POWERMANAGEMENT_CONFIGURATION mSocketPowermanagementConfiguration;
SOCKET_COMMONRC_CONFIGURATION mSocketCommonRcConfiguration;
SOCKET_PROCESSORCORE_CONFIGURATION   mSocketProcessorCoreConfiguration;

CPU_CONFIG_CONTEXT_BUFFER *mCpuConfigLibConfigContextBuffer = NULL;

EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE *mSpcrTable = NULL;

/**
  The constructor function of AcpiPlatormL Library.

  The constructor function caches the value of PCD entry

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
AcpiPlatformLibConstructor (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_HOB_GUID_TYPE             *GuidHob;

  DYNAMIC_SI_LIBARY_PROTOCOL  *DynamicSiLibraryProtocol = NULL;

  mCpuConfigLibConfigContextBuffer = (CPU_CONFIG_CONTEXT_BUFFER *) (UINTN) PcdGet64 (PcdCpuConfigContextBuffer);
  if (mCpuConfigLibConfigContextBuffer == NULL) {
    ASSERT_EFI_ERROR (RETURN_NOT_FOUND);
    return RETURN_NOT_FOUND;
  }

  Status = gBS->LocateProtocol (&gDynamicSiLibraryProtocolGuid, NULL, (VOID **) &DynamicSiLibraryProtocol);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  GuidHob    = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  if (GuidHob == NULL) {
    ASSERT (GuidHob != NULL);
    return EFI_NOT_FOUND;
  }
  mPlatformInfo = GET_GUID_HOB_DATA (GuidHob);

  //
  // Locate the IIO Protocol Interface
  //
  Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid, NULL, (VOID **) &mIioUds);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  Status = gBS->LocateProtocol (&gEfiCpuCsrAccessGuid, NULL, (VOID **) &mCpuCsrAccess);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  mCpuCsrAccessVarPtr = DynamicSiLibraryProtocol->GetSysCpuCsrAccessVar ();
  mSystemMemoryMap = DynamicSiLibraryProtocol->GetSystemMemoryMapData ();
  ASSERT (mSystemMemoryMap != NULL);

  mMpService = NULL;

  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, (VOID **) &mMpService);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  //
  // Determine the number of processors
  //
  mMpService->GetNumberOfProcessors (
    mMpService,
    &mNumberOfCPUs,
    &mNumberOfEnabledCPUs
    );
  ASSERT (mNumberOfCPUs <= MAX_CPU_NUM && mNumberOfEnabledCPUs >= 1);
  if (mNumberOfCPUs > MAX_CPU_NUM) {
    mNumberOfCPUs = MAX_CPU_NUM;
  }

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &mAcpiTable);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  return EFI_SUCCESS;
}


/**
  Platform hook to initialize Platform Specific ACPI Parameters

  @retval EFI_SUCCESS            Platform specific parameters in mAcpiParameter
                                 initialized successfully.
  @retval EFI_INVALID_PARAMETER  mAcpiParameter global was NULL.
**/
EFI_STATUS
PlatformHookAfterAcpiParamInit (
  VOID
  )
{
  EFI_STATUS Status;

  Status = EFI_SUCCESS;

  if (mAcpiParameter == NULL) {
    ASSERT (mAcpiParameter != NULL);
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_INFO, "ACPI Parameter Block Address: 0x%X\n", mAcpiParameter));


  //
  // Call for Local APIC ID Reorder
  //
  Status = SortCpuLocalApicInTable ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ACPI] ERROR: SortCpuLocalApicInTable failed: %r\n", Status));
    return Status;
  }

  return Status;
}


EFI_STATUS
AcpiPlatformHooksIsActiveTable (
  IN OUT EFI_ACPI_COMMON_HEADER     *Table
  )
{
  EFI_NFIT_TABLE_UPDATE_PROTOCOL  *NfitTableUpdateProtocol;
  EFI_ACPI_DESCRIPTION_HEADER     *TableHeader;
  EFI_STATUS                      Status;
  SETUP_DATA                      SetupData;
  SYSTEM_CONFIGURATION            SystemConfiguration;
  UINT8                           *OemSkuAcpiName;

  Status = GetEntireConfig (&SetupData);
  ASSERT_EFI_ERROR (Status);
  CopyMem (&SystemConfiguration, &(SetupData.SystemConfig), sizeof(SYSTEM_CONFIGURATION));

  if (mSystemMemoryMap == NULL) {

    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }
  TableHeader   = (EFI_ACPI_DESCRIPTION_HEADER *) Table;
  if (TableHeader->Signature == ACPI_PMTT_TABLE_SIGNATURE) {
    if (!mSystemMemoryMap->DcpmmPresent) {
      return EFI_NOT_FOUND;
    }
  }

  if (TableHeader->Signature == EFI_BDAT_TABLE_SIGNATURE) {
    if (mSocketMemoryConfiguration.bdatEn == 0) {
      return EFI_NOT_FOUND;
    }
  }

  if (TableHeader->Signature == NVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE_SIGNATURE) {
    if (mSystemMemoryMap->DcpmmPresent == 0) {
        return EFI_NOT_FOUND;
    }
  }

  if (TableHeader->Signature == NVDIMM_FW_INTERFACE_TABLE_SIGNATURE) {
    Status = gBS->LocateProtocol (&gEfiNfitTableUpdateProtocolGuid, NULL, (VOID **) &NfitTableUpdateProtocol);
    if (EFI_ERROR (Status)){
      // If NfitTableUpdateProtocol is not found we assume no NVDIMM is present - it means don't publish NFIT
      DEBUG ((DEBUG_ERROR, "NfitTableUpdateProtocol is not installed.\n"));
      return EFI_NOT_FOUND;
    }
  }
  if (TableHeader->Signature  ==  EFI_ACPI_6_2_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_SIGNATURE) {
    //
    // Initialize the SPCR table pointer.
    // SPCR table is not ready yet, update it before booting.
    //
    mSpcrTable = (EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE *) Table;
    return EFI_NOT_READY;
  }

  if ((TableHeader->Signature == EFI_ACPI_6_2_SYSTEM_LOCALITY_INFORMATION_TABLE_SIGNATURE) ||
      (TableHeader->Signature == EFI_ACPI_6_2_MAXIMUM_SYSTEM_CHARACTERISTICS_TABLE_SIGNATURE) ||
      (TableHeader->Signature == EFI_ACPI_6_2_SYSTEM_RESOURCE_AFFINITY_TABLE_SIGNATURE) ||
      TableHeader->Signature == EFI_HETEROGENEOUS_MEMORY_ATTRIBUTE_TABLE_SIGNATURE) {
    //
    // Only publish SRAT/SLIT/MSCT/HMAT if NUMA is enabled in setup.
    //
    if (!mSocketCommonRcConfiguration.NumaEn) {

      DEBUG ((DEBUG_INFO, "[ACPI] NUMA disabled, do not publish '%c%c%c%c' table\n",
              ((CHAR8*)&TableHeader->Signature)[0], ((CHAR8*)&TableHeader->Signature)[1],
              ((CHAR8*)&TableHeader->Signature)[2], ((CHAR8*)&TableHeader->Signature)[3]));
      return EFI_NOT_FOUND;
    }
  }

  if (TableHeader->Signature == EFI_ACPI_6_2_DEBUG_PORT_2_TABLE_SIGNATURE) {
      return EFI_NOT_FOUND;
  }

  if (TableHeader->Signature == EFI_ACPI_6_2_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
    OemSkuAcpiName = PcdGetPtr (PcdOemSkuAcpiName);
    if (OemSkuAcpiName == NULL) {
      return EFI_UNSUPPORTED;
    }
    if ( (0 == CompareMem (&(TableHeader->OemTableId), OemSkuAcpiName, 8)) ) {
      Status = PlatformGetAcpiFixTableDataPointer ((VOID ** ) &mAmlOffsetTablePointer);
      if (!EFI_ERROR(Status)) {
        DEBUG((DEBUG_INFO, "[ACPI] Platform DSDT Fixup table found\n"));
        DEBUG((DEBUG_INFO, "[ACPI] Platform SRP: Using %a DSDT\n", OemSkuAcpiName));
        return EFI_SUCCESS;
      } else {
        DEBUG((DEBUG_ERROR, "[ACPI] Platform DSDT Fixup table not found.\n"));
        return EFI_UNSUPPORTED;
      }
    } else {
      DEBUG ((DEBUG_INFO, "[ACPI] Platform DSDT OemSkuAcpiName '%c%c%c%c%c%c%c%c':\n",
              ((CHAR8*)OemSkuAcpiName)[0], ((CHAR8*)OemSkuAcpiName)[1],
              ((CHAR8*)OemSkuAcpiName)[2], ((CHAR8*)OemSkuAcpiName)[3],
              ((CHAR8*)OemSkuAcpiName)[4], ((CHAR8*)OemSkuAcpiName)[5],
              ((CHAR8*)OemSkuAcpiName)[6], ((CHAR8*)OemSkuAcpiName)[7]));

      DEBUG ((DEBUG_INFO, " no match for OemTableId '%c%c%c%c%c%c%c%c' from FwVol.\n",
              ((CHAR8*)&TableHeader->OemTableId)[0], ((CHAR8*)&TableHeader->OemTableId)[1],
              ((CHAR8*)&TableHeader->OemTableId)[2], ((CHAR8*)&TableHeader->OemTableId)[3],
              ((CHAR8*)&TableHeader->OemTableId)[4], ((CHAR8*)&TableHeader->OemTableId)[5],
              ((CHAR8*)&TableHeader->OemTableId)[6], ((CHAR8*)&TableHeader->OemTableId)[7]));

      return EFI_UNSUPPORTED;
    }
  }

  //
  // Hooks for checking additional platform ACPI Table is active or not.
  // If ACPI Table is not in list, it should be reported and returned EFI_SUCCESS.
  //
  return PlatformAcpiReportHooksTableIsActive (Table);
}


/**
  This function will update any runtime platform specific information.
  This currently includes:
    Setting OEM table values, ID, table ID, creator ID and creator revision.
    Enabling the proper processor entries in the APIC tables.

  @param Table  -  The table to update

  @retval EFI_SUCCESS  -  The function completed successfully.
**/
EFI_STATUS
PlatformUpdateTables (
  IN OUT EFI_ACPI_COMMON_HEADER     *Table,
  IN OUT EFI_ACPI_TABLE_VERSION     *Version
  )
{
  EFI_ACPI_DESCRIPTION_HEADER               *TableHeader;
  EFI_STATUS                                Status;
  UINT64                                    TempOemTableId;
  UINT32                                    Data32;

  DYNAMIC_SI_LIBARY_PROTOCOL2  *DynamicSiLibraryProtocol2 = NULL;

  Status = gBS->LocateProtocol (&gDynamicSiLibraryProtocol2Guid, NULL, (VOID **) &DynamicSiLibraryProtocol2);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  TableHeader             = NULL;
  Status = EFI_SUCCESS;

  //
  // By default, a table belongs in all ACPI table versions published.
  // Some tables will override this because they have different versions of the table.
  //
  *Version = EFI_ACPI_TABLE_VERSION_2_0;

  if (Table->Signature != EFI_ACPI_6_2_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE &&
      Table->Signature != EFI_ACPI_6_2_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE &&
      Table->Signature != SIGNATURE_32('N', 'F', 'I', 'T') &&
      Table->Signature != SIGNATURE_32('P', 'C', 'A', 'T') &&
      Table->Signature != SIGNATURE_32('O', 'E', 'M', '1') &&
      Table->Signature != SIGNATURE_32('O', 'E', 'M', '2') &&
      Table->Signature != SIGNATURE_32('O', 'E', 'M', '3') &&
      Table->Signature != SIGNATURE_32('O', 'E', 'M', '4'))
  {
    TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) Table;
    //
    // Update the OEMID and OEM Table ID.
    //
    TempOemTableId = PcdGet64 (PcdAcpiDefaultOemTableId);

    CopyMem (TableHeader->OemId, PcdGetPtr(PcdAcpiDefaultOemId), sizeof(TableHeader->OemId));
    CopyMem (&TableHeader->OemTableId, &TempOemTableId, sizeof(TableHeader->OemTableId));

    //
    // Update the creator ID
    //
    TableHeader->CreatorId = EFI_ACPI_CREATOR_ID;

    //
    // Update the creator revision
    //
    TableHeader->CreatorRevision = EFI_ACPI_CREATOR_REVISION;
  }
  //
  // Complete this function
  //

  //ASSERT (mMaxNumberOfCPUs <= MAX_CPU_NUM && mNumberOfEnabledCPUs >= 1);

  //
  // Assign a invalid intial value for update
  //
  //
  // Update the processors in the APIC table
  //
  DEBUG ((DEBUG_INFO, "[ACPI] Patching '%c%c%c%c' table...\n",
          ((CHAR8*)&Table->Signature)[0], ((CHAR8*)&Table->Signature)[1],
          ((CHAR8*)&Table->Signature)[2], ((CHAR8*)&Table->Signature)[3]));
  switch (Table->Signature) {

  //
  // Do not allow a prebuilt MADT, since it is built dynamically.
  //
  case EFI_ACPI_6_2_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE:
    DEBUG ((DEBUG_ERROR, "[ACPI] ERROR: Prebuilt MADT found\n"));
    Status = EFI_INVALID_PARAMETER;
    ASSERT_EFI_ERROR (Status);
    break;

  case EFI_ACPI_6_2_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE:
    Status = PatchFadtTable (Table);
    break;

  case EFI_ACPI_6_2_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
    //
    // Patch the memory resource
    //
    Status = PatchDsdtTable (Table);
    break;

  case EFI_ACPI_6_2_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
    Status = PatchSsdtTable (Table,Version);
    break;

  case EFI_ACPI_6_2_HIGH_PRECISION_EVENT_TIMER_TABLE_SIGNATURE:
    //
    // Adjust HPET Table to correct the Base Address
    // Get the address bits in RCRB that configure HPET MMIO space
    //  and create an offset to the pre-defined HEPT base address
    //
    DynamicSiLibraryProtocol2->PchHpetBaseGet (&Data32);
    //
    // Add the offset to the base address and copy into HPET DSDT table
    //
    ((EFI_ACPI_HIGH_PRECISION_EVENT_TIMER_TABLE_HEADER *) Table)->BaseAddressLower32Bit.Address = Data32;
    break;

  case EFI_ACPI_6_2_PCI_EXPRESS_MEMORY_MAPPED_CONFIGURATION_SPACE_BASE_ADDRESS_DESCRIPTION_TABLE_SIGNATURE:
    Status = PatchMcfgAcpiTable (Table);
    break;

  case EFI_ACPI_6_2_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_SIGNATURE:
    Status = PatchSpcrAcpiTable (Table);
    break;

  case ACPI_PMTT_TABLE_SIGNATURE:
    if (!PcdGetBool (PcdPlatformNotSupportAcpiTable)) {
      Status = PatchPlatformMemoryTopologyTable (Table);
    }
    break;

  case EFI_HETEROGENEOUS_MEMORY_ATTRIBUTE_TABLE_SIGNATURE:
    if (!PcdGetBool (PcdPlatformNotSupportAcpiTable)) {
      Status = PatchHmatAcpiTable (Table);
    }
    break;

  case EFI_ACPI_6_2_MAXIMUM_SYSTEM_CHARACTERISTICS_TABLE_SIGNATURE:
    if (!PcdGetBool (PcdPlatformNotSupportAcpiTable)) {
      Status = PatchMsctAcpiTable (Table);
    }
    break;

  case EFI_MIGT_ACPI_TABLE_SIGNATURE:
    if (!PcdGetBool (PcdPlatformNotSupportAcpiTable)) {
      if (PcdGetBool (ReservedB)) {
        Status = PatchMigtAcpiTable (Table);
      }
    }
    break;

  case EFI_BDAT_TABLE_SIGNATURE:
    if (!PcdGetBool (PcdPlatformNotSupportAcpiBdatTable)) {
      Status = PatchBdatAcpiTable (Table);
    }
    break;

  case NVDIMM_FW_INTERFACE_TABLE_SIGNATURE:
    Status = UpdateNfitTable (Table);
    break;

  case NVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE_SIGNATURE:
    Status = UpdatePcatTable (Table);
    break;

  //Patch Dynamic OEM SSDT table
  case OEM1_SSDT_TABLE_SIGNATURE:
    Status = PatchOem1SsdtTable (Table);  //CPU EIST
    break;

  case OEM2_SSDT_TABLE_SIGNATURE:
    Status = PatchOem2SsdtTable (Table);  //CPU HWP
    break;

  case OEM3_SSDT_TABLE_SIGNATURE:
    Status = PatchOem3SsdtTable (Table);  //CPU TST
    break;

  case OEM4_SSDT_TABLE_SIGNATURE:
    Status = PatchOem4SsdtTable (Table);  //CPU CST
    break;
  case ACPI_WSMT_SIGNATURE:
    (((ACPI_WINDOWS_SMM_SECURITY_MITIGATIONS_TABLE *)Table)->ProtectionFlags.Flags) = (UINT32 ) (WSMT_PROTECTION_FLAG & PcdGet32(PcdWsmtProtectionFlags));
  DEBUG ((DEBUG_INFO, "[WSMT] ProtectionFlags = 0x%x\n", (((ACPI_WINDOWS_SMM_SECURITY_MITIGATIONS_TABLE *)Table)->ProtectionFlags.Flags)));
      break;

  default:
    //
    // Hooks for Platform only table. If the ACPI Table is Platform only, add this table in below hook. then continue to update other Table.
    //
    Status = PatchPlatformSpecificAcpiTableHooks (Table);
    break;
  }
  //
  //
  // Update the hardware signature in the FACS structure
  //
  //
  //
  return Status;
}

/**
  Give the platform a chance to build tables.

  Some tables can be built from scratch more efficiently than being prebuilt
  and updated. This function builds any such tables for the platform.

  @retval EFI_SUCCESS   Any platform tables were successfully built.
**/
EFI_STATUS
PlatformBuildTables (
  VOID
  )
{
  EFI_STATUS ReturnStatus = EFI_SUCCESS;
  EFI_STATUS Status;

  Status = InstallMadtFromScratch ();
  if (EFI_ERROR (Status)) {
    ReturnStatus = Status;
  }

  if (mSocketCommonRcConfiguration.NumaEn) {
    Status = InstallSlitTable ();
    if (EFI_ERROR (Status)) {
      ReturnStatus = Status;
    }

    Status = InstallSratTable ();
    if (EFI_ERROR (Status)) {
      ReturnStatus = Status;
    }
  }
  //
  // Return the first error code that occured, or success.
  //
  return ReturnStatus;
}
