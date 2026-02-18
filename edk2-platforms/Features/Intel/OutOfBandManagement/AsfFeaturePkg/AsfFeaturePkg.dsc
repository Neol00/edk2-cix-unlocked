## @file
# Asf Package
#
# Copyright (c) 1985 - 2022, AMI. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
##

[Defines]
  PLATFORM_NAME                 = AsfFeaturePkg
  PLATFORM_GUID                 = 79D22E13-3F30-470A-AF9D-B80CB4324379
  PLATFORM_VERSION              = 0.1
  DSC_SPECIFICATION             = 0x00010005
  OUTPUT_DIRECTORY              = Build/$(PLATFORM_NAME)
  SUPPORTED_ARCHITECTURES       = IA32|X64
  BUILD_TARGETS                 = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER              = DEFAULT
  PEI_ARCH                      = IA32
  DXE_ARCH                      = X64

[Packages]
  MinPlatformPkg/MinPlatformPkg.dec

#
# MinPlatform common include for required feature PCD
# These PCD must be set before the core include files, CoreCommonLib,
# CorePeiLib, and CoreDxeLib.
#
!include MinPlatformPkg/Include/Dsc/MinPlatformFeaturesPcd.dsc.inc

#
# Include common libraries
#
!include MinPlatformPkg/Include/Dsc/CoreCommonLib.dsc
!include MinPlatformPkg/Include/Dsc/CorePeiLib.dsc
!include MinPlatformPkg/Include/Dsc/CoreDxeLib.dsc

#
# This package always builds the feature.
#
!include Include/AsfFeature.dsc
