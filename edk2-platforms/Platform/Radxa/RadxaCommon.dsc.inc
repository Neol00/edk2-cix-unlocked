## @file
#
#  Copyright 2024 Radxa Computer (Shenzhen) Co., Ltd. All Rights Reserved.
#  Copyright (c) 2018, Linaro Ltd. All rights reserved.
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Components.common]

[BuildOptions]
  GCC:*_*_*_CC_FLAGS              = -DCOMPILE_BUILD_DATE=$(COMPILE_BUILD_DATE)
!if $(TARGET) == RELEASE
  GCC:*_*_*_CC_FLAGS              = -DUEFI_FW_VERSION=$(COMPILE_DEB_VERSION)
!else
  GCC:*_*_*_CC_FLAGS              = -DUEFI_FW_VERSION=$(COMPILE_DEB_VERSION)"-"$(COMPILE_COMMIT_HASH)"-debug:"$(COMPILE_BUILD_DATE)
!endif

[PcdsFixedAtBuild.common]
  # Version string
!if $(TARGET) == RELEASE
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVersionString|L"$(COMPILE_DEB_VERSION)"
!else
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVersionString|L"$(COMPILE_DEB_VERSION)-$(COMPILE_COMMIT_HASH)-debug:$(COMPILE_BUILD_DATE)"
!endif

  # Code info
  gRadxaTokenSpaceGuid.PcdEdk2CixCommitHash|L"$(COMPILE_COMMIT_HASH)"
  gRadxaTokenSpaceGuid.PcdEdk2CommitHash|L"$(COMPILE_EDK2_COMMIT_HASH)"
  gRadxaTokenSpaceGuid.PcdEdk2NonOSICommitHash|L"$(COMPILE_EDK2_NON_OSI_COMMIT_HASH)"
  gRadxaTokenSpaceGuid.PcdEdk2PlatformsCommitHash|L"$(COMPILE_EDK2_PLATFORMS_COMMIT_HASH)"
