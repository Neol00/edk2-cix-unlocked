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
#include <Protocol/DynamicSiLibraryProtocol2.h>

extern BIOS_ACPI_PARAM             *mAcpiParameter;
extern EFI_IIO_UDS_PROTOCOL        *mIioUds;

extern SOCKET_MEMORY_CONFIGURATION mSocketMemoryConfiguration;
extern SOCKET_POWERMANAGEMENT_CONFIGURATION mSocketPowermanagementConfiguration;

extern BOOLEAN      mCpuOrderSorted;
extern UINT32       mApicIdMap[MAX_SOCKET][MAX_CORE * MAX_THREAD];
extern UINT32       mNumOfBitShift;

extern UINT32       mEnabledProcessor[MAX_SOCKET];

extern EFI_CPU_CSR_ACCESS_PROTOCOL *mCpuCsrAccess;
UINT32 mCpuPCPSInfo[MAX_SOCKET];
UINT32 mNcpuValue[MAX_SOCKET];

extern CPU_ID_ORDER_MAP             mCpuApicIdOrderTable[];
extern UINT8                        mPStateEnable;

#ifndef MSR_MISC_ENABLES
#define MSR_MISC_ENABLES         0x01A0
#endif

/**
  This function detects PCPS Info

  mCpuPCPSInfo usage:
    Bit[15:0]: Enabled processors in current socket
    Bit[16]: Hyperthreading enable
    Bit[17]: PCPS disable in system

  @param None

  @retval VOID

**/
VOID
DetectPcpsInfo (
  VOID
  )
{
  UINT8                   Socket;
  UINT32                  CpuPCPSInfo = 0;
  UINT32                  SmtDisable = 0;
  UINT32                  Csr;

  EFI_STATUS                      Status = EFI_SUCCESS;
  DYNAMIC_SI_LIBARY_PROTOCOL2     *DynamicSiLibraryProtocol2 = NULL;

  Status = gBS->LocateProtocol (&gDynamicSiLibraryProtocol2Guid, NULL, (VOID **) &DynamicSiLibraryProtocol2);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return;
  }

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Socket].Valid) {
      SmtDisable = DynamicSiLibraryProtocol2->PcuGetDesiredCoreSmtDis (Socket);

      Csr = mCpuCsrAccess->ReadCpuCsr (Socket, 0, 0x04320084);

      if (!SmtDisable && !(Csr & (1 << 12))) {
        mCpuPCPSInfo[Socket] |= B_PCPS_HT_ENABLE;
      }
      CpuPCPSInfo = mCpuPCPSInfo[Socket];

      //
      // Update NCPU
      //
      mNcpuValue[Socket] = mCpuPCPSInfo[Socket] & 0xFF;

      if (((CpuPCPSInfo & B_PCPS_DISABLE) == 0)) {
        if (CpuPCPSInfo & B_PCPS_HT_ENABLE) {
          mNcpuValue[Socket] = 2;
        } else {
          mNcpuValue[Socket] = 1;
        }
      }
    }
  }

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (mCpuPCPSInfo[Socket] == 0) {
      mCpuPCPSInfo[Socket] = CpuPCPSInfo;
      mNcpuValue[Socket] = mNcpuValue[0];
    }
  }
}

/**
  Find APICID in mCpuApicIdOrderTable

  @param SocketIndex      - In: Acpi thread number
  @param ThreadIndex      - In: Acpi thread number

  @retval APICID     - If not found, return 0xFFFFFFFF
**/
UINT32
LocateApicIdInfo (
  IN   UINT32                 SocketIndex,
  IN   UINT32                 ThreadIndex
  )
{
  if (mApicIdMap[SocketIndex][ThreadIndex] == (UINT32) -1) {
    return (UINT32) -1;
  }

  return (mApicIdMap[SocketIndex][ThreadIndex] + (SocketIndex << mNumOfBitShift));
}

/**
  Gather the EIST information

  @param ThreadId       - In: Acpi thread number
  @param CpuMiscData    - In/Out: Pointer to thread's CPU_MISC_DTAT struct

  @retval EFI_SUCCESS   - EIST info retrieved
**/
EFI_STATUS
LocateCpuEistInfo (
  IN   UINT32                        CpuIndex,
  OUT  CPU_MISC_DATA                 **CpuMiscData
  )
{
  UINTN                       Index;
  UINT32                      Socket;
  UINT32                      ApicId;
  const UINT32                *ApicMapPtr;

  Socket = CpuIndex / (MAX_CORE * MAX_THREAD);
  Index  = CpuIndex % (MAX_CORE * MAX_THREAD);
  ApicMapPtr = mApicIdMap[Socket];

  ApicId = mAcpiParameter->ProcessorApicIdBase[Socket] + ApicMapPtr[Index];

  for (Index = 0; Index < mCpuConfigLibConfigContextBuffer->NumberOfProcessors; ++Index) {
    if (mCpuConfigLibConfigContextBuffer->CollectedDataBuffer[Index].CpuMiscData.ApicID == ApicId) {
      *CpuMiscData = &mCpuConfigLibConfigContextBuffer->CollectedDataBuffer[Index].CpuMiscData;
      break;
    }
  }

  if (*CpuMiscData == NULL) {  //use SBSP's data
    *CpuMiscData = &mCpuConfigLibConfigContextBuffer->CollectedDataBuffer[0].CpuMiscData;
  }

  return EFI_SUCCESS;
}

/**
  Determine turbo mode status

  @param None

  @retval TRUE if turbo enabled, FALSE if disabled
**/
BOOLEAN
IsTurboModeEnabled (
  VOID
  )
{
  EFI_CPUID_REGISTER     CpuidRegisters;
  BOOLEAN                Status;
  UINT64                 MiscEnable;

  Status = FALSE;
  AsmCpuid (CPUID_THERMAL_POWER_MANAGEMENT, &CpuidRegisters.RegEax, &CpuidRegisters.RegEbx, &CpuidRegisters.RegEcx, &CpuidRegisters.RegEdx);
  if (((CPUID_THERMAL_POWER_MANAGEMENT_EAX*)&CpuidRegisters.RegEax)->Bits.TurboBoostTechnology != 0) {
    //
    // Turbo mode is supported on this processor (Available)
    //

    MiscEnable = AsmReadMsr64 (MSR_MISC_ENABLES);
    if ((RShiftU64 (MiscEnable, 38) & 1) == 0) { // Bit 38 is TurboModeDisable
      //
      // Turbo mode is supported on this processor (Available)
      //
      Status = TRUE;
    }
  }

  return Status;
}

/**
  Finds the actual beginning of a CPU SSDT table, skips the If(Zero) { External() ... } opcode
  auto-generated by the iASL 6.1 compiler

  From iASL/6.1/changes.txt
  "Completed full support for the ACPI 6.0 External() AML opcode. The
  compiler emits an external AML opcode for each ASL External statement.
  This opcode is used by the disassembler to assist with the disassembly of
  external control methods by specifying the required number of arguments
  for the method. AML interpreters do not use this opcode. To ensure that
  interpreters do not even see the opcode, a block of one or more external
  opcodes is surrounded by an "If(0)" construct. As this feature becomes
  commonly deployed in BIOS code, the ability of disassemblers to correctly
  disassemble AML code will be greatly improved."

  The AML code contains external (_SB_SCKxCyyy) opcodes within a If(Zero) statement;
  we have to ignore this opcodes and start patching from the actual table begin marker, i.e., "TBST"

  @param[in] BeginPtr a pointer to the begin of a SSDT table
  @param[in] EndPtr   a pointer to the end of a SSDT table

  @retval       beginning of the part of SSDT table past external _SB_ opcodes
**/
static inline
UINT8 *
SkipExternalSbOpcodes(
  IN UINT8  *BeginPtr,
  IN UINT8  *EndPtr,
  IN UINT32 ExternSbExpected
  )
{
  UINT8  *CurPtr = BeginPtr;
  UINT32 ExternSbFound = 0;

  ASSERT (BeginPtr < EndPtr);

  DEBUG ((DEBUG_VERBOSE, "SkipExternalSbOpcodes start\n"));

  for (CurPtr = BeginPtr; CurPtr < EndPtr; ++CurPtr) {
    UINT32 Signature = *(UINT32 *) CurPtr;

    if (SIGNATURE_32 ('_', 'S', 'B', '_') == Signature) {
      CONST EXTERNAL_OBJECT_DECL *ExternDecl = ACPI_EXTERNAL_OBJECT_DECL_FROM_NAME_STR (CurPtr);

      ASSERT (BeginPtr < (UINT8 *)ExternDecl);

      if ((AML_EXTERNAL_OP == ExternDecl->ExternalOp ) &&
          (AML_ROOT_CHAR == ExternDecl->RootChar) &&
          (AML_MULTI_NAME_PREFIX == ExternDecl->MultiNamePrefix) &&
          (0x3 <= ExternDecl->SegCount)) {
        ++ExternSbFound;
      } else {
        break;
      }
    }
  }

  DEBUG ((DEBUG_ERROR, "ExternSbExpected: %d, ExternSbFound: %d\n", ExternSbExpected, ExternSbFound));

  ASSERT ((ExternSbFound % ExternSbExpected) == 0);

  DEBUG ((DEBUG_VERBOSE, "SkipExternalSbOpcodes end\n"));

  return CurPtr;
}

/**
  Update the CPU PM SSDT table

  @param[in,out]  TableHeader     The table to be set

  @retval         EFI_SUCCESS     Returns Success
  @retval         EFI_UNSUPPORTED Table is not supported
**/
EFI_STATUS
PatchCpuPmSsdtTable (
  IN OUT  EFI_ACPI_COMMON_HEADER      *Table
  )
{
  UINT8                       *CurrPtr;
  UINT8                       *EndPtr;
  UINT8                       *SsdtPointer;
  UINT32                      Signature;
  UINT32                      CpuFixes;
  UINT32                      CpuSkt;
  UINT32                      CpuIndex;
  UINT32                      ThreadIndex;
  UINT32                      AdjustSize;
  ACPI_NAMEPACK_DWORD         *NamePtr;
  UINT32                      DomnValue;
  ACPI_NAME_COMMAND           *PsdPackage;
  PSD_PACKAGE_LAYOUT          *PsdPackageItemPtr;
  CPU_MISC_DATA               *CpuMiscData;

  DEBUG ((DEBUG_INFO, "Patching SSDT PatchCpuPmSsdtTable\n"));

  //
  // Loop through the ASL looking for values that we must fix up.
  //
  DomnValue = 0;
  CpuFixes  = 0;
  CpuSkt    = 0;
  CpuIndex  = 0;
  ThreadIndex = 0;
  CurrPtr = (UINT8 *) Table;
  EndPtr  = (CurrPtr + ((EFI_ACPI_COMMON_HEADER *) CurrPtr)->Length);
  CpuMiscData = NULL;

  DetectPcpsInfo ();

  //
  // CurrPtr = beginning of table to search
  //
  CurrPtr = SkipExternalSbOpcodes (CurrPtr, EndPtr, (UINT32) MAX_CPU_NUM);

  //
  // Subtract 11 from EndPtr - this is the size of the largest data item we will search for
  //  so that we do not try to read past the end of the table
  //
  EndPtr -= 11;

  for (SsdtPointer = CurrPtr; SsdtPointer <= EndPtr; ++SsdtPointer) {
    Signature = *(UINT32 *) SsdtPointer;
    CpuIndex  = 0;
    AdjustSize  = 0;

    switch (Signature) {

    //
    // The AML code contains strings in the form of _SB_SCKxCyyy where x is the socket number
    // and yyy is the thread number in hexadecimal, this first case parses that string and saves
    // the socket number into CpuSkt and the thread number into CpuIndex.
    //
    case SIGNATURE_32 ('_', 'S', 'B', '_'):
      //
      // SKTX
      //
      CpuSkt = *(SsdtPointer + 7);
      if ((CpuSkt < '0') || ((CpuSkt - '0') > MAX_SOCKET)) {
        CpuSkt = '0';
      }
      CpuSkt -= '0';

      if ((*(SsdtPointer + 8) != 'C')) {
        continue;
      }

      if ((*(SsdtPointer + 11) > '0') && (*(SsdtPointer + 11) <= '9')) {
        CpuIndex = (*(SsdtPointer + 11) -'0');
      } else if ((*(SsdtPointer + 11) >= 'A') && (*(SsdtPointer + 11) <= 'F')) {
        CpuIndex = (*(SsdtPointer + 11) -'A' + 10);
      }

      if ((*(SsdtPointer + 10) > '0') && ( *(SsdtPointer + 10) <= '9')) {
        AdjustSize = (*(SsdtPointer + 10) -'0') * 0x10;
      } else if ((*(SsdtPointer + 10) >= 'A') && (*(SsdtPointer + 10) <= 'F')) {
        AdjustSize = (*(SsdtPointer + 10) -'A' + 10) * 0x10;
      }

      CpuIndex += AdjustSize;
      AdjustSize  = 0;

      if ((*(SsdtPointer + 9) > '0') && (*(SsdtPointer + 9) <= '9')) {
        AdjustSize = (*(SsdtPointer + 9) -'0') * 0x100;
      } else if ((*(SsdtPointer + 9) >= 'A') && (*(SsdtPointer + 9) <= 'F')) {
        AdjustSize = (*(SsdtPointer + 9) -'A' + 10) * 0x100;
      }

      CpuIndex += AdjustSize;
      ThreadIndex  = CpuIndex;

      //
      // PCPS - Update DOMN
      //
      DomnValue = (UINT8) CpuSkt;

      if ((mCpuPCPSInfo[CpuSkt] & B_PCPS_DISABLE) == 0) {
        DomnValue = LocateApicIdInfo (CpuSkt, ThreadIndex);

        if (mNcpuValue[CpuSkt] == 2) {
          DomnValue = (DomnValue >> 1);
        }
      }

      DEBUG ((
        DEBUG_INFO,
        ":ACPI: PatchCpuPmSsdtTable(): CpuSkt: %d ThreadIndex: %d, NcpuValue: %d, DomnValue: %d\n",
        CpuSkt,
        ThreadIndex,
        mNcpuValue[CpuSkt],
        DomnValue
        ));
      ++CpuFixes;
      CpuMiscData = NULL;
      LocateCpuEistInfo (0, &CpuMiscData);  // use CPU0 for update NPSS and SPSS
      break;

    case SIGNATURE_32 ('D', 'O', 'M', 'N'):
      NamePtr = ACPI_NAME_COMMAND_FROM_NAMEPACK_STR (SsdtPointer);
      if (NamePtr->StartByte != AML_NAME_OP) {
        continue;
      }

      if (NamePtr->Size != AML_NAME_DWORD_SIZE) {
        continue;
      }

      NamePtr->Value = DomnValue;
      break;

    case SIGNATURE_32 ('N', 'C', 'P', 'U'):
      NamePtr = ACPI_NAME_COMMAND_FROM_NAMEPACK_STR (SsdtPointer);
      if (NamePtr->StartByte != AML_NAME_OP) {
        continue;
      }

      if (NamePtr->Size != AML_NAME_DWORD_SIZE) {
        continue;
      }

      NamePtr->Value = (UINT32) mNcpuValue[CpuSkt];
      break;

    case SIGNATURE_32 ('P', 'S', 'D', 'C'):
    case SIGNATURE_32 ('P', 'S', 'D', 'E'):
      PsdPackage = ACPI_NAME_COMMAND_FROM_NAME_STR (SsdtPointer);
      if (PsdPackage->StartByte != AML_NAME_OP) {
        continue;
      }

      PsdPackageItemPtr       = (PSD_PACKAGE_LAYOUT *) ((UINT8 *) PsdPackage);
      DEBUG ((
        DEBUG_VERBOSE,
        "PatchCpuPmSsdtTable(): PsdPackageItemPtr table: %x is detected...\n",
        PsdPackage->NameStr
        ));
      DEBUG ((
        DEBUG_VERBOSE,
        "   Initial Values:     Domain = %x,    CoordType = %x,   NumProcessors = %x\n",
        PsdPackageItemPtr->Domain,
        PsdPackageItemPtr->CoordType,
        PsdPackageItemPtr->NumProcessors
        ));

      PsdPackageItemPtr->Domain = DomnValue;
      PsdPackageItemPtr->NumProcessors = (UINT32) mNcpuValue[CpuSkt];
      DEBUG ((
        DEBUG_VERBOSE,
        "   PsdPackage = %x,    PsdPackageItemPtr = %x,   SsdtPointer = %x\n",
        (UINT8 *)PsdPackage,
        (UINT8 *)PsdPackageItemPtr,
        (UINT8 *)SsdtPointer
        ));
      DEBUG ((
        DEBUG_VERBOSE,
        "   Updated PSD Domain = %x,    CoordType = %x,   NumProcessors = %x\n",
        PsdPackageItemPtr->Domain,
        PsdPackageItemPtr->CoordType,
        PsdPackageItemPtr->NumProcessors
        ));
      break;

    default:
      break;
    } // switch
  } // for

  //
  // N fixes together currently
  //
  ASSERT (CpuFixes == (UINT32) MAX_CPU_NUM);

  return EFI_SUCCESS;
}

/**

    Update the OEM1 P-State SSDT table (EIST)

    @param *TableHeader   - The table to be set

    @retval EFI_SUCCESS -  Returns Success

**/
EFI_STATUS
PatchOem1SsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER  *Table
  )
{
  EFI_STATUS                  Status;
  UINT8                       *CurrPtr;
  UINT8                       *EndPtr;
  UINT8                       *SsdtPointer;
  UINT32                      Signature;
  UINT32                      CpuFixes;
  UINT32                      NpssFixes;
  UINT32                      GpssFixes;
  UINT32                      CpuSkt;
  UINT32                      CpuIndex;
  UINT32                      PackageSize;
  UINT32                      NewPackageSize;
  UINT32                      AdjustSize;
  UINTN                       TableIndex;
  ACPI_NAME_COMMAND           *PssTable;
  PSS_PACKAGE                 *PssTableItemPtr;
  CPU_MISC_DATA               *CpuMiscData;
  FVID_ENTRY                  *PssState;

  DEBUG ((DEBUG_INFO, "Patching SSDT PatchOem1SsdtTable\n"));

  //
  // Loop through the ASL looking for values that we must fix up.
  //
  NpssFixes = 0;
  GpssFixes = 0;
  CpuFixes  = 0;
  CpuSkt    = 0;
  CpuIndex  = 0;

  CurrPtr = (UINT8 *) Table;
  EndPtr  = (CurrPtr + ((EFI_ACPI_COMMON_HEADER *) CurrPtr)->Length);
  CpuMiscData = NULL;

  Status = LocateCpuEistInfo (0, &CpuMiscData);   // get BSP's data
  if( (EFI_ERROR (Status)) || (CpuMiscData == NULL ) ){
    DEBUG ((DEBUG_WARN, " PatchGv3SsdtTable - EIST info for BSP index not found \n"));
    return Status;
  }

  mPStateEnable = 1;

  //
  // CurrPtr = beginning of table to search
  //
  CurrPtr = SkipExternalSbOpcodes (CurrPtr, EndPtr, (UINT32) MAX_CPU_NUM);

  //
  // Subtract 11 from EndPtr - this is the size of the larget data item we will search for
  // so that we do not try to read past the end of the table
  //
  EndPtr -= 11;

  for (SsdtPointer = CurrPtr; SsdtPointer <= EndPtr; ++SsdtPointer) {
    Signature = *(UINT32 *) SsdtPointer;
    CpuIndex  = 0;
    AdjustSize  = 0;

    switch (Signature) {
      case SIGNATURE_32 ('_', 'S', 'B', '_'):
        //
        // SKTX
        //
        CpuSkt = *(SsdtPointer + 7);
        if ((CpuSkt < '0') || ((CpuSkt - '0') > MAX_SOCKET)) {
          CpuSkt = '0';
        }

        CpuSkt -= '0';

        if ((*(SsdtPointer + 11) > '0') && (*(SsdtPointer + 11) <= '9')) {
          CpuIndex = (*(SsdtPointer + 11) -'0');
        } else if ((*(SsdtPointer + 11) >= 'A') && (*(SsdtPointer + 11) <= 'F')) {
          CpuIndex = (*(SsdtPointer + 11) -'A' + 10);
        }

        if ((*(SsdtPointer + 10) > '0') && ( *(SsdtPointer + 10) <= '9')) {
          AdjustSize = (*(SsdtPointer + 10) -'0') * 0x10;
        } else if ((*(SsdtPointer + 10) >= 'A') && (*(SsdtPointer + 10) <= 'F')) {
          AdjustSize = (*(SsdtPointer + 10) -'A' + 10) * 0x10;
        }

        CpuIndex += AdjustSize;
        AdjustSize  = 0;

        if ((*(SsdtPointer + 9) > '0') && (*(SsdtPointer + 9) <= '9')) {
          AdjustSize = (*(SsdtPointer + 9) -'0') * 0x100;
        } else if ((*(SsdtPointer + 9) >= 'A') && (*(SsdtPointer + 9) <= 'F')) {
          AdjustSize = (*(SsdtPointer + 9) -'A' + 10) * 0x100;
        }

        CpuIndex += AdjustSize;

        ++CpuFixes;
        CpuMiscData = NULL;
        LocateCpuEistInfo (0, &CpuMiscData);  // use CPU0 for update NPSS and SPSS
        break;

      case SIGNATURE_32 ('N', 'P', 'S', 'S'):
      case SIGNATURE_32 ('S', 'P', 'S', 'S'):

        PssTable = ACPI_NAME_COMMAND_FROM_NAME_STR (SsdtPointer);
        if (PssTable->StartByte != AML_NAME_OP) {
          continue;
        }

        ASSERT (CpuMiscData != NULL);
        PssState = CpuMiscData->FvidTable;

        AdjustSize  = PssTable->NumEntries * sizeof (PSS_PACKAGE);
        AdjustSize -= (UINT32)(CpuMiscData->NumberOfPStates * sizeof (PSS_PACKAGE));
        PackageSize     = (PssTable->Size & 0xF) + ((PssTable->Size & 0xFF00) >> 4);
        NewPackageSize  = PackageSize - AdjustSize;
        PssTable->Size  = (UINT16) ((NewPackageSize & 0xF) + ((NewPackageSize & 0x0FF0) << 4));

        //
        // Set most significant two bits of byte zero to 01, meaning two bytes used
        //
        PssTable->Size |= 0x40;

        //
        // Set unused table to Noop Code
        //
        SetMem (
          (UINT8 *) PssTable + NewPackageSize + AML_NAME_PREFIX_SIZE,
          AdjustSize,
          AML_NOOP_OP
          );
        PssTable->NumEntries  = (UINT8) CpuMiscData->NumberOfPStates;
        PssTableItemPtr       = (PSS_PACKAGE *) ((UINT8 *) PssTable + sizeof (ACPI_NAME_COMMAND));

        //
        // Update the size
        //

        if (CpuMiscData->NumberOfPStates == 1) {
          mPStateEnable = 0;
        }

        for (TableIndex = 0; TableIndex < CpuMiscData->NumberOfPStates; ++TableIndex) {
          PssTableItemPtr->CoreFreq = (UINT32) (CpuMiscData->IntendedFsbFrequency * PssState[TableIndex].Ratio);
          if (mSocketPowermanagementConfiguration.TurboMode && (TableIndex == 0) && IsTurboModeEnabled ()) {
            PssTableItemPtr->CoreFreq = (UINT32)((CpuMiscData->IntendedFsbFrequency * PssState[TableIndex + 1].Ratio) + 1);
          }


          PssTableItemPtr->Power    =  (UINT32)(PssState[TableIndex].Power); // when calulate Tdp already make it mW;
          if (PssTable->NameStr == SIGNATURE_32 ('N', 'P', 'S', 'S')) {
            PssTableItemPtr->TransLatency = (UINT32)(PssState[TableIndex].TransitionLatency);
              PssTableItemPtr->Control  = (UINT32)(PssState[TableIndex].Ratio << 8);
              PssTableItemPtr->Status   = (UINT32)(PssState[TableIndex].Ratio << 8);
          } else {
            //
            // This method should be supported by SMM PPM Handler
            //
            // Status is simply the state number.
            // Use the state number w/ OS command value so that the
            // legacy interface may be used.  Latency for SMM is 100 + BM latency.
            PssTableItemPtr->Status   = (UINT32)TableIndex;
            PssTableItemPtr->TransLatency = (UINT32)(100 + PssState[TableIndex].TransitionLatency);
            PssTableItemPtr->Control  = (UINT32)(SW_SMI_OS_REQUEST | (TableIndex << 8));
          }

          PssTableItemPtr->BMLatency    = (UINT32)(PssState[TableIndex].BusMasterLatency);

          ++PssTableItemPtr;
        }

        if (PssTable->NameStr == SIGNATURE_32 ('N', 'P', 'S', 'S')) {
          ++NpssFixes;
        }

        SsdtPointer = (UINT8 *) PssTable + PackageSize;
        break;

      case SIGNATURE_32 ('G', 'P', 'S', 'S'):

        PssTable = ACPI_NAME_COMMAND_FROM_NAME_STR (SsdtPointer);
        if (PssTable->StartByte != AML_NAME_OP) {
          continue;
        }

        ASSERT (CpuMiscData != NULL);
        PssState = CpuMiscData->GreaterFvidTable;

        ASSERT (CpuMiscData->GreaterNumberOfPStates <= GPSS_FVID_MAX_STATES);
        if (CpuMiscData->GreaterNumberOfPStates > GPSS_FVID_MAX_STATES) {
          continue;
        }

        AdjustSize  = PssTable->NumEntries * sizeof (PSS_PACKAGE);
        AdjustSize -= (UINT32)(CpuMiscData->GreaterNumberOfPStates * sizeof (PSS_PACKAGE));
        PackageSize     = (PssTable->Size & 0xF) + ((PssTable->Size & 0xFF00) >> 4);
        NewPackageSize  = PackageSize - AdjustSize;
        PssTable->Size  = (UINT16) ((NewPackageSize & 0xF) + ((NewPackageSize & 0x0FF0) << 4));

        //
        // Set most significant two bits of byte zero to 01, meaning two bytes used
        //
        PssTable->Size |= 0x40;

        //
        // Set unused table to Noop Code
        //
        SetMem (
          (UINT8 *) PssTable + NewPackageSize + AML_NAME_PREFIX_SIZE,
          AdjustSize,
          AML_NOOP_OP
          );
        PssTable->NumEntries  = (UINT8) CpuMiscData->GreaterNumberOfPStates;
        PssTableItemPtr       = (PSS_PACKAGE *) ((UINT8 *) PssTable + sizeof (ACPI_NAME_COMMAND));

        //
        // Update the size
        //
        for (TableIndex = 0; TableIndex < CpuMiscData->GreaterNumberOfPStates; ++TableIndex) {
          PssTableItemPtr->CoreFreq = (UINT32) (CpuMiscData->IntendedFsbFrequency * PssState[TableIndex].Ratio);
          if (mSocketPowermanagementConfiguration.TurboMode && (TableIndex == 0) && IsTurboModeEnabled()) {
            PssTableItemPtr->CoreFreq = (UINT32)((CpuMiscData->IntendedFsbFrequency * PssState[TableIndex + 1].Ratio) + 1);
          }

          //
          // If Turbo mode is supported, add one to the Max Non-Turbo frequency
          //
          PssTableItemPtr->Power = (UINT32)(PssState[TableIndex].Power); // when calulate Tdp already make it mW;
          if (PssTable->NameStr == SIGNATURE_32 ('G', 'P', 'S', 'S')) {
            PssTableItemPtr->TransLatency = (UINT32)(PssState[TableIndex].TransitionLatency);
            PssTableItemPtr->Control  = (UINT32)(PssState[TableIndex].Ratio << 8);
            PssTableItemPtr->Status   = (UINT32)(PssState[TableIndex].Ratio << 8);
          } else {
            //
            // This method should be supported by SMM PPM Handler
            //
            // Status is simply the state number.
            // Use the state number w/ OS command value so that the
            // legacy interface may be used.  Latency for SMM is 100 + BM latency.
            //
            PssTableItemPtr->Status   = (UINT32)TableIndex;
            PssTableItemPtr->TransLatency = (UINT32)(100 + PssState[TableIndex].TransitionLatency);
            PssTableItemPtr->Control  = (UINT32)(SW_SMI_OS_REQUEST | (TableIndex << 8));
          }

          PssTableItemPtr->BMLatency    = (UINT32)(PssState[TableIndex].BusMasterLatency);

          ++PssTableItemPtr;
        }

        if (PssTable->NameStr == SIGNATURE_32 ('G', 'P', 'S', 'S')) {
          ++GpssFixes;
        }

        SsdtPointer = (UINT8 *) PssTable + PackageSize;
        break;

      default:
        break;
    } // switch
  } // for

  //
  // N fixes together currently
  //
  ASSERT (CpuFixes == (UINT32) MAX_CPU_NUM);

  if (!mPStateEnable || !mSocketPowermanagementConfiguration.ProcessorEistEnable || (mSocketPowermanagementConfiguration.ProcessorHWPMEnable > HWP_MODE_NATIVE) ) {
      Status = EFI_UNSUPPORTED;  //CPU EIST
      return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PatchSsdtTable (
  IN OUT EFI_ACPI_COMMON_HEADER     *Table,
  IN OUT EFI_ACPI_TABLE_VERSION     *Version
  )
{
  EFI_STATUS                    Status;
  EFI_ACPI_DESCRIPTION_HEADER   *TableHeader;

  Status = EFI_SUCCESS;
  TableHeader = (EFI_ACPI_DESCRIPTION_HEADER   *)Table;

    //
    // Do not load the xHCI table. It is handled by separate function.
    //
    TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) Table;
    if (CompareMem (&TableHeader->OemTableId, "xh_", 3) == 0) {
      DEBUG ((DEBUG_ERROR,"Xhci TableHeader->OemTableId = %x\n ", TableHeader->OemTableId));
      *Version = EFI_ACPI_TABLE_VERSION_NONE;
    }

    if (TableHeader->OemTableId == SIGNATURE_64 ('S', 'S', 'D', 'T', ' ', ' ', 'P', 'M')) {
      PatchCpuPmSsdtTable (Table);  //CPU PM
    }

  return Status;
}

/**
  Update the OEM2 HWP SSDT table if needed

  @param *TableHeader   - The table to be set

  @retval EFI_SUCCESS -  Returns Success
**/
EFI_STATUS
PatchOem2SsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER  *Table
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;

  if ((mSocketPowermanagementConfiguration.ProcessorHWPMEnable == 0) || (mSocketPowermanagementConfiguration.ProcessorHWPMEnable == HWP_MODE_OOB)) {
    Status = EFI_UNSUPPORTED;
  }

  return Status;
}

/**
  Update the OEM3 T-State SSDT table (TST)

  @param *TableHeader   - The table to be set

  @retval EFI_SUCCESS -  Returns Success
**/
EFI_STATUS
PatchOem3SsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER  *Table
  )
{
  UINT8                       *CurrPtr;
  UINT8                       *EndPtr;
  UINT8                       *SsdtPointer;
  UINT32                      Signature;
  UINT32                      CpuFixes;
  UINT32                      CpuSkt;
  UINT32                      CpuIndex;
  UINT32                      ThreadIndex;
  UINT32                      AdjustSize;
  UINT32                      DomnValue;
  ACPI_NAME_COMMAND           *PsdPackage;
  PSD_PACKAGE_LAYOUT          *PsdPackageItemPtr;

  DEBUG ((DEBUG_INFO, "Patching SSDT PatchOem3SsdtTable\n"));

  if (!mSocketPowermanagementConfiguration.TStateEnable || (mSocketPowermanagementConfiguration.ProcessorHWPMEnable > HWP_MODE_NATIVE) ) {
    return EFI_UNSUPPORTED;
  }

  //
  // Loop through the ASL looking for values that we must fix up.
  //
  DomnValue = 0;
  CpuFixes  = 0;
  CpuSkt    = 0;
  ThreadIndex  = 0;
  CurrPtr = (UINT8 *) Table;
  EndPtr  = (CurrPtr + ((EFI_ACPI_COMMON_HEADER *) CurrPtr)->Length);

  //
  // CurrPtr = beginning of table we want to search
  //
  CurrPtr = SkipExternalSbOpcodes(CurrPtr, EndPtr, (UINT32) MAX_CPU_NUM);

  //
  // Subtract 11 from EndPtr - this is the size of the larget data item we will read
  //  so that we don't read beyond the end of the table
  //
  EndPtr -= 11;

  for (SsdtPointer = CurrPtr; SsdtPointer <= EndPtr; ++SsdtPointer) {
    Signature = *(UINT32 *) SsdtPointer;
    CpuIndex  = 0;
    AdjustSize  = 0;

    switch (Signature) {

    case SIGNATURE_32 ('_', 'S', 'B', '_'):
      //
      // SKTX
      //
      CpuSkt = *(SsdtPointer + 7);
      CpuSkt -= '0';

      if ((*(SsdtPointer + 8) != 'C')) {
        continue;
      }

      if ((*(SsdtPointer + 11) > '0') && (*(SsdtPointer + 11) <= '9')) {
        CpuIndex = (*(SsdtPointer + 11) -'0');
      } else if ((*(SsdtPointer + 11) >= 'A') && (*(SsdtPointer + 11) <= 'F')) {
        CpuIndex = (*(SsdtPointer + 11) -'A' + 10);
      }

      if ((*(SsdtPointer + 10) > '0') && ( *(SsdtPointer + 10) <= '9')) {
        AdjustSize = (*(SsdtPointer + 10) -'0') * 0x10;
      } else if ((*(SsdtPointer + 10) >= 'A') && (*(SsdtPointer + 10) <= 'F')) {
        AdjustSize = (*(SsdtPointer + 10) -'A' + 10) * 0x10;
      }

      CpuIndex += AdjustSize;
      AdjustSize  = 0;

      if ((*(SsdtPointer + 9) > '0') && (*(SsdtPointer + 9) <= '9')) {
        AdjustSize = (*(SsdtPointer + 9) -'0') * 0x100;
      } else if ((*(SsdtPointer + 9) >= 'A') && (*(SsdtPointer + 9) <= 'F')) {
        AdjustSize = (*(SsdtPointer + 9) -'A' + 10) * 0x100;
      }

      CpuIndex += AdjustSize;
      ThreadIndex = CpuIndex;

      //
      // PCPS - Update DOMN
      //
      DomnValue = (UINT8) CpuSkt;

      if ((mCpuPCPSInfo[CpuSkt] & B_PCPS_DISABLE) == 0) {
        DomnValue = LocateApicIdInfo (CpuSkt, ThreadIndex);

        if (mNcpuValue[CpuSkt] == 2) {
          DomnValue = (DomnValue >> 1);
        }
      }

      DEBUG ((
        DEBUG_VERBOSE,
        "PatchOem3SsdtTable(): CpuIndex: 0x%x ThreadIndex: 0x%x CpuFixes: 0x%x (%d)\n",
        CpuIndex,
        ThreadIndex,
        CpuFixes,
        CpuFixes
        ));

      DEBUG ((
        DEBUG_INFO,
        "PatchOem3SsdtTable(): CpuSkt: %d CpuIndex: 0x%x, NcpuValue = 0x%x, DomnValue = 0x%x\n",
        CpuSkt,
        CpuIndex,
        mNcpuValue[CpuSkt],
        DomnValue
        ));
      ++CpuFixes;
      break;

    case SIGNATURE_32 ('T', 'S', 'D', 'C'):
    case SIGNATURE_32 ('T', 'S', 'D', 'D'):
      PsdPackage = ACPI_NAME_COMMAND_FROM_NAME_STR (SsdtPointer);
      if (PsdPackage->StartByte != AML_NAME_OP) {
        continue;
      }

      PsdPackageItemPtr       = (PSD_PACKAGE_LAYOUT *) ((UINT8 *) PsdPackage);
      DEBUG ((
        DEBUG_VERBOSE,
        "TSDC: PsdPackageItemPtr table: %x is detected...\n",
        PsdPackage->NameStr
        ));
      DEBUG ((
        DEBUG_VERBOSE,
        "   Initial Values:     Domain = %x,    CoordType = %x,   NumProcessors = %x\n",
        PsdPackageItemPtr->Domain,
        PsdPackageItemPtr->CoordType,
        PsdPackageItemPtr->NumProcessors
        ));

      PsdPackageItemPtr->Domain = DomnValue;
      PsdPackageItemPtr->NumProcessors = mNcpuValue[CpuSkt];
      DEBUG ((
        DEBUG_VERBOSE,
        "   PsdPackage = %x,    PsdPackageItemPtr = %x,   SsdtPointer = %x\n",
        (UINT8 *)PsdPackage,
        (UINT8 *)PsdPackageItemPtr,
        (UINT8 *)SsdtPointer
        ));
      DEBUG ((
        DEBUG_VERBOSE,
        "   Updated TSD Domain = %x,    CoordType = %x,   NumProcessors = %x\n",
        PsdPackageItemPtr->Domain,
        PsdPackageItemPtr->CoordType,
        PsdPackageItemPtr->NumProcessors
        ));
      break;

    default:
      break;
    } // switch
  } // for

  //
  // N fixes together currently
  //
  ASSERT (CpuFixes == (UINT32) MAX_CPU_NUM);

  return EFI_SUCCESS;
}

/**
  Update the OEM4 C State SSDT table (CST)

  @param *TableHeader   - The table to be set

  @retval EFI_SUCCESS -  Returns Success
**/
EFI_STATUS
PatchOem4SsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER  *Table
  )
{
  return EFI_SUCCESS;
}
