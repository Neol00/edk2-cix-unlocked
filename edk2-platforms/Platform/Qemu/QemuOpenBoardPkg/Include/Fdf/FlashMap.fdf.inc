## @file
# Flashmap and variable definitions for QemuOpenBoardPkg FVs and FD
#
# @copyright
# Copyright (C) 2022 Theo Jehl
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
##

#
# These three items are tightly coupled.
# The spare area size must be >= the first two areas.
# The total size must match the size in the EFI_FIRMWARE_VOLUME_HEADER.
# The NvStorageVariableSize must also match the VARIABLE_STORE_HEADER size.
# The EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER doesn't have size info.
#
# There isn't really a benefit to a larger spare area unless the FLASH device
# block size is larger than the size specified.
#
SET gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableSize    = 0x0003C000
SET gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingSize  = 0x00004000
SET gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareSize    = gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableSize + gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingSize

#
# Early FV
#
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvPreMemorySize           = 0x00081000
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvBspPreMemorySize        = 0x00040000
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspTSize                = 0x00010000
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspMSize                = 0x00040000
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspSSize                = 0x00020000
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvPostMemorySize          = 0x00080000
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvBspSize                 = 0x00020000

#
# Later FV
#
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvUefiBootSize            = 0x00400000
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvOsBootSize              = 0x00100000
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvSecuritySize            = 0x00080000
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvAdvancedSize            = gMinPlatformPkgTokenSpaceGuid.PcdFlashAreaSize - gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableSize - gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingSize - gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareSize - gMinPlatformPkgTokenSpaceGuid.PcdFlashFvPreMemorySize - gMinPlatformPkgTokenSpaceGuid.PcdFlashFvBspSize - gMinPlatformPkgTokenSpaceGuid.PcdFlashFvPostMemorySize - gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspSSize - gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspMSize - gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspTSize - gMinPlatformPkgTokenSpaceGuid.PcdFlashFvBspPreMemorySize - gMinPlatformPkgTokenSpaceGuid.PcdFlashFvUefiBootSize - gMinPlatformPkgTokenSpaceGuid.PcdFlashFvOsBootSize - gMinPlatformPkgTokenSpaceGuid.PcdFlashFvSecuritySize

#
# Calculate Offsets Once (Do not modify)
# This layout is specified by the EDK II Minimum Platform Archicture specification.
# Each offset is the prior region's offset plus the prior region's size.
#

SET gMinPlatformPkgTokenSpaceGuid.PcdFlashNvStorageVariableOffset   = 0x00000000
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingOffset = gMinPlatformPkgTokenSpaceGuid.PcdFlashNvStorageVariableOffset   + gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableSize
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareOffset   = gMinPlatformPkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingOffset + gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingSize

SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvAdvancedOffset          = 0x00000000
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvSecurityOffset          = gMinPlatformPkgTokenSpaceGuid.PcdFlashFvAdvancedOffset          + gMinPlatformPkgTokenSpaceGuid.PcdFlashFvAdvancedSize
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvOsBootOffset            = gMinPlatformPkgTokenSpaceGuid.PcdFlashFvSecurityOffset          + gMinPlatformPkgTokenSpaceGuid.PcdFlashFvSecuritySize
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvUefiBootOffset          = gMinPlatformPkgTokenSpaceGuid.PcdFlashFvOsBootOffset            + gMinPlatformPkgTokenSpaceGuid.PcdFlashFvOsBootSize

SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvBspOffset               = gMinPlatformPkgTokenSpaceGuid.PcdFlashFvUefiBootOffset          + gMinPlatformPkgTokenSpaceGuid.PcdFlashFvUefiBootSize
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvPostMemoryOffset        = gMinPlatformPkgTokenSpaceGuid.PcdFlashFvBspOffset               + gMinPlatformPkgTokenSpaceGuid.PcdFlashFvBspSize
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspSOffset              = gMinPlatformPkgTokenSpaceGuid.PcdFlashFvPostMemoryOffset        + gMinPlatformPkgTokenSpaceGuid.PcdFlashFvPostMemorySize
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspMOffset              = gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspSOffset              + gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspSSize
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspTOffset              = gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspMOffset              + gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspMSize
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvBspPreMemoryOffset      = gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspTOffset              + gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspTSize
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvPreMemoryOffset         = gMinPlatformPkgTokenSpaceGuid.PcdFlashFvBspPreMemoryOffset      + gMinPlatformPkgTokenSpaceGuid.PcdFlashFvBspPreMemorySize

#
# Calculate base addresses
# QemuOpenBoardPkgVars FD
#

SET gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareBase    = gMinPlatformPkgTokenSpaceGuid.PcdFlashAreaBaseAddress
SET gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingBase  = gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareBase    + gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareSize
SET gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableBase    = gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingBase  + gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingSize

SET gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableBase      = gMinPlatformPkgTokenSpaceGuid.PcdFlashAreaBaseAddress
SET gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingBase    = gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableSize  + gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableBase
SET gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareBase      = gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingBase  +  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingSize

#
# QemuOpenBoardPkg FD
#

SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvAdvancedBase            = gMinPlatformPkgTokenSpaceGuid.PcdFlashAreaBaseAddress           + gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareSize +gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareSize
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvSecurityBase            = gMinPlatformPkgTokenSpaceGuid.PcdFlashFvAdvancedBase            + gMinPlatformPkgTokenSpaceGuid.PcdFlashFvAdvancedSize
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvOsBootBase              = gMinPlatformPkgTokenSpaceGuid.PcdFlashFvSecurityBase            + gMinPlatformPkgTokenSpaceGuid.PcdFlashFvSecuritySize
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvUefiBootBase            = gMinPlatformPkgTokenSpaceGuid.PcdFlashFvOsBootBase              + gMinPlatformPkgTokenSpaceGuid.PcdFlashFvOsBootSize

SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvBspBase                 = gMinPlatformPkgTokenSpaceGuid.PcdFlashFvUefiBootBase            + gMinPlatformPkgTokenSpaceGuid.PcdFlashFvUefiBootSize
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvPostMemoryBase          = gMinPlatformPkgTokenSpaceGuid.PcdFlashFvBspBase                 + gMinPlatformPkgTokenSpaceGuid.PcdFlashFvBspSize
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspSBase                = gMinPlatformPkgTokenSpaceGuid.PcdFlashFvPostMemoryBase          + gMinPlatformPkgTokenSpaceGuid.PcdFlashFvPostMemorySize
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspMBase                = gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspSBase                + gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspSSize
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspTBase                = gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspMBase                + gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspMSize
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvBspPreMemoryBase        = gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspTBase                + gMinPlatformPkgTokenSpaceGuid.PcdFlashFvFspTSize
SET gMinPlatformPkgTokenSpaceGuid.PcdFlashFvPreMemoryBase           = gMinPlatformPkgTokenSpaceGuid.PcdFlashFvBspPreMemoryBase        + gMinPlatformPkgTokenSpaceGuid.PcdFlashFvBspPreMemorySize
