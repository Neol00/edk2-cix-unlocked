/** @file

  Copyright 2023-2024 Cix Technology (Shanghai) Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "AcpiPlatformDxe.h"
#include <Protocol/ArmScmiPerformanceProtocol.h>

// Turn on debug message by enabling below define
#define ACPI_DEBUG

#ifdef ACPI_DEBUG
#define DBG(arg ...)  DEBUG((EFI_D_ERROR,## arg))
// #define DBG(arg ...)  DebugPrint(EFI_D_ERROR,## arg)
#else
#define DBG(arg ...)
#endif

#define EFI_ACPI_MAX_NUM_TABLES  20
#define DSDT_SIGNATURE           0x54445344

#define CPC_HIGHEST_PERFORMANCE_OFFSET           2
#define CPC_NOMINAL_PERFORMANCE_OFFSET           3
#define CPC_LOWEST_NONLINEAR_PERFORMANCE_OFFSET  4
#define CPC_LOWEST_PERFORMANCE_OFFSET            5
#define CPC_LOWEST_FREQUENCY_OFFSET              21
#define CPC_NOMINAL_FREQUENCY_OFFSET             22
#define CPC_GRANULARITYMHZ                       1

CHAR8   *mCixAcpiDevId[]              = { "ACPI0007" };
UINT32  mCpuUIDFastChannelMapping[12] = { 2, 2, 2, 2, 5, 5, 6, 6, 3, 3, 4, 4 };

typedef enum {
  DsdtDeviceUnknown,
  DsdtDeviceCpu
} DSDT_DEVICE_TYPE;

STATIC
EFI_STATUS
GetCpuPerfData (
  IN     UINTN  CpuID,
  IN OUT UINTN  *HighestPerf,
  IN OUT UINTN  *NominalPerf,
  IN OUT UINTN  *LowestNonlinearPerf,
  IN OUT UINTN  *LowestPerf
  )
{
  EFI_STATUS                 Status;
  SCMI_PERFORMANCE_PROTOCOL  *ScmiPerfProtocol = NULL;
  UINT32                     DomainId;
  UINT32                     NumLevels;
  UINT32                     LevelArraySize = 0;
  SCMI_PERFORMANCE_LEVEL     *LevelArra;

  DBG ("CpuUid = %d\n", CpuID);

  if (CpuID > ARRAY_SIZE (mCpuUIDFastChannelMapping)-1) {
    DEBUG ((EFI_D_ERROR, " Cpuid is over the max range, max cpuid = %d, current cpu id = %d\n", ARRAY_SIZE (mCpuUIDFastChannelMapping)-1, CpuID));
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (
                  &gArmScmiPerformanceProtocolGuid,
                  NULL,
                  (VOID **)&ScmiPerfProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "gArmScmiPerformanceProtocolGuid NOT found.\n"));
    return Status;
  }

  DomainId = mCpuUIDFastChannelMapping[CpuID];

  Status = ScmiPerfProtocol->DescribeLevels (ScmiPerfProtocol, DomainId, &NumLevels, &LevelArraySize, LevelArra);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Status = gBS->AllocatePool (EfiBootServicesData, LevelArraySize, (VOID **)&LevelArra);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, " Out of memory resource\n"));
      return Status;
    }

    Status = ScmiPerfProtocol->DescribeLevels (ScmiPerfProtocol, DomainId, &NumLevels, &LevelArraySize, LevelArra);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, " Perfomance [%d] get DescribeLevels failed.\n", DomainId));
      return Status;
    }

    for (UINT8 i = 0; i < NumLevels; i++) {
      DBG ("[CPU%d] Performace_Level = [%d].   Level = [%d]    ,  PowerCost = [%d], Latency = [%d]\n", CpuID, i, LevelArra[i].Level, LevelArra[i].PowerCost, LevelArra[i].Latency);
    }

    *HighestPerf         = LevelArra[NumLevels-1].Level;
    *NominalPerf         = LevelArra[NumLevels-1].Level;
    *LowestNonlinearPerf = LevelArra[0].Level;
    *LowestPerf          = LevelArra[0].Level;
  } else {
    DEBUG ((EFI_D_ERROR, " Perfomance [%d] get DescribeLevels failed.\n", DomainId));
    return Status;
  }

  DBG ("HighestPerf = %x\n", *HighestPerf);
  DBG ("NominalPerf = %x\n", *NominalPerf);
  DBG ("LowestNonlinearPerf = %x\n", *LowestNonlinearPerf);
  DBG ("LowestPerf = %x\n", *LowestPerf);
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
UpdatePerfInOption (
  IN EFI_ACPI_SDT_PROTOCOL  *AcpiTableProtocol,
  IN EFI_ACPI_HANDLE        ChildHandle,
  IN UINTN                  CpuUid
  )
{
  EFI_STATUS          Status;
  EFI_ACPI_DATA_TYPE  DataType;
  CONST UINT8         *Data;
  CONST VOID          *Buffer;
  UINTN               DataSize;
  UINTN               Index = 0;
  UINTN               NumEntries;
  EFI_ACPI_HANDLE     CurrentHandle = NULL;
  UINTN               HighestPerf;
  UINTN               NominalPerf;
  UINTN               LowestNonlinearPerf;
  UINTN               LowestPerf;
  UINTN               PerfData;
  BOOLEAN             UpdatePerf;

  // Get NumEntries from package
  Status = AcpiTableProtocol->GetChild (ChildHandle, &CurrentHandle);
  if (EFI_ERROR (Status) || (CurrentHandle == NULL)) {
    DBG ("Get NumEntries from package fail, %r\n", NumEntries, Status);
    return Status;
  }

  Status = AcpiTableProtocol->GetOption (CurrentHandle, 1, &DataType, &Buffer, &DataSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (DataType != EFI_ACPI_DATA_TYPE_UINT) {
    return EFI_UNSUPPORTED;
  }

  Data       = Buffer;
  NumEntries = *(UINT8 *)Data;
  DBG ("_CPC pakage NumEntries =  %d\n", NumEntries);
  Index++;

  // Get CPU performace parameter
  Status = GetCpuPerfData (CpuUid, &HighestPerf, &NominalPerf, &LowestNonlinearPerf, &LowestPerf);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Walk node in package
  for ( ; Index < NumEntries; Index++) {
    Status = AcpiTableProtocol->GetChild (ChildHandle, &CurrentHandle);
    if (EFI_ERROR (Status) || (CurrentHandle == NULL)) {
      return EFI_UNSUPPORTED;
    }

    switch (Index) {
      case CPC_HIGHEST_PERFORMANCE_OFFSET:
        PerfData   = HighestPerf;
        UpdatePerf = TRUE;
        break;
      case CPC_NOMINAL_PERFORMANCE_OFFSET:
        PerfData   = NominalPerf;
        UpdatePerf = TRUE;
        break;
      case CPC_LOWEST_NONLINEAR_PERFORMANCE_OFFSET:
        PerfData   = LowestNonlinearPerf;
        UpdatePerf = TRUE;
        break;
      case CPC_LOWEST_PERFORMANCE_OFFSET:
        PerfData   = LowestPerf;
        UpdatePerf = TRUE;
        break;
      case CPC_LOWEST_FREQUENCY_OFFSET:
        PerfData   = LowestPerf * CPC_GRANULARITYMHZ;
        UpdatePerf = TRUE;
        break;
      case CPC_NOMINAL_FREQUENCY_OFFSET:
        PerfData   = NominalPerf * CPC_GRANULARITYMHZ;
        UpdatePerf = TRUE;
        break;
      default:
        UpdatePerf = FALSE;
    }

    if (UpdatePerf) {
      // Get node data type
      Status = AcpiTableProtocol->GetOption (CurrentHandle, 0, &DataType, &Buffer, &DataSize);
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "_CPC package index %d get option fail: %r\n", Index, Status));
        continue;
      }

      Data = Buffer;
      if ((Data[0] == AML_BYTE_PREFIX) || (Data[0] == AML_WORD_PREFIX) || (Data[0] == AML_DWORD_PREFIX) || (Data[0] == AML_QWORD_PREFIX)) {
        Status = AcpiTableProtocol->GetOption (CurrentHandle, 1, &DataType, &Buffer, &DataSize);
        if (EFI_ERROR (Status)) {
          continue;
        }

        DBG ("DataType =  %x, DataSize = %x\n", DataType, DataSize);

        // FIXME: Assume the CPU is little endian
        Status = AcpiTableProtocol->SetOption (
                                      CurrentHandle,
                                      1,
                                      &PerfData,
                                      DataSize
                                      );
        if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_ERROR, "_CPC package index %d set option fail: %r\n", Index, Status));
          continue;
        }

        DBG ("_CPC package index %d update success, PerfData = %x\n", Index, PerfData);
      } else if (Data[0] == AML_BUFFER_OP) {
        // Cix Sky1 current not support
      } else {
        DEBUG ((EFI_D_ERROR, "_CPC package index %d update fail, type is not dword or buffer\n", Index));
      }
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GetCpuUID (
  EFI_ACPI_SDT_PROTOCOL  *AcpiTableProtocol,
  EFI_ACPI_HANDLE        ChildHandle,
  UINTN                  *DevUID
  )
{
  EFI_STATUS          Status;
  EFI_ACPI_DATA_TYPE  DataType;
  CONST UINT8         *Data;
  CONST VOID          *Buffer;
  UINTN               DataSize;
  EFI_ACPI_HANDLE     DevHandle;

  Status = AcpiTableProtocol->GetOption (ChildHandle, 0, &DataType, &Buffer, &DataSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Data = Buffer;
  //
  // Skip all non-device type
  //
  if ((DataSize != 2) || (Data[0] != AML_EXT_OP) || (Data[1] != AML_EXT_DEVICE_OP)) {
    return EFI_NOT_FOUND;
  }

  //
  // Walk the device type node
  //
  for (DevHandle = NULL; ; ) {
    Status = AcpiTableProtocol->GetChild (ChildHandle, &DevHandle);
    if (EFI_ERROR (Status) || (DevHandle == NULL)) {
      break;
    }

    //
    // Search for _UID
    //
    Status = AcpiTableProtocol->GetOption (DevHandle, 0, &DataType, &Buffer, &DataSize);
    if (EFI_ERROR (Status)) {
      break;
    }

    Data = Buffer;
    DBG ("Data Type 0x%02X %02X\n", Data[0], DataSize > 1 ? Data[1] : 0);
    if ((DataSize == 1) && (Data[0] == AML_NAME_OP)) {
      Status = AcpiTableProtocol->GetOption (DevHandle, 1, &DataType, &Buffer, &DataSize);
      if (EFI_ERROR (Status)) {
        break;
      }

      Data = Buffer;
      if (DataType == EFI_ACPI_DATA_TYPE_NAME_STRING) {
        if (AsciiStrnCmp ((CHAR8 *)Data, "_UID", 4) == 0) {
          EFI_ACPI_HANDLE  ValueHandle;
          Status = AcpiTableProtocol->GetOption (DevHandle, 2, &DataType, &Buffer, &DataSize);
          if (EFI_ERROR (Status)) {
            break;
          }

          if (DataType != EFI_ACPI_DATA_TYPE_CHILD) {
            continue;
          }

          AcpiTableProtocol->Open ((VOID *)Buffer, &ValueHandle);
          Status = AcpiTableProtocol->GetOption (ValueHandle, 1, &DataType, &Buffer, &DataSize);

          Data = Buffer;
          DBG ("[%a:%d] - _UID = %x , DataType = %x\n", __FUNCTION__, __LINE__, *(UINT8 *)Data, DataType);
          if (EFI_ERROR (Status)) {
            AcpiTableProtocol->Close (ValueHandle);
            continue;
          }

          // According to ACPI spec chap 6.1.12, _UID in CPU must return an integer
          *DevUID = *(UINT8 *)Data;
          break;
        }
      }
    }
  }

  return Status;
}

EFI_STATUS
GetDeviceInfo (
  EFI_ACPI_SDT_PROTOCOL  *AcpiTableProtocol,
  EFI_ACPI_HANDLE        ChildHandle,
  UINTN                  *DevID,
  DSDT_DEVICE_TYPE       *FoundDev
  )
{
  EFI_STATUS          Status;
  EFI_ACPI_DATA_TYPE  DataType;
  CHAR8               Data[5];
  CONST VOID          *Buffer;
  UINTN               DataSize;

  // Get NameString
  Status = AcpiTableProtocol->GetOption (ChildHandle, 1, &DataType, &Buffer, &DataSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[%a:%d] Get NameString failed: %r\n", __FUNCTION__, __LINE__, Status));
    return Status;
  }

  CopyMem (Data, Buffer, 4);
  DBG ("Size %p Data %02x %02x %02x %02x\n", DataSize, Data[0], Data[1], Data[2], Data[3]);

  Data[4] = '\0';
  if ((DataSize != 4) || ((Data[3] > '9') || (Data[3] < '0'))) {
    DEBUG ((DEBUG_ERROR, "The NameString %a is not CPUn\n", Data));
    return EFI_INVALID_PARAMETER;
  }

  if (AsciiStrnCmp ("CP", Data, 2) == 0) {
    *FoundDev = DsdtDeviceCpu;
    Status    = GetCpuUID (AcpiTableProtocol, ChildHandle, DevID);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Get Cpu UID fail\n"));
    }
  } else {
    DEBUG (
      (DEBUG_ERROR, "[%a:%d] The NameString %a is not CPUn\n",
       __FUNCTION__, __LINE__, Data)
      );
    return EFI_INVALID_PARAMETER;
  }

  return Status;
}

EFI_STATUS
ProcessDSDTDevice (
  EFI_ACPI_SDT_PROTOCOL  *AcpiTableProtocol,
  EFI_ACPI_HANDLE        ChildHandle
  )
{
  EFI_STATUS          Status;
  EFI_ACPI_DATA_TYPE  DataType;
  CONST UINT8         *Data;
  CONST VOID          *Buffer;
  UINTN               DataSize;
  EFI_ACPI_HANDLE     DevHandle;
  DSDT_DEVICE_TYPE    FoundDev = DsdtDeviceUnknown;
  UINTN               DevNextID;
  BOOLEAN             CixAcpiDevNotFound;
  UINTN               Index;

  Status = AcpiTableProtocol->GetOption (ChildHandle, 0, &DataType, &Buffer, &DataSize);
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  Data = Buffer;
  //
  // Skip all non-device type
  //
  if ((DataSize != 2) || (Data[0] != AML_EXT_OP) || (Data[1] != AML_EXT_DEVICE_OP)) {
    return EFI_SUCCESS;
  }

  //
  // Walk the device type node
  //
  for (DevHandle = NULL; ; ) {
    Status = AcpiTableProtocol->GetChild (ChildHandle, &DevHandle);
    if (EFI_ERROR (Status) || (DevHandle == NULL)) {
      break;
    }

    //
    // Search for _HID with Device ID
    //
    Status = AcpiTableProtocol->GetOption (DevHandle, 0, &DataType, &Buffer, &DataSize);
    if (EFI_ERROR (Status)) {
      break;
    }

    Data = Buffer;
    DBG ("Data Type 0x%02X %02X\n", Data[0], DataSize > 1 ? Data[1] : 0);
    if ((DataSize == 1) && (Data[0] == AML_NAME_OP)) {
      Status = AcpiTableProtocol->GetOption (DevHandle, 1, &DataType, &Buffer, &DataSize);
      if (EFI_ERROR (Status)) {
        break;
      }

      Data = Buffer;
      if (DataType == EFI_ACPI_DATA_TYPE_NAME_STRING) {
        if (AsciiStrnCmp ((CHAR8 *)Data, "_HID", 4) == 0) {
          EFI_ACPI_HANDLE  ValueHandle;

          Status = AcpiTableProtocol->GetOption (DevHandle, 2, &DataType, &Buffer, &DataSize);
          if (EFI_ERROR (Status)) {
            break;
          }

          if (DataType != EFI_ACPI_DATA_TYPE_CHILD) {
            continue;
          }

          AcpiTableProtocol->Open ((VOID *)Buffer, &ValueHandle);
          Status = AcpiTableProtocol->GetOption (ValueHandle, 1, &DataType, &Buffer, &DataSize);

          Data = Buffer;
          DBG ("[%a:%d] - _HID = %a\n", __FUNCTION__, __LINE__, Data);

          if (EFI_ERROR (Status) ||
              (DataType != EFI_ACPI_DATA_TYPE_STRING))
          {
            AcpiTableProtocol->Close (ValueHandle);
            FoundDev = DsdtDeviceUnknown;
            continue;
          }

          CixAcpiDevNotFound = TRUE;
          for (Index = 0; Index < ARRAY_SIZE (mCixAcpiDevId); Index++) {
            if (AsciiStrCmp ((CHAR8 *)Data, mCixAcpiDevId[Index]) == 0) {
              CixAcpiDevNotFound = FALSE;
              break;
            }
          }

          if (CixAcpiDevNotFound) {
            AcpiTableProtocol->Close (ValueHandle);
            FoundDev = DsdtDeviceUnknown;
            continue;
          }

          DBG ("Found device\n");
          AcpiTableProtocol->Close (ValueHandle);
          Status = GetDeviceInfo (AcpiTableProtocol, ChildHandle, &DevNextID, &FoundDev);
          if (EFI_ERROR (Status)) {
            continue;
          }
        } else if ((FoundDev == DsdtDeviceCpu) && (AsciiStrnCmp ((CHAR8 *)Data, "_CPC", 4) == 0)) {
          //
          // Patch CPC data
          //
          EFI_ACPI_HANDLE  PkgHandle;
          Status = AcpiTableProtocol->GetOption (DevHandle, 2, &DataType, &Buffer, &DataSize);
          if (EFI_ERROR (Status)) {
            break;
          }

          if (DataType != EFI_ACPI_DATA_TYPE_CHILD) {
            continue;
          }

          //
          // Open package data
          //
          AcpiTableProtocol->Open ((VOID *)Buffer, &PkgHandle);
          Status = AcpiTableProtocol->GetOption (PkgHandle, 0, &DataType, &Buffer, &DataSize);

          Data = Buffer;
          DBG (
            "_CPC Subnode Store Op Code 0x%02X %02X\n",
            Data[0],
            DataSize > 1 ? Data[1] : 0
            );

          //
          // Walk the _CPC node
          //
          if ((DataSize == 1) && (Data[0] == AML_PACKAGE_OP)) {
            Status = UpdatePerfInOption (AcpiTableProtocol, PkgHandle, DevNextID);
            // Status = SearchReplacePackageAddress (AcpiTableProtocol, PkgHandle, DevNextID, FoundDev);
          }

          AcpiTableProtocol->Close (PkgHandle);
        }
      }
    } else if ((DataSize == 2) && (Data[0] == AML_EXT_OP) && (Data[1] == AML_EXT_DEVICE_OP)) {
      ProcessDSDTDevice (AcpiTableProtocol, DevHandle);
    }
  }

  return EFI_SUCCESS;
}

BOOLEAN
IsSbScope (
  EFI_ACPI_SDT_PROTOCOL  *AcpiTableProtocol,
  EFI_ACPI_HANDLE        ChildHandle
  )
{
  EFI_STATUS          Status;
  EFI_ACPI_DATA_TYPE  DataType;
  CONST UINT8         *Data;
  CONST VOID          *Buffer;
  UINTN               DataSize;

  Status = AcpiTableProtocol->GetOption (ChildHandle, 0, &DataType, &Buffer, &DataSize);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  Data = Buffer;
  if ((DataSize != 1) || (Data[0] != AML_SCOPE_OP)) {
    return FALSE;
  }

  return TRUE;
}

EFI_STATUS
ProcessDSDTChild (
  EFI_ACPI_SDT_PROTOCOL  *AcpiTableProtocol,
  EFI_ACPI_HANDLE        ChildHandle
  )
{
  EFI_STATUS       Status;
  EFI_ACPI_HANDLE  DevHandle;

  // Check Scope(_SB) at first
  if (!IsSbScope (AcpiTableProtocol, ChildHandle)) {
    return ProcessDSDTDevice (AcpiTableProtocol, ChildHandle);
  }

  for (DevHandle = NULL; ; ) {
    Status = AcpiTableProtocol->GetChild (ChildHandle, &DevHandle);
    if (EFI_ERROR (Status) || (DevHandle == NULL)) {
      break;
    }

    ProcessDSDTDevice (AcpiTableProtocol, DevHandle);
  }

  return EFI_SUCCESS;
}

static EFI_STATUS
ProcessDSDT (
  EFI_ACPI_SDT_PROTOCOL  *AcpiTableProtocol,
  EFI_ACPI_HANDLE        TableHandle
  )
{
  EFI_STATUS       Status;
  EFI_ACPI_HANDLE  ChildHandle;

  //
  // Parse table for device type
  DBG ("[%a:%d] - TableHandle=%p\n", __FUNCTION__, __LINE__, TableHandle);
  for (ChildHandle = NULL; ; ) {
    Status = AcpiTableProtocol->GetChild (TableHandle, &ChildHandle);
    DBG ("[%a:%d] - Child=%p, %r\n", __FUNCTION__, __LINE__, ChildHandle, Status);
    if (EFI_ERROR (Status)) {
      break;
    }

    if (ChildHandle == NULL) {
      break;
    }

    ProcessDSDTChild (AcpiTableProtocol, ChildHandle);
  }

  return EFI_SUCCESS;
}

STATIC
VOID
AcpiCheckSum (
  IN OUT  EFI_ACPI_SDT_HEADER  *Table
  )
{
  UINTN  ChecksumOffset;
  UINT8  *Buffer;

  ChecksumOffset = OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER, Checksum);
  Buffer         = (UINT8 *)Table;

  //
  // set checksum to 0 first
  //
  Buffer[ChecksumOffset] = 0;

  //
  // Update checksum value
  //
  Buffer[ChecksumOffset] = CalculateCheckSum8 (Buffer, Table->Length);
}

EFI_STATUS
UpdateAcpiDsdtTable (
  VOID
  )
{
  EFI_STATUS              Status;
  EFI_ACPI_SDT_PROTOCOL   *AcpiTableProtocol;
  EFI_ACPI_SDT_HEADER     *Table;
  EFI_ACPI_TABLE_VERSION  TableVersion;
  UINTN                   TableKey;
  EFI_ACPI_HANDLE         TableHandle;
  UINTN                   i;

  DEBUG ((EFI_D_ERROR, "Updating CPU _CPC in ACPI DSDT...\n"));

  //
  // Find the AcpiTable protocol
  Status = gBS->LocateProtocol (&gEfiAcpiSdtProtocolGuid, NULL, (VOID **)&AcpiTableProtocol);
  if (EFI_ERROR (Status)) {
    DBG ("Unable to locate ACPI table protocol\n");
    return Status;
  }

  //
  // Search for DSDT Table
  for (i = 0; i < EFI_ACPI_MAX_NUM_TABLES; i++) {
    Status = AcpiTableProtocol->GetAcpiTable (i, &Table, &TableVersion, &TableKey);
    if (EFI_ERROR (Status)) {
      break;
    }

    if (Table->Signature != DSDT_SIGNATURE) {
      continue;
    }

    Status = AcpiTableProtocol->OpenSdt (TableKey, &TableHandle);
    if (EFI_ERROR (Status)) {
      break;
    }

    ProcessDSDT (AcpiTableProtocol, TableHandle);

    AcpiTableProtocol->Close (TableHandle);
    AcpiCheckSum (Table);
  }

  return EFI_SUCCESS;
}
