/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include "AcpiSocDxe.h"
#include <PlatformSetupVar.h>
#include <AcpiRamVariable.h>
#include <Library/PcieWindowLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PL011UartClockLib.h>
#include <Library/PL011UartLib.h>
#include <Library/HwHarvestLib.h>
#include <Protocol/PlatformConfigParamsManageProtocol.h>

typedef struct {
  UINT8    SupportStatus;
  UINT8    SupportInfoValue;
} NpuCoreStatus;

NpuCoreStatus  NpuStatus[] = {
  { 3, 0 },
  { 2, 2 },
  { 1, 1 },
  { 0, 3 }
};

EFI_ACPI_TABLE_PROTOCOL         *AcpiTableProtocol = NULL;
static EFI_ACPI_SDT_PROTOCOL    *mAcpiSdt          = NULL;
static EFI_ACPI_TABLE_PROTOCOL  *mAcpiTable        = NULL;

EFI_STATUS EFIAPI UpdateAcpiGpnv (VOID);
VOID EFIAPI InitializeSystemAcpiRam (VOID);

static EFI_STATUS EFIAPI RefreshSystemAcpiRam (VOID)
{
  InitializeSystemAcpiRam ();
  return EFI_SUCCESS;
}

ACPI_FUNCTION_ON_READ_TO_BOOT_HOOK  mAcpiFunctionOReadyToBootHook[] = { InstallAcpiOnReadyToBoot, SpcrDisable, RefreshSystemAcpiRam, UpdateAcpiGpnv, AcpiInstallSratTable, NULL };

EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER  McfgHeader = {
  {
    EFI_ACPI_6_0_PCI_EXPRESS_MEMORY_MAPPED_CONFIGURATION_SPACE_BASE_ADDRESS_DESCRIPTION_TABLE_SIGNATURE,
    0,  // To be filled
    EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_SPACE_ACCESS_TABLE_REVISION,
    0x00,  // Checksum will be updated at runtime
    EFI_ACPI_OEM_ID_PCI,
    EFI_ACPI_OEM_TABLE_ID,
    EFI_ACPI_OEM_REVISION,
    EFI_ACPI_CREATOR_ID,
    EFI_ACPI_CREATOR_REVISION
  },
  0x0000000000000000,  // Reserved
};

EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE  McfgNodeTemplate = {
  .BaseAddress           = 0,
  .PciSegmentGroupNumber = 0,
  .StartBusNumber        = 0,
  .EndBusNumber          = 0,
  .Reserved              = 0,
};

// PLATFORM_ACPI_CONFIG_PROTOCOL  gPlatformAcpiConfigProtocol =
// {
//   PLATFORM_ACPI_CONFIG_PROTOCOL_SIGNATURE,
//   PLATFORM_ACPI_CONFIG_PROTOCOL_VERSION,
//   NULL,
//   0,
//   NULL,
//   NULL,
//   GetAcpiTableBySignature,
//   UpdateNameAslCode
// };

PLATFORM_ACPI_CONFIG_PROTOCOL  *pPlatformAcpiConfigProtocol = NULL;

/**
  Initialize the ASL update library state.
  This must be called at the beginning of the function calls in this library.

  @retval EFI_SUCCESS          - The function completed successfully.
**/
EFI_STATUS
InitializeAslUpdateLib (
  VOID
  )
{
  EFI_STATUS  Status;

  ///
  /// Locate ACPI tables
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiSdtProtocolGuid, NULL, (VOID **)&mAcpiSdt);
  ASSERT_EFI_ERROR (Status);
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&mAcpiTable);
  ASSERT_EFI_ERROR (Status);
  return Status;
}

/**
  This function uses the ACPI SDT protocol to locate an ACPI table.
  It is really only useful for finding tables that only have a single instance,
  e.g. FADT, FACS, MADT, etc.  It is not good for locating SSDT, etc.
  Matches are determined by finding the table with ACPI table that has
  a matching signature.

  @param[in] Signature           - Pointer to an ASCII string containing the OEM Table ID from the ACPI table header
  @param[in, out] Table          - Updated with a pointer to the table
  @param[in, out] Handle         - AcpiSupport protocol table handle for the table found
  @param[in, out] Version        - The version of the table desired

  @retval EFI_SUCCESS            - The function completed successfully.
  @retval EFI_NOT_FOUND          - Failed to locate AcpiTable.
  @retval EFI_NOT_READY          - Not ready to locate AcpiTable.
**/
EFI_STATUS
EFIAPI
GetAcpiTableBySignature (
  IN   PLATFORM_ACPI_CONFIG_PROTOCOL   *This,
  IN      UINT32                       Signature,
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER  **Table,
  IN OUT  UINTN                        *Handle
  )
{
  EFI_STATUS              Status;
  INTN                    Index;
  EFI_ACPI_TABLE_VERSION  Version;
  EFI_ACPI_SDT_HEADER     *OrgTable;

  // if (mAcpiSdt == NULL) {
  //   InitializeAslUpdateLib ();
  //   if (mAcpiSdt == NULL) {
  //     return EFI_NOT_READY;
  //   }
  // }

  ///
  /// Locate table with matching ID
  ///
  Version = 0;
  Index   = 0;
  do {
    Status = This->pAcpiSdtProtocol->GetAcpiTable (Index, &OrgTable, &Version, Handle);
    if (Status == EFI_NOT_FOUND) {
      DEBUG ((DEBUG_INFO, "GetAcpiTableBySignature: Not found %d\n", Index));
      break;
    }

    ASSERT_EFI_ERROR (Status);
    Index++;
  } while (OrgTable->Signature != Signature);

  if (Status != EFI_NOT_FOUND) {
    *Table = AllocateCopyPool (OrgTable->Length, OrgTable);
    ASSERT (*Table);
  }

  ///
  /// If we found the table, there will be no error.
  ///
  return Status;
}

VOID *
GetAcpiTableBySign (
  UINT32            Sign,
  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_ACPI_6_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Rsdp = NULL;
  EFI_ACPI_DESCRIPTION_HEADER                   *Xsdt;
  UINTN                                         Index;
  UINT64                                        *XTableAddress;
  UINTN                                         TableCount;
  EFI_ACPI_DESCRIPTION_HEADER                   *Hdr;

  for (Index = 0; Index < SystemTable->NumberOfTableEntries; Index++) {
    if (CompareGuid (&gEfiAcpiTableGuid, &(SystemTable->ConfigurationTable[Index].VendorGuid))) {
      Rsdp = (EFI_ACPI_6_0_ROOT_SYSTEM_DESCRIPTION_POINTER *)SystemTable->ConfigurationTable[Index].VendorTable;
      break;
    }
  }

  if (Rsdp == NULL) {
    DEBUG ((DEBUG_ERROR, "GetAcpiTableBySign: Failed to find RSDP\n"));
    return NULL;
  }

  DEBUG ((DEBUG_INFO, "GetAcpiTableBySign: RSDP found\n"));

  Xsdt       = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Rsdp->XsdtAddress;
  TableCount = (Xsdt->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER)) / sizeof (UINT64);
  DEBUG ((DEBUG_INFO, "TableCount: %x\n", TableCount));
  XTableAddress = (UINT64 *)(Xsdt + 1);
  for (Index = 0; Index < TableCount; Index++) {
    Hdr = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)XTableAddress[Index];
    DEBUG ((DEBUG_INFO, "Signature: %X\n", Hdr->Signature));
    if (Hdr->Signature == Sign) {
      return (VOID *)Hdr;
    }
  }

  return NULL;
}

/**
  This procedure will update immediate value assigned to a Name.

  @param[in] AslSignature      - The signature of Operation Region that we want to update.
  @param[in] Buffer            - source of data to be written over original aml
  @param[in] Length            - length of data to be overwritten

  @retval EFI_SUCCESS          - The function completed successfully.
  @retval EFI_NOT_FOUND        - Failed to locate AcpiTable.
  @retval EFI_NOT_READY        - Not ready to locate AcpiTable.
**/
EFI_STATUS
EFIAPI
UpdateNameAslCode (
  IN   PLATFORM_ACPI_CONFIG_PROTOCOL  *This,
  IN     UINT32                       AslSignature,
  IN     VOID                         *Buffer,
  IN     UINTN                        Length
  )
{
  EFI_STATUS                   Status;
  EFI_ACPI_DESCRIPTION_HEADER  *Table = NULL;
  UINT8                        *CurrPtr;
  UINT32                       *Signature;
  UINT8                        *DsdtPointer;
  UINTN                        Handle;
  UINT8                        DataSize;
  EFI_ACPI_TABLE_PROTOCOL      *pAcpiTable = NULL;

  pAcpiTable = This->pAcpiTableProtocol;

  DEBUG ((EFI_D_INFO, "UpdateNameAslCode Entry\n"));

  ///
  /// Locate table with matching ID
  ///
  Handle = 0;
  Status = This->GetAcpiTableBySignature (
                   This,
                   EFI_ACPI_6_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE,
                   (EFI_ACPI_DESCRIPTION_HEADER **)&Table,
                   &Handle
                   );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "GetAcpiTableBySignature 'DSDT' failed"));
    return EFI_NOT_FOUND;
  }

  // Table = GetAcpiTableBySign(EFI_ACPI_6_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE, gST);
  // if (Table == NULL) {
  //   DEBUG((EFI_D_ERROR, "GetAcpiTableBySignature 'DSDT' failed"));
  //   return EFI_NOT_FOUND;
  // }

  ///
  /// Point to the beginning of the DSDT table
  ///
  CurrPtr = (UINT8 *)Table;

  ///
  /// Loop through the ASL looking for values that we must fix up.
  ///
  for (DsdtPointer = CurrPtr; DsdtPointer <= (CurrPtr + ((EFI_ACPI_COMMON_HEADER *)CurrPtr)->Length); DsdtPointer++) {
    ///
    /// Get a pointer to compare for signature
    ///
    Signature = (UINT32 *)DsdtPointer;
    ///
    /// Check if this is the Device Object signature we are looking for
    ///
    if ((*Signature) == AslSignature) {
      ///
      /// Look for Name Encoding
      ///
      DEBUG ((EFI_D_INFO, "Found ASL Code Name\n"));
      if (*(DsdtPointer-1) == AML_NAME_OP) {
        ///
        /// Check if size of new and old data is the same
        ///
        DataSize = *(DsdtPointer+4);
        DEBUG ((EFI_D_INFO, "Current DSDT DataSize:%X Length:%X\n", DataSize, Length));
        if (((Length == 1) && (DataSize == 0xA)) ||
            ((Length == 2) && (DataSize == 0xB)) ||
            ((Length == 4) && (DataSize == 0xC)))
        {
          CopyMem (DsdtPointer+5, Buffer, Length);
        } else if ((Length == 1) && (((*(UINT8 *)Buffer) == 0) || ((*(UINT8 *)Buffer) == 1)) && ((DataSize == 0) || (DataSize == 1))) {
          CopyMem (DsdtPointer+4, Buffer, Length);
        } else {
          FreePool (Table);
          return EFI_BAD_BUFFER_SIZE;
        }

        DEBUG ((EFI_D_INFO, "Current Acpi Handle:%X\n", Handle));
        Status = pAcpiTable->UninstallAcpiTable (
                               pAcpiTable,
                               Handle
                               );
        Handle = 0;
        Status = pAcpiTable->InstallAcpiTable (
                               pAcpiTable,
                               Table,
                               Table->Length,
                               &Handle
                               );
        FreePool (Table);
        return Status;
      }
    }
  }

  return EFI_NOT_FOUND;
}

VOID
EFIAPI
AcpiHookFunctionOnReadyToBoot (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS  Status;
  UINT32      Index = 0;

  DEBUG ((DEBUG_INFO, "ACPI hook function on ready to boot start\n"));

  while (mAcpiFunctionOReadyToBootHook[Index] != NULL) {
    Status = mAcpiFunctionOReadyToBootHook[Index]();
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "ACPI hook function index [%d] fail on ready to boot\n", Index));
    }

    Index++;
  }

  //
  // Close the event, so it will not be signalled again.
  //
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  DEBUG ((DEBUG_INFO, "ACPI hook function on ready to boot end\n"));
}

EFI_STATUS
EFIAPI
AcpiInstallMcfgTable (
  VOID
  )
{
  EFI_STATUS               Status;
  PCI_ROOT_BRIDGE          *RootBridges    = NULL;
  UINTN                    RootBridgeCount = 0;
  UINT32                   McfgBufferSize;
  UINT8                    Index;
  UINTN                    TableHandle        = 0;
  VOID                     *McfgBuffer        = NULL;
  VOID                     *pTemp             = NULL;
  EFI_ACPI_TABLE_PROTOCOL  *AcpiTableProtocol = NULL;

  Status = gBS->LocateProtocol (
                  &gEfiAcpiTableProtocolGuid,
                  NULL,
                  (VOID **)&AcpiTableProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MCFG: Unable to locate ACPI table entry\n"));
    return Status;
  }

  RootBridges = PciHostBridgeGetRootBridges (&RootBridgeCount);
  if (RootBridgeCount == 0) {
    return EFI_SUCCESS;
  }

  McfgBufferSize = sizeof (EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER) + sizeof (EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE) * RootBridgeCount;
  McfgBuffer     = AllocateZeroPool (McfgBufferSize);
  if (McfgBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  pTemp                    = McfgBuffer;
  McfgHeader.Header.Length = McfgBufferSize;
  CopyMem (pTemp, &McfgHeader, sizeof (EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER));

  pTemp += sizeof (EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER);

  for (Index = 0; Index < RootBridgeCount; Index++) {
    McfgNodeTemplate.BaseAddress           = 0x20000000; // PCIe ECAM Base Address
    McfgNodeTemplate.PciSegmentGroupNumber = RootBridges[Index].Segment;
    McfgNodeTemplate.StartBusNumber        = RootBridges[Index].Bus.Base;
    McfgNodeTemplate.EndBusNumber          = RootBridges[Index].Bus.Limit;
    CopyMem (pTemp, &McfgNodeTemplate, sizeof (EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE));
    pTemp += sizeof (EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE);
  }

  Status = AcpiTableProtocol->InstallAcpiTable (
                                AcpiTableProtocol,
                                McfgBuffer,
                                McfgBufferSize,
                                &TableHandle
                                );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MCFG: Unable to install MCFG table entry\n"));
  }

  FreePool (McfgBuffer);
  return Status;
}

EFI_STATUS
EFIAPI
UpdateAcpiGpnv (
  VOID
  )
{
  EFI_STATUS  Status;
  UINT32      Address;
  UINT16      Length;

  Address = (UINT32)(UINTN)(pPlatformAcpiConfigProtocol->pAcpiRamAddress);
  Length  = pPlatformAcpiConfigProtocol->AcpiRamSize;
  DEBUG ((DEBUG_INFO, "System Gpnv Area Address %x Length %x\n", Address, Length));
  Status = pPlatformAcpiConfigProtocol->UpdateNameAslCode (pPlatformAcpiConfigProtocol, SIGNATURE_32 ('G', 'N', 'V', 'A'), &Address, sizeof (UINT32));
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Update DSDT GNVA failed, Status=%r\n", Status));
    return Status;
  }

  Status = pPlatformAcpiConfigProtocol->UpdateNameAslCode (pPlatformAcpiConfigProtocol, SIGNATURE_32 ('G', 'N', 'V', 'L'), &Length, sizeof (UINT16));
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Update DSDT GNVL failed, Status=%r\n", Status));
  }

  return Status;
}

VOID
EFIAPI
UpdateAcpiOnExitBootServices (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  DEBUG ((DEBUG_INFO, "Update ACPI table on exit boot services\n"));
  //
  // Close the event, so it will not be signalled again.
  //
  if (Event != NULL) {
    gBS->CloseEvent (Event);
    UpdateAcpiGpnv ();
  }
}

/** Initialise the serial port for SPCR table.

  @retval RETURN_SUCCESS            The serial device was initialised.
  @retval RETURN_INVALID_PARAMETER  One or more of the default settings
                                    has an unsupported value.
 **/
EFI_STATUS
EFIAPI
SpcrSerialPortInitialize (
  VOID
  )
{
  UINT64              BaudRate;
  UINT32              ReceiveFifoDepth;
  EFI_PARITY_TYPE     Parity;
  UINT8               DataBits;
  EFI_STOP_BITS_TYPE  StopBits;
  UINT32              PL011Base;

  BaudRate         = FixedPcdGet64 (PcdSerialDbgUartBaudRate);
  ReceiveFifoDepth = 0;         // Use default FIFO depth
  Parity           = (EFI_PARITY_TYPE)1;
  DataBits         = 8;
  StopBits         = (EFI_STOP_BITS_TYPE)1;
  PL011Base        = FixedPcdGet64 (PcdSerialDbgRegisterBase);
  return PL011UartInitializePort (
           (UINTN)PL011Base,
           PL011UartClockGetFreq (),
           &BaudRate,
           &ReceiveFifoDepth,
           &Parity,
           &DataBits,
           &StopBits
           );
}

EFI_STATUS
EFIAPI
UninstallSpcrTable (
  VOID
  )
{
  EFI_STATUS                   Status      = EFI_SUCCESS;
  EFI_ACPI_TABLE_PROTOCOL      *pAcpiTable = NULL;
  EFI_ACPI_DESCRIPTION_HEADER  *Table      = NULL;
  UINTN                        Handle      = 0;

  if (pPlatformAcpiConfigProtocol == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Not find pPlatformAcpiConfigProtocol\n", __FUNCTION__));
    return EFI_NOT_FOUND;
  }

  // Check if SPCR Table Exist
  Status = pPlatformAcpiConfigProtocol->GetAcpiTableBySignature (
                                          pPlatformAcpiConfigProtocol,
                                          EFI_ACPI_6_0_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_SIGNATURE,
                                          (EFI_ACPI_DESCRIPTION_HEADER **)&Table,
                                          &Handle
                                          );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "GetAcpiTableBySignature 'SPCR' failed"));
    return EFI_NOT_FOUND;
  }

  pAcpiTable = pPlatformAcpiConfigProtocol->pAcpiTableProtocol;
  Status     = pAcpiTable->UninstallAcpiTable (
                             pAcpiTable,
                             Handle
                             );
  return Status;
}

EFI_STATUS
EFIAPI
SpcrDisable (
  VOID
  )
{
  EFI_STATUS                                  Status;
  CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL  *PlatformConfigManage;

  Status = gBS->LocateProtocol (
                  &gCixPlatformConfigParamsManageProtocolGuid,
                  NULL,
                  (VOID **)&PlatformConfigManage
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: platform config parameters invalid %r\n", __FUNCTION__, Status));
  } else {
    if (PlatformConfigManage->Data->SpcrEnable) {
      SpcrSerialPortInitialize ();
    } else {
      // Uninstall SPCR Table
      UninstallSpcrTable ();
    }
  }

  return Status;
}

EFI_STATUS
EFIAPI
InstallAcpiOnReadyToBoot (
  VOID
  )
{
  EFI_STATUS  Status;

  DEBUG ((DEBUG_INFO, "Install acpi table on ready to boot\n"));
  Status = AcpiInstallMcfgTable ();
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Installed MCFG table\n"));
  }

  return Status;
}

VOID
EFIAPI
InitializeSystemAcpiRam (
  VOID
  )
{
  EFI_STATUS  Status;
  UINTN                              VarSize;
  PLATFORM_SETUP_DATA                PlatformSetupVar;
  UINT8                              *pSystemGpnvArea = NULL;
  UINT32                             NpuSupportStatus = 0;
  UINT32                             SupportInfoValue = 3;
  CONFIG_PARAMS_DATA_BLOCK           *ConfigData      = NULL;
  CIX_CONFIG_PARAMS_MANAGE_PROTOCOL  *ConfigManage;
  PCIE_MEM64_WINDOW                  Mem64Windows[PCIE_MAX_ROOTBRIDGE];

  if (pPlatformAcpiConfigProtocol == NULL) {
    return;
  }

  pSystemGpnvArea = (UINT8 *)pPlatformAcpiConfigProtocol->pAcpiRamAddress;

  // Read eDP support setting from BIOS setup variable
  VarSize = sizeof (PLATFORM_SETUP_DATA);
  Status  = gRT->GetVariable (
                  PLATFORM_SETUP_VAR,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &PlatformSetupVar
                  );
  if (!EFI_ERROR (Status)) {
    pSystemGpnvArea[ARV_EDP_SUPPORT_OFFSET] = PlatformSetupVar.EdpSupport;
  }

  Status = gBS->LocateProtocol (
                  &gCixConfigParamsManageProtocolGuid,
                  NULL,
                  (VOID **)&ConfigManage
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: config parameters invalid %r\n", __FUNCTION__, Status));
  } else {
    ConfigData                                          = ConfigManage->Data;
    pSystemGpnvArea[ARV_FCH_I2C_0_ENABLE_OFFSET]        = ConfigData->Fch.I2c[0].Enable;
    pSystemGpnvArea[ARV_FCH_I2C_1_ENABLE_OFFSET]        = ConfigData->Fch.I2c[1].Enable;
    pSystemGpnvArea[ARV_FCH_I2C_2_ENABLE_OFFSET]        = ConfigData->Fch.I2c[2].Enable;
    pSystemGpnvArea[ARV_FCH_I2C_3_ENABLE_OFFSET]        = ConfigData->Fch.I2c[3].Enable;
    pSystemGpnvArea[ARV_FCH_I2C_4_ENABLE_OFFSET]        = ConfigData->Fch.I2c[4].Enable;
    pSystemGpnvArea[ARV_FCH_I2C_5_ENABLE_OFFSET]        = ConfigData->Fch.I2c[5].Enable;
    pSystemGpnvArea[ARV_FCH_I2C_6_ENABLE_OFFSET]        = ConfigData->Fch.I2c[6].Enable;
    pSystemGpnvArea[ARV_FCH_I2C_7_ENABLE_OFFSET]        = ConfigData->Fch.I2c[7].Enable;
    pSystemGpnvArea[ARV_FCH_I2C_0_FREQ_OFFSET]          = (UINT8)(ConfigData->Fch.I2c[0].BusFreq / ARV_I2C_FREQ_UNIT_HZ);
    pSystemGpnvArea[ARV_FCH_I2C_1_FREQ_OFFSET]          = (UINT8)(ConfigData->Fch.I2c[1].BusFreq / ARV_I2C_FREQ_UNIT_HZ);
    pSystemGpnvArea[ARV_FCH_I2C_2_FREQ_OFFSET]          = (UINT8)(ConfigData->Fch.I2c[2].BusFreq / ARV_I2C_FREQ_UNIT_HZ);
    pSystemGpnvArea[ARV_FCH_I2C_3_FREQ_OFFSET]          = (UINT8)(ConfigData->Fch.I2c[3].BusFreq / ARV_I2C_FREQ_UNIT_HZ);
    pSystemGpnvArea[ARV_FCH_I2C_4_FREQ_OFFSET]          = (UINT8)(ConfigData->Fch.I2c[4].BusFreq / ARV_I2C_FREQ_UNIT_HZ);
    pSystemGpnvArea[ARV_FCH_I2C_5_FREQ_OFFSET]          = (UINT8)(ConfigData->Fch.I2c[5].BusFreq / ARV_I2C_FREQ_UNIT_HZ);
    pSystemGpnvArea[ARV_FCH_I2C_6_FREQ_OFFSET]          = (UINT8)(ConfigData->Fch.I2c[6].BusFreq / ARV_I2C_FREQ_UNIT_HZ);
    pSystemGpnvArea[ARV_FCH_I2C_7_FREQ_OFFSET]          = (UINT8)(ConfigData->Fch.I2c[7].BusFreq / ARV_I2C_FREQ_UNIT_HZ);
    pSystemGpnvArea[ARV_PCIE_RP_00_ENABLE_OFFSET]       = ConfigData->Pcie.PcieRpEnable[0];
    pSystemGpnvArea[ARV_PCIE_RP_01_ENABLE_OFFSET]       = ConfigData->Pcie.PcieRpEnable[1];
    pSystemGpnvArea[ARV_PCIE_RP_02_ENABLE_OFFSET]       = ConfigData->Pcie.PcieRpEnable[2];
    pSystemGpnvArea[ARV_PCIE_RP_03_ENABLE_OFFSET]       = ConfigData->Pcie.PcieRpEnable[3];
    pSystemGpnvArea[ARV_PCIE_RP_04_ENABLE_OFFSET]       = ConfigData->Pcie.PcieRpEnable[4];
    pSystemGpnvArea[ARV_PCIE_RP_00_LINK_STS_OFFSET]     = ConfigData->Pcie.PcieLinkUpStatus[0];
    pSystemGpnvArea[ARV_PCIE_RP_01_LINK_STS_OFFSET]     = ConfigData->Pcie.PcieLinkUpStatus[1];
    pSystemGpnvArea[ARV_PCIE_RP_02_LINK_STS_OFFSET]     = ConfigData->Pcie.PcieLinkUpStatus[2];
    pSystemGpnvArea[ARV_PCIE_RP_03_LINK_STS_OFFSET]     = ConfigData->Pcie.PcieLinkUpStatus[3];
    pSystemGpnvArea[ARV_PCIE_RP_04_LINK_STS_OFFSET]     = ConfigData->Pcie.PcieLinkUpStatus[4];
    pSystemGpnvArea[ARV_PCIE_RP_00_BANDWITCH_OFFSET]    = ConfigData->Pcie.PcieWidth[0];
    pSystemGpnvArea[ARV_PCIE_RP_01_BANDWITCH_OFFSET]    = ConfigData->Pcie.PcieWidth[1];
    pSystemGpnvArea[ARV_PCIE_RP_02_BANDWITCH_OFFSET]    = ConfigData->Pcie.PcieWidth[2];
    pSystemGpnvArea[ARV_PCIE_RP_03_BANDWITCH_OFFSET]    = ConfigData->Pcie.PcieWidth[3];
    pSystemGpnvArea[ARV_PCIE_RP_04_BANDWITCH_OFFSET]    = ConfigData->Pcie.PcieWidth[4];
    pSystemGpnvArea[ARV_PCIE_RP_00_MAX_SPEED_OFFSET]    = ConfigData->Pcie.PcieMaxSpeed[0];
    pSystemGpnvArea[ARV_PCIE_RP_01_MAX_SPEED_OFFSET]    = ConfigData->Pcie.PcieMaxSpeed[1];
    pSystemGpnvArea[ARV_PCIE_RP_02_MAX_SPEED_OFFSET]    = ConfigData->Pcie.PcieMaxSpeed[2];
    pSystemGpnvArea[ARV_PCIE_RP_03_MAX_SPEED_OFFSET]    = ConfigData->Pcie.PcieMaxSpeed[3];
    pSystemGpnvArea[ARV_PCIE_RP_04_MAX_SPEED_OFFSET]    = ConfigData->Pcie.PcieMaxSpeed[4];
    pSystemGpnvArea[ARV_PCIE_RP_00_MAX_PAYLOAD_OFFSET]  = ConfigData->Pcie.PcieMaxPayload[0];
    pSystemGpnvArea[ARV_PCIE_RP_01_MAX_PAYLOAD_OFFSET]  = ConfigData->Pcie.PcieMaxPayload[1];
    pSystemGpnvArea[ARV_PCIE_RP_02_MAX_PAYLOAD_OFFSET]  = ConfigData->Pcie.PcieMaxPayload[2];
    pSystemGpnvArea[ARV_PCIE_RP_03_MAX_PAYLOAD_OFFSET]  = ConfigData->Pcie.PcieMaxPayload[3];
    pSystemGpnvArea[ARV_PCIE_RP_04_MAX_PAYLOAD_OFFSET]  = ConfigData->Pcie.PcieMaxPayload[4];
    pSystemGpnvArea[ARV_PCIE_RP_00_MAX_ASPM_OFFSET]     = ConfigData->Pcie.PcieAspmMaxSupport[0];
    pSystemGpnvArea[ARV_PCIE_RP_01_MAX_ASPM_OFFSET]     = ConfigData->Pcie.PcieAspmMaxSupport[1];
    pSystemGpnvArea[ARV_PCIE_RP_02_MAX_ASPM_OFFSET]     = ConfigData->Pcie.PcieAspmMaxSupport[2];
    pSystemGpnvArea[ARV_PCIE_RP_03_MAX_ASPM_OFFSET]     = ConfigData->Pcie.PcieAspmMaxSupport[3];
    pSystemGpnvArea[ARV_PCIE_RP_04_MAX_ASPM_OFFSET]     = ConfigData->Pcie.PcieAspmMaxSupport[4];
    pSystemGpnvArea[ARV_PCIE_RP_00_ASPM_OFFSET]         = ConfigData->Pcie.PcieAspm[0];
    pSystemGpnvArea[ARV_PCIE_RP_01_ASPM_OFFSET]         = ConfigData->Pcie.PcieAspm[1];
    pSystemGpnvArea[ARV_PCIE_RP_02_ASPM_OFFSET]         = ConfigData->Pcie.PcieAspm[2];
    pSystemGpnvArea[ARV_PCIE_RP_03_ASPM_OFFSET]         = ConfigData->Pcie.PcieAspm[3];
    pSystemGpnvArea[ARV_PCIE_RP_04_ASPM_OFFSET]         = ConfigData->Pcie.PcieAspm[4];

    //
    // Publish the 64-bit MMIO windows the firmware actually programmed, so the
    // _CRS the OS reads matches them.  Linux resizes BARs against these
    // windows, so a stale _CRS here is what makes a large-BAR GPU fail.
    //
    PcieGetMem64Windows (Mem64Windows);
    for (UINTN Rb = 0; Rb < PCIE_MAX_ROOTBRIDGE; Rb++) {
      pSystemGpnvArea[ARV_PCIE_RP_00_MEM64_BASE_OFFSET + Rb] =
        (UINT8)RShiftU64 (Mem64Windows[Rb].Base, ARV_PCIE_MEM64_UNIT_SHIFT);
      pSystemGpnvArea[ARV_PCIE_RP_00_MEM64_SIZE_OFFSET + Rb] =
        (UINT8)RShiftU64 (Mem64Windows[Rb].Size, ARV_PCIE_MEM64_UNIT_SHIFT);
    }
    pSystemGpnvArea[ARV_USB3_TYPEC_DRD_ENABLE_OFFSET]   = ConfigData->UsbCDrd[0].Enable;
    pSystemGpnvArea[ARV_USB3_TYPEC_HOST0_ENABLE_OFFSET] = ConfigData->UsbC[0].Enable;
    pSystemGpnvArea[ARV_USB3_TYPEC_HOST1_ENABLE_OFFSET] = ConfigData->UsbC[1].Enable;
    pSystemGpnvArea[ARV_USB3_TYPEC_HOST2_ENABLE_OFFSET] = ConfigData->UsbC[2].Enable;
    pSystemGpnvArea[ARV_USB2_HOST0_ENABLE_OFFSET]       = ConfigData->Usb20[0].Enable;
    pSystemGpnvArea[ARV_USB2_HOST1_ENABLE_OFFSET]       = ConfigData->Usb20[1].Enable;
    pSystemGpnvArea[ARV_USB2_HOST2_ENABLE_OFFSET]       = ConfigData->Usb20[2].Enable;
    pSystemGpnvArea[ARV_USB2_HOST3_ENABLE_OFFSET]       = ConfigData->Usb20[3].Enable;
    pSystemGpnvArea[ARV_USB3_TYPEA_DRD0_ENABLE_OFFSET]  = ConfigData->Usb32Drd[0].Enable;
    pSystemGpnvArea[ARV_USB3_TYPEA_DRD1_ENABLE_OFFSET]  = ConfigData->Usb32Drd[1].Enable;
    pSystemGpnvArea[ARV_USB3_TYPEC_DRD_ROLE_OFFSET]     = ConfigData->UsbCDrd[0].DataRole;
    pSystemGpnvArea[ARV_USB3_TYPEA_DRD0_ROLE_OFFSET]    = ConfigData->Usb32Drd[0].DataRole;
    pSystemGpnvArea[ARV_USB3_TYPEA_DRD1_ROLE_OFFSET]    = ConfigData->Usb32Drd[1].DataRole;
    if (PcdGetBool (PcdI2c0Runtime)) {
      pSystemGpnvArea[ARV_FCH_I2C_0_ENABLE_OFFSET] = 0;
    }

    if (PcdGetBool (PcdI2c1Runtime)) {
      pSystemGpnvArea[ARV_FCH_I2C_1_ENABLE_OFFSET] = 0;
    }

    if (PcdGetBool (PcdI2c2Runtime)) {
      pSystemGpnvArea[ARV_FCH_I2C_2_ENABLE_OFFSET] = 0;
    }

    if (PcdGetBool (PcdI2c3Runtime)) {
      pSystemGpnvArea[ARV_FCH_I2C_3_ENABLE_OFFSET] = 0;
    }

    if (PcdGetBool (PcdI2c4Runtime)) {
      pSystemGpnvArea[ARV_FCH_I2C_4_ENABLE_OFFSET] = 0;
    }

    if (PcdGetBool (PcdI2c5Runtime)) {
      pSystemGpnvArea[ARV_FCH_I2C_5_ENABLE_OFFSET] = 0;
    }

    if (PcdGetBool (PcdI2c6Runtime)) {
      pSystemGpnvArea[ARV_FCH_I2C_6_ENABLE_OFFSET] = 0;
    }

    if (PcdGetBool (PcdI2c7Runtime)) {
      pSystemGpnvArea[ARV_FCH_I2C_7_ENABLE_OFFSET] = 0;
    }

    switch (ConfigData->Cpu.LpiState) {
      case 1:
        pSystemGpnvArea[ARV_CPU_LPI0_ENABLE_OFFSET] = 1;
        pSystemGpnvArea[ARV_CPU_LPI1_ENABLE_OFFSET] = 0;
        pSystemGpnvArea[ARV_CPU_LPI2_ENABLE_OFFSET] = 0;
        break;

      case 2:
        pSystemGpnvArea[ARV_CPU_LPI0_ENABLE_OFFSET] = 1;
        pSystemGpnvArea[ARV_CPU_LPI1_ENABLE_OFFSET] = 1;
        pSystemGpnvArea[ARV_CPU_LPI2_ENABLE_OFFSET] = 0;
        break;

      case 3:
        pSystemGpnvArea[ARV_CPU_LPI0_ENABLE_OFFSET] = 1;
        pSystemGpnvArea[ARV_CPU_LPI1_ENABLE_OFFSET] = 1;
        pSystemGpnvArea[ARV_CPU_LPI2_ENABLE_OFFSET] = 1;
        break;

      default:
        pSystemGpnvArea[ARV_CPU_LPI0_ENABLE_OFFSET] = 0;
        pSystemGpnvArea[ARV_CPU_LPI1_ENABLE_OFFSET] = 0;
        pSystemGpnvArea[ARV_CPU_LPI2_ENABLE_OFFSET] = 0;
        break;
    }
  }

  // NPU
  NpuSupportStatus = (IsIpHarvested (NpuCore0)<<1)|IsIpHarvested (NpuCore1_2);
  for (UINT32 i = 0; i < 4; i++) {
    if (NpuSupportStatus == NpuStatus[i].SupportStatus) {
      SupportInfoValue = NpuStatus[i].SupportInfoValue;
      break;
    }
  }

  pSystemGpnvArea[ARV_NPU_SUPPORT_INFO_OFFSET] = SupportInfoValue;

  pSystemGpnvArea[ARV_DPU_00_SUPPORT_OFFSET] = IsIpHarvested (DpuCore0) ? 0 : 1;
  pSystemGpnvArea[ARV_DPU_01_SUPPORT_OFFSET] = IsIpHarvested (DpuCore1) ? 0 : 1;
  pSystemGpnvArea[ARV_DPU_02_SUPPORT_OFFSET] = IsIpHarvested (DpuCore2) ? 0 : 1;
  pSystemGpnvArea[ARV_DPU_03_SUPPORT_OFFSET] = IsIpHarvested (DpuCore3) ? 0 : 1;
  pSystemGpnvArea[ARV_DPU_04_SUPPORT_OFFSET] = IsIpHarvested (DpuCore4) ? 0 : 1;
  pSystemGpnvArea[ARV_AUDIO_SUPPORT_OFFSET]  = IsIpHarvested (Audio) ? 0 : 1;
  pSystemGpnvArea[ARV_ISP_SUPPORT_OFFSET]    = IsIpHarvested (Isp) ? 0 : 1;

  // eDP panel support: disabled by default (no panel connected on most boards)
  pSystemGpnvArea[ARV_EDP_SUPPORT_OFFSET]    = 0;

  return;
}

EFI_STATUS
EFIAPI
UpdateAcpiDsdt (
  VOID
  )
{
  EFI_ACPI_TABLE_PROTOCOL  *AcpiTableProtocol;
  EFI_STATUS               Status;

  DEBUG ((DEBUG_INFO, "Update acpi DSDT table on ready to boot\n"));

  Status = gBS->LocateProtocol (
                  &gEfiAcpiTableProtocolGuid,
                  NULL,
                  (VOID **)&AcpiTableProtocol
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, " Unable to locate ACPI table protocol\n"));
    return Status;
  }

  Status = UpdateAcpiDsdtTable ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, " UpdateAcpiDsdtTable Failed, Status = %r\n", Status));
  }

  return Status;
}

EFI_STATUS
EFIAPI
AcpiSocDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS            Status;
  UINTN                 Pages;
  EFI_PHYSICAL_ADDRESS  Address;
  EFI_EVENT             ReadyToBootEvent;
  EFI_EVENT             ExitBootServicesEvent;

  // VOID                     *Registration;
  // EFI_ACPI_TABLE_PROTOCOL  *AcpiTableProtocol = NULL;

  DEBUG ((DEBUG_INFO, "Acpi Soc Dxe Entry\n"));
  pPlatformAcpiConfigProtocol = AllocateZeroPool (sizeof (PLATFORM_ACPI_CONFIG_PROTOCOL));
  if (pPlatformAcpiConfigProtocol == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to allocate memory for PlatformAcpiConfigProtocol\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  pPlatformAcpiConfigProtocol->Signature = PLATFORM_ACPI_CONFIG_PROTOCOL_SIGNATURE;
  pPlatformAcpiConfigProtocol->Revision  = PLATFORM_ACPI_CONFIG_PROTOCOL_REVISION;

  ///
  /// Locate ACPI tables
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiSdtProtocolGuid, NULL, (VOID **)&pPlatformAcpiConfigProtocol->pAcpiSdtProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Locate Acpi Sdt Protocol: %r \n", Status));
    return Status;
  }

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&pPlatformAcpiConfigProtocol->pAcpiTableProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to Locate Acpi table Protocol: %r \n", Status));
    return Status;
  }

  Pages   = EFI_SIZE_TO_PAGES (ARV_VARIABLE_TABLE_TOTAL_SIZE);
  Address = 0xffffffff; // allocate address below 4G.

  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiACPIMemoryNVS,
                  Pages,
                  &Address
                  );
  ASSERT_EFI_ERROR (Status);

  pPlatformAcpiConfigProtocol->pAcpiRamAddress = (UINT32 *)Address;
  pPlatformAcpiConfigProtocol->AcpiRamSize     = ARV_VARIABLE_TABLE_TOTAL_SIZE;
  SetMem (pPlatformAcpiConfigProtocol->pAcpiRamAddress, pPlatformAcpiConfigProtocol->AcpiRamSize, 0);

  pPlatformAcpiConfigProtocol->GetAcpiTableBySignature = GetAcpiTableBySignature;
  pPlatformAcpiConfigProtocol->UpdateNameAslCode       = UpdateNameAslCode;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gPlatformAcpiConfigProtocolGuid,
                  pPlatformAcpiConfigProtocol,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to install platform acpi config protocol: %r \n", Status));
    return Status;
  }

  Status = gBS->LocateProtocol (&gPlatformAcpiConfigProtocolGuid, NULL, (VOID **)&pPlatformAcpiConfigProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to locate platform acpi config protocol: %r \n", Status));
    return Status;
  }

  InitializeSystemAcpiRam ();

  Status = LocateAndInstallAcpiFromFv (&gEfiAcpiTableStorageGuid);
  DEBUG ((DEBUG_INFO, "Install Acpi table status:%r\n", Status));

  Status = gBS->CreateEvent (
                  EVT_SIGNAL_EXIT_BOOT_SERVICES,
                  TPL_CALLBACK,
                  UpdateAcpiOnExitBootServices,
                  NULL,
                  &ExitBootServicesEvent
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  AcpiHookFunctionOnReadyToBoot,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &ReadyToBootEvent
                  );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "Acpi Soc Dxe Exit\n"));

  return EFI_SUCCESS;
}
