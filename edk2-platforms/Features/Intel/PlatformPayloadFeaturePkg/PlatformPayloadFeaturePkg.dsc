## @file
# platform Payload Package
#
# Provides platform specific drivers and definitions to create a platform FV
# to work with universal UEFI payload to provide support for common Intel platforms.
#
# Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                       = PlatformPayloadFeaturePkg
  PLATFORM_GUID                       = D3C551BE-9BC6-48F5-AA8A-F49425C28CA9
  PLATFORM_VERSION                    = 0.1
  DSC_SPECIFICATION                   = 0x00010005
  SUPPORTED_ARCHITECTURES             = X64
  BUILD_TARGETS                       = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER                    = DEFAULT
  OUTPUT_DIRECTORY                    = Build/$(PLATFORM_NAME)
  FLASH_DEFINITION                    = $(PLATFORM_NAME)/PlatformPayloadFeaturePkg.fdf
  PCD_DYNAMIC_AS_DYNAMICEX            = TRUE
  DXE_ARCH                            = X64

  !ifndef SMM_VARIABLE
    DEFINE SMM_VARIABLE                 = TRUE
  !endif

[Packages]
  MinPlatformPkg/MinPlatformPkg.dec

################################################################################
#
# SKU Identification section - list of all SKU IDs supported by this Platform.
#
################################################################################
[SkuIds]
  0|DEFAULT

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################

  #
  # MinPlatform common include for required feature PCD
  # These PCD must be set before the core include files, CoreCommonLib,
  # CorePeiLib, and CoreDxeLib.
  # Optional MinPlatformPkg features should be enabled after this
  #
  !include MinPlatformPkg/Include/Dsc/MinPlatformFeaturesPcd.dsc.inc

[PcdsPatchableInModule]
  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x7
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x8000004F
!if $(SOURCE_DEBUG_ENABLE)
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x17
!else
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2F
!endif

  #
  # The following parameters are set by Library/PlatformHookLib
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialUseMmio|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialBaudRate|115200
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterStride|1

################################################################################
#
# Pcd DynamicEx Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdsDynamicExDefault]
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress|0
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseSize|0


#
# Include common libraries
#
!include MinPlatformPkg/Include/Dsc/CoreCommonLib.dsc
!include MinPlatformPkg/Include/Dsc/CorePeiLib.dsc
!include MinPlatformPkg/Include/Dsc/CoreDxeLib.dsc

#
# For standalone platform payload package build there are specific library requirements
#

[LibraryClasses]
  !if $(SMM_VARIABLE) == TRUE
    PciLib|MdePkg/Library/BasePciLibPciExpress/BasePciLibPciExpress.inf
    PciExpressLib|MdePkg/Library/BasePciExpressLib/BasePciExpressLib.inf

    #
    # Optional for variable modules debug output
    #
    DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
    PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
    DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  !endif

[LibraryClasses.common.DXE_SMM_DRIVER,LibraryClasses.common.DXE_RUNTIME_DRIVER]
  !if $(SMM_VARIABLE) == TRUE
    HobLib|UefiPayloadPkg/Library/DxeHobLib/DxeHobLib.inf
    TimerLib|UefiCpuPkg/Library/CpuTimerLib/BaseCpuTimerLib.inf
  !endif

#
# This package always builds the feature.
#
!include Include/PlatformPayloadFeature.dsc

[BuildOptions]
  *_*_*_CC_FLAGS                 = -D DISABLE_NEW_DEPRECATED_INTERFACES
  GCC:*_UNIXGCC_*_CC_FLAGS       = -DMDEPKG_NDEBUG
  GCC:RELEASE_*_*_CC_FLAGS       = -DMDEPKG_NDEBUG
  INTEL:RELEASE_*_*_CC_FLAGS     = /D MDEPKG_NDEBUG
  MSFT:RELEASE_*_*_CC_FLAGS      = /D MDEPKG_NDEBUG

[BuildOptions.common.EDKII.DXE_RUNTIME_DRIVER]
  GCC:*_*_*_DLINK_FLAGS      = -z common-page-size=0x1000
  XCODE:*_*_*_DLINK_FLAGS    = -seg1addr 0x1000 -segalign 0x1000
  XCODE:*_*_*_MTOC_FLAGS     = -align 0x1000
  CLANGPDB:*_*_*_DLINK_FLAGS = /ALIGN:4096
  MSFT:*_*_*_DLINK_FLAGS     = /ALIGN:4096
