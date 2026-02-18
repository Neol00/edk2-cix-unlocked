/** @file
  SSDT Cpu Topology Table Generator.

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
  Copyright (c) 2021, Arm Limited. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Reference(s):
    - ACPI 6.3 Specification - January 2019 - s8.4 Declaring Processors
**/

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/AcpiTable.h>

// Module specific include files.
#include <AcpiTableGenerator.h>
#include <ConfigurationManagerObject.h>
#include <ConfigurationManagerHelper.h>
#include <Library/AcpiHelperLib.h>
#include <Library/TableHelperLib.h>
#include <Library/AmlLib/AmlLib.h>
#include <Protocol/ConfigurationManagerProtocol.h>

#include "SsdtCpuTopologyGenerator.h"
#include <CIXAmlLib.h>

CM_CIX_LPI_MAP_INFO  LpiMapInfo              = PLAT_LPI_MAP_INFO;
AML_PSD_INFO         PsdInfo[PLAT_CPU_COUNT] = PLAT_PSD_INFO;

/** This macro expands to a function that retrieves the GIC
    CPU interface Information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceArm,
  EArmObjGicCInfo,
  CM_ARM_GICC_INFO
  );

/**
  This macro expands to a function that retrieves the CPC
  information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceArm,
  EArmObjCpcInfo,
  CM_ARM_CPC_INFO
  );

/**
  This macro expands to a function that retrieves the Mappint
  table for CPU UID to Core number.
*/
GET_OBJECT_LIST (
  EObjNameSpaceCix,
  ECixObjCpuUidtoCoreNumberMap,
  CM_CIX_CPUUID_CORENUMBER_MAP
  );

/** Write a string 'CPUx\0' in AslName (5 bytes long),
  and with 'PUx' being overrided from back to front by Value in hexadecimal.
  For instance,
    Value = 0x5,    AslName = CPU5
    Value = 0x1A,   AslName = CP15
    Value = 0x1A5,  AslName = C1A5

  As 'xxx' in hexadecimal represents a number on 12 bits,
  we have Value < (1 << 12).

  @param [in]       Value     Hex value of the name.
                              Must be lower than (2 << 12).
  @param [in, out]  AslName   Pointer to write the 'Xxxx' string to.
                              Must be at least 5 bytes long.

  @retval EFI_SUCCESS               Success.
  @retval EFI_INVALID_PARAMETER     Invalid parameter.
**/
STATIC
EFI_STATUS
EFIAPI
WriteCpuAslName (
  IN      UINT32  Value,
  IN OUT  CHAR8   *AslName
  )
{
  UINT8  Index;

  if ((Value >= MAX_NODE_COUNT)  ||
      (AslName == NULL))
  {
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  AslName[0]                 = 'C';
  AslName[1]                 = 'P';
  AslName[2]                 = 'U';
  AslName[3]                 = '0';
  AslName[AML_NAME_SEG_SIZE] = '\0';

  for (Index = 0; Index < AML_NAME_SEG_SIZE - 1; Index++) {
    if (Value >> (4 * Index) == 0) {
      break;
    }

    AslName[AML_NAME_SEG_SIZE - Index - 1] =
      AsciiFromHex (((Value >> (4 * Index)) & 0xF));
  }

  return EFI_SUCCESS;
}

/** Create and add an _PSD Node to Cpu Node.
  For instance, transform an AML node from:
  Device (CPU2)
  {
      Name (_UID, 2)
      Name (_HID, "ACPI0007")
  }

  To:
  Device (CPU2)
  {
      Name (_UID, 2)
      Name (_HID, "ACPI0007")
      Name (_PSD, Package () {
        Package(){5, 0, 0, 0xFD, 4}
      })
  }

  @param [in]  PsdInfo                AML_PSD_INFO object
                                      describing the P-State Dependency.
  @param [in]  Node                   CPU Node to which the _CPC node is
                                      attached.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory.
**/
STATIC
EFI_STATUS
EFIAPI
CreateAmlPsdNode (
  IN  AML_PSD_INFO            PsdInfo,
  IN  AML_OBJECT_NODE_HANDLE  *Node
  )
{
  EFI_STATUS  Status;

  ASSERT (Node != NULL);

  Status = AmlCreatePsdNode (
             PsdInfo.NumEntries,
             PsdInfo.Revision,
             PsdInfo.Domain,
             PsdInfo.CoordType,
             PsdInfo.NumProcessors,
             Node,
             NULL
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
  }

  return Status;
}

/** Create and add an _CPC Node to Cpu Node.

  For instance, transform an AML node from:
  Device (CPU2)
  {
      Name (_UID, 2)
      Name (_HID, "ACPI0007")
  }

  To:
  Device (CPU2)
  {
      Name (_UID, 2)
      Name (_HID, "ACPI0007")
      Name(_CPC, Package()
      {
        NumEntries,                              // Integer
        Revision,                                // Integer
        HighestPerformance,                      // Integer or Buffer (Resource Descriptor)
        NominalPerformance,                      // Integer or Buffer (Resource Descriptor)
        LowestNonlinearPerformance,              // Integer or Buffer (Resource Descriptor)
        LowestPerformance,                       // Integer or Buffer (Resource Descriptor)
        GuaranteedPerformanceRegister,           // Buffer (Resource Descriptor)
        DesiredPerformanceRegister ,             // Buffer (Resource Descriptor)
        MinimumPerformanceRegister ,             // Buffer (Resource Descriptor)
        MaximumPerformanceRegister ,             // Buffer (Resource Descriptor)
        PerformanceReductionToleranceRegister,   // Buffer (Resource Descriptor)
        TimeWindowRegister,                      // Buffer (Resource Descriptor)
        CounterWraparoundTime,                   // Integer or Buffer (Resource Descriptor)
        ReferencePerformanceCounterRegister,     // Buffer (Resource Descriptor)
        DeliveredPerformanceCounterRegister,     // Buffer (Resource Descriptor)
        PerformanceLimitedRegister,              // Buffer (Resource Descriptor)
        CPPCEnableRegister                       // Buffer (Resource Descriptor)
        AutonomousSelectionEnable,               // Integer or Buffer (Resource Descriptor)
        AutonomousActivityWindowRegister,        // Buffer (Resource Descriptor)
        EnergyPerformancePreferenceRegister,     // Buffer (Resource Descriptor)
        ReferencePerformance                     // Integer or Buffer (Resource Descriptor)
        LowestFrequency,                         // Integer or Buffer (Resource Descriptor)
        NominalFrequency                         // Integer or Buffer (Resource Descriptor)
      })
  }

  @param [in]  CfgMgrProtocol         Pointer to the Configuration Manager
                                      Protocol Interface.
  @param [in]  Coreid                 CPU Core id.
  @param [in]  Node                   CPU Node to which the _CPC node is
                                      attached.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory.
**/
STATIC
EFI_STATUS
EFIAPI
CreateAmlCpcNode (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  IN  UINTN                                               Coreid,
  IN  AML_OBJECT_NODE_HANDLE                              *Node
  )
{
  EFI_STATUS       Status;
  CM_ARM_CPC_INFO  *CpcInfo;
  UINT32           CpcInfoCount;

  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Node != NULL);

  Status = GetEArmObjCpcInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &CpcInfo,
             &CpcInfoCount
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return Status;
  }

  Status = AmlCreateCpcNode (
             &CpcInfo[Coreid],
             Node,
             NULL
             );
  ASSERT_EFI_ERROR (Status);
  return Status;
}

/** Create and add an _LPI method to Cpu/Cluster Node.

  For instance, transform an AML node from:
  Device (CPU2)
  {
      Name (_UID, 2)
      Name (_HID, "ACPI0007")
  }

  To:
  Device (CPU2)
  {
      Name (_UID, 2)
      Name (_HID, "ACPI0007")
      Method (_LPI, 0, NotSerialized)
      {
          Return (\_SB.LPIL)
      }
  }

  @param [in]  Node                   Node to which the _LPI method is
                                      attached. Can represent a Cpu or a
                                      Cluster.
  @param [in]  LpiName                The _LPI method return NameString

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory.
**/
STATIC
EFI_STATUS
EFIAPI
CreateAmlLpiMethod (
  IN  AML_OBJECT_NODE_HANDLE  *Node,
  IN  CHAR8                   *LpiName
  )
{
  EFI_STATUS  Status;
  CHAR8       AslName[SB_SCOPE_PREFIX_SIZE + AML_NAME_SEG_SIZE];

  ASSERT (Node != NULL);
  ASSERT (AslName != NULL);

  CopyMem (AslName, SB_SCOPE_PREFIX, SB_SCOPE_PREFIX_SIZE);
  CopyMem (AslName+ SB_SCOPE_PREFIX_SIZE - 1, LpiName, 5);

  // ASL:
  // Method (_LPI, 0) {
  //   Return ([AslName])
  // }
  Status = AmlCodeGenMethodRetNameString (
             "_LPI",
             AslName,
             0,
             FALSE,
             0,
             Node,
             NULL
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
  }

  return Status;
}

/** Create a set of CPU passive thermal node to _SB scope

  @param [in] CfgMgrProtocol   Pointer to the Configuration Manager
                               Protocol Interface.
  @param [in] ScopeNode        Scope node handle ('\_SB' scope).

  @retval EFI_SUCCESS             Success.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory.
**/
STATIC
EFI_STATUS
EFIAPI
CreateAmlCpuPTNode (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  IN        AML_OBJECT_NODE_HANDLE                        ScopeNode
  )
{
  EFI_STATUS        Status;
  CM_ARM_GICC_INFO  *GicCInfo;
  UINT32            GicCInfoCount;
  UINT32            CpuIndex;
  UINT32            CoreId;
  UINT32            CpuNameStringLength = SB_SCOPE_PREFIX_SIZE + AML_NAME_SEG_SIZE;
  CHAR8             CpuName[AML_NAME_SEG_SIZE + 1];
  CHAR8             CpuAllNameStringList[PLAT_CPU_COUNT][CpuNameStringLength];
  CHAR8             CpuL0NameStringList[PLAT_CPU_L0_COUNT][CpuNameStringLength];
  CHAR8             CpuM0NameStringList[PLAT_CPU_M0_COUNT][CpuNameStringLength];
  CHAR8             CpuM1NameStringList[PLAT_CPU_M1_COUNT][CpuNameStringLength];
  CHAR8             CpuB0NameStringList[PLAT_CPU_B0_COUNT][CpuNameStringLength];
  CHAR8             CpuB1NameStringList[PLAT_CPU_B1_COUNT][CpuNameStringLength];
  UINT8             CpuL0Count = 0, CpuM0Count = 0, CpuM1Count = 0, CpuB0Count = 0, CpuB1Count = 0;

  CM_CIX_CPUUID_CORENUMBER_MAP  *CpuUidtoCoreNumberMap;
  UINT32                        CpuUidtoCoreNumberMapCount;

  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (ScopeNode != NULL);

  Status = GetEArmObjGicCInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &GicCInfo,
             &GicCInfoCount
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return Status;
  }

  Status = GetECixObjCpuUidtoCoreNumberMap (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &CpuUidtoCoreNumberMap,
             &CpuUidtoCoreNumberMapCount
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return Status;
  }

  // Convert cpu number to cpu name string list
  for (CpuIndex = 0; CpuIndex < GicCInfoCount; CpuIndex++) {
    Status = WriteCpuAslName (CpuIndex, CpuName);
    if (EFI_ERROR (Status)) {
      ASSERT (0);
      return Status;
    }

    CoreId = CpuUidtoCoreNumberMap[CpuIndex];
    if ((CoreId >= 0) && (CoreId <= 3)) {
      CopyMem (CpuL0NameStringList[CpuL0Count], SB_SCOPE_PREFIX, SB_SCOPE_PREFIX_SIZE);
      CopyMem (CpuL0NameStringList[CpuL0Count]+ SB_SCOPE_PREFIX_SIZE - 1, CpuName, 5);
      CpuL0Count++;
    } else if ((CoreId >= 4) && (CoreId <= 5)) {
      CopyMem (CpuM0NameStringList[CpuM0Count], SB_SCOPE_PREFIX, SB_SCOPE_PREFIX_SIZE);
      CopyMem (CpuM0NameStringList[CpuM0Count]+ SB_SCOPE_PREFIX_SIZE - 1, CpuName, 5);
      CpuM0Count++;
    } else if ((CoreId >= 6) && (CoreId <= 7)) {
      CopyMem (CpuM1NameStringList[CpuM1Count], SB_SCOPE_PREFIX, SB_SCOPE_PREFIX_SIZE);
      CopyMem (CpuM1NameStringList[CpuM1Count]+ SB_SCOPE_PREFIX_SIZE - 1, CpuName, 5);
      CpuM1Count++;
    } else if ((CoreId >= 8) && (CoreId <= 9)) {
      CopyMem (CpuB0NameStringList[CpuB0Count], SB_SCOPE_PREFIX, SB_SCOPE_PREFIX_SIZE);
      CopyMem (CpuB0NameStringList[CpuB0Count]+ SB_SCOPE_PREFIX_SIZE - 1, CpuName, 5);
      CpuB0Count++;
    } else if ((CoreId >= 10) && (CoreId <= 11)) {
      CopyMem (CpuB1NameStringList[CpuB1Count], SB_SCOPE_PREFIX, SB_SCOPE_PREFIX_SIZE);
      CopyMem (CpuB1NameStringList[CpuB1Count]+ SB_SCOPE_PREFIX_SIZE - 1, CpuName, 5);
      CpuB1Count++;
    }

    CopyMem (CpuAllNameStringList[CpuIndex], SB_SCOPE_PREFIX, SB_SCOPE_PREFIX_SIZE);
    CopyMem (CpuAllNameStringList[CpuIndex]+ SB_SCOPE_PREFIX_SIZE - 1, CpuName, 5);
  }

  Status = AmlCreateCpuPTNode (
             "CPL0",
             (CHAR8 *)CpuL0NameStringList,
             CpuL0Count,
             CpuNameStringLength,
             ScopeNode
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
  }

  Status = AmlCreateCpuPTNode (
             "CPM0",
             (CHAR8 *)CpuM0NameStringList,
             CpuM0Count,
             CpuNameStringLength,
             ScopeNode
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
  }

  Status = AmlCreateCpuPTNode (
             "CPM1",
             (CHAR8 *)CpuM1NameStringList,
             CpuM1Count,
             CpuNameStringLength,
             ScopeNode
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
  }

  Status = AmlCreateCpuPTNode (
             "CPB0",
             (CHAR8 *)CpuB0NameStringList,
             CpuB0Count,
             CpuNameStringLength,
             ScopeNode
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
  }

  Status = AmlCreateCpuPTNode (
             "CPB1",
             (CHAR8 *)CpuB1NameStringList,
             CpuB1Count,
             CpuNameStringLength,
             ScopeNode
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
  }

  Status = AmlCreateCpuPTNode (
             "CPUL",
             (CHAR8 *)CpuAllNameStringList,
             GicCInfoCount,
             CpuNameStringLength,
             ScopeNode
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
  }

  return Status;
}

/** Create a Cpu in the AML namespace.

  This generates the following ASL code:
  Device (CPU2)
  {
      Name (_UID, 2)
      Name (_HID, "ACPI0007")
  }

  @param [in]  Generator    The SSDT Cpu Topology generator.
  @param [in]  ParentNode   Parent node to attach the Cpu node to.
  @param [in]  GicCInfo     CM_ARM_GICC_INFO object used to create the node.
  @param [in]  CpuIndex     Index used to generate the node name.
  @param [out] CpuNodePtr   If not NULL, return the created Cpu node.

  @retval EFI_SUCCESS             Success.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory.
**/
STATIC
EFI_STATUS
EFIAPI
CreateAmlCpu (
  IN   ACPI_TABLE_GENERATOR    *Generator,
  IN   AML_NODE_HANDLE         ParentNode,
  IN   CM_ARM_GICC_INFO        *GicCInfo,
  IN   UINT32                  CpuIndex,
  OUT  AML_OBJECT_NODE_HANDLE  *CpuNodePtr OPTIONAL
  )
{
  EFI_STATUS              Status;
  AML_OBJECT_NODE_HANDLE  CpuNode;
  CHAR8                   AslName[AML_NAME_SEG_SIZE + 1];

  ASSERT (Generator != NULL);
  ASSERT (ParentNode != NULL);
  ASSERT (GicCInfo != NULL);

  Status = WriteCpuAslName (CpuIndex, AslName);
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return Status;
  }

  Status = AmlCodeGenDevice (AslName, ParentNode, &CpuNode);
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return Status;
  }

  Status = AmlCodeGenNameString (
             "_HID",
             ACPI_HID_PROCESSOR_DEVICE,
             CpuNode,
             NULL
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return Status;
  }

  Status = AmlCodeGenNameInteger (
             "_UID",
             GicCInfo->AcpiProcessorUid,
             CpuNode,
             NULL
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return Status;
  }

  // If requested, return the handle to the CpuNode.
  if (CpuNodePtr != NULL) {
    *CpuNodePtr = CpuNode;
  }

  return Status;
}

/** Create the processor hierarchy AML tree from CM_ARM_GICC_INFO
    CM objects.

  A processor container is by extension any non-leave device in the cpu topology.

  @param [in] Generator        The SSDT Cpu Topology generator.
  @param [in] CfgMgrProtocol   Pointer to the Configuration Manager
                               Protocol Interface.
  @param [in] ScopeNode        Scope node handle ('\_SB' scope).

  @retval EFI_SUCCESS             Success.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory.
**/
STATIC
EFI_STATUS
EFIAPI
CreateTopologyFromGicC (
  IN        ACPI_TABLE_GENERATOR                          *Generator,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  IN        AML_OBJECT_NODE_HANDLE                        ScopeNode
  )
{
  EFI_STATUS                    Status;
  CM_ARM_GICC_INFO              *GicCInfo;
  UINT32                        GicCInfoCount;
  UINT32                        CpuUidtoCoreNumberMapCount;
  UINT32                        Index;
  AML_OBJECT_NODE_HANDLE        CpuNode;
  UINT32                        CpuUid;
  UINT32                        Coreid;
  CM_CIX_CPUUID_CORENUMBER_MAP  *CpuUidtoCoreNumberMap;

  ASSERT (Generator != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (ScopeNode != NULL);

  Status = GetEArmObjGicCInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &GicCInfo,
             &GicCInfoCount
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return Status;
  }

  Status = GetECixObjCpuUidtoCoreNumberMap (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &CpuUidtoCoreNumberMap,
             &CpuUidtoCoreNumberMapCount
             );
  if (EFI_ERROR (Status)) {
    ASSERT (0);
    return Status;
  }

  // For each CM_ARM_GICC_INFO object, create an AML node.
  for (Index = 0; Index < GicCInfoCount; Index++) {
    Status = CreateAmlCpu (
               Generator,
               ScopeNode,
               &GicCInfo[Index],
               Index,
               &CpuNode
               );
    if (EFI_ERROR (Status)) {
      ASSERT (0);
      break;
    }

    CpuUid = GicCInfo[Index].AcpiProcessorUid;
    Coreid = CpuUidtoCoreNumberMap[CpuUid];
    Status = CreateAmlLpiMethod (CpuNode, LpiMapInfo[Coreid]);
    if (EFI_ERROR (Status)) {
      ASSERT (0);
      return Status;
    }

    Status = CreateAmlCpcNode (CfgMgrProtocol, Coreid, CpuNode);
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      break;
    }

    Status = CreateAmlPsdNode (PsdInfo[Coreid], CpuNode);
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      break;
    }
  } // for

  return Status;
}

/** Construct the SSDT Cpu Topology ACPI table.

  This function invokes the Configuration Manager protocol interface
  to get the required hardware information for generating the ACPI
  table.

  If this function allocates any resources then they must be freed
  in the FreeXXXXTableResources function.

  @param [in]  This           Pointer to the table generator.
  @param [in]  AcpiTableInfo  Pointer to the ACPI Table Info.
  @param [in]  CfgMgrProtocol Pointer to the Configuration Manager
                              Protocol Interface.
  @param [out] Table          Pointer to the constructed ACPI Table.

  @retval EFI_SUCCESS           Table generated successfully.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The required object was not found.
  @retval EFI_BAD_BUFFER_SIZE   The size returned by the Configuration
                                Manager is less than the Object size for the
                                requested object.
**/
STATIC
EFI_STATUS
EFIAPI
BuildSsdtCpuTopologyTable (
  IN  CONST ACPI_TABLE_GENERATOR                  *CONST  This,
  IN  CONST CM_STD_OBJ_ACPI_TABLE_INFO            *CONST  AcpiTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  OUT       EFI_ACPI_DESCRIPTION_HEADER          **CONST  Table
  )
{
  EFI_STATUS              Status;
  AML_ROOT_NODE_HANDLE    RootNode;
  AML_OBJECT_NODE_HANDLE  ScopeNode;
  ACPI_TABLE_GENERATOR    *Generator;

  ASSERT (This != NULL);
  ASSERT (AcpiTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Table != NULL);
  ASSERT (AcpiTableInfo->TableGeneratorId == This->GeneratorID);
  ASSERT (AcpiTableInfo->AcpiTableSignature == This->AcpiTableSignature);

  Generator = (ACPI_TABLE_GENERATOR *)This;

  Status = AddSsdtAcpiHeader (
             CfgMgrProtocol,
             This,
             AcpiTableInfo,
             &RootNode
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = AmlCodeGenScope (SB_SCOPE, RootNode, &ScopeNode);
  if (EFI_ERROR (Status)) {
    goto exit_handler;
  }

  Status = CreateTopologyFromGicC (
             Generator,
             CfgMgrProtocol,
             ScopeNode
             );
  if (EFI_ERROR (Status)) {
    goto exit_handler;
  }

  Status = CreateAmlCpuPTNode (
             CfgMgrProtocol,
             ScopeNode
             );
  if (EFI_ERROR (Status)) {
    goto exit_handler;
  }

  Status = AmlSerializeDefinitionBlock (
             RootNode,
             Table
             );
  if (EFI_ERROR (Status)) {
    DEBUG (
      (
       DEBUG_ERROR,
       "ERROR: SSDT-CPU-TOPOLOGY: Failed to Serialize SSDT Table Data."
       " Status = %r\n",
       Status
      )
      );
    goto exit_handler;
  }

exit_handler:
  // Delete the RootNode and its attached children.
  return AmlDeleteTree (RootNode);
}

/** Free any resources allocated for constructing the
    SSDT Cpu Topology ACPI table.

  @param [in]      This           Pointer to the table generator.
  @param [in]      AcpiTableInfo  Pointer to the ACPI Table Info.
  @param [in]      CfgMgrProtocol Pointer to the Configuration Manager
                                  Protocol Interface.
  @param [in, out] Table          Pointer to the ACPI Table.

  @retval EFI_SUCCESS           The resources were freed successfully.
  @retval EFI_INVALID_PARAMETER The table pointer is NULL or invalid.
**/
STATIC
EFI_STATUS
FreeSsdtCpuTopologyTableResources (
  IN      CONST ACPI_TABLE_GENERATOR                  *CONST  This,
  IN      CONST CM_STD_OBJ_ACPI_TABLE_INFO            *CONST  AcpiTableInfo,
  IN      CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  IN OUT        EFI_ACPI_DESCRIPTION_HEADER          **CONST  Table
  )
{
  ASSERT (This != NULL);
  ASSERT (AcpiTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (AcpiTableInfo->TableGeneratorId == This->GeneratorID);
  ASSERT (AcpiTableInfo->AcpiTableSignature == This->AcpiTableSignature);

  if ((Table == NULL) || (*Table == NULL)) {
    DEBUG ((DEBUG_ERROR, "ERROR: SSDT-CPU-TOPOLOGY: Invalid Table Pointer\n"));
    ASSERT ((Table != NULL) && (*Table != NULL));
    return EFI_INVALID_PARAMETER;
  }

  FreePool (*Table);
  *Table = NULL;
  return EFI_SUCCESS;
}

/** The interface for the SSDT Cpu Topology Table Generator.
*/
STATIC
ACPI_TABLE_GENERATOR  SsdtCpuTopologyGenerator = {
  // Generator ID
  CREATE_STD_ACPI_TABLE_GEN_ID (EStdAcpiTableIdSsdtCpuTopology),
  // Generator Description
  L"ACPI.STD.SSDT.CPU.TOPOLOGY.GENERATOR",
  // ACPI Table Signature
  EFI_ACPI_6_3_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE,
  // ACPI Table Revision - Unused
  0,
  // Minimum ACPI Table Revision - Unused
  0,
  // Creator ID
  EFI_ACPI_CREATOR_ID,
  // Creator Revision
  EFI_ACPI_CREATOR_REVISION,
  // Build Table function
  BuildSsdtCpuTopologyTable,
  // Free Resource function
  FreeSsdtCpuTopologyTableResources,
  // Extended build function not needed
  NULL,
  // Extended build function not implemented by the generator.
  // Hence extended free resource function is not required.
  NULL
};

/** Register the Generator with the ACPI Table Factory.

  @param [in]  ImageHandle  The handle to the image.
  @param [in]  SystemTable  Pointer to the System Table.

  @retval EFI_SUCCESS           The Generator is registered.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_ALREADY_STARTED   The Generator for the Table ID
                                is already registered.
**/
EFI_STATUS
EFIAPI
AcpiSsdtCpuTopologyLibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterAcpiTableGenerator (&SsdtCpuTopologyGenerator);
  DEBUG (
    (
     DEBUG_INFO,
     "SSDT-CPU-TOPOLOGY: Register Generator. Status = %r\n",
     Status
    )
    );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/** Deregister the Generator from the ACPI Table Factory.

  @param [in]  ImageHandle  The handle to the image.
  @param [in]  SystemTable  Pointer to the System Table.

  @retval EFI_SUCCESS           The Generator is deregistered.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The Generator is not registered.
**/
EFI_STATUS
EFIAPI
AcpiSsdtCpuTopologyLibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterAcpiTableGenerator (&SsdtCpuTopologyGenerator);
  DEBUG (
    (
     DEBUG_INFO,
     "SSDT-CPU-TOPOLOGY: Deregister Generator. Status = %r\n",
     Status
    )
    );
  ASSERT_EFI_ERROR (Status);
  return Status;
}
