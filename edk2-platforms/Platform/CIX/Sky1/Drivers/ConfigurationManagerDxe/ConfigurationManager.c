#/** @file
#
#  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
#
#  @par Glossary:
#    - Cm or CM   - Configuration Manager
#    - Obj or OBJ - Object
#**/

#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>
#include <IndustryStandard/SerialPortConsoleRedirectionTable.h>
#include <Library/ArmLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/ConfigurationManagerProtocol.h>

/** The platform specific headers
*/
#include "ConfigurationManager.h"
#include "MemoryMap.h"
#include <Protocol/ArmScmiPerformanceProtocol.h>
#include <PlatformSetupVar.h>
#include <Library/CpuInfoLib.h>

/** The platform configuration repository information.
*/
STATIC
SKY1_PLATFORM_REPOSITORY_INFO  Sky1PlatformRepositoryInfo = {
  /// Configuration Manager information
  { CONFIGURATION_MANAGER_REVISION, CFG_MGR_OEM_ID },

  // ACPI Table List
  {
    // MADT Table
    {
      EFI_ACPI_6_3_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE,
      EFI_ACPI_6_3_MULTIPLE_APIC_DESCRIPTION_TABLE_REVISION,
      CREATE_STD_ACPI_TABLE_GEN_ID (EStdAcpiTableIdMadt),
      NULL,
      EFI_ACPI_OEM_TABLE_ID,
      EFI_ACPI_OEM_REVISION
    },
    // SSDT Table
    {
      EFI_ACPI_6_3_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE,
      0,
      CREATE_STD_ACPI_TABLE_GEN_ID (EStdAcpiTableIdSsdtCpuTopology),
      NULL,
      EFI_ACPI_OEM_TABLE_ID,
      EFI_ACPI_OEM_REVISION
    }
  },

  // Pointer to GIC CPU Interface information
  NULL,

  // GIC Distributor Info
  PLAT_GIC_DISTRIBUTOR_INFO,

  // GIC Redistributor
  PLAT_GIC_REDISTRIBUTOR_INFO,

  // GIC ITS
  PLAT_GIC_ITS_INFO,

  // CPU _CPC information
  PLAT_CPC_INFO
};

UINT32  CpuCount;

/** Get the Cpu performace capability granularity

  @param [in]      CpuID              the id of cpu
  @param [in,out]  CpcGranularity     Cpu performace capability granularity, unit of Hz/perf_level

  @retval EFI_SUCCESS   Success
**/
STATIC
EFI_STATUS
GetCpcGranularity (
  IN     UINTN   CpuID,
  IN OUT UINT32  *CpcGranularity
  )
{
  EFI_STATUS                          Status;
  SCMI_PERFORMANCE_PROTOCOL           *ScmiPerfProtocol = NULL;
  UINT32                              DomainId;
  AML_PSD_INFO                        PsdInfo[PLAT_CPU_COUNT] = PLAT_PSD_INFO;
  SCMI_PERFORMANCE_DOMAIN_ATTRIBUTES  DomainAttribute;

  if (CpuID >= PLAT_CPU_COUNT) {
    DEBUG ((DEBUG_ERROR, "Cpuid is over the max range, max cpuid = %d, current cpu id = %d\n", PLAT_CPU_COUNT - 1, CpuID));
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (
                  &gArmScmiPerformanceProtocolGuid,
                  NULL,
                  (VOID **)&ScmiPerfProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "gArmScmiPerformanceProtocolGuid NOT found.\n"));
    return Status;
  }

  DomainId = PsdInfo[CpuID].Domain;

  Status = ScmiPerfProtocol->GetDomainAttributes (ScmiPerfProtocol, DomainId, &DomainAttribute);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Perfomance [%d] get domain attributes failed.\n", DomainId));
    return Status;
  }

  *CpcGranularity = (DomainAttribute.SustainedFreq*1000) / DomainAttribute.SustainedPerfLevel;
  DEBUG ((DEBUG_INFO, "CpcGranularity = %d\n", *CpcGranularity));
  return Status;
}

STATIC
EFI_STATUS
GetCpuPerfData (
  IN     UINTN   CpuID,
  IN OUT UINT32  *HighestPerf,
  IN OUT UINT32  *NominalPerf,
  IN OUT UINT32  *LowestNonlinearPerf,
  IN OUT UINT32  *LowestPerf
  )
{
  EFI_STATUS                 Status;
  SCMI_PERFORMANCE_PROTOCOL  *ScmiPerfProtocol = NULL;
  UINT32                     DomainId;
  UINT32                     NumLevels;
  UINT32                     LevelArraySize = 0;
  SCMI_PERFORMANCE_LEVEL     *LevelArra;
  AML_PSD_INFO               PsdInfo[PLAT_CPU_COUNT] = PLAT_PSD_INFO;

  if (CpuID >= PLAT_CPU_COUNT) {
    DEBUG ((DEBUG_ERROR, "Cpuid is over the max range, max cpuid = %d, current cpu id = %d\n", PLAT_CPU_COUNT - 1, CpuID));
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (
                  &gArmScmiPerformanceProtocolGuid,
                  NULL,
                  (VOID **)&ScmiPerfProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "gArmScmiPerformanceProtocolGuid NOT found.\n"));
    return Status;
  }

  DomainId = PsdInfo[CpuID].Domain;

  Status = ScmiPerfProtocol->DescribeLevels (ScmiPerfProtocol, DomainId, &NumLevels, &LevelArraySize, LevelArra);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Status = gBS->AllocatePool (EfiBootServicesData, LevelArraySize, (VOID **)&LevelArra);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Out of memory resource\n"));
      return Status;
    }

    Status = ScmiPerfProtocol->DescribeLevels (ScmiPerfProtocol, DomainId, &NumLevels, &LevelArraySize, LevelArra);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Perfomance [%d] get DescribeLevels failed.\n", DomainId));
      return Status;
    }

    for (UINT8 i = 0; i < NumLevels; i++) {
      DEBUG ((
        DEBUG_INFO,
        "[CPU%d] Performace_Level = [%d].   Level = [%d]    ,  PowerCost = [%d], Latency = [%d]\n", \
        CpuID,
        i,
        LevelArra[i].Level,
        LevelArra[i].PowerCost,
        LevelArra[i].Latency
        ));
    }

    *HighestPerf         = LevelArra[NumLevels-1].Level;
    *NominalPerf         = LevelArra[NumLevels-1].Level;
    *LowestNonlinearPerf = LevelArra[0].Level;
    *LowestPerf          = LevelArra[0].Level;
  } else {
    DEBUG ((DEBUG_ERROR, "Perfomance [%d] get DescribeLevels failed.\n", DomainId));
    return Status;
  }

  DEBUG ((DEBUG_INFO, "HighestPerf = %x\n", *HighestPerf));
  DEBUG ((DEBUG_INFO, "NominalPerf = %x\n", *NominalPerf));
  DEBUG ((DEBUG_INFO, "LowestNonlinearPerf = %x\n", *LowestNonlinearPerf));
  DEBUG ((DEBUG_INFO, "LowestPerf = %x\n", *LowestPerf));

  return EFI_SUCCESS;
}

/** Initialize the CPU CPC interface information in platform configuration repository.

  @param [in]  This     Pointer to the Configuration Manager Protocol.

  @retval EFI_SUCCESS   Success
**/
STATIC
EFI_STATUS
EFIAPI
InitializeCmArmCpcInfo (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  This
  )
{
  EFI_STATUS       Status;
  CM_ARM_CPC_INFO  *CpuCpcInfo;
  UINT32           HighestPerf;
  UINT32           NominalPerf;
  UINT32           LowestNonlinearPerf;
  UINT32           LowestPerf;
  UINT32           i;
  UINT32           CpuCoreMask, MaxCpuCoreNum;
  UINT32           CpcGranularity;

  GetCpuCoreMask (&CpuCoreMask, &MaxCpuCoreNum);

  CpuCpcInfo = This->PlatRepoInfo->CpuCpcInfo;
  for (i = 0; i < PLAT_CPU_COUNT; i++) {
    // If a cluster of cpu is masked, then get core performace of this cpu cluster will fail
    if (((CpuCoreMask>>i)&BIT0) == 1) {
      continue;
    }

    // Get CPU performace parameter
    Status = GetCpuPerfData (i, &HighestPerf, &NominalPerf, &LowestNonlinearPerf, &LowestPerf);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Get CPU%d performance data fail!\n"));
      continue;
    }

    // Get CPU performace granularity
    Status = GetCpcGranularity (i, &CpcGranularity);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Get CPU%d performance granularity fail!\n"));
      continue;
    }

    CpuCpcInfo[i].HighestPerformanceInteger         = HighestPerf;
    CpuCpcInfo[i].NominalPerformanceInteger         = NominalPerf;
    CpuCpcInfo[i].LowestNonlinearPerformanceInteger = LowestNonlinearPerf;
    CpuCpcInfo[i].LowestPerformanceInteger          = LowestPerf;
    CpuCpcInfo[i].LowestFrequencyInteger            = ROUND_DIVISION (LowestPerf * CpcGranularity, 1000000);
    CpuCpcInfo[i].NominalFrequencyInteger           = ROUND_DIVISION (NominalPerf * CpcGranularity, 1000000);
    DEBUG ((DEBUG_INFO, "LowestFrequencyInteger = %d\n", CpuCpcInfo[i].LowestFrequencyInteger));
    DEBUG ((DEBUG_INFO, "NominalFrequencyInteger = %d\n", CpuCpcInfo[i].NominalFrequencyInteger));
  }

  return EFI_SUCCESS;
}

/** Initialize the GIC CPU interface information in platform configuration repository.

  @param [in]  This     Pointer to the Configuration Manager Protocol.

  @retval EFI_SUCCESS   Success
**/
STATIC
EFI_STATUS
EFIAPI
InitializeCmArmGiccInfo (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  This
  )
{
  EFI_STATUS                     Status;
  SKY1_PLATFORM_REPOSITORY_INFO  *PlatformRepo;
  UINT8                          CpuBootCoreId, CpuCoreNum;
  UINT8                          SetupCpuIndex;
  UINT32                         CpuCoreMask, MaxCpuCoreNum, i, Index;
  UINTN                          VarSize;
  CM_ARM_GICC_INFO               DefalutGicCInfo[PLAT_CPU_COUNT] = PLAT_GIC_CPU_INTERFACE;
  PLATFORM_SETUP_DATA            PlatformSetupVar;

  PlatformRepo = This->PlatRepoInfo;

  VarSize = sizeof (PLATFORM_SETUP_DATA);
  Status  = gRT->GetVariable (
                   PLATFORM_SETUP_VAR,
                   &gPlatformSetupVariableGuid,
                   NULL,
                   &VarSize,
                   &PlatformSetupVar
                   );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Get platform setup variable Fail! %r\n", Status));
    return EFI_NOT_FOUND;
  }

  GetCpuBootCoreId (&CpuBootCoreId);
  DEBUG ((DEBUG_INFO, "Cpu Boot Core Id 0x%x.\n", CpuBootCoreId));

  GetCpuCoreMask (&CpuCoreMask, &MaxCpuCoreNum);
  GetValidCpuCoreNum (&CpuCoreNum);
  CpuCount = 0;
  for (i = 0; i < CpuCoreNum; i++) {
    if (PlatformSetupVar.CpuCoreEnable[i] == 1) {
      CpuCount++;
    }
  }

  if (CpuCount == 0) {
    DEBUG ((DEBUG_ERROR, "All Cpu core not available!\n"));
    return EFI_DEVICE_ERROR;
  }

 #ifndef MULTICORE_ENABLE
  CpuCount = 1;
 #endif

  DEBUG ((DEBUG_INFO, "CpuCount 0x%x.\n", CpuCount));

  Status = gBS->AllocatePool (EfiBootServicesData, CpuCount*sizeof (CM_ARM_GICC_INFO), (VOID **)&(PlatformRepo->GicCInfo));
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Out of memory resource\n"));
    return Status;
  }

  ZeroMem (PlatformRepo->GicCInfo, CpuCount*sizeof (CM_ARM_GICC_INFO));

  // Initialize the GIC CPU interface information
  Index                                            = 0;
  PlatformRepo->GicCInfo[Index]                    = DefalutGicCInfo[CpuBootCoreId];
  PlatformRepo->GicCInfo[Index].CPUInterfaceNumber = 0;
  PlatformRepo->GicCInfo[Index].AcpiProcessorUid   = Index;
  PlatformRepo->CpuUidtoCoreNumberMap[Index]       = CpuBootCoreId;
  CpuCoreMask                                      = CpuCoreMask|(1<<CpuBootCoreId);
  Index++;
  SetupCpuIndex = 0;

  if (CpuCount == 1) {
    return EFI_SUCCESS;
  }

  for (i = 0; i < PLAT_CPU_COUNT; i++) {
    if (((CpuCoreMask>>i)&BIT0) == 1) {
      continue;
    }

    SetupCpuIndex++;
    if (PlatformSetupVar.CpuCoreEnable[SetupCpuIndex] == 0) {
      continue;
    }

    PlatformRepo->GicCInfo[Index]                    = DefalutGicCInfo[i];
    PlatformRepo->GicCInfo[Index].CPUInterfaceNumber = 0;
    PlatformRepo->GicCInfo[Index].AcpiProcessorUid   = Index;
    PlatformRepo->CpuUidtoCoreNumberMap[Index]       = i;
    Index++;
  }

  return EFI_SUCCESS;
}

/** Initialize the platform configuration repository.

  @param [in]  This     Pointer to the Configuration Manager Protocol.

  @retval EFI_SUCCESS   Success
**/
STATIC
EFI_STATUS
EFIAPI
InitializePlatformRepository (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  This
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  Status = InitializeCmArmGiccInfo (This);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: Failed to initialize GIC CPU interface information." \
      " Status = %r\n",
      Status
      ));
  }

  Status = InitializeCmArmCpcInfo (This);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: Failed to initialize CPU CPC interface information." \
      " Status = %r\n",
      Status
      ));
  }

  return Status;
}

/** A helper function for returning the Configuration Manager Objects.

  @param [in]       CmObjectId     The Configuration Manager Object ID.
  @param [in]       Object         Pointer to the Object(s).
  @param [in]       ObjectSize     Total size of the Object(s).
  @param [in]       ObjectCount    Number of Objects.
  @param [in, out]  CmObjectDesc   Pointer to the Configuration Manager Object
                                   descriptor describing the requested Object.

  @retval EFI_SUCCESS           Success.
**/
STATIC
EFI_STATUS
EFIAPI
HandleCmObject (
  IN  CONST CM_OBJECT_ID                CmObjectId,
  IN        VOID                        *Object,
  IN  CONST UINTN                       ObjectSize,
  IN  CONST UINTN                       ObjectCount,
  IN  OUT   CM_OBJ_DESCRIPTOR   *CONST  CmObjectDesc
  )
{
  CmObjectDesc->ObjectId = CmObjectId;
  CmObjectDesc->Size     = ObjectSize;
  CmObjectDesc->Data     = (VOID *)Object;
  CmObjectDesc->Count    = ObjectCount;
  DEBUG (
    (
     DEBUG_INFO,
     "INFO: CmObjectId = %x, Ptr = 0x%p, Size = %d, Count = %d\n",
     CmObjectId,
     CmObjectDesc->Data,
     CmObjectDesc->Size,
     CmObjectDesc->Count
    )
    );
  return EFI_SUCCESS;
}

/** Return a standard namespace object.

  @param [in]      This        Pointer to the Configuration Manager Protocol.
  @param [in]      CmObjectId  The Configuration Manager Object ID.
  @param [in]      Token       An optional token identifying the object. If
                               unused this must be CM_NULL_TOKEN.
  @param [in, out] CmObject    Pointer to the Configuration Manager Object
                               descriptor describing the requested Object.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The required object information is not found.
**/
EFI_STATUS
EFIAPI
GetStandardNameSpaceObject (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  This,
  IN  CONST CM_OBJECT_ID                                  CmObjectId,
  IN  CONST CM_OBJECT_TOKEN                               Token OPTIONAL,
  IN  OUT   CM_OBJ_DESCRIPTOR                     *CONST  CmObject
  )
{
  EFI_STATUS                     Status;
  SKY1_PLATFORM_REPOSITORY_INFO  *PlatformRepo;

  Status = EFI_SUCCESS;
  if ((This == NULL) || (CmObject == NULL)) {
    ASSERT (This != NULL);
    ASSERT (CmObject != NULL);
    return EFI_INVALID_PARAMETER;
  }

  PlatformRepo = This->PlatRepoInfo;

  switch (GET_CM_OBJECT_ID (CmObjectId)) {
    case EStdObjCfgMgrInfo:
      Status = HandleCmObject (
                 CmObjectId,
                 &PlatformRepo->CmInfo,
                 sizeof (PlatformRepo->CmInfo),
                 1,
                 CmObject
                 );
      break;
    case EStdObjAcpiTableList:
      Status = HandleCmObject (
                 CmObjectId,
                 &PlatformRepo->CmAcpiTableList,
                 sizeof (PlatformRepo->CmAcpiTableList),
                 ARRAY_SIZE (PlatformRepo->CmAcpiTableList),
                 CmObject
                 );
      break;
    default:
    {
      Status = EFI_NOT_FOUND;
      DEBUG ((
        DEBUG_ERROR,
        "WARNING: Object 0x%x. Status = %r\n",
        CmObjectId,
        Status
        ));
      break;
    }
  }

  return Status;
}

/** Return an CIX namespace object.

  @param [in]      This        Pointer to the Configuration Manager Protocol.
  @param [in]      CmObjectId  The Configuration Manager Object ID.
  @param [in]      Token       An optional token identifying the object. If
                               unused this must be CM_NULL_TOKEN.
  @param [in, out] CmObject    Pointer to the Configuration Manager Object
                               descriptor describing the requested Object.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The required object information is not found.
**/
EFI_STATUS
EFIAPI
GetCixNameSpaceObject (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  This,
  IN  CONST CM_OBJECT_ID                                  CmObjectId,
  IN  CONST CM_OBJECT_TOKEN                               Token OPTIONAL,
  IN  OUT   CM_OBJ_DESCRIPTOR                     *CONST  CmObject
  )
{
  EFI_STATUS                     Status;
  SKY1_PLATFORM_REPOSITORY_INFO  *PlatformRepo;

  Status = EFI_SUCCESS;
  if ((This == NULL) || (CmObject == NULL)) {
    ASSERT (This != NULL);
    ASSERT (CmObject != NULL);
    return EFI_INVALID_PARAMETER;
  }

  PlatformRepo = This->PlatRepoInfo;

  switch (GET_CM_OBJECT_ID (CmObjectId)) {
    case ECixObjCpuUidtoCoreNumberMap:
      Status = HandleCmObject (
                 CmObjectId,
                 &PlatformRepo->CpuUidtoCoreNumberMap,
                 PLAT_CPU_COUNT*sizeof (UINTN),
                 PLAT_CPU_COUNT,
                 CmObject
                 );
      break;

    default:
    {
      Status = EFI_NOT_FOUND;
      DEBUG ((
        DEBUG_ERROR,
        "WARNING: Object 0x%x. Status = %r\n",
        CmObjectId,
        Status
        ));
      break;
    }
  }// switch

  return Status;
}

/** Return an ARM namespace object.

  @param [in]      This        Pointer to the Configuration Manager Protocol.
  @param [in]      CmObjectId  The Configuration Manager Object ID.
  @param [in]      Token       An optional token identifying the object. If
                               unused this must be CM_NULL_TOKEN.
  @param [in, out] CmObject    Pointer to the Configuration Manager Object
                               descriptor describing the requested Object.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The required object information is not found.
**/
EFI_STATUS
EFIAPI
GetArmNameSpaceObject (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  This,
  IN  CONST CM_OBJECT_ID                                  CmObjectId,
  IN  CONST CM_OBJECT_TOKEN                               Token OPTIONAL,
  IN  OUT   CM_OBJ_DESCRIPTOR                     *CONST  CmObject
  )
{
  EFI_STATUS                     Status;
  SKY1_PLATFORM_REPOSITORY_INFO  *PlatformRepo;

  Status = EFI_SUCCESS;
  if ((This == NULL) || (CmObject == NULL)) {
    ASSERT (This != NULL);
    ASSERT (CmObject != NULL);
    return EFI_INVALID_PARAMETER;
  }

  PlatformRepo = This->PlatRepoInfo;

  switch (GET_CM_OBJECT_ID (CmObjectId)) {
    case EArmObjGicCInfo:
      Status = HandleCmObject (
                 CmObjectId,
                 PlatformRepo->GicCInfo,
                 CpuCount*sizeof (CM_ARM_GICC_INFO),
                 CpuCount,
                 CmObject
                 );
      break;

    case EArmObjGicDInfo:
      Status = HandleCmObject (
                 CmObjectId,
                 &PlatformRepo->GicDInfo,
                 sizeof (PlatformRepo->GicDInfo),
                 1,
                 CmObject
                 );
      break;

    case EArmObjGicRedistributorInfo:
      Status = HandleCmObject (
                 CmObjectId,
                 &PlatformRepo->GicRedistInfo,
                 sizeof (PlatformRepo->GicRedistInfo),
                 PLAT_GIC_REDISTRIBUTOR_COUNT,
                 CmObject
                 );
      break;

    case EArmObjGicItsInfo:
      Status = HandleCmObject (
                 CmObjectId,
                 &PlatformRepo->GicItsInfo,
                 sizeof (PlatformRepo->GicItsInfo),
                 PLAT_GIC_ITS_COUNT,
                 CmObject
                 );
      break;

    case EArmObjCpcInfo:
      Status = HandleCmObject (
                 CmObjectId,
                 &PlatformRepo->CpuCpcInfo,
                 PLAT_CPU_COUNT*sizeof (CM_ARM_CPC_INFO),
                 PLAT_CPU_COUNT,
                 CmObject
                 );
      break;

    default:
    {
      Status = EFI_NOT_FOUND;
      DEBUG ((
        DEBUG_ERROR,
        "WARNING: Object 0x%x. Status = %r\n",
        CmObjectId,
        Status
        ));
      break;
    }
  }// switch

  return Status;
}

/** Return an OEM namespace object.

  @param [in]      This        Pointer to the Configuration Manager Protocol.
  @param [in]      CmObjectId  The Configuration Manager Object ID.
  @param [in]      Token       An optional token identifying the object. If
                               unused this must be CM_NULL_TOKEN.
  @param [in, out] CmObject    Pointer to the Configuration Manager Object
                               descriptor describing the requested Object.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The required object information is not found.
**/
EFI_STATUS
EFIAPI
GetOemNameSpaceObject (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  This,
  IN  CONST CM_OBJECT_ID                                  CmObjectId,
  IN  CONST CM_OBJECT_TOKEN                               Token OPTIONAL,
  IN  OUT   CM_OBJ_DESCRIPTOR                     *CONST  CmObject
  )
{
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;
  if ((This == NULL) || (CmObject == NULL)) {
    ASSERT (This != NULL);
    ASSERT (CmObject != NULL);
    return EFI_INVALID_PARAMETER;
  }

  switch (GET_CM_OBJECT_ID (CmObjectId)) {
    default:
    {
      Status = EFI_NOT_FOUND;
      DEBUG ((
        DEBUG_ERROR,
        "WARNING: Object 0x%x. Status = %r\n",
        CmObjectId,
        Status
        ));
      break;
    }
  }

  return Status;
}

/** The GetObject function defines the interface implemented by the
    Configuration Manager Protocol for returning the Configuration
    Manager Objects.

  @param [in]      This        Pointer to the Configuration Manager Protocol.
  @param [in]      CmObjectId  The Configuration Manager Object ID.
  @param [in]      Token       An optional token identifying the object. If
                               unused this must be CM_NULL_TOKEN.
  @param [in, out] CmObject    Pointer to the Configuration Manager Object
                               descriptor describing the requested Object.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The required object information is not found.
**/
EFI_STATUS
EFIAPI
Sky1PlatformGetObject (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  This,
  IN  CONST CM_OBJECT_ID                                  CmObjectId,
  IN  CONST CM_OBJECT_TOKEN                               Token OPTIONAL,
  IN  OUT   CM_OBJ_DESCRIPTOR                     *CONST  CmObject
  )
{
  EFI_STATUS  Status;

  if ((This == NULL) || (CmObject == NULL)) {
    ASSERT (This != NULL);
    ASSERT (CmObject != NULL);
    return EFI_INVALID_PARAMETER;
  }

  switch (GET_CM_NAMESPACE_ID (CmObjectId)) {
    case EObjNameSpaceStandard:
      Status = GetStandardNameSpaceObject (This, CmObjectId, Token, CmObject);
      break;
    case EObjNameSpaceArm:
      Status = GetArmNameSpaceObject (This, CmObjectId, Token, CmObject);
      break;
    case EObjNameSpaceCix:
      Status = GetCixNameSpaceObject (This, CmObjectId, Token, CmObject);
      break;
    case EObjNameSpaceOem:
      Status = GetOemNameSpaceObject (This, CmObjectId, Token, CmObject);
      break;
    default:
    {
      Status = EFI_INVALID_PARAMETER;
      DEBUG ((
        DEBUG_ERROR,
        "ERROR: Unknown Namespace Object = 0x%x. Status = %r\n",
        CmObjectId,
        Status
        ));
      break;
    }
  }

  return Status;
}

/** The SetObject function defines the interface implemented by the
    Configuration Manager Protocol for updating the Configuration
    Manager Objects.

  @param [in]      This        Pointer to the Configuration Manager Protocol.
  @param [in]      CmObjectId  The Configuration Manager Object ID.
  @param [in]      Token       An optional token identifying the object. If
                               unused this must be CM_NULL_TOKEN.
  @param [in]      CmObject    Pointer to the Configuration Manager Object
                               descriptor describing the Object.

  @retval EFI_UNSUPPORTED  This operation is not supported.
**/
EFI_STATUS
EFIAPI
Sky1PlatformSetObject (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  This,
  IN  CONST CM_OBJECT_ID                                  CmObjectId,
  IN  CONST CM_OBJECT_TOKEN                               Token OPTIONAL,
  IN        CM_OBJ_DESCRIPTOR                     *CONST  CmObject
  )
{
  return EFI_UNSUPPORTED;
}

/** A structure describing the configuration manager protocol interface.
*/
STATIC
CONST
EDKII_CONFIGURATION_MANAGER_PROTOCOL  Sky1ConfigManagerProtocol = {
  CREATE_REVISION (1,    0),
  Sky1PlatformGetObject,
  Sky1PlatformSetObject,
  &Sky1PlatformRepositoryInfo
};

/**
  Entrypoint of Configuration Manager Dxe.

  @param  ImageHandle
  @param  SystemTable

  @return EFI_SUCCESS
  @return EFI_LOAD_ERROR
  @return EFI_OUT_OF_RESOURCES

**/
EFI_STATUS
EFIAPI
ConfigurationManagerDxeInitialize (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gEdkiiConfigurationManagerProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  (VOID *)&Sky1ConfigManagerProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: Failed to install Configuration Manager Protocol." \
      " Status = %r\n",
      Status
      ));
    goto error_handler;
  }

  Status = InitializePlatformRepository (
             &Sky1ConfigManagerProtocol
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: Failed to initialize the Platform Configuration Repository." \
      " Status = %r\n",
      Status
      ));
  }

error_handler:
  return Status;
}
