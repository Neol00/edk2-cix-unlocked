## @file
# This package provides Beep Debug feature.
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

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  !ifndef $(PEI_ARCH)
    !error "PEI_ARCH must be specified to build this feature!"
  !endif
  !ifndef $(DXE_ARCH)
    !error "DXE_ARCH must be specified to build this feature!"
  !endif

  DEFINE BEEP_PEIM_FILENAME  = b2356504-8ea3-42bd-912a-4b331990644a
  DEFINE BEEP_DXE_FILENAME   = f1211fa9-d83d-4c79-8726-3afaebba1070
  DEFINE BEEP_SMM_FILENAME   = a82cd452-0f17-4417-b8be-bb8cfdf9fa26

################################################################################
#
# Component section - list of all components that need built for this feature.
#
# Note: The EDK II DSC file is not used to specify how compiled binary images get placed
#       into firmware volume images. This section is just a list of modules to compile from
#       source into UEFI-compliant binaries.
#       It is the FDF file that contains information on combining binary files into firmware
#       volume images, whose concept is beyond UEFI and is described in PI specification.
#       There may also be modules listed in this section that are not required in the FDF file,
#       When a module listed here is excluded from FDF file, then UEFI-compliant binary will be
#       generated for it, but the binary will not be put into any firmware volume.
#
################################################################################
#
# Feature PEI Components
#

#
# By default, make the functional control a patcheable in module PCD
#
[PcdsPatchableInModule]
  gBeepDebugFeaturePkgTokenSpaceGuid.PcdStatusCodeUseBeep

[LibraryClasses.Common]
  BeepLib|BeepDebugFeaturePkg/Library/BeepLib/BeepLibNull.inf
  BeepMapLib|BeepDebugFeaturePkg/Library/BeepMapLib/BeepMapLib.inf

[LibraryClasses.PEIM, LibraryClasses.PEI_CORE]
  StatusCodeHandlerLib|BeepDebugFeaturePkg/Library/BeepStatusCodeHandlerLib/PeiBeepStatusCodeHandlerLib.inf

[LibraryClasses.DXE_RUNTIME_DRIVER]
  StatusCodeHandlerLib|BeepDebugFeaturePkg/Library/BeepStatusCodeHandlerLib/RuntimeDxeBeepStatusCodeHandlerLib.inf

[LibraryClasses.DXE_SMM_DRIVER]
  StatusCodeHandlerLib|BeepDebugFeaturePkg/Library/BeepStatusCodeHandlerLib/SmmBeepStatusCodeHandlerLib.inf

[Components.IA32]

  MdeModulePkg/Universal/StatusCodeHandler/Pei/StatusCodeHandlerPei.inf {
    <Defines>
      #
      # Many boards already have StatusCodeHandler components built from the common core code
      # Providing a unique name avoids collisions.  Both status code handler components will
      # install their listeners with the status code routers.
      #
      FILE_GUID = $(BEEP_PEIM_FILENAME)
    <LibraryClasses>
      NULL|BeepDebugFeaturePkg/Library/BeepStatusCodeHandlerLib/PeiBeepStatusCodeHandlerLib.inf
  }

[Components.X64]
  MdeModulePkg/Universal/StatusCodeHandler/RuntimeDxe/StatusCodeHandlerRuntimeDxe.inf {
    <Defines>
      #
      # Many boards already have StatusCodeHandler components built from the common core code
      # Providing a unique name avoids collisions.  Both status code handler components will
      # install their listeners with the status code routers.
      #
      FILE_GUID = $(BEEP_DXE_FILENAME)
    <LibraryClasses>
      NULL|BeepDebugFeaturePkg/Library/BeepStatusCodeHandlerLib/RuntimeDxeBeepStatusCodeHandlerLib.inf
  }

  MdeModulePkg/Universal/StatusCodeHandler/Smm/StatusCodeHandlerSmm.inf {
    <Defines>
      #
      # Many boards already have StatusCodeHandler components built from the common core code
      # Providing a unique name avoids collisions.  Both status code handler components will
      # install their listeners with the status code routers.
      #
      FILE_GUID = $(BEEP_SMM_FILENAME)
    <LibraryClasses>
      NULL|BeepDebugFeaturePkg/Library/BeepStatusCodeHandlerLib/SmmBeepStatusCodeHandlerLib.inf
  }
