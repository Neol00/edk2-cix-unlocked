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


#define CPM_MMIO_SIZE           0x100000000         // 4G MMIO resource for CPM
#define HQM_MMIO_SIZE           0x400000000         // 16G MMIO resource for HQM

extern BIOS_ACPI_PARAM             *mAcpiParameter;
extern struct SystemMemoryMapHob   *mSystemMemoryMap;
extern EFI_IIO_UDS_PROTOCOL        *mIioUds;
extern CPU_CSR_ACCESS_VAR          *mCpuCsrAccessVarPtr;

extern SOCKET_MP_LINK_CONFIGURATION  mSocketMpLinkConfiguration;
extern SOCKET_IIO_CONFIGURATION     mSocketIioConfiguration;
extern SOCKET_POWERMANAGEMENT_CONFIGURATION mSocketPowermanagementConfiguration;

extern BOOLEAN      mCpuOrderSorted;
extern UINT32       mApicIdMap[MAX_SOCKET][MAX_CORE * MAX_THREAD];
extern UINT32       mNumOfBitShift;
extern CPU_ID_ORDER_MAP             mCpuApicIdOrderTable[MAX_CPU_NUM];


AML_OFFSET_TABLE_ENTRY            *mAmlOffsetTablePointer = NULL;

/**
    Check current thread status

    @param ApicId - current thread ApicId

    @retval EFI_SUCCESS     Returns Success if current thread is active
    @retval EFI_UNSUPPORTED Table is not supported
**/
EFI_STATUS
CheckCurrentThreadStatus (
  UINT32 ApicId
  )
{
  UINT32 Index;

  for (Index = 0; Index < MAX_CPU_NUM; Index++) {
    if ((mCpuApicIdOrderTable[Index].Flags == 1) && (mCpuApicIdOrderTable[Index].ApicId == ApicId)) {
      return EFI_SUCCESS;
    }
  }
  return EFI_UNSUPPORTED;
}


/**
  Get socket, stack and optionaly port index from PCI device path.

  The PCI device path is typically:
  '_SB_.PCxy.FIXz' for PCIe stack object
  '_SB_.UCxy.FIXz' for UBOX stack object
  '_SB_.PCxy.RPya' for PCIe bridge root port object
  where x and y are hex digits, and 'a' is a letter like 'A', 'B',..,'H'.

  NOTE: 'xy' is decimal number of subsequent PCIe stack, not including UBOX.
        For UBOX UCxy, 'x' is socket, 'y' is stack.

  @param[in]  DevPathPtr - PCI device path, e.g. '_SB_.PC00.FIX1'
  @param[out] SocketPtr  - Buffer for socket index.
  @param[out] StackPtr   - Buffer for stack index.
  @param[out] PortkPtr   - Buffer for port index.
**/
VOID
AcpiPciDevPath2SktStkPort (
  IN  CHAR8  *DevPathPtr,
  OUT UINT8  *SocketPtr,
  OUT UINT8  *StackPtr,
  OUT UINT8  *PortPtr
  )
{
  UINT16 SysStackNo;
  UINT8  SocketNo = 0xFF;
  UINT8  StackNo = 0xFF;
  UINT8  PortNo = 0xFF;

  if (PortPtr != NULL) {
    //
    // Device path should contain bridge root port object, let's verify.
    //
    if (AsciiStrLen (DevPathPtr) < 3*4 + 2 ||
        DevPathPtr[10]  != 'R' || DevPathPtr[12]  != 'P' || DevPathPtr[13]  < 'A' || DevPathPtr[13]  > 'H') {

      goto ErrExit;
    }
    PortNo = DevPathPtr[13] - 'A';
  }
  if (AsciiStrLen (DevPathPtr) < 2*4 + 1 || DevPathPtr[7]  < '0' || DevPathPtr[8]  < '0') {

    goto ErrExit;
  }
  switch (DevPathPtr[5] << 8 | DevPathPtr[6]) {

    case ('P' << 8 | 'C'):
      if (DevPathPtr[7] > '9' || DevPathPtr[8] > '9') {

        goto ErrExit;
      }
      SysStackNo = (DevPathPtr[7] - '0') * 10;
      SysStackNo += DevPathPtr[8] - '0';
      SocketNo = (UINT8)(SysStackNo / MAX_IIO_STACK);
      StackNo = (UINT8)(SysStackNo % MAX_IIO_STACK);
      break;

    case ('U' << 8 | 'C'):
      if (DevPathPtr[7] <= '9') {

        SocketNo = DevPathPtr[7] - '0';

      } else if (DevPathPtr[7] <= 'F') {

        if (DevPathPtr[7] < 'A') {

          goto ErrExit;
        }
        SocketNo = 10 + DevPathPtr[7] - 'A';

      } else if (DevPathPtr[7] <= 'f') {

        if (DevPathPtr[7] < 'a') {

          goto ErrExit;
        }
        SocketNo = 10 + DevPathPtr[7] - 'a';

      } else {
        goto ErrExit;
      }
      if (DevPathPtr[8] <= '9') {

        StackNo = DevPathPtr[8] - '0';

      } else if (DevPathPtr[8] <= 'F') {

        if (DevPathPtr[8] < 'A') {

          goto ErrExit;
        }
        StackNo = 10 + DevPathPtr[8] - 'A';

      } else {
        goto ErrExit;
      }
      break;

    default:
    ErrExit:
      DEBUG ((DEBUG_ERROR, "[ACPI] ERROR: String '%a' is not valid PCI stack name, ", DevPathPtr));
      DEBUG ((DEBUG_ERROR, "expect _SB_.PCxy.FIXz, or _SB_.UCxv.FIXz, or _SB_.PCxy.RPya\n"));
      break;
  }
  if (SocketPtr != NULL) {
    *SocketPtr = SocketNo;
  }
  if (StackPtr != NULL) {
    *StackPtr = StackNo;
  }
  if (PortPtr != NULL) {
    *PortPtr = PortNo;
  }
  return;
}


/**
  Update the DSDT table

  @param[in,out] *Table   - The table to be set

  @retval EFI_SUCCESS - DSDT updated
  @retval EFI_INVALID_PARAMETER - DSDT not updated
**/
EFI_STATUS
PatchDsdtTable (
  IN OUT EFI_ACPI_COMMON_HEADER   *Table
  )
{
  EFI_STATUS Status;
  UINT8                         *DsdtPointer;
  UINT32                        *Signature;
  UINT32                        Fixes;
  UINT32                        NodeIndex;
  UINT8                         Counter;
  UINT16                        i;  // DSDT_PLATEXRP_OffsetTable LUT entries extends beyond 256!
  UINT8                         BusBase = 0, BusLimit = 0;
  UINT16                        IoBase  = 0, IoLimit  = 0;
  UINT32                        MemBase32 = 0, MemLimit32 = 0;
  UINT64                        MemBase64 = 0, MemLimit64 = 0;
  AML_RESOURCE_ADDRESS16        *AmlResourceAddress16Pointer;
  AML_RESOURCE_ADDRESS32        *AmlResourceAddress32Pointer;
  AML_RESOURCE_ADDRESS64        *AmlResourceAddress64Pointer;
  EFI_ACPI_DESCRIPTION_HEADER   *TableHeader;
  UINT32                        AdjustSize = 0;
  UINT32                        CpuSkt = 0;
  UINT32                        CpuIndex = 0;
  ACPI_NAMEPACK_DWORD           *NamePtr;
  UINT8                         *CurrPtr;
  UINT8                         *EndPtr;
  const UINT32                  *ApicMapPtr;
  UINT8                         Socket;
  UINT8                         Stack;
  UINT8                         UboxStack;
  UINT8                         PropagateSerrOption;
  UINT8                         PropagatePerrOption;

  Status = GetOptionData (&gEfiSetupVariableGuid, OFFSET_OF(SYSTEM_CONFIGURATION, PropagateSerr), &PropagateSerrOption, sizeof(PropagateSerrOption));
  if (EFI_ERROR (Status)) {
    mAcpiParameter->PropagateSerrOption = 1;
  } else {
    mAcpiParameter->PropagateSerrOption = PropagateSerrOption;
  }

  Status = GetOptionData (&gEfiSetupVariableGuid, OFFSET_OF(SYSTEM_CONFIGURATION, PropagatePerr), &PropagatePerrOption, sizeof(PropagatePerrOption));
  if (EFI_ERROR (Status)) {
    mAcpiParameter->PropagatePerrOption = 1;
  } else {
    mAcpiParameter->PropagatePerrOption = PropagatePerrOption;
  }

  TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *)Table;

  if (mAmlOffsetTablePointer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  mAcpiParameter->SocketBitMask = mCpuCsrAccessVarPtr->socketPresentBitMap;

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (!mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Socket].Valid) {
      mAcpiParameter->IioPresentBitMask[Socket] = 0;
      continue;
    }
    mAcpiParameter->IioPresentBitMask[Socket] = mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Socket].stackPresentBitmap;
    for (Stack = 0; Stack < MAX_LOGIC_IIO_STACK; Stack++) {

      mAcpiParameter->BusBase[Socket][Stack] = mIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[Stack].BusBase;
      DEBUG ((DEBUG_INFO, "[ACPI](DSDT) [%d.%d] BusBase: 0x%02X\n", Socket, Stack, mAcpiParameter->BusBase[Socket][Stack]));
    }
  } // for (Socket...)

  //
  // Update IIO PCIe Root Port PCIe Capability offset
  // for 10nm process CPUs with PCIe GEN4/GEN5 controller, PCIe Capability offset is at 0x40
  //
  mAcpiParameter->IioPcieRpCapOffset = 0x40;

  //
  // Initialize TsegSize - 1MB aligned.
  //
  Fixes = 0;
  //
  // Loop through the AML looking for values that we must fix up.
  //
  for (i = 0; mAmlOffsetTablePointer[i].Pathname != 0; i++) {
    //
    // Point to offset in DSDT for current item in AmlOffsetTable.
    //
    DsdtPointer = (UINT8 *) (TableHeader) + mAmlOffsetTablePointer[i].Offset;

    if (mAmlOffsetTablePointer[i].Opcode == AML_DWORD_PREFIX) {
      //
      // If Opcode is 0x0C, then operator is Name() or OperationRegion().
      // (TableHeader + AmlOffsetTable.Offset) is at offset for value to change.
      //
      // The assert below confirms that AML structure matches the offsets table.
      // If not then patching the AML would just corrupt it and result in OS failure.
      // If you encounter this assert something went wrong in *.offset.h files
      // generation. Remove the files and rebuild.
      //
      ASSERT (DsdtPointer[-1] == mAmlOffsetTablePointer[i].Opcode);
      //
      // AmlOffsetTable.Value has FIX tag, so check that to decide what to modify.
      //
      Signature = (UINT32 *) (&mAmlOffsetTablePointer[i].Value);
      switch (*Signature) {
        //
        // Due to iASL compiler change and DSDT patch design change, if these items need support
        // then the ASI files will need to conform to the format requires for iASL to add the items
        // to the offset table, and we will need to filter them out when iASL is executed.
        //
        // "FIX0" OperationRegion() in Acpi\AcpiTables\Dsdt\CommonPlatform.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', '0')):
          *(UINT32*)DsdtPointer = (UINT32)(UINTN)mAcpiParameter;
          Fixes++;
          break;

        default:
          DEBUG ((DEBUG_ERROR, "[ACPI](DSDT) WARNING: Object '%a' with opcode 0x%02X not patched\n",
                  mAmlOffsetTablePointer[i].Pathname, mAmlOffsetTablePointer[i].Opcode));
          break;
      }
    } else if (mAmlOffsetTablePointer[i].Opcode == AML_INDEX_OP) {
      //
      // If Opcode is 0x88, then operator is WORDBusNumber() or WORDIO().
      // (TableHeader + AmlOffsetTable.Offset) must be cast to AML_RESOURCE_ADDRESS16 to change values.
      //
      AmlResourceAddress16Pointer = (AML_RESOURCE_ADDRESS16 *) (DsdtPointer);
      //
      // The assert below confirms that AML structure matches the offsets table.
      // If not then patching the AML would just corrupt it and result in OS failure.
      // If you encounter this assert something went wrong in *.offset.h files
      // generation. Remove the files and rebuild.
      //
      ASSERT (AmlResourceAddress16Pointer->DescriptorType == mAmlOffsetTablePointer[i].Opcode);

      //
      // Last 4 chars of AmlOffsetTable.Pathname has FIX tag.
      //
      Signature = (UINT32 *) (mAmlOffsetTablePointer[i].Pathname + AsciiStrLen(mAmlOffsetTablePointer[i].Pathname) - 4);
      switch (*Signature) {
        //
        // "FIX1" BUS resource for PCXX in Acpi\AcpiTables\Dsdt\SysBus.asi and PCXX.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', '1')):
          AcpiPciDevPath2SktStkPort (mAmlOffsetTablePointer[i].Pathname, &Socket, &Stack, NULL);

          BusBase = mIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[Stack].BusBase;
          BusLimit = mIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[Stack].BusLimit;

          AmlResourceAddress16Pointer->Granularity = 0;
          if ((BusLimit > BusBase)) {
            AmlResourceAddress16Pointer->Minimum = (UINT16) BusBase;
            AmlResourceAddress16Pointer->Maximum = (UINT16) BusLimit;
            AmlResourceAddress16Pointer->AddressLength = (UINT16) (BusLimit - BusBase + 1);
          }

          Fixes++;
          break;

        //
        // "FIXB" BUS resource for FpgaKtiXX in Acpi\AcpiTables\Dsdt\FpgaKtiXX.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', 'B')):
          break;

        //
        // "FIX2" IO resource for for PCXX in Acpi\AcpiTables\Dsdt\SysBus.asi and PCXX.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', '2')):
          AcpiPciDevPath2SktStkPort (mAmlOffsetTablePointer[i].Pathname, &Socket, &Stack, NULL);

          IoBase = mIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[Stack].PciResourceIoBase;
          IoLimit = mIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[Stack].PciResourceIoLimit;
          if (IoLimit > IoBase) {
            AmlResourceAddress16Pointer->Minimum = (UINT16) IoBase;
            AmlResourceAddress16Pointer->Maximum = (UINT16) IoLimit;
            AmlResourceAddress16Pointer->AddressLength = (UINT16) (IoLimit - IoBase + 1);
          }
          AmlResourceAddress16Pointer->Granularity = 0;

          Fixes++;
          break;

        //
        // "FIX9" BUS resource for UNXX in Acpi\AcpiTables\Dsdt\Uncore.asi
        //
        case (SIGNATURE_32('F', 'I', 'X', '9')) :
          AcpiPciDevPath2SktStkPort (mAmlOffsetTablePointer[i].Pathname, &Socket, &Stack, NULL);
          UboxStack = UBOX_STACK;
          BusBase = mIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[UboxStack].BusBase;
          BusLimit = mIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[UboxStack].BusLimit;
          if (mIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[UboxStack].Personality != TYPE_UBOX ||
              BusBase > BusLimit) {

            DEBUG ((DEBUG_ERROR, "[ACPI](DSDT) ERROR: Stack [%d.%d] of type %d is not UBOX, '%a' not patched\n",
                    Socket, UboxStack, mIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[UboxStack].Personality,
                    mAmlOffsetTablePointer[i].Pathname));
            break;
          }
          AmlResourceAddress16Pointer->Granularity = 0;
          if (Stack & 1) {
            AmlResourceAddress16Pointer->Minimum = BusLimit;
            AmlResourceAddress16Pointer->Maximum = BusLimit;
          } else {
            AmlResourceAddress16Pointer->Minimum = BusBase;
            AmlResourceAddress16Pointer->Maximum = BusBase;
          }
          AmlResourceAddress16Pointer->AddressLength = 1;
          mAcpiParameter->BusBase[Socket][Stack] = (UINT8)AmlResourceAddress16Pointer->Minimum;
          mAcpiParameter->IioPresentBitMask[Socket] |= 1 << Stack;
          Fixes++;
          break;

        //
        // "FIX6" IO resource for PCXX in Acpi\AcpiTables\Dsdt\PCXX.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', '6')):
          AcpiPciDevPath2SktStkPort (mAmlOffsetTablePointer[i].Pathname, &Socket, &Stack, NULL);
          AmlResourceAddress16Pointer->Granularity = 0;
          if ((mSocketMpLinkConfiguration.LegacyVgaSoc == Socket) &&
              (mSocketMpLinkConfiguration.LegacyVgaStack == Stack)){

            AmlResourceAddress16Pointer->Minimum = (UINT16) 0x03b0;
            AmlResourceAddress16Pointer->Maximum = (UINT16) 0x03bb;
            AmlResourceAddress16Pointer->AddressLength = (UINT16) 0x000C;
          }
          Fixes++;
         break;

        //
        // "FIX7" IO resource for PCXX in Acpi\AcpiTables\Dsdt\PCXX.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', '7')):
          AcpiPciDevPath2SktStkPort (mAmlOffsetTablePointer[i].Pathname, &Socket, &Stack, NULL);
          AmlResourceAddress16Pointer->Granularity = 0;
          if ((mSocketMpLinkConfiguration.LegacyVgaSoc == Socket) &&
              (mSocketMpLinkConfiguration.LegacyVgaStack == Stack)) {

            AmlResourceAddress16Pointer->Minimum = (UINT16) 0x03c0;
            AmlResourceAddress16Pointer->Maximum = (UINT16) 0x03df;
            AmlResourceAddress16Pointer->AddressLength = (UINT16) 0x0020;
          }
          Fixes++;
          break;

        default:
          DEBUG ((DEBUG_ERROR, "[ACPI](DSDT) WARNING: Object '%a' with opcode 0x%02X not patched\n",
                  mAmlOffsetTablePointer[i].Pathname, mAmlOffsetTablePointer[i].Opcode));
          break;
      }
    } else if (mAmlOffsetTablePointer[i].Opcode == AML_SIZE_OF_OP) {
      //
      // If Opcode is 0x87, then operator is DWORDMemory().
      // (TableHeader + AmlOffsetTable.Offset) must be cast to AML_RESOURCE_ADDRESS32 to change values.
      //
      AmlResourceAddress32Pointer = (AML_RESOURCE_ADDRESS32 *) (DsdtPointer);
      //
      // The assert below confirms that AML structure matches the offsets table.
      // If not then patching the AML would just corrupt it and result in OS failure.
      // If you encounter this assert something went wrong in *.offset.h files
      // generation. Remove the files and rebuild.
      //
      ASSERT (AmlResourceAddress32Pointer->DescriptorType == mAmlOffsetTablePointer[i].Opcode);
      //
      // Last 4 chars of AmlOffsetTable.Pathname has FIX tag.
      //
      Signature = (UINT32 *) (mAmlOffsetTablePointer[i].Pathname + AsciiStrLen(mAmlOffsetTablePointer[i].Pathname) - 4);
      switch (*Signature) {
        //
        // "FIX3" PCI32 resource for PCXX in Acpi\AcpiTables\Dsdt\SysBus.asi and PCXX.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', '3')):
          AcpiPciDevPath2SktStkPort (mAmlOffsetTablePointer[i].Pathname, &Socket, &Stack, NULL);

          MemBase32 = mIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[Stack].PciResourceMem32Base;
          MemLimit32 = mIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[Stack].PciResourceMem32Limit;

          if (MemLimit32 > MemBase32) {
            AmlResourceAddress32Pointer->Minimum = (UINT32) MemBase32;
            AmlResourceAddress32Pointer->Maximum = (UINT32) MemLimit32;
            AmlResourceAddress32Pointer->AddressLength = (UINT32) (MemLimit32 - MemBase32 + 1);
          }
          AmlResourceAddress32Pointer->Granularity = 0;

          Fixes++;
          break;

        //
        // "FIX5" IO resource for PCXX in Acpi\AcpiTables\Dsdt\PCXX.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', '5')):
          AcpiPciDevPath2SktStkPort (mAmlOffsetTablePointer[i].Pathname, &Socket, &Stack, NULL);
          AmlResourceAddress32Pointer->Granularity = 0;
          if ((mSocketMpLinkConfiguration.LegacyVgaSoc == Socket) &&
              (mSocketMpLinkConfiguration.LegacyVgaStack == Stack)) {
              AmlResourceAddress32Pointer->Minimum = 0x000a0000;
              AmlResourceAddress32Pointer->Maximum = 0x000bffff;
              AmlResourceAddress32Pointer->AddressLength = 0x00020000;
          }
          Fixes++;
          break;

        //
        // "FIXZ" IO resource for FpgaBusXX in Acpi\AcpiTables\Dsdt\FpgaBusXX.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', 'Z')):
          break;

        default:
          DEBUG ((DEBUG_ERROR, "[ACPI](DSDT) WARNING: Object '%a' with opcode 0x%02X not patched\n",
                  mAmlOffsetTablePointer[i].Pathname, mAmlOffsetTablePointer[i].Opcode));
          break;
      }
    } else if (mAmlOffsetTablePointer[i].Opcode == AML_CREATE_DWORD_FIELD_OP) {
      //
      // If Opcode is 0x8A, then operator is QWORDMemory().
      // (TableHeader + AmlOffsetTable.Offset) must be cast to AML_RESOURCE_ADDRESS64 to change values.
      //
      AmlResourceAddress64Pointer = (AML_RESOURCE_ADDRESS64 *) (DsdtPointer);
      //
      // The assert below confirms that AML structure matches the offsets table.
      // If not then patching the AML would just corrupt it and result in OS failure.
      // If you encounter this assert something went wrong in *.offset.h files
      // generation. Remove the files and rebuild.
      //
      ASSERT (AmlResourceAddress64Pointer->DescriptorType == mAmlOffsetTablePointer[i].Opcode);
      //
      // Last 4 chars of AmlOffsetTable.Pathname has FIX tag.
      //
      Signature = (UINT32 *) (mAmlOffsetTablePointer[i].Pathname + AsciiStrLen(mAmlOffsetTablePointer[i].Pathname) - 4);
      switch (*Signature) {
        //
        // "FIX4" PCI64 resource for PCXX in Acpi\AcpiTables\Dsdt\SysBus.asi and PCXX.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', '4')):
          if (mSocketIioConfiguration.Pci64BitResourceAllocation) {

            AcpiPciDevPath2SktStkPort (mAmlOffsetTablePointer[i].Pathname, &Socket, &Stack, NULL);
            MemBase64 = mIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[Stack].PciResourceMem64Base;
            MemLimit64 = mIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[Stack].PciResourceMem64Limit;
            if (MemLimit64 > MemBase64) {
              AmlResourceAddress64Pointer->Granularity = 0;
              AmlResourceAddress64Pointer->Minimum = (UINT64) MemBase64;
              AmlResourceAddress64Pointer->Maximum = (UINT64) MemLimit64;
              AmlResourceAddress64Pointer->AddressLength = (UINT64) (MemLimit64 - MemBase64 + 1);
            }

            Fixes++;
          }
          break;

        default:
          DEBUG ((DEBUG_ERROR, "[ACPI](DSDT) WARNING: Object '%a' with opcode 0x%02X not patched\n",
                  mAmlOffsetTablePointer[i].Pathname, mAmlOffsetTablePointer[i].Opcode));
          break;
      }
    } else {
      DEBUG ((DEBUG_ERROR, "[ACPI](DSDT) WARNING: Object '%a' with opcode 0x%02X not patched\n",
              mAmlOffsetTablePointer[i].Pathname, mAmlOffsetTablePointer[i].Opcode));
    }
  }

  // CurrPtr = beginning of table
  //
  CurrPtr = (UINT8 *) TableHeader;

  // EndPtr = beginning of table + length of table
  //
  EndPtr = (CurrPtr + ((EFI_ACPI_COMMON_HEADER *) CurrPtr)->Length);

  // Subtract from End Ptr the largest data item we read from table
  //  so we don't try to access data beyond end of table
  //
  EndPtr -= 9;

  for (DsdtPointer = CurrPtr; DsdtPointer <= EndPtr; DsdtPointer++) {

    //
    // fix CpuMemHp.asi, force no same ASL code string as it...
    //
    if ((DsdtPointer[0] == 'C') && (DsdtPointer[6] == 0x4)  && (DsdtPointer[5] == 0x10)) {
      if (mCpuOrderSorted) {
        CpuSkt = (UINT32) DsdtPointer[4];
        AdjustSize = 0;
        if ((DsdtPointer[1] > '0') && (DsdtPointer[1] <= '9')) {
          AdjustSize = (UINT32) ((DsdtPointer[1] -'0') * 0x100);
        } else if ((DsdtPointer[1] >= 'A') && (DsdtPointer[1] <= 'F')) {
          AdjustSize = (UINT32) ((DsdtPointer[1] -'A' + 10) * 0x100);
        }

        CpuIndex = AdjustSize;

        AdjustSize = 0;
        if ((DsdtPointer[2] > '0') && (DsdtPointer[2] <= '9')) {
          AdjustSize = (UINT32) ((DsdtPointer[2] -'0') * 0x10);
        } else if ((DsdtPointer[2] >= 'A') && (DsdtPointer[2] <= 'F')) {
          AdjustSize = (UINT32) ((DsdtPointer[2] -'A' + 10) * 0x10);
        }

        CpuIndex += AdjustSize;

        AdjustSize = 0;
        if ((DsdtPointer[3] > '0') && (DsdtPointer[3] <= '9')) {
          AdjustSize = (UINT32) (DsdtPointer[3] -'0');
        } else if ((DsdtPointer[3] >= 'A') && (DsdtPointer[3] <= 'F')) {
          AdjustSize = (UINT32) (DsdtPointer[3] -'A' + 10);
        }

        CpuIndex += AdjustSize;

        NodeIndex = (UINT32) (CpuSkt << mNumOfBitShift) + mApicIdMap[CpuSkt][CpuIndex] ;

        DsdtPointer[4] = (UINT8) 0xFF;
        if (((mCpuCsrAccessVarPtr->socketPresentBitMap >> CpuSkt) & BIT0) == 1) {
          if (CheckCurrentThreadStatus (NodeIndex) == EFI_SUCCESS) {
            DsdtPointer[4] = (UINT8) (NodeIndex & 0xFF);
          }
        }

        //
        // Update IO Address
        //
        *(UINT16 *)(DsdtPointer+5) = (UINT16)(PM_BASE_ADDRESS + 0x10);
      }
    }

    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {

      if ((mCpuCsrAccessVarPtr->socketPresentBitMap & (BIT0 << Socket)) == 0) {
        continue;
      }
      //
      // Find APT##socket name
      //
      if ((DsdtPointer[0] == 'A') && (DsdtPointer[1] == 'P') && (DsdtPointer[2] == 'T') && (DsdtPointer[3] == '0' + Socket)) {
        NamePtr = ACPI_NAME_COMMAND_FROM_NAMEPACK_STR (DsdtPointer);
        ApicMapPtr = mApicIdMap[Socket];
        if (NamePtr->StartByte != AML_NAME_OP) {
          continue;
        }

        Counter = DsdtPointer[8];
        ASSERT (Counter >= (UINT32) (MAX_THREAD * MAX_CORE));
        DEBUG ((DEBUG_INFO, "\n::ACPI::  Found 'APT%x'...Counter = DsdtPointer[7] = %x\n\n", Socket, Counter));
         for (i = 0; i < (MAX_THREAD * MAX_CORE); i++) {
           DEBUG ((DEBUG_VERBOSE, "Before override, DsdtPointer[%x] = %x,   ", i, DsdtPointer[i+9]));
           DsdtPointer[i+9] = (UINT8)ApicMapPtr[i];
           DEBUG ((DEBUG_VERBOSE, "Then override value = %x \n", DsdtPointer[i+9]));
         }
      }
    }
    //
    // Fix up _S3
    //
    if ((DsdtPointer[0] == '_') && (DsdtPointer[1] == 'S') && (DsdtPointer[2] == '3')) {
      NamePtr = ACPI_NAME_COMMAND_FROM_NAMEPACK_STR (DsdtPointer);
      if (NamePtr->StartByte != AML_NAME_OP) {
        continue;
      }

      if (!mSocketPowermanagementConfiguration.AcpiS3Enable) {
        //
        // S3 disabled
        //
        DsdtPointer[0] = 'D';
      }
    }
    //
    // Fix up _S4
    //
    if ((DsdtPointer[0] == '_') && (DsdtPointer[1] == 'S') && (DsdtPointer[2] == '4')) {
      NamePtr = ACPI_NAME_COMMAND_FROM_NAMEPACK_STR (DsdtPointer);
      if (NamePtr->StartByte != AML_NAME_OP) {
        continue;
      }
      if (!mSocketPowermanagementConfiguration.AcpiS4Enable) {
        //
        // S4 disabled
        //
        DsdtPointer[0] = 'D';
      }
    }
  }
  return EFI_SUCCESS;
}
