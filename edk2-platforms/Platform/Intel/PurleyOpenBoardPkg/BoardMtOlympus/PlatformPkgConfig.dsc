## @file
#
# Copyright (c) 2018 - 2021, Intel Corporation. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

#
# TRUE is ENABLE. FALSE is DISABLE.
#

[PcdsFixedAtBuild]
  gMinPlatformPkgTokenSpaceGuid.PcdBootStage|4

[PcdsFeatureFlag]
  #
  # MinPlatform common include for required feature PCD
  # These PCD must be set before the core include files, CoreCommonLib,
  # CorePeiLib, and CoreDxeLib.
  # Optional MinPlatformPkg features should be enabled after this
  #
  !include MinPlatformPkg/Include/Dsc/MinPlatformFeaturesPcd.dsc.inc

  #
  # Commonly used MinPlatform feature configuration logic that maps functionity to stage
  #
  !include BoardModulePkg/Include/Dsc/CommonStageConfig.dsc.inc

  gMinPlatformPkgTokenSpaceGuid.PcdPerformanceEnable|TRUE

  gPlatformTokenSpaceGuid.PcdFastBoot|FALSE
!if gPlatformTokenSpaceGuid.PcdFastBoot == TRUE
  gIpmiFeaturePkgTokenSpaceGuid.PcdIpmiFeatureEnable|FALSE
  gPlatformTokenSpaceGuid.PcdUpdateConsoleInBds|FALSE
!endif
