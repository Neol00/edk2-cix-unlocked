## @file
# Commonly used stage configuration logic
#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[PcdsFeatureFlag.Common]
  !if gMinPlatformPkgTokenSpaceGuid.PcdBootStage >= 1
    gMinPlatformPkgTokenSpaceGuid.PcdStopAfterDebugInit|TRUE
  !endif

  !if gMinPlatformPkgTokenSpaceGuid.PcdBootStage >= 2
    gMinPlatformPkgTokenSpaceGuid.PcdStopAfterDebugInit|FALSE
    gMinPlatformPkgTokenSpaceGuid.PcdStopAfterMemInit|TRUE
  !endif

  !if gMinPlatformPkgTokenSpaceGuid.PcdBootStage >= 3
    gMinPlatformPkgTokenSpaceGuid.PcdStopAfterMemInit|FALSE
    gMinPlatformPkgTokenSpaceGuid.PcdBootToShellOnly|TRUE
  !endif

  !if gMinPlatformPkgTokenSpaceGuid.PcdBootStage >= 4
    gMinPlatformPkgTokenSpaceGuid.PcdBootToShellOnly|FALSE
  !endif

  !if gMinPlatformPkgTokenSpaceGuid.PcdBootStage >= 5
    gMinPlatformPkgTokenSpaceGuid.PcdUefiSecureBootEnable|TRUE
    gMinPlatformPkgTokenSpaceGuid.PcdTpm2Enable|TRUE
  !endif

[PcdsFixedAtBuild.Common]
  !if $(TARGET) != RELEASE
    gEfiMdeModulePkgTokenSpaceGuid.PcdSmiHandlerProfilePropertyMask|0x1
  !endif
