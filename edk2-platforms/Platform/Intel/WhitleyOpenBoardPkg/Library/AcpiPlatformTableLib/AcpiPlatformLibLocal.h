/** @file

  @copyright
  Copyright 1999 - 2020 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _ACPI_PLATFORM__LIB_LOCAL_H_
#define _ACPI_PLATFORM__LIB_LOCAL_H_

//
// Statements that include other header files
//
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/AcpiPlatformLib.h>
#include <Library/CpuConfigLib.h>
#include <Library/DevicePathLib.h>
#include <Library/SetupLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BuildAcpiTablesLib.h>

#include <Guid/PlatformInfo.h>
#include <Guid/MemoryMapData.h>
#include <Guid/GlobalVariable.h>

#include <Protocol/AcpiTable.h>
#include <Protocol/Smbios.h>
#include <Protocol/SuperIo.h>
#include <Protocol/NfitTableUpdateProtocol.h>
#include <DataTypes.h>
#include <IndustryStandard/SerialPortConsoleRedirectionTable.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>
#include <IioRegs.h>
#include <Platform.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/IioUds.h>
#include <SystemBoard.h>
#include <Protocol/SerialIo.h>
#include <Protocol/DevicePath.h>
#include <Register/ArchitecturalMsr.h>
#include <PpmPolicyPeiDxeCommon.h>
#include <Acpi/Bdat.h>
#include <Acpi/Hpet.h>
#include <Acpi/Mcfg.h>
#include <Acpi/Msct.h>
#include <Acpi/Slit.h>
#include <Acpi/Srat.h>
#include <Acpi/Migt.h>
#include <Acpi/Nfit.h>
#include <Acpi/Pcat.h>
#include <Acpi/Pmtt.h>
#include <Acpi/Hmat.h>
#include <Acpi/Wsmt.h>
#include <Acpi/amlresrc.h> // came from https://acpica.org/downloads/source_code.php acpica-win-20130214.zip/source/include
#include <IndustryStandard/AcpiAml.h>

#include <Guid/SocketMpLinkVariable.h>
#include <Guid/SocketIioVariable.h>
#include <Guid/SocketPowermanagementVariable.h>
#include <Guid/SocketCommonRcVariable.h>

#include <Guid/FpgaSocketVariable.h>
#include <Fpga.h>
#include <Register/Cpuid.h>
#include <Protocol/DynamicSiLibraryProtocol.h>
#include <Protocol/DynamicSiLibraryProtocol2.h>
#include <Upi/KtiHost.h>

#define OEM1_SSDT_TABLE_SIGNATURE  SIGNATURE_32('O', 'E', 'M', '1')
#define OEM2_SSDT_TABLE_SIGNATURE  SIGNATURE_32('O', 'E', 'M', '2')
#define OEM3_SSDT_TABLE_SIGNATURE  SIGNATURE_32('O', 'E', 'M', '3')
#define OEM4_SSDT_TABLE_SIGNATURE  SIGNATURE_32('O', 'E', 'M', '4')

#define AML_NAME_OP           0x08
#define AML_NAME_PREFIX_SIZE  0x06
#define AML_NAME_DWORD_SIZE   0x0C

#ifndef AML_EXTERNAL_OP
#define AML_EXTERNAL_OP       0x15 /* ACPI 6.0 */
#endif

#define GPSS_FVID_MAX_STATES  40  // Greater Than 16 p-state support

//
// mCpuPCPSInfo usage
//
#define B_PCPS_DISABLE                     (1 << 17) // Bit 17
#define B_PCPS_HT_ENABLE                   (1 << 16) // Bit 16
#define PCIE_BUS_0         0x00
#define PCIE_BUS_1         0x01
#define PCIE_BUS_2         0x02
#define PCIE_BUS_3         0x03
#define PCIE_BUS_4         0x04
#define PCIE_BUS_5         0x05

#define PCIE_PORT_ALL_FUNC 0x00

#define PCIE_PORT_0_DEV    0x00

#define PCIE_PORT_1A_DEV   0x00
#define PCIE_PORT_1B_DEV   0x01
#define PCIE_PORT_1C_DEV   0x02
#define PCIE_PORT_1D_DEV   0x03

#define PCIE_PORT_2A_DEV   0x00
#define PCIE_PORT_2B_DEV   0x01
#define PCIE_PORT_2C_DEV   0x02
#define PCIE_PORT_2D_DEV   0x03

#define PCIE_PORT_3A_DEV   0x00
#define PCIE_PORT_3B_DEV   0x01
#define PCIE_PORT_3C_DEV   0x02
#define PCIE_PORT_3D_DEV   0x03

#define PCIE_PORT_4A_DEV   0x00
#define PCIE_PORT_4B_DEV   0x01
#define PCIE_PORT_4C_DEV   0x02
#define PCIE_PORT_4D_DEV   0x03

#define PCIE_PORT_5_DEV   0x00

#define MAX_IO_APICS_10NM   1

//
// Define flag bits
//
#define POLARITY_ACTIVE_HIGH   0x01
#define POLARITY_ACTIVE_LOW    0x03
#define TRIGGERMODE_EDGE       0x04
#define TRIGGERMODE_LEVEL      0x0C

#pragma pack(1)
typedef struct {
  UINT32   AcpiProcessorId;
  UINT32   ApicId;
  UINT32   Flags;
  UINT32   SocketNum;
} CPU_ID_ORDER_MAP;

typedef struct {
  UINT8   AcpiProcessorId;
  UINT8   ApicId;
  UINT16  Flags;
} CPU_ID_MAP;

typedef struct {
  UINT8   StartByte;
  UINT32  NameStr;
  UINT8   Size;
  UINT32  Value;
} ACPI_NAMEPACK_DWORD;

typedef struct {
  UINT8   StartByte;
  UINT32  NameStr;
  UINT8   OpCode;
  UINT16  Size;                     // Hardcode to 16bit width because the table we use is fixed size
  UINT8   NumEntries;
} ACPI_NAME_COMMAND;

typedef struct {
  UINT8   PackageOp;
  UINT8   PkgLeadByte;
  UINT8   NumEntries;
  UINT8   DwordPrefix0;
  UINT32  CoreFreq;
  UINT8   DwordPrefix1;
  UINT32  Power;
  UINT8   DwordPrefix2;
  UINT32  TransLatency;
  UINT8   DwordPrefix3;
  UINT32  BMLatency;
  UINT8   DwordPrefix4;
  UINT32  Control;
  UINT8   DwordPrefix5;
  UINT32  Status;
} PSS_PACKAGE;

typedef struct {
  UINT8     NameOp;           // 08h ;First opcode is a NameOp.
  UINT32    PackageName;      // PSDC/PSDE
  UINT8     Length;
  UINT8     DwordPrefix1;
  UINT8     Revision;
  UINT8     PackageOp;
  UINT8     PackageLen;
  UINT8     PackLen;
  UINT16    WordValue1;
  UINT16    WordValue2;
  UINT8     BytePrefix2;
  UINT32    Domain;
  UINT8     BytePrefix3;
  UINT8     CoordType;        // 0xFC(SW_ALL), 0xFE(HW_ALL)
  UINT8     BytePrefix4;
  UINT32    NumProcessors;
} PSD_PACKAGE_LAYOUT;

struct CpcRegPack {
  UINT8    ResourceOp;           // 11h
  UINT8    Length;               // 14h
  UINT32   FFixedHW;
  UINT32   RegisterBitsUsage;
  UINT32   RegisterBitsShift;
  UINT32   RegisterOffset;
  UINT32   RegisterByteSize;
};

typedef struct {
  UINT8  ExternalOp;
  UINT8  RootChar;
  UINT8  MultiNamePrefix;
  UINT8  SegCount;
  UINT32 NameStr[0];
} EXTERNAL_OBJECT_DECL;

#define ACPI_NAME_COMMAND_FROM_NAME_STR(a)  BASE_CR (a, ACPI_NAME_COMMAND, NameStr)
#define ACPI_NAME_COMMAND_FROM_NAMEPACK_STR(a)  BASE_CR (a, ACPI_NAMEPACK_DWORD, NameStr)
#define ACPI_EXTERNAL_OBJECT_DECL_FROM_NAME_STR(a) BASE_CR (a, EXTERNAL_OBJECT_DECL, NameStr)

//
// Private Driver Data
//
//
// Define Union of IO APIC & Local APIC structure;
//
typedef union {
  EFI_ACPI_6_2_PROCESSOR_LOCAL_APIC_STRUCTURE AcpiLocalApic;
  EFI_ACPI_6_2_IO_APIC_STRUCTURE              AcpiIoApic;
  EFI_ACPI_6_2_PROCESSOR_LOCAL_X2APIC_STRUCTURE AcpiLocalx2Apic;
  struct {
    UINT8 Type;
    UINT8 Length;
  } AcpiApicCommon;
} ACPI_APIC_STRUCTURE_PTR;

#pragma pack()

EFI_STATUS
PatchBdatAcpiTable (
  IN OUT  EFI_ACPI_COMMON_HEADER  *Table
  );

/**
  Build from scratch and install the MADT.

  @retval EFI_SUCCESS           The MADT was installed successfully.
  @retval EFI_OUT_OF_RESOURCES  Could not allocate required structures.
**/
EFI_STATUS
InstallMadtFromScratch (
  VOID
  );

EFI_STATUS
InstallSratTable (
  VOID
  );

EFI_STATUS
PatchFadtTable(
   IN OUT   EFI_ACPI_COMMON_HEADER  *Table
   );

EFI_STATUS
PatchDsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  );

EFI_STATUS
PatchMcfgAcpiTable (
  IN OUT  EFI_ACPI_COMMON_HEADER  *Table
  );

EFI_STATUS
PatchSLitTable(
   IN OUT   EFI_ACPI_COMMON_HEADER  *Table
   );

EFI_STATUS
InstallSlitTable(
   VOID
   );

/**
  Finds the Proximity Domain which the element in the Memory Map belongs to.

  @param [in]  SocketId        SocketId which the element belongs to.
  @param [in]  MemType         MemType of the element.
  @param [in]  SncEnabled      Bit to indicate if SNC is enabled in the setup options.
  @param [in]  ImcInterBitmap  IMC interleave bitmap for this element.
  @param [in]  MemMode         Current memory mode. 1LM, 2LM etc.
  @param [in]  LastDomainId    Last Domain ID.

  @retval Proximity Domain.

**/
UINT32
ProximityDomainOf (
  UINT8  SocketId,
  UINT16 MemType,
  UINT8  MaxEnabledImc,
  UINT8  SncEnabled,
  UINT8  SncNumOfCluster,
  UINT8  ImcInterBitmap,
  UINT8  MemMode,
  UINT32 LastDomainId
  );

EFI_STATUS
PatchSratTable (
   IN OUT   STATIC_RESOURCE_AFFINITY_TABLE  *Table
   );

EFI_STATUS
PatchSsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table,
  IN OUT EFI_ACPI_TABLE_VERSION     *Version
  );

EFI_STATUS
PatchCpuPmSsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  );

EFI_STATUS
PatchOem1SsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  );

EFI_STATUS
PatchOem2SsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  );

EFI_STATUS
PatchOem3SsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  );

EFI_STATUS
PatchOem4SsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  );

EFI_STATUS
UpdateNfitTable (
   IN OUT   EFI_ACPI_COMMON_HEADER  *Table
   );

EFI_STATUS
UpdatePcatTable (
   IN OUT   EFI_ACPI_COMMON_HEADER  *Table
   );

/**
  Detect the APICID map and initialize the module global pointer.
**/
VOID
DetectApicIdMap (
  VOID
  );

/**
  Sort CPU Local APIC Information.

  This function gets the CPU local APIC information from the MP service
  protocol into the local table structure, and sorts it based on APIC ID.

  @retval EFI_SUCCESS   Local APIC information was successfully sorted.
**/
EFI_STATUS
SortCpuLocalApicInTable (
  VOID
  );

/**
  Get the socket logical index.

  This function convert the socket physical index to logical index (0 based).
  If the specified physical socket is not enabled, an invalid logical index 0xff
  will be returned. The socket physical index and logical index will be the same
  if the indexes of enabled sockets are continuous.

  @param[in] SocketPhysicalId    Socket physical index.

  @retval Socket logical index.
**/
UINT8
GetSocketLogicalId (
  IN UINT8  SocketPhysicalId
  );

/**
  Get the socket physical index.

  This function convert the socket logical index to physical index (0 based).
  If the specified logical socket does not exist, an invalid physical index 0xff
  will be returned. The socket physical index and logical index will be the same
  if the indexes of enabled sockets are continuous.

  @param[in] SocketLogicalId    Socket logical index.

  @retval Socket physical index.
**/
UINT8
GetSocketPhysicalId (
  IN UINT8  SocketlogicId
  );

/**
  Verifies whether sockets are linked

  @param[in]      SourceSocket  Source Socket ID
  @param[in]      TargetSocket  Targer Socket ID

  @retval TRUE link between source socket and target socket was found
          FALSE otherwise

**/
BOOLEAN
SocketsLinked (
  IN UINT32  SourceSocket,
  IN UINT32  TargetSocket
  );

//
// AcpiPlatformTableLib private share
//
extern EFI_MP_SERVICES_PROTOCOL    *mMpService;
extern BOOLEAN                     mCpuOrderSorted;
extern CPU_ID_ORDER_MAP            mCpuApicIdOrderTable[];
extern UINTN                       mNumberOfCPUs;
extern UINTN                       mNumberOfEnabledCPUs;
extern AML_OFFSET_TABLE_ENTRY      *mAmlOffsetTablePointer;
extern CPU_CONFIG_CONTEXT_BUFFER  *mCpuConfigLibConfigContextBuffer;


#endif
