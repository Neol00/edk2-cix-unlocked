## @file
#  Advanced Feature Package build description file.
#
# Advanced features allow a board package to be extended with advanced functionality.
#
# This package should not contain any specific advanced features. The package has two responsibilities:
# 1. Present a consolidated and simplified view of all available advanced features to board packages.
# 2. Provide a simple, single package build for all available advanced features.
#
# Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
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
  PLATFORM_NAME                       = AdvancedFeaturePkg
  PLATFORM_GUID                       = 5E4C05BC-C5F0-4843-BAE1-3AAFE269DB8F
  PLATFORM_VERSION                    = 0.1
  DSC_SPECIFICATION                   = 0x00010005
  OUTPUT_DIRECTORY                    = Build/AdvancedFeaturePkg
  SUPPORTED_ARCHITECTURES             = IA32|X64
  BUILD_TARGETS                       = DEBUG|RELEASE
  SKUID_IDENTIFIER                    = DEFAULT
  PEI_ARCH                            = IA32
  DXE_ARCH                            = X64
  FLASH_DEFINITION                    = AdvancedFeaturePkg/AdvancedFeaturePkg.fdf

[Packages]
  MdePkg/MdePkg.dec
  MinPlatformPkg/MinPlatformPkg.dec

!include AdvancedFeaturePkg/Include/AdvancedFeaturesPcd.dsc

################################################################################
#
# Advanced Feature Enable section - all advanced features are enabling for the
#                                   AdvancedFeaturePkg build.
#
################################################################################
[PcdsFeatureFlag]
  #
  # Debugging features
  #
  gAcpiDebugFeaturePkgTokenSpaceGuid.PcdAcpiDebugFeatureEnable              |TRUE
  gAcpiDebugFeaturePkgTokenSpaceGuid.PcdUseSmmVersion                       |FALSE
  gBeepDebugFeaturePkgTokenSpaceGuid.PcdBeepDebugFeatureEnable              |TRUE
  gPostCodeDebugFeaturePkgTokenSpaceGuid.PcdPostCodeDebugFeatureEnable      |TRUE
  gUsb3DebugFeaturePkgTokenSpaceGuid.PcdUsb3DebugFeatureEnable              |TRUE

  #
  # Network features
  #
  gNetworkFeaturePkgTokenSpaceGuid.PcdNetworkFeatureEnable                  |TRUE

  #
  # OutOfBandManagement features
  #
  gIpmiFeaturePkgTokenSpaceGuid.PcdIpmiFeatureEnable                        |TRUE
  gSpcrFeaturePkgTokenSpaceGuid.PcdSpcrFeatureEnable                        |TRUE
  gAsfFeaturePkgTokenSpaceGuid.PcdAsfFeatureEnable                          |TRUE

  #
  # PowerManagement features
  #
  gS3FeaturePkgTokenSpaceGuid.PcdS3FeatureEnable                            |TRUE

  #
  # SystemInformation features
  #
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosFeatureEnable                    |TRUE

  #
  # UserInterface features
  #
  gLogoFeaturePkgTokenSpaceGuid.PcdLogoFeatureEnable                        |TRUE
  gUserAuthFeaturePkgTokenSpaceGuid.PcdUserAuthenticationFeatureEnable      |TRUE
  gVirtualKeyboardFeaturePkgTokenSpaceGuid.PcdVirtualKeyboardFeatureEnable  |TRUE

  #
  # Individual features
  #
  gPlatformPayloadFeaturePkgTokenSpaceGuid.PcdPlatformPayloadFeatureEnable  |TRUE

#
# PCD that are required to be set by the build target should be configured here for test purposes
# These settings are only for the purposes of buildings, boards should follow instructions in Readme files.
#

[PcdsFixedAtBuild]
  gUsb3DebugFeaturePkgTokenSpaceGuid.PcdUsb3DebugPortLibInstance|1

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
# Libraries required to be specified by the build target
#
[LibraryClasses.Common]
  PlatformHookLib|MdeModulePkg/Library/BasePlatformHookLibNull/BasePlatformHookLibNull.inf # Required by BeepDebugFeature

[LibraryClasses.Common.PEIM]
  SmmAccessLib|IntelSiliconPkg/Feature/SmmAccess/Library/PeiSmmAccessLib/PeiSmmAccessLib.inf # Required by S3Feature

#
# This package builds all advanced features.
#
!include Include/AdvancedFeatures.dsc
