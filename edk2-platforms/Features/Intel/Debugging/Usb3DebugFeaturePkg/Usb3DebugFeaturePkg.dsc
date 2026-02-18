## @file
# This package provides advanced feature functionality for USB3 Debug support.
# This package should only depend on EDK II Core packages, IntelSiliconPkg, and MinPlatformPkg.
#
# The DEC files are used by the utilities that parse DSC and
# INF files to generate AutoGen.c and AutoGen.h files
# for the build infrastructure.
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]
  PLATFORM_NAME                  = Usb3DebugFeaturePkg
  PLATFORM_GUID                  = 3A861E9B-E2B3-4725-BD3B-93A7FB5E861E
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/$(PLATFORM_NAME)
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER               = DEFAULT
  PEI_ARCH                       = IA32
  DXE_ARCH                       = X64

[PcdsFixedAtBuild]
  gUsb3DebugFeaturePkgTokenSpaceGuid.PcdUsb3DebugPortLibInstance|0

#
# This package always builds the feature.
#
!include Include/Usb3DebugFeature.dsc

#
# This package currently only contains library classes.  To force them to be built since there is no code to use them
# we just tell the build that they are components and the build will compile the libraries
#

[Components]
  Usb3DebugFeaturePkg/Library/Usb3DebugPortLib/Usb3DebugPortLibNull.inf
  Usb3DebugFeaturePkg/Library/Usb3DebugPortParamLibPcd/Usb3DebugPortParamLibPcd.inf

  Usb3DebugFeaturePkg/Library/Usb3DebugPortLib/Usb3DebugPortLibPei.inf
  Usb3DebugFeaturePkg/Library/Usb3DebugPortLib/Usb3DebugPortLibPeiIoMmu.inf

  Usb3DebugFeaturePkg/Library/Usb3DebugPortLib/Usb3DebugPortLibDxe.inf
  Usb3DebugFeaturePkg/Library/Usb3DebugPortLib/Usb3DebugPortLibDxeIoMmu.inf
