/** @file
  Dynamic link silicon library service access Protocol for later boot functions

  This protocol abstracts silicon static library accesses via a protocol

  @copyright
  Copyright 2021 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _DYNAMIC_SI_LIBARY_PROTOCOL2_H_
#define _DYNAMIC_SI_LIBARY_PROTOCOL2_H_

#include <Protocol/IioUds.h>
#include <Protocol/CpuCsrAccess.h>
#include <IndustryStandard/Acpi.h>
#include <Library/CpuConfigLib.h>
#include <IndustryStandard/Acpi.h>

#define DYNAMIC_SI_LIBARY_PROTOCOL2_GUID { 0x98bdd399, 0x9349, 0x4131, { 0x87, 0x60, 0x90, 0xaf, 0x68, 0x01, 0x21, 0xee } }

#define DYNAMIC_SI_LIBARY_PROTOCOL2_SIGNATURE  SIGNATURE_32('D', 'S', 'L', '2')
#define DYNAMIC_SI_LIBARY_PROTOCOL2_VERSION    0x01

//
// Functions
//

typedef
UINT32
(EFIAPI *DXE_GetVtdBar) (
  IN UINT8     SocId,
  IN UINT8     StackId
  );

typedef
UINT8
(EFIAPI *DXE_GetMaxPortPerSocket) (
  IN UINT8     SocId
  );

typedef
UINT8
(EFIAPI *DXE_GetStackPerPort) (
  IN UINT8     SocId,
  IN UINT8     PortIndex
  );

typedef
UINT8
(EFIAPI *DXE_GetSocketPortBusNum) (
  IN UINT8     SocId,
  IN UINT8     PortIndex
  );

typedef
BOOLEAN
(EFIAPI *DXE_IioNtbIsEnabled) (
  IN  UINT8    IioIndex,
  IN  UINT8    IioPort,
  OUT UINT8   *DevNoPtr,
  OUT UINT8   *FuncNoPtr
  );

typedef
BOOLEAN
(EFIAPI *DXE_IioVmdPortIsEnabled) (
  IN  UINT8    IioIndex,
  IN  UINT8    IioPort
  );

typedef
EFI_STATUS
(EFIAPI *DXE_IioVmdGetPciLocation) (
  IN  UINT8    IioIndex,
  IN  UINT8    IioStack,
  OUT UINT8   *PciDevPtr,
  OUT UINT8   *PciFuncPtr
  );

typedef
UINT8
(EFIAPI *DXE_GetCurrentPXPMap) (
  IN UINT8     SocId,
  IN UINT8     PortIndex
  );

typedef
BOOLEAN
(EFIAPI *DXE_IsSlowBoot) (
  VOID
  );

typedef
EFI_STATUS
(EFIAPI *DXE_UpdatePcatTable) (
   IN OUT   EFI_ACPI_COMMON_HEADER  *Table
   );

#pragma pack(1)
typedef struct {
  UINT32  ApicId;
  UINT32  ThreadIdValue;
  UINT32  CollocatedChaId;
  UINT32  SNCProximityDomain;
} CPU_LOGICAL_THREAD_ID_TABLE;
#pragma pack()

typedef
UINT8
(EFIAPI *DXE_GetNumOfClusterPerSystem) (
  VOID
  );

typedef
UINT8
(EFIAPI *DXE_GetMaxPhysicalAddrBits) (
  VOID
  );

typedef
BOOLEAN
(EFIAPI *DXE_IsMemTypeVolatile) (
  MEM_TYPE MemType
  );

typedef
BOOLEAN
(EFIAPI *DXE_IsMemType2lm) (
  MEM_TYPE MemType
  );

typedef
BOOLEAN
(EFIAPI *DXE_IsMemTypeReserved) (
  MEM_TYPE MemType
  );

typedef
BOOLEAN
(EFIAPI *DXE_IsMemTypeAppDirect) (
  MEM_TYPE MemType
  );

typedef
BOOLEAN
(EFIAPI *DXE_IsMemTypeFpga) (
  MEM_TYPE MemType
  );

typedef
UINT8
(EFIAPI *DXE_GetKtiPortCnt) (
  VOID
  );

typedef
UINT8
(EFIAPI *DXE_GetMaxImc) (
  VOID
  );

typedef
UINT8
(EFIAPI *DXE_GetNumChannelPerMc) (
  VOID
  );

typedef
UINT8
(EFIAPI *DXE_GetAcpiDieCount) (
  IN UINT8   SocketId
  );

typedef
EFI_STATUS
(EFIAPI *DXE_SpdReadByte) (
  IN UINT8      Socket,
  IN UINT8      Chan,
  IN UINT8      Dimm,
  IN UINT16     ByteOffset,
  OUT UINT8     *Data
  );

typedef
UINT8
(EFIAPI *DXE_DetectHwpFeature) (
   VOID
  );

typedef
BOOLEAN
(EFIAPI *DXE_SocketPresent) (
  IN UINT32     SocId
  );

typedef
BOOLEAN
(EFIAPI *DXE_IfStackPresent) (
  IN UINT8     SocId,
  IN UINT8     StackId
  );

typedef
EFI_STATUS
(EFIAPI *DXE_PchHpetBaseGet) (
  OUT UINT32                            *HpetBase
  );

typedef
UINT32
(EFIAPI *DXE_PcuGetDesiredCoreSmtDis) (
  UINT8 Socket
  );

//
// UBA specific silicon abstraction protocol
//
typedef struct {
  UINT32                                  Signature;
  UINT32                                  Version;

  DXE_GetVtdBar                           GetVtdBar;
  DXE_GetMaxPortPerSocket                 GetMaxPortPerSocket;
  DXE_GetStackPerPort                     GetStackPerPort;
  DXE_GetSocketPortBusNum                 GetSocketPortBusNum;
  DXE_IioNtbIsEnabled                     IioNtbIsEnabled;
  DXE_IioVmdPortIsEnabled                 IioVmdPortIsEnabled;
  DXE_IioVmdGetPciLocation                IioVmdGetPciLocation;
  DXE_GetCurrentPXPMap                    GetCurrentPXPMap;
  DXE_IsSlowBoot                          IsSlowBoot;
  DXE_UpdatePcatTable                     UpdatePcatTable;
  DXE_GetNumOfClusterPerSystem            GetNumOfClusterPerSystem;
  DXE_GetMaxPhysicalAddrBits              GetMaxPhysicalAddrBits;
  DXE_IsMemTypeVolatile                   IsMemTypeVolatile;
  DXE_IsMemType2lm                        IsMemType2lm;
  DXE_IsMemTypeReserved                   IsMemTypeReserved;
  DXE_IsMemTypeAppDirect                  IsMemTypeAppDirect;
  DXE_IsMemTypeFpga                       IsMemTypeFpga;
  DXE_GetKtiPortCnt                       GetKtiPortCnt;
  DXE_GetMaxImc                           GetMaxImc;
  DXE_GetNumChannelPerMc                  GetNumChannelPerMc;
  DXE_GetAcpiDieCount                     GetAcpiDieCount;
  DXE_SpdReadByte                         SpdReadByte;
  DXE_DetectHwpFeature                    DetectHwpFeature;
  DXE_SocketPresent                       SocketPresent;
  DXE_IfStackPresent                      IfStackPresent;
  DXE_PchHpetBaseGet                      PchHpetBaseGet;
  DXE_PcuGetDesiredCoreSmtDis             PcuGetDesiredCoreSmtDis;
} DYNAMIC_SI_LIBARY_PROTOCOL2;

extern EFI_GUID gDynamicSiLibraryProtocol2Guid;

#endif // _DYNAMIC_SI_LIBARY_PROTOCOL2_H_
