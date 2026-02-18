/** @file
  CIX AML Lib.

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef CIX_AML_LIB_H_
#define CIX_AML_LIB_H_

/** Create a _PSD node.

  Creates and optionally adds the following node
   Name(_PSD, Package()
   {
      Package()
      {
        NumEntries,             //Integer
        Revision,               //Integer(Byte)
        Domain,                 //Integer(DWORD)
        CoordType,              //Integer(DWORD)
        NumProcessors,          //Integer(DWORD)
      }
   }


  Cf. ACPI 6.5, s8.4.5.5  _PSD (P-State Dependency)

  @ingroup CodeGenApis

  @param [in]  NumEntries            The number of entries in the PStateDependency
                                     package including this field.
  @param [in]  Revision              The revision number of the PStateDependency package format.
  @param [in]  Domain                The dependency domain number to which this P state entry belongs.
  @param [in]  CoordType             P-state coordination types.
  @param [in]  NumProcessors         The number of processors belonging to the domain for
                                     this logical processor’s P-states.
  @param [in]  ParentNode            If provided, set ParentNode as the parent
                                     of the node created.
  @param [out] NewCpcNode            If success and provided, contains the created node.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory.
**/
EFI_STATUS
EFIAPI
AmlCreatePsdNode (
  IN  UINT16                  NumEntries,
  IN  UINT8                   Revision,
  IN  UINT32                  Domain,
  IN  UINT32                  CoordType,
  IN  UINT32                  NumProcessors,
  IN  AML_NODE_HANDLE         ParentNode,
  OUT AML_OBJECT_NODE_HANDLE  *NewCpcNode   OPTIONAL
  );

/** Create a CPU passive thermal node.

  @ingroup CodeGenApis

  @param [in]  NameString            The new variable name.
                                     Must be a NULL-terminated ASL NameString
                                     e.g.: "DEV0", "DV15.DEV0", etc.
                                     The input string is copied.
  @param [in]  CpuNameStringPointer  The pinter point to cpu string list
  @param [in]  CpuNumber             The number of valid CPU
  @param [in]  CpuNameStringLength   The string length of cpu name
  @param [in]  ParentNode            If provided, set ParentNode as the parent
                                     of the node created.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory.
**/
EFI_STATUS
EFIAPI
AmlCreateCpuPTNode (
  IN  CONST CHAR8      *NameString,
  IN  CHAR8            *CpuNameStringPointer,
  IN  UINT32           CpuNumber,
  IN  UINT32           CpuNameStringLength,
  IN  AML_NODE_HANDLE  ParentNode
  );

#endif // CIX_AML_LIB_H_
