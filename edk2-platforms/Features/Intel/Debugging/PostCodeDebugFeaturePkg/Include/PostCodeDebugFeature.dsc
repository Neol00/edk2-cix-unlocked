## @file
# This package provides PostCode Debug feature.
# This file should be included into another package DSC file to build this feature.
#
# The DEC files are used by the utilities that parse DSC and
# INF files to generate AutoGen.c and AutoGen.h files
# for the build infrastructure.
#
# Copyright (c) 2020 - 2021, Intel Corporation. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]
  !ifndef $(PEI_ARCH)
    !error "PEI_ARCH must be specified to build this feature!"
  !endif
  !ifndef $(DXE_ARCH)
    !error "DXE_ARCH must be specified to build this feature!"
  !endif

  DEFINE POST_CODE_PEIM_FILENAME  = 3ea07dd3-f837-40c0-ac56-f3e18a30d01b
  DEFINE POST_CODE_DXE_FILENAME   = e7d785f5-e2f3-45e3-b0e7-2291a6c6dea6
  DEFINE POST_CODE_SMM_FILENAME   = 02a955c7-48c0-4178-989b-b3fea4b3c6a2

[PcdsDynamicExDefault]
  #
  # By default, make the functional control DynamicExDefault PCD so that it can be enabled when debugging.
  #
  gPostCodeDebugFeaturePkgTokenSpaceGuid.PcdStatusCodeUsePostCode

[LibraryClasses.Common]
  PostCodeMapLib|PostCodeDebugFeaturePkg/Library/PostCodeMapLib/PostCodeMapLib.inf

[LibraryClasses.PEIM, LibraryClasses.PEI_CORE]
  StatusCodeHandlerLib|PostCodeDebugFeaturePkg/Library/PostCodeStatusCodeHandlerLib/PeiPostCodeStatusCodeHandlerLib.inf

[LibraryClasses.DXE_RUNTIME_DRIVER]
  StatusCodeHandlerLib|PostCodeDebugFeaturePkg/Library/PostCodeStatusCodeHandlerLib/RuntimeDxePostCodeStatusCodeHandlerLib.inf

[LibraryClasses.DXE_SMM_DRIVER]
  StatusCodeHandlerLib|PostCodeDebugFeaturePkg/Library/PostCodeStatusCodeHandlerLib/SmmPostCodeStatusCodeHandlerLib.inf

[Components.IA32]

  MdeModulePkg/Universal/StatusCodeHandler/Pei/StatusCodeHandlerPei.inf {
    <Defines>
      #
      # Many boards already have StatusCodeHandler components built from the common core code
      # Providing a unique name avoids collisions.  Both status code handler components will
      # install their listeners with the status code routers.
      #
      FILE_GUID = $(POST_CODE_PEIM_FILENAME)
    <LibraryClasses>
      NULL|PostCodeDebugFeaturePkg/Library/PostCodeStatusCodeHandlerLib/PeiPostCodeStatusCodeHandlerLib.inf
  }

[Components.X64]
  MdeModulePkg/Universal/StatusCodeHandler/RuntimeDxe/StatusCodeHandlerRuntimeDxe.inf {
    <Defines>
      #
      # Many boards already have StatusCodeHandler components built from the common core code
      # Providing a unique name avoids collisions.  Both status code handler components will
      # install their listeners with the status code routers.
      #
      FILE_GUID = $(POST_CODE_DXE_FILENAME)
    <LibraryClasses>
      NULL|PostCodeDebugFeaturePkg/Library/PostCodeStatusCodeHandlerLib/RuntimeDxePostCodeStatusCodeHandlerLib.inf
  }

  MdeModulePkg/Universal/StatusCodeHandler/Smm/StatusCodeHandlerSmm.inf {
    <Defines>
      #
      # Many boards already have StatusCodeHandler components built from the common core code
      # Providing a unique name avoids collisions.  Both status code handler components will
      # install their listeners with the status code routers.
      #
      FILE_GUID = $(POST_CODE_SMM_FILENAME)
    <LibraryClasses>
      NULL|PostCodeDebugFeaturePkg/Library/PostCodeStatusCodeHandlerLib/SmmPostCodeStatusCodeHandlerLib.inf
  }
