## @file
# Build file for generating AML offset table
#
# @copyright
# Copyright (C) 2021 Intel Corporation.
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
##

[Defines]
  PLATFORM_NAME                       = $(RP_PKG)
  PLATFORM_GUID                       = D7EAF54D-C9B9-4075-89F0-71943DBCFA61
  PLATFORM_VERSION                    = 0.1
  DSC_SPECIFICATION                   = 0x00010005
  OUTPUT_DIRECTORY                    = Build/$(RP_PKG)
  SUPPORTED_ARCHITECTURES             = IA32|X64
  BUILD_TARGETS                       = DEBUG|RELEASE
  PLATFORM_SI_PACKAGE                 = ClientOneSiliconPkg
  DEFINE      PLATFORM_SI_BIN_PACKAGE = WhitleySiliconBinPkg
  PEI_ARCH                            = IA32
  DXE_ARCH                            = X64

!if $(CPUTARGET) == "CPX"
  DEFINE FSP_BIN_PKG            = CedarIslandFspBinPkg
  DEFINE IIO_INSTANCE           = Skx
!elseif $(CPUTARGET) == "ICX"
  DEFINE FSP_BIN_PKG            = WhitleyFspBinPkg
  DEFINE IIO_INSTANCE           = Icx
!else
  DEFINE IIO_INSTANCE           = UnknownCpu
!endif

  #
  # Platform On/Off features are defined here
  #
  !include $(RP_PKG)/PlatformPkgConfig.dsc

[Components.X64]
  $(RP_PKG)/WilsonCityRvp/AmlOffsets/AmlOffsets.inf

!include $(RP_PKG)/Include/Dsc/BuildOptions.dsc
