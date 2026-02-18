## @file
# This is a build description file for the USB3 Debug advanced feature.
# This file should be included into another package DSC file to build this feature.
#
# The DEC files are used by the utilities that parse DSC and
# INF files to generate AutoGen.c and AutoGen.h files
# for the build infrastructure.
#
# Copyright (c) 2019 - 2021, Intel Corporation. All rights reserved.<BR>
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

################################################################################
#
# Library Class section - list of all Library Classes needed by this feature.
#
################################################################################

[LibraryClasses.Common]
  Usb3DebugPortParamLib|Usb3DebugFeaturePkg/Library/Usb3DebugPortParamLibPcd/Usb3DebugPortParamLibPcd.inf

#
# If NULL Usb3DebugPortLib library instance desired
#
!if gUsb3DebugFeaturePkgTokenSpaceGuid.PcdUsb3DebugPortLibInstance == 0
  [LibraryClasses.Common]
    Usb3DebugPortLib|Usb3DebugFeaturePkg/Library/Usb3DebugPortLib/Usb3DebugPortLibNull.inf
!endif

#
# If regular Usb3DebugPortLib library instance desired
#
!if gUsb3DebugFeaturePkgTokenSpaceGuid.PcdUsb3DebugPortLibInstance == 1
  [LibraryClasses.PEI_CORE, LibraryClasses.PEIM]
    Usb3DebugPortLib|Usb3DebugFeaturePkg/Library/Usb3DebugPortLib/Usb3DebugPortLibPei.inf

  [LibraryClasses.DXE_CORE, LibraryClasses.DXE_DRIVER, LibraryClasses.DXE_RUNTIME_DRIVER, LibraryClasses.SMM_CORE, LibraryClasses.DXE_SMM_DRIVER, LibraryClasses.UEFI_DRIVER]
    Usb3DebugPortLib|Usb3DebugFeaturePkg/Library/Usb3DebugPortLib/Usb3DebugPortLibDxe.inf
!endif

#
# If regular Usb3DebugPortLib library instance desired
#
!if gUsb3DebugFeaturePkgTokenSpaceGuid.PcdUsb3DebugPortLibInstance == 2
  [LibraryClasses.PEI_CORE, LibraryClasses.PEIM]
    Usb3DebugPortLib|Usb3DebugFeaturePkg/Library/Usb3DebugPortLib/Usb3DebugPortLibPeiIoMmu.inf

  [LibraryClasses.DXE_CORE, LibraryClasses.DXE_DRIVER, LibraryClasses.DXE_RUNTIME_DRIVER, LibraryClasses.SMM_CORE, LibraryClasses.DXE_SMM_DRIVER, LibraryClasses.UEFI_DRIVER]
    Usb3DebugPortLib|Usb3DebugFeaturePkg/Library/Usb3DebugPortLib/Usb3DebugPortLibDxeIoMmu.inf
!endif
