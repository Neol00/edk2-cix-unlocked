/** @file

  Copyright (c) 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Uefi.h>

#include <Guid/PlatformInfoHob.h>
#include <Library/AmpereCpuLib.h>
#include <Library/ArmLib/ArmLibPrivate.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/NVParamLib.h>
#include <NVParamDef.h>
#include <Platform/Ac01.h>

PLATFORM_INFO_HOB *
GetPlatformHob (
  VOID
  );

/**
  Get current CPU frequency.

  @param    Socket    Socket index.
  @return   UINTN     Current CPU frequency.

**/
UINTN
EFIAPI
CpuGetCurrentFreq (
  UINT8 Socket
  )
{
  PLATFORM_INFO_HOB  *PlatformHob;

  PlatformHob = GetPlatformHob ();
  ASSERT (PlatformHob != NULL);

  return PlatformHob->CpuClk;
}

/**
  Get maximum CPU frequency.

  @param    Socket    Socket index.
  @return   UINTN     Maximum CPU frequency.

**/
UINTN
EFIAPI
CpuGetMaxFreq (
  UINT8 Socket
  )
{
  PLATFORM_INFO_HOB  *PlatformHob;

  PlatformHob = GetPlatformHob ();
  ASSERT (PlatformHob != NULL);

  if (PlatformHob->TurboCapability[Socket]) {
    return PlatformHob->TurboFrequency[Socket];
  }

  return PlatformHob->CpuClk;
}

/**
  Get CPU voltage.

  @param    Socket    Socket index.
  @return   UINT8     CPU voltage.

**/
UINT8
EFIAPI
CpuGetVoltage (
  UINT8 Socket
  )
{
  PLATFORM_INFO_HOB  *PlatformHob;
  UINT8              Voltage;

  PlatformHob = GetPlatformHob ();
  ASSERT (PlatformHob != NULL);

  Voltage = 0x80 | (PlatformHob->CoreVoltage[Socket] / 100);

  return Voltage;
}

/**
  Get the SubNUMA mode.

  @return   UINT8      The SubNUMA mode.

**/
UINT8
EFIAPI
CpuGetSubNumaMode (
  VOID
  )
{
  PLATFORM_INFO_HOB  *PlatformHob;

  PlatformHob = GetPlatformHob ();
  if (PlatformHob == NULL) {
    return SUBNUMA_MODE_MONOLITHIC;
  }

  return PlatformHob->SubNumaMode[0];
}

/**
  Get the number of SubNUMA region.

  @return   UINT8      The number of SubNUMA region.

**/
UINT8
EFIAPI
CpuGetNumberOfSubNumaRegion (
  VOID
  )
{
  UINT8 SubNumaMode;
  UINT8 NumberOfSubNumaRegion;

  SubNumaMode = CpuGetSubNumaMode ();
  ASSERT (SubNumaMode <= SUBNUMA_MODE_QUADRANT);

  switch (SubNumaMode) {
  case SUBNUMA_MODE_MONOLITHIC:
    NumberOfSubNumaRegion = MONOLITIC_NUM_OF_REGION;
    break;

  case SUBNUMA_MODE_HEMISPHERE:
    NumberOfSubNumaRegion = HEMISPHERE_NUM_OF_REGION;
    break;

  case SUBNUMA_MODE_QUADRANT:
    NumberOfSubNumaRegion = QUADRANT_NUM_OF_REGION;
    break;

  default:
    // Should never reach there.
    NumberOfSubNumaRegion = 0;
    ASSERT (FALSE);
    break;
  }

  return NumberOfSubNumaRegion;
}

/**
  Get the SubNUMA node of a CPM.

  @param    SocketId    Socket index.
  @param    Cpm         CPM index.
  @return   UINT8       The SubNUMA node of a CPM.

**/
UINT8
EFIAPI
CpuGetSubNumNode (
  UINT8  SocketId,
  UINT16 Cpm
  )
{
  BOOLEAN IsAsymMesh;
  UINT8   SubNumaNode;
  UINT16  MaxNumberOfCPM;
  UINT8   MiddleRow;
  UINT8   QuadrantHigherRowNodeNumber[NUM_OF_CPM_PER_MESH_ROW] = {1, 1, 1, 1, 3, 3, 3, 3};
  UINT8   QuadrantLowerRowNodeNumber[NUM_OF_CPM_PER_MESH_ROW]  = {0, 0, 0, 0, 2, 2, 2, 2};
  UINT8   QuadrantMiddleRowNodeNumber[NUM_OF_CPM_PER_MESH_ROW] = {0, 0, 1, 1, 3, 3, 2, 2};
  UINT8   SubNumaMode;

  MaxNumberOfCPM = GetMaximumNumberOfCPMs ();
  SubNumaMode = CpuGetSubNumaMode ();
  ASSERT (SubNumaMode <= SUBNUMA_MODE_QUADRANT);

  switch (SubNumaMode) {
  case SUBNUMA_MODE_MONOLITHIC:
    SubNumaNode = (SocketId == 0) ? 0 : 1;
    break;

  case SUBNUMA_MODE_HEMISPHERE:
    if (CPM_PER_ROW_OFFSET (Cpm) >= SUBNUMA_CPM_REGION_SIZE) {
      SubNumaNode = 1;
    } else {
      SubNumaNode = 0;
    }

    if (SocketId == 1) {
      SubNumaNode += HEMISPHERE_NUM_OF_REGION;
    }
    break;

  case SUBNUMA_MODE_QUADRANT:
    //
    // CPM Mesh Rows
    //
    // |---------------------------------------|
    // | 00 ----------- 03 | 04 ----------- 07 | Row 0
    // |-------------------|-------------------|
    // | 08 ----------- 11 | 12 ----------- 15 | Row 1
    // |-------------------|-------------------|
    // | 16 - 17 | 18 - 19 | 20 - 21 | 22 - 23 | Middle Row
    // |-------------------|-------------------|
    // | 24 ----------- 27 | 28 ----------- 31 | Row 3
    // |-------------------|-------------------|
    // | 32 ----------- 35 | 36 ----------- 39 | Row 4
    // |---------------------------------------|
    //

    IsAsymMesh = (BOOLEAN)(CPM_ROW_NUMBER (MaxNumberOfCPM) % 2 != 0);
    MiddleRow = CPM_ROW_NUMBER (MaxNumberOfCPM) / 2;
    if (IsAsymMesh
        && CPM_ROW_NUMBER (Cpm) == MiddleRow)
    {
      SubNumaNode = QuadrantMiddleRowNodeNumber[CPM_PER_ROW_OFFSET (Cpm)];

    } else if (CPM_ROW_NUMBER (Cpm) >= MiddleRow) {
      SubNumaNode = QuadrantHigherRowNodeNumber[CPM_PER_ROW_OFFSET (Cpm)];

    } else {
      SubNumaNode = QuadrantLowerRowNodeNumber[CPM_PER_ROW_OFFSET (Cpm)];
    }

    if (SocketId == 1) {
      SubNumaNode += QUADRANT_NUM_OF_REGION;
    }
    break;

  default:
    // Should never reach there.
    SubNumaNode = 0;
    ASSERT (FALSE);
    break;
  }

  return SubNumaNode;
}

/**
  Get the number of supported socket.

  @return   UINT8      Number of supported socket.

**/
UINT8
EFIAPI
GetNumberOfSupportedSockets (
  VOID
  )
{
  PLATFORM_INFO_HOB  *PlatformHob;

  PlatformHob = GetPlatformHob ();
  if (PlatformHob == NULL) {
    //
    // By default, the number of supported sockets is 1.
    //
    return 1;
  }

  return (sizeof (PlatformHob->ClusterEn) / sizeof (PLATFORM_CLUSTER_EN));
}

/**
  Get the number of active socket.

  @return   UINT8      Number of active socket.

**/
UINT8
EFIAPI
GetNumberOfActiveSockets (
  VOID
  )
{
  UINT8               NumberOfActiveSockets, Count, Index, Index1;
  PLATFORM_CLUSTER_EN *Socket;
  PLATFORM_INFO_HOB   *PlatformHob;

  PlatformHob = GetPlatformHob ();
  if (PlatformHob == NULL) {
    //
    // By default, the number of active sockets is 1.
    //
    return 1;
  }

  NumberOfActiveSockets = 0;

  for (Index = 0; Index < GetNumberOfSupportedSockets (); Index++) {
    Socket = &PlatformHob->ClusterEn[Index];
    Count = ARRAY_SIZE (Socket->EnableMask);
    for (Index1 = 0; Index1 < Count; Index1++) {
      if (Socket->EnableMask[Index1] != 0) {
        NumberOfActiveSockets++;
        break;
      }
    }
  }

  return NumberOfActiveSockets;
}

/**
  Get the number of active CPM per socket.

  @param    SocketId    Socket index.
  @return   UINT16      Number of CPM.

**/
UINT16
EFIAPI
GetNumberOfActiveCPMsPerSocket (
  UINT8 SocketId
  )
{
  UINT16              NumberOfCPMs, Count, Index;
  UINT32              Val32;
  PLATFORM_CLUSTER_EN *Socket;
  PLATFORM_INFO_HOB   *PlatformHob;

  PlatformHob = GetPlatformHob ();
  if (PlatformHob == NULL) {
    return 0;
  }

  if (SocketId >= GetNumberOfActiveSockets ()) {
    return 0;
  }

  NumberOfCPMs = 0;
  Socket = &PlatformHob->ClusterEn[SocketId];
  Count = ARRAY_SIZE (Socket->EnableMask);
  for (Index = 0; Index < Count; Index++) {
    Val32 = Socket->EnableMask[Index];
    while (Val32 > 0) {
      if ((Val32 & 0x1) != 0) {
        NumberOfCPMs++;
      }
      Val32 >>= 1;
    }
  }

  return NumberOfCPMs;
}

/**
  Get the number of configured CPM per socket. This number
  should be the same for all sockets.

  @param    SocketId    Socket index.
  @return   UINT8       Number of configured CPM.

**/
UINT16
EFIAPI
GetNumberOfConfiguredCPMs (
  UINT8 SocketId
  )
{
  EFI_STATUS Status;
  UINT32     Value;
  UINT32     Param, ParamStart, ParamEnd;
  UINT16     Count;

  Count = 0;
  ParamStart = NV_SI_S0_PCP_ACTIVECPM_0_31 + SocketId * NV_PARAM_ENTRYSIZE * (PLATFORM_CPU_MAX_CPM / 32);
  ParamEnd = ParamStart + NV_PARAM_ENTRYSIZE * (PLATFORM_CPU_MAX_CPM / 32);
  for (Param = ParamStart; Param < ParamEnd; Param += NV_PARAM_ENTRYSIZE) {
    Status = NVParamGet (
               Param,
               NV_PERM_ATF | NV_PERM_BIOS | NV_PERM_MANU | NV_PERM_BMC,
               &Value
               );
    if (EFI_ERROR (Status)) {
      break;
    }
    while (Value != 0) {
      if ((Value & 0x01) != 0) {
        Count++;
      }
      Value >>= 1;
    }
  }

  return Count;
}

/**
  Set the number of configured CPM per socket.

  @param    SocketId        Socket index.
  @param    NumberOfCPMs    Number of CPM to be configured.
  @return   EFI_SUCCESS     Operation succeeded.
  @return   Others          An error has occurred.

**/
EFI_STATUS
EFIAPI
SetNumberOfConfiguredCPMs (
  UINT8  SocketId,
  UINT16 NumberOfCPMs
  )
{
  EFI_STATUS Status;
  UINT32     Value;
  UINT32     Param, ParamStart, ParamEnd;
  BOOLEAN    IsClear;

  IsClear = FALSE;
  if (NumberOfCPMs == 0) {
    IsClear = TRUE;
  }

  Status = EFI_SUCCESS;

  ParamStart = NV_SI_S0_PCP_ACTIVECPM_0_31 + SocketId * NV_PARAM_ENTRYSIZE * (PLATFORM_CPU_MAX_CPM / 32);
  ParamEnd = ParamStart + NV_PARAM_ENTRYSIZE * (PLATFORM_CPU_MAX_CPM / 32);
  for (Param = ParamStart; Param < ParamEnd; Param += NV_PARAM_ENTRYSIZE) {
    if (NumberOfCPMs >= 32) {
      Value = 0xffffffff;
      NumberOfCPMs -= 32;
    } else {
      Value = 0;
      while (NumberOfCPMs > 0) {
        Value |= (1 << (--NumberOfCPMs));
      }
    }
    if (IsClear) {
      /* Clear this param */
      Status = NVParamClr (
                 Param,
                 NV_PERM_BIOS | NV_PERM_MANU
                 );
    } else {
      Status = NVParamSet (
                 Param,
                 NV_PERM_ATF | NV_PERM_BIOS | NV_PERM_MANU | NV_PERM_BMC,
                 NV_PERM_BIOS | NV_PERM_MANU,
                 Value
                 );
    }
  }

  return Status;
}

/**
  Get the maximum number of core per socket.

  @return   UINT16      Maximum number of core.

**/
UINT16
EFIAPI
GetMaximumNumberOfCores (
  VOID
  )
{
  PLATFORM_INFO_HOB  *PlatformHob;

  PlatformHob = GetPlatformHob ();
  if (PlatformHob == NULL) {
    return 0;
  }

  return PlatformHob->MaxNumOfCore[0];
}

/**
  Get the maximum number of CPM per socket. This number
  should be the same for all sockets.

  @return   UINT16      Maximum number of CPM.

**/
UINT16
EFIAPI
GetMaximumNumberOfCPMs (
  VOID
  )
{
  return GetMaximumNumberOfCores () / PLATFORM_CPU_NUM_CORES_PER_CPM;
}

/**
  Get the number of active cores of a sockets.

  @param    SocketId    Socket Index.
  @return   UINT16      Number of active core.

**/
UINT16
EFIAPI
GetNumberOfActiveCoresPerSocket (
  UINT8 SocketId
  )
{
  return GetNumberOfActiveCPMsPerSocket (SocketId) * PLATFORM_CPU_NUM_CORES_PER_CPM;
}

/**
  Get the number of active cores of all sockets.

  @return   UINT16      Number of active core.

**/
UINT16
EFIAPI
GetNumberOfActiveCores (
  VOID
  )
{
  UINT16 NumberOfActiveCores;
  UINT8  Index;

  NumberOfActiveCores = 0;

  for (Index = 0; Index < GetNumberOfActiveSockets (); Index++) {
    NumberOfActiveCores += GetNumberOfActiveCoresPerSocket (Index);
  }

  return NumberOfActiveCores;
}

/**
  Check if the logical CPU is enabled or not.

  @param    CpuId       The logical Cpu ID. Started from 0.
  @return   BOOLEAN     TRUE if the Cpu enabled
                        FALSE if the Cpu disabled.

**/
BOOLEAN
EFIAPI
IsCpuEnabled (
  UINT16 CpuId
  )
{
  PLATFORM_CLUSTER_EN *Socket;
  PLATFORM_INFO_HOB   *PlatformHob;
  UINT8               SocketId;
  UINT16              ClusterId;

  SocketId = SOCKET_ID (CpuId);
  ClusterId = CLUSTER_ID (CpuId);

  PlatformHob = GetPlatformHob ();
  if (PlatformHob == NULL) {
    return FALSE;
  }

  if (SocketId >= GetNumberOfActiveSockets ()) {
    return FALSE;
  }

  Socket = &PlatformHob->ClusterEn[SocketId];
  if ((Socket->EnableMask[ClusterId / 32] & (1 << (ClusterId % 32))) != 0) {
    return TRUE;
  }

  return FALSE;
}

/**
  Check if the slave socket is present

  @return   BOOLEAN     TRUE if the Slave Cpu is present
                        FALSE if the Slave Cpu is not present

**/
BOOLEAN
EFIAPI
IsSlaveSocketAvailable (
  VOID
  )
{
  UINT32 Value;

  Value = MmioRead32 (SMPRO_EFUSE_SHADOW0 + CFG2P_OFFSET);

  return ((Value & SLAVE_PRESENT_N) != 0) ? FALSE : TRUE;
}

/**
  Check if the slave socket is active

  @return   BOOLEAN     TRUE if the Slave CPU Socket is active.
                        FALSE if the Slave CPU Socket is not active.

**/
BOOLEAN
EFIAPI
IsSlaveSocketActive (
  VOID
  )
{
  return (GetNumberOfActiveSockets () > 1) ? TRUE : FALSE;
}

/**
  Check if the CPU product ID is Ac01
  @return   BOOLEAN     TRUE if the Product ID is Ac01
                        FALSE otherwise.

**/
BOOLEAN
EFIAPI
IsAc01Processor (
  VOID
  )
{
  PLATFORM_INFO_HOB  *PlatformHob;

  PlatformHob = GetPlatformHob ();
  ASSERT (PlatformHob != NULL);

  if (PlatformHob != NULL) {
    if ((PlatformHob->ScuProductId[0] & 0xFF) == 0x01) {
      return TRUE;
    }
  }

  return FALSE;
}
