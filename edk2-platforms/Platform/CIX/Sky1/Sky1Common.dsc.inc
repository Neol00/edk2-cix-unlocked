# @file
#
#  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
#  Copyright (c) 2011 - 2020, ARM Limited. All rights reserved.
#  Copyright (c) 2017 - 2018, Andrei Warkentin <andrey.warkentin@gmail.com>
#  Copyright (c) 2015 - 2021, Intel Corporation. All rights reserved.
#  Copyright (c) 2014, Linaro Limited. All rights reserved.
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################

!include MdePkg/MdeLibs.dsc.inc
!if $(EXT4_DRIVER_SUPPORT) == TRUE
!include Features/Ext4Pkg/Ext4.dsc.inc
!endif
[LibraryClasses.common]

!if $(PRINT_TIMESTAMP_ENABLE) == TRUE
  DebugLib|Platform/CIX/Sky1/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!else
  DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!endif

  PostCodeLib|Silicon/CIX/Sky1/Library/CixBasePostCodeLibMmio/BasePostCodeLibMmio.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf

  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  SafeIntLib|MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf
  BmpSupportLib|MdeModulePkg/Library/BaseBmpSupportLib/BaseBmpSupportLib.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  ReportStatusCodeLib|MdePkg/Library/BaseReportStatusCodeLibNull/BaseReportStatusCodeLibNull.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  PeCoffLib|MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  UefiDecompressLib|MdePkg/Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  CpuLib|MdePkg/Library/BaseCpuLib/BaseCpuLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf

  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
  OrderedCollectionLib|MdePkg/Library/BaseOrderedCollectionRedBlackTreeLib/BaseOrderedCollectionRedBlackTreeLib.inf

  #
  # Ramdisk Requirements
  #
  FileExplorerLib|MdeModulePkg/Library/FileExplorerLib/FileExplorerLib.inf

  # Allow dynamic PCDs
  #
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf

  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf

  #
  # It is not possible to prevent the ARM compiler from inserting calls to intrinsic functions.
  # This library provides the instrinsic functions such a compiler may generate calls to.
  #
  NULL|ArmPkg/Library/CompilerIntrinsicsLib/CompilerIntrinsicsLib.inf

  # Add support for GCC stack protector
  NULL|MdePkg/Library/BaseStackCheckLib/BaseStackCheckLib.inf

  # ARM Architectural Libraries
  CacheMaintenanceLib|ArmPkg/Library/ArmCacheMaintenanceLib/ArmCacheMaintenanceLib.inf
  DefaultExceptionHandlerLib|Silicon/CIX/Sky1/Library/DefaultExceptionHandlerLib/DefaultExceptionHandlerLib.inf
  CpuExceptionHandlerLib|ArmPkg/Library/ArmExceptionLib/ArmExceptionLib.inf
  ArmDisassemblerLib|ArmPkg/Library/ArmDisassemblerLib/ArmDisassemblerLib.inf
  ArmGicLib|ArmPkg/Drivers/ArmGic/ArmGicLib.inf
  ArmGicArchLib|ArmPkg/Library/ArmGicArchLib/ArmGicArchLib.inf
  DmaLib|EmbeddedPkg/Library/NonCoherentDmaLib/NonCoherentDmaLib.inf
  TimeBaseLib|EmbeddedPkg/Library/TimeBaseLib/TimeBaseLib.inf
  ArmPlatformStackLib|ArmPlatformPkg/Library/ArmPlatformStackLib/ArmPlatformStackLib.inf
  ArmSmcLib|ArmPkg/Library/ArmSmcLib/ArmSmcLib.inf
  ArmHvcLib|ArmPkg/Library/ArmHvcLib/ArmHvcLib.inf
  ArmGenericTimerCounterLib|ArmPkg/Library/ArmGenericTimerPhyCounterLib/ArmGenericTimerPhyCounterLib.inf

  UncachedMemoryAllocationLib|Silicon/CIX/Sky1/Library/UncachedMemoryAllocationLib/UncachedMemoryAllocationLib.inf

  # ARM PL011 UART Driver
  PL011UartClockLib|Silicon/CIX/Sky1/Library/PL011UartClockLib/PL011UartClockLib.inf
  PL011UartLib|ArmPlatformPkg/Library/PL011UartLib/PL011UartLib.inf
  SerialPortLib|Silicon/CIX/Sky1/Library/PL011SerialPortLib/PL011SerialPortLib.inf
  TimerLib|Silicon/CIX/Sky1/Library/ArmArchTimerLib/ArmArchTimerLib.inf

  #
  # Uncomment (and comment out the next line) For RealView Debugger. The Standard IO window
  # in the debugger will show load and unload commands for symbols. You can cut and paste this
  # into the command window to load symbols. We should be able to use a script to do this, but
  # the version of RVD I have does not support scripts accessing system memory.
  #
  #PeCoffExtraActionLib|ArmPkg/Library/RvdPeCoffExtraActionLib/RvdPeCoffExtraActionLib.inf
  PeCoffExtraActionLib|ArmPkg/Library/DebugPeCoffExtraActionLib/DebugPeCoffExtraActionLib.inf
  #PeCoffExtraActionLib|MdePkg/Library/BasePeCoffExtraActionLibNull/BasePeCoffExtraActionLibNull.inf

  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
  DebugAgentTimerLib|EmbeddedPkg/Library/DebugAgentTimerLibNull/DebugAgentTimerLibNull.inf

  # Flattened Device Tree (FDT) access library
  FdtLib|EmbeddedPkg/Library/FdtLib/FdtLib.inf

  PartitionLib|Platform/CIX/Sky1/Library/PartitionLib/PartitionLib.inf
  FlashUpdateLib|Platform/CIX/Sky1/Library/FlashUpdateLib/FlashUpdateLib.inf
  AvbLib|Platform/CIX/Sky1/Library/avb/AvbLib.inf
  BootLib|Platform/CIX/Sky1/Library/BootLib/BootLib.inf
  LaunchEfiLib|Platform/CIX/Sky1/Library/LaunchEfiLib/LaunchEfiLib.inf
  MiniFastbootLib|Platform/CIX/Sky1/Library/FastbootLib/MiniFastbootLib.inf
  OpteeLib|Platform/CIX/Sky1/Library/OpteeLib/OpteeLib.inf

  #
  # Secure Boot dependencies
  #
!if $(SECURE_BOOT_ENABLE) == TRUE
  TpmMeasurementLib|SecurityPkg/Library/DxeTpmMeasurementLib/DxeTpmMeasurementLib.inf
  AuthVariableLib|SecurityPkg/Library/AuthVariableLib/AuthVariableLib.inf
  SecureBootVariableLib|SecurityPkg/Library/SecureBootVariableLib/SecureBootVariableLib.inf
  SecureBootVariableProvisionLib|SecurityPkg/Library/SecureBootVariableProvisionLib/SecureBootVariableProvisionLib.inf

  # re-use the UserPhysicalPresent() dummy implementation from the ovmf tree
  PlatformSecureLib|OvmfPkg/Library/PlatformSecureLib/PlatformSecureLib.inf

  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
  IntrinsicLib|CryptoPkg/Library/IntrinsicLib/IntrinsicLib.inf
  OpensslLib|CryptoPkg/Library/OpensslLib/OpensslLib.inf
  TlsLib|CryptoPkg/Library/TlsLib/TlsLib.inf
  PlatformPKProtectionLib|SecurityPkg/Library/PlatformPKProtectionLibVarPolicy/PlatformPKProtectionLibVarPolicy.inf
!else
  TpmMeasurementLib|MdeModulePkg/Library/TpmMeasurementLibNull/TpmMeasurementLibNull.inf
  AuthVariableLib|MdeModulePkg/Library/AuthVariableLibNull/AuthVariableLibNull.inf
!endif
  # RngLib|MdePkg/Library/DxeRngLib/DxeRngLib.inf
  RngLib|MdePkg/Library/BaseRngLibTimerLib/BaseRngLibTimerLib.inf
  VarCheckLib|MdeModulePkg/Library/VarCheckLib/VarCheckLib.inf
  VariableFlashInfoLib|MdeModulePkg/Library/BaseVariableFlashInfoLib/BaseVariableFlashInfoLib.inf
  VariablePolicyLib|MdeModulePkg/Library/VariablePolicyLib/VariablePolicyLib.inf
  VariablePolicyHelperLib|MdeModulePkg/Library/VariablePolicyHelperLib/VariablePolicyHelperLib.inf

  CustomizedDisplayLib|MdeModulePkg/Library/CustomizedDisplayLib/CustomizedDisplayLib.inf
  ConfigParamsDataBlockLib|Silicon/CIX/Sky1/Library/ConfigParamsDataBlockLib/ConfigParamsDataBlockLib.inf
  PlatformConfigParamsDataBlockLib|Platform/CIX/Sky1/Library/PlatformConfigParamsDataBlockLib/PlatformConfigParamsDataBlockLib.inf
  PinMuxLib|Silicon/CIX/Sky1/Library/PinMuxLib/PinMuxLib.inf
  PinMuxTableLib|Silicon/CIX/Sky1/Library/PinMuxTableNullLib/PinMuxTableNullLib.inf
  GpioLib|Silicon/CIX/Sky1/Library/GpioLib/GpioLib.inf
  GpioTableLib|Silicon/CIX/Sky1/Library/GpioTableNullLib/GpioTableNullLib.inf
  FchDmaLib|Silicon/CIX/Sky1/Library/FchDmaLib/FchDmaLib.inf
  FchTimerLib|Silicon/CIX/Sky1/Library/FchTimerLib/FchTimerLib.inf
!if $(SOC_HDA_ENABLE) == TRUE
  # HdaVerbTableLib|Silicon/CIX/Sky1/Library/HdaVerbTableNullLib/HdaVerbTableNullLib.inf
  HdaVerbTableLib|Platform/CIX/Sky1/Library/HdaVerbTableLib/HdaVerbTableLib.inf
!endif
!if $(SOC_SPI_ENABLE) == TRUE
  SpiLib|Silicon/CIX/Sky1/Library/SpiLib/SpiLib.inf
!endif
!if $(SOC_I2C_ENABLE) == TRUE
  I2cLib|Silicon/CIX/Sky1/Library/I2cLib/I2cLib.inf
  !if $(I2C_EC_ENABLE) == TRUE
  EcLib|Platform/CIX/Sky1/Library/Ite5570EcLib/Ite5570EcLib.inf
  !else
  EcLib|Platform/CIX/Library/EcLibNull/EcLibNull.inf
  !endif
!endif
  ConfigParamsHookLib|Silicon/CIX/Sky1/Library/ConfigParamsHookNullLib/ConfigParamsHookNullLib.inf
  PlatformConfigParamsHookLib|Platform/CIX/Sky1/Library/PlatformConfigParamsHookNullLib/PlatformConfigParamsHookNullLib.inf
  PlatformEnvHookLib|Platform/CIX/Sky1/Library/PlatformEnvHookNullLib/PlatformEnvHookNullLib.inf
  PlatformBootHookLib|Platform/CIX/Sky1/Library/PlatformBootHookNullLib/PlatformBootHookNullLib.inf
!if $(STMM_SUPPORT) == TRUE
  NorFlashLib|Silicon/CIX/Sky1/Library/NorFlashLib/NorFlashStmmRuntimeLib.inf
!else
  NorFlashLib|Silicon/CIX/Sky1/Library/NorFlashLib/NorFlashLib.inf
!endif
  PciHostBridgeLib|Silicon/CIX/Sky1/Library/PciHostBridgeLib/PciHostBridgeLib.inf
  PciSegmentLib|Silicon/CIX/Sky1/Library/PciSegmentLib/PciSegmentLib.inf
  PlatformPcieLib|Silicon/CIX/Sky1/Library/PlatformPcieLib/PlatformPcieLib.inf
  NetLib|NetworkPkg/Library/DxeNetLib/DxeNetLib.inf

!if $(SOC_GOP_ENABLE) == TRUE
  DpuDxe|Silicon/CIX/Sky1/Drivers/DpuDxe/DpuDxe.inf
  DisplayLib|Silicon/CIX/Sky1/Library/DisplayLib/DisplayLib.inf
!endif

  ArmMtlLib|Silicon/CIX/Sky1/Library/CixMtlLib/CixMtlLib.inf
  DtbUpdateLib|Platform/CIX/Sky1/Library/DtbUpdateLib/DtbUpdateLib.inf

  ArmLib|ArmPkg/Library/ArmLib/ArmBaseLib.inf
  ArmMmuLib|ArmPkg/Library/ArmMmuLib/ArmMmuBaseLib.inf
  ArmPlatformLib|Platform/CIX/Sky1/Library/PlatformLib/PlatformLib.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
  UefiBootManagerLib|MdeModulePkg/Library/UefiBootManagerLib/UefiBootManagerLib.inf
  BootLogoLib|MdeModulePkg/Library/BootLogoLib/BootLogoLib.inf
!if $(COMPILE_SYSTEM_LOADER) == android
  PlatformBootManagerLib|Platform/CIX/Sky1/Library/PlatformBootManagerLib/PlatformBootManagerLib.inf
!else
  PlatformBootManagerLib|Platform/CIX/Sky1/Library/CixPlatformBootManagerLib/PlatformBootManagerLib.inf
!endif
  FileExplorerLib|MdeModulePkg/Library/FileExplorerLib/FileExplorerLib.inf
  AcpiLib|EmbeddedPkg/Library/AcpiLib/AcpiLib.inf
  SocInitLib|Silicon/CIX/Sky1/Library/SocInitLib/SocInitLib.inf
  FirmwareParseLib|Silicon/CIX/Sky1/Library/FirmwareParseLib/FirmwareParseLib.inf
  HwMutexLib|Silicon/CIX/Sky1/Library/HwMutexLib/HwMutexLib.inf
!if $(CAPSULE_ENABLE)
  OpensslLib|CryptoPkg/Library/OpensslLib/OpensslLib.inf
  IntrinsicLib|CryptoPkg/Library/IntrinsicLib/IntrinsicLib.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
!if $(CAPSULE_ENABLE)
  #CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibFmp/DxeCapsuleLib.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
!else
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
!endif
  EdkiiSystemCapsuleLib|Platform/CIX/Sky1/Library/EdkiiSystemCapsuleLib/EdkiiSystemCapsuleLib.inf
  FmpAuthenticationLib|SecurityPkg/Library/FmpAuthenticationLibPkcs7/FmpAuthenticationLibPkcs7.inf
  IniParsingLib|SignedCapsulePkg/Library/IniParsingLib/IniParsingLib.inf
  PlatformFlashAccessLib|SignedCapsulePkg/Library/PlatformFlashAccessLibNull/PlatformFlashAccessLibNull.inf
#  PlatformFlashAccessLib|Platform/RaspberryPi/Library/PlatformFlashAccessLib/PlatformFlashAccessLib.inf
  DisplayUpdateProgressLib|MdeModulePkg/Library/DisplayUpdateProgressLibGraphics/DisplayUpdateProgressLibGraphics.inf
!endif
!if $(REBOOT_REASON_ENABLE)
  #RebootReasonLib|Silicon/CIX/Sky1/Library/RebootReasonLib/RebootReasonLib.inf
!else
  #RebootReasonLib|Silicon/CIX/Sky1/Library/RebootReasonLib/RebootReasonLibNull.inf
!endif
  CsuPmIpcLib|Silicon/CIX/Sky1/Library/CsuPmIpcLib/CsuPmIpcLib.inf

  ShMemLib|Silicon/CIX/Sky1/Library/ShMemLib/ShMemLib.inf
  MailBoxLib|Silicon/CIX/Sky1/Library/MailBoxLib/MailBoxLib.inf

  RealTimeClockLib|EmbeddedPkg/Library/VirtualRealTimeClockLib/VirtualRealTimeClockLib.inf
!if $(ACPI_ENABLE) == TRUE
  AcpiHelperLib|DynamicTablesPkg/Library/Common/AcpiHelperLib/AcpiHelperLib.inf
  AmlLib|Platform/CIX/Sky1/Library/Acpi/CIX/AmlLib/AmlLib.inf
  TableHelperLib|DynamicTablesPkg/Library/Common/TableHelperLib/TableHelperLib.inf
!endif
  CixSipLib|Silicon/CIX/Sky1/Library/CixSipLib/CixSipLib.inf

  UsbLib|Silicon/CIX/Sky1/Library/UsbLib/UsbLib.inf
  CpuInfoLib|Silicon/CIX/Sky1/Library/CpuInfoLib/CpuInfoLib.inf
  HwHarvestLib|Silicon/CIX/Sky1/Library/HwHarvestLib/HwHarvestLib.inf
  #CommonLib|Platform/CIX/Sky1/Library/CommonLib/CommonLib.inf
  CixS3IoLib|Platform/CIX/Sky1/Library/CixS3IoLib/CixS3IoLib.inf
!if $(SMBIOS_ENABLE) == TRUE
  OemMiscLib|Platform/CIX/Sky1/Library/SmbiosMiscLib/SmbiosMiscLib.inf
!endif
!if $(FUNC_BOOT_PERF_ENABLE) == TRUE
  CixFwBootPerfLib|Platform/CIX/Sky1/Library/CixFwBootPerfLib/CixFwBootPerfLib.inf
!else
  CixFwBootPerfLib|Platform/CIX/Sky1/Library/CixFwBootPerfLib/CixFwBootPerfLibNull.inf
!endif
  EfiResetSystemLib|ArmPkg/Library/ArmPsciResetSystemLib/ArmPsciResetSystemLib.inf

[LibraryClasses.common.SEC]
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  MemoryInitPeiLib|Platform/CIX/Sky1/Library/MemoryInitPeiLib/MemoryInitPeiLib.inf
  PlatformPeiLib|ArmPlatformPkg/PlatformPei/PlatformPeiLib.inf
  ExtractGuidedSectionLib|EmbeddedPkg/Library/PrePiExtractGuidedSectionLib/PrePiExtractGuidedSectionLib.inf
  LzmaDecompressLib|MdeModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
  PrePiLib|EmbeddedPkg/Library/PrePiLib/PrePiLib.inf
  HobLib|EmbeddedPkg/Library/PrePiHobLib/PrePiHobLib.inf
  PrePiHobListPointerLib|ArmPlatformPkg/Library/PrePiHobListPointerLib/PrePiHobListPointerLib.inf
  MemoryAllocationLib|EmbeddedPkg/Library/PrePiMemoryAllocationLib/PrePiMemoryAllocationLib.inf

[LibraryClasses.common.DXE_CORE]
  HobLib|MdePkg/Library/DxeCoreHobLib/DxeCoreHobLib.inf
  MemoryAllocationLib|MdeModulePkg/Library/DxeCoreMemoryAllocationLib/DxeCoreMemoryAllocationLib.inf
  DxeCoreEntryPoint|MdePkg/Library/DxeCoreEntryPoint/DxeCoreEntryPoint.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  PerformanceLib|MdeModulePkg/Library/DxeCorePerformanceLib/DxeCorePerformanceLib.inf

[LibraryClasses.common.DXE_DRIVER]
  SecurityManagementLib|MdeModulePkg/Library/DxeSecurityManagementLib/DxeSecurityManagementLib.inf
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
!if $(INCLUDE_TFTP_COMMAND) == TRUE
  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
!endif
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  NonDiscoverableDeviceRegistrationLib|MdeModulePkg/Library/NonDiscoverableDeviceRegistrationLib/NonDiscoverableDeviceRegistrationLib.inf

[LibraryClasses.common.UEFI_APPLICATION]
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
!if $(CAPSULE_ENABLE)
  AblAuthenticateLib|Platform/CIX/Sky1/Library/AblAuthenticateLib/AblAuthenticateLib.inf
!endif

[LibraryClasses.common.UEFI_DRIVER]
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  UefiScsiLib|MdePkg/Library/UefiScsiLib/UefiScsiLib.inf
  BaseHidParseReportLib|Platform/CIX/Sky1/Library/BaseHidParseReportLib/BaseHidParseReportLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
!if $(STMM_SUPPORT) == TRUE && $(RUNTIME_DEBUG_MSG_ENABLE) == TRUE
  DebugLib|Platform/CIX/Sky1/Library/DxeRuntimeDebugLibSerialPort/DxeRuntimeDebugLibSerialPort.inf
  SerialOutStmmLib|Silicon/CIX/Sky1/Library/SerialOutStmmLib/SerialOutStmmRuntimeLib.inf
!else
  # Runtime debug messages may crash an OS unless serial output to MMIO mapped UARTs is inhibited
  DebugLib|MdePkg/Library/DxeRuntimeDebugLibSerialPort/DxeRuntimeDebugLibSerialPort.inf
!endif
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
  EfiResetSystemLib|ArmPkg/Library/ArmPsciResetSystemLib/ArmPsciResetSystemLib.inf
  ArmSmcLib|ArmPkg/Library/ArmSmcLib/ArmSmcLib.inf
  PinMuxLib|Silicon/CIX/Sky1/Library/PinMuxLib/PinMuxRuntimeLib.inf
  GpioLib|Silicon/CIX/Sky1/Library/GpioLib/GpioRuntimeLib.inf
  VariablePolicyLib|MdeModulePkg/Library/VariablePolicyLib/VariablePolicyLibRuntimeDxe.inf

!if $(SECURE_BOOT_ENABLE) == TRUE
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/RuntimeCryptLib.inf
!endif
!if $(CAPSULE_ENABLE)
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibFmp/DxeRuntimeCapsuleLib.inf
!endif

###################################################################################################
# BuildOptions Section - Define the module specific tool chain flags that should be used as
#                        the default flags for a module. These flags are appended to any
#                        standard flags that are defined by the build process.
###################################################################################################

[BuildOptions]
  GCC:*_*_*_CC_FLAGS          = -fno-common
[BuildOptions.common.EDKII.DXE_RUNTIME_DRIVER]
  GCC:*_*_AARCH64_DLINK_FLAGS = -z common-page-size=0x10000

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdsFeatureFlag.common]

  gEmbeddedTokenSpaceGuid.PcdPrePiProduceMemoryTypeInformationHob|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdTurnOffUsbLegacySupport|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutGopSupport|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdInstallAcpiSdtProtocol|TRUE
!if $(VARIABLE_SUPPORT) == "SPI" && $(STMM_SUPPORT) == TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdEnableVariableRuntimeCache|FALSE
!else
  gEfiMdeModulePkgTokenSpaceGuid.PcdVariableCollectStatistics|TRUE
!endif

[PcdsDynamicExDefault.common.DEFAULT]
!if $(CAPSULE_ENABLE)                                    #351c9808-4b53-4363-9314-d339386e5685
  gEfiSignedCapsulePkgTokenSpaceGuid.PcdEdkiiSystemFirmwareImageDescriptor|{0x0}|VOID*|0x100
  gEfiMdeModulePkgTokenSpaceGuid.PcdSystemFmpCapsuleImageTypeIdGuid|{0x08, 0x98, 0x1c, 0x35, 0x53, 0x4b, 0x63, 0x43, 0x93, 0x14, 0xd3, 0x39, 0x38, 0x6e, 0x56, 0x85}
  gEfiSignedCapsulePkgTokenSpaceGuid.PcdEdkiiSystemFirmwareFileGuid|{0xcf, 0x4f, 0x2e, 0x64, 0xf7, 0x2d, 0x15, 0x44, 0x8b, 0x70, 0xa0, 0x39, 0x09, 0xc5, 0x7b, 0x55}
!endif
[PcdsFixedAtBuild.common]
  gEfiMdePkgTokenSpaceGuid.PcdMaximumUnicodeStringLength|1000000
  gEfiMdePkgTokenSpaceGuid.PcdMaximumAsciiStringLength|1000000
  gEfiMdePkgTokenSpaceGuid.PcdMaximumLinkedListLength|1000000
  gEfiMdePkgTokenSpaceGuid.PcdSpinLockTimeout|10000000
  gEfiMdePkgTokenSpaceGuid.PcdDebugClearMemoryValue|0xAF
  gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|0
  gEfiMdePkgTokenSpaceGuid.PcdPostCodePropertyMask|0x08
  gEfiMdePkgTokenSpaceGuid.PcdUefiLibMaxPrintBufferSize|320
  # DEBUG_ASSERT_ENABLED       0x01
  # DEBUG_PRINT_ENABLED        0x02
  # DEBUG_CODE_ENABLED         0x04
  # CLEAR_MEMORY_ENABLED       0x08
  # ASSERT_BREAKPOINT_ENABLED  0x10
  # ASSERT_DEADLOOP_ENABLED    0x20
!if $(TARGET) == RELEASE
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x21
!else
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2f
!endif
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|$(DEBUG_PRINT_ERROR_LEVEL)

  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x07

  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiACPIReclaimMemory|0
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiACPIMemoryNVS|0
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiReservedMemoryType|0
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiRuntimeServicesData|0x450
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiRuntimeServicesCode|1200
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiBootServicesCode|1500
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiBootServicesData|13000
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiLoaderCode|0x1000
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiLoaderData|0
  gEmbeddedTokenSpaceGuid.PcdDmaDeviceOffset|0x00000000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVersionString|L"cix_sky1_alpha_0.6"
  gEfiMdePkgTokenSpaceGuid.PcdUefiVariableDefaultPlatformLangCodes|"en-US"

[PcdsFixedAtBuild.common]
  gArmPlatformTokenSpaceGuid.PcdClusterCount|1
!if $(COMPILE_SMP_ENABLE) == 1
  gArmPlatformTokenSpaceGuid.PcdCoreCount|12
!else
  gArmPlatformTokenSpaceGuid.PcdCoreCount|1
!endif

!if $(COMPILE_BOARD_NAME) == fpga
  gArmTokenSpaceGuid.PcdArmPrimaryCore|0x000  #core0
!else
  # The Primary Core is 0.0.4.0
  gArmTokenSpaceGuid.PcdArmPrimaryCore|0x400  #core4
!endif

  gArmPlatformTokenSpaceGuid.PcdCPUCorePrimaryStackSize|0x40000
  gArmTokenSpaceGuid.PcdSystemProductName|L"Sky1"
  # Runtime Variable storage
  gEfiMdeModulePkgTokenSpaceGuid.PcdEmuVariableNvStoreReserved|0
!if $(VARIABLE_SUPPORT) == "SPI"
  gEfiMdeModulePkgTokenSpaceGuid.PcdEmuVariableNvModeEnable|FALSE
!elseif $(VARIABLE_SUPPORT) == "EMU"
  gEfiMdeModulePkgTokenSpaceGuid.PcdEmuVariableNvModeEnable|TRUE
!endif
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x2000
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxAuthVariableSize|0x2800

  # Size of the region used by UEFI in permanent memory (Reserved 64MB)
  gArmPlatformTokenSpaceGuid.PcdSystemMemoryUefiRegionSize|0x08000000
  #
  # 0x82500000 - 0x01F00000  Reserved share memory
  # 0x84400000 - 0x00400000  FD (PcdFdBaseAddress, PcdFdSize)
  # 0x84800000 - 0x00800000  GOP Frame buffer
  # 0x85000000               normal memory
  # 0xA0000000 - 0x08000000  load boot.img (Grub+ OS Image + DTB + rootfs images) to 4G SRAM offset 0x2000_0000, which is 0xA000_0000 for AP CPU
  #
  #
  # This matches PcdFvBaseAddress, since everything less is the FD, and
  # will be reserved away.
  #

  gArmTokenSpaceGuid.PcdSystemMemoryBase|0x80000000
  !if $(COMPILE_SYSTEM_LOADER) == android
  gArmTokenSpaceGuid.PcdSystemMemorySize|0x200000000
  !else
  gArmTokenSpaceGuid.PcdSystemMemorySize|0x100000000
  !endif
  gCixPlatformTokenSpaceGuid.PcdKernelBootImgBase|0xA0000000
  gCixPlatformTokenSpaceGuid.PcdKernelBootImgSize|0x08000000
  gCixPlatformTokenSpaceGuid.PcdReservedSecureMemoryBase|0x80000000
  gCixPlatformTokenSpaceGuid.PcdReservedSecureMemorySize|0x02500000
  gCixPlatformTokenSpaceGuid.PcdReservedShareMemoryBase|0x82500000
  gCixPlatformTokenSpaceGuid.PcdReservedShareMemorySize|0x01F00000
  gArmPlatformTokenSpaceGuid.PcdArmLcdDdrFrameBufferBase|0x84800000
  gArmPlatformTokenSpaceGuid.PcdArmLcdDdrFrameBufferSize|0x00800000
  gCixPlatformTokenSpaceGuid.PcdReservedLinuxBase|0x86000000
  gCixPlatformTokenSpaceGuid.PcdReservedLinuxSize|0x06200000
  gCixPlatformTokenSpaceGuid.PcdReservedAndroidBase|0x140000000
  gCixPlatformTokenSpaceGuid.PcdReservedAndroidSize|0x120000000
  #
  # ARM General Interrupt Controller
  #
  gArmTokenSpaceGuid.PcdGicDistributorBase|0x0e010000    #0x0e001000
  gArmTokenSpaceGuid.PcdGicRedistributorsBase|0x0e090000 #0x0e081000
  ## SBSA Watchdog Count
  gArmPlatformTokenSpaceGuid.PcdWatchdogCount|1
  gArmTokenSpaceGuid.PcdGenericWatchdogControlBase|0x16003000
  gArmTokenSpaceGuid.PcdGenericWatchdogRefreshBase|0x16008000
  gArmTokenSpaceGuid.PcdGenericWatchdogEl2IntrNum|408
  # PL011 - Serial Terminal

  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x040D0000 #0x091e1000 #0x0a051000

  ## PL011 - EMS UART Terminal
  gArmPlatformTokenSpaceGuid.PcdSerialDbgRegisterBase|0x040e0000
  gArmPlatformTokenSpaceGuid.PcdSerialDbgInterrupt|331
  gArmPlatformTokenSpaceGuid.PcdSerialDbgUartBaudRate|115200
!if $(COMPILE_BOARD_NAME) == emu
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultBaudRate|921600
  gArmPlatformTokenSpaceGuid.PL011UartClkInHz|25000000
  #
  # ARM Architectural Timer Frequency
  #
  gArmTokenSpaceGuid.PcdArmArchTimerFreqInHz|1000000
!elseif $(COMPILE_BOARD_NAME) == fpga
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultBaudRate|115200
  gArmPlatformTokenSpaceGuid.PL011UartClkInHz|5000000
  #
  # ARM Architectural Timer Frequency
  #
  gArmTokenSpaceGuid.PcdArmArchTimerFreqInHz|100000000
!else #post-silion setting for evb crb cloudbook batura
  # uart
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultBaudRate|115200 #115200 921600
  gArmPlatformTokenSpaceGuid.PL011UartClkInHz|100000000 #100Mhz 100000000 #25Mhz 25000000
  # timer
  gArmTokenSpaceGuid.PcdArmArchTimerFreqInHz|1000000000 #1Ghz 1000000000 #1Mhz 1000000
!endif
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultReceiveFifoDepth|0
  gArmPlatformTokenSpaceGuid.PL011UartInterrupt|330

  # We enlarge  PcdDiskIoDataBufferBlockNum to 1024 to align with write buffer size in Fastboot
  gEfiMdeModulePkgTokenSpaceGuid.PcdDiskIoDataBufferBlockNum|1024

  #
  # ACPI
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiExposedTableVersions|0x20
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiACPIReclaimMemory|0
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiACPIMemoryNVS|0
  # SMBIOS entry point version
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdSmbiosVersion|0x0300
  gEfiMdeModulePkgTokenSpaceGuid.PcdSmbiosDocRev|0x0

  gArmTokenSpaceGuid.PcdProcessorManufacturer|L"Cix Technology Group Co., Ltd."
  gArmTokenSpaceGuid.PcdProcessorVersion|L"CIX P1 CD8180"
  gArmTokenSpaceGuid.PcdProcessorSerialNumber|L""
  gArmTokenSpaceGuid.PcdProcessorAssetTag|L" "
  gArmTokenSpaceGuid.PcdProcessorPartNumber|L" "
  #
  #
  #
  # Fixed CPU settings.
  #

  ## Default Terminal Type
  ## 0-PCANSI, 1-VT100, 2-VT00+, 3-UTF8, 4-TTYTERM
  gEfiMdePkgTokenSpaceGuid.PcdDefaultTerminalType|4

  gEfiMdeModulePkgTokenSpaceGuid.PcdResetOnMemoryTypeInformationChange|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdBootManagerMenuFile|{ 0x21, 0xaa, 0x2c, 0x46, 0x14, 0x76, 0x03, 0x45, 0x83, 0x6e, 0x8a, 0xb6, 0xf4, 0x66, 0x23, 0x31 }

  gCixTokenSpaceGuid.PcdNorFlashNvramAddr|$(SPI_VARIABLE_BASE)
  gCixTokenSpaceGuid.PcdNorFlashNvramSize|$(SPI_VARIABLE_SIZE)

!if $(VARIABLE_SUPPORT) == "SPI" && $(STMM_SUPPORT) == FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableSize|$(SPI_VARIABLE_SIZE)
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingSize|$(SPI_VARIABLE_SIZE)
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareSize|$(SPI_VARIABLE_SIZE)
!endif

!if $(SOC_GPU_ENABLE) == TRUE
  gCixTokenSpaceGuid.PcdGpuEnable|TRUE
!endif

  gEmbeddedTokenSpaceGuid.PcdPrePiCpuIoSize|44

!if $(COMPILE_ACPI_ENABLE) == 1
  gCixTokenSpaceGuid.PcdSystemTableSelect|0x00
!endif

[PcdsPatchableInModule]
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialClockRate|500000000

[PcdsDynamicHii.common.DEFAULT]
  #
  # Reset-related.
  #


  #
  # Common UEFI ones.
  #
!if $(NO_GIC_NO_TIMER) == TRUE
  gEfiMdePkgTokenSpaceGuid.PcdPlatformBootTimeOut|L"Timeout"|gEfiGlobalVariableGuid|0x0|0
!else
  gEfiMdePkgTokenSpaceGuid.PcdPlatformBootTimeOut|L"Timeout"|gEfiGlobalVariableGuid|0x0|5
!endif
  gEfiMdeModulePkgTokenSpaceGuid.PcdBootDiscoveryPolicy|L"BootDiscoveryPolicy"|gBootDiscoveryPolicyMgrFormsetGuid|0

[PcdsDynamicDefault.common]
  #
  # Set video resolution for boot options and for text setup.
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoHorizontalResolution|1920
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoVerticalResolution|1080
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoHorizontalResolution|640
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoVerticalResolution|480

!if $(VARIABLE_SUPPORT) == "SPI"
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableBase64|$(SPI_VARIABLE_BASE)
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingBase64|($(SPI_VARIABLE_BASE) + $(SPI_VARIABLE_SIZE))
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareBase64|($(SPI_VARIABLE_BASE) + $(SPI_VARIABLE_SIZE) + $(SPI_VARIABLE_SIZE))
!endif
!if $(COMPILE_SYSTEM_LOADER) == android
  gEmbeddedTokenSpaceGuid.PcdDmaDeviceLimit|0x27fffffff
!else
  gEmbeddedTokenSpaceGuid.PcdDmaDeviceLimit|0x17fffffff
!endif
################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform
#
################################################################################
[Components.common]
  #
  # PEI Phase modules
  #
!if $(DIAGS_KERNEL_ENABLE) == TRUE
  Edk2OverridePkg/ArmPlatformPkg/PrePi/PeiMPCore.inf
!else
  ArmPlatformPkg/PrePi/PeiUniCore.inf{
    <LibraryClasses>
      #DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  }
!endif

  #
  # DXE
  #
!if $(DIAGS_KERNEL_ENABLE) == TRUE
  Edk2OverridePkg/MdeModulePkg/Core/Dxe/DxeMain.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/DxeCrc32GuidedSectionExtractLib/DxeCrc32GuidedSectionExtractLib.inf
#      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  }
!else
  MdeModulePkg/Core/Dxe/DxeMain.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/DxeCrc32GuidedSectionExtractLib/DxeCrc32GuidedSectionExtractLib.inf
      #DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  }
!endif
  MdeModulePkg/Universal/PCD/Dxe/Pcd.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  }

  #
  # Architectural Protocols
  #
  ArmPkg/Drivers/CpuDxe/CpuDxe.inf
  MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf
  Platform/CIX/Sky1/Drivers/PlatformConfigParamsManageDxe/PlatformConfigParamsManageDxe.inf
  Silicon/CIX/Sky1/Drivers/ConfigParamsManageDxe/ConfigParamsManageDxe.inf
!if $(TOKEN_SETUP_SUPPORT) == TRUE
  Platform/CIX/Sky1/Drivers/PlatformSetupVariableInitDxe/PlatformSetupVariableInitDxe.inf
!endif
  Silicon/CIX/Sky1/Drivers/SocDxe/SocDxe.inf
  Platform/CIX/Sky1/Drivers/PlatformDxe/PlatformDxe.inf
  # Platform/CIX/Sky1/Drivers/SampleCodeDxe/SampleCodeDxe.inf
!if $(STMM_SUPPORT) == TRUE
  Platform/CIX/Sky1/Drivers/MmCommunicationDxe/ArmMmCommunication.inf {
    <LibraryClasses>
      NULL|StandaloneMmPkg/Library/VariableMmDependency/VariableMmDependency.inf
  }
!endif
!if $(STMM_SUPPORT) == TRUE
  Silicon/CIX/Sky1/Drivers/NorFlashDxe/NorFlashSmmRuntimeDxe.inf
!else
  Silicon/CIX/Sky1/Drivers/NorFlashDxe/NorFlashDxe.inf
!endif
!if $(VARIABLE_SUPPORT) == "SPI" && $(STMM_SUPPORT) == FALSE
  MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteDxe.inf
!endif
!if $(VARIABLE_SUPPORT) == "SPI" && $(STMM_SUPPORT) == TRUE
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableSmmRuntimeDxe.inf {
    <LibraryClasses>
      MmUnblockMemoryLib|MdePkg/Library/MmUnblockMemoryLib/MmUnblockMemoryLibNull.inf
  }
!else
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/VarCheckUefiLib/VarCheckUefiLib.inf
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  }
!endif

!if $(PCIE_HOST_ENABLE) == TRUE
  Silicon/CIX/Sky1/Drivers/PcieInitDxe/PcieInitDxe.inf
!endif
!if $(SECURE_BOOT_ENABLE) == TRUE
  MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf {
    <LibraryClasses>
      NULL|SecurityPkg/Library/DxeImageVerificationLib/DxeImageVerificationLib.inf
  }
  SecurityPkg/VariableAuthenticated/SecureBootConfigDxe/SecureBootConfigDxe.inf
  SecurityPkg/EnrollFromDefaultKeysApp/EnrollFromDefaultKeysApp.inf
  Platform/CIX/Sky1/Drivers/SecureBootDxe/SecureBootDxe.inf
  SecurityPkg/VariableAuthenticated/SecureBootDefaultKeysDxe/SecureBootDefaultKeysDxe.inf
!else
  MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf
!endif
  MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
  MdeModulePkg/Universal/MonotonicCounterRuntimeDxe/MonotonicCounterRuntimeDxe.inf
  EmbeddedPkg/ResetRuntimeDxe/ResetRuntimeDxe.inf
  EmbeddedPkg/RealTimeClockRuntimeDxe/RealTimeClockRuntimeDxe.inf
  EmbeddedPkg/MetronomeDxe/MetronomeDxe.inf

!if $(SLT_ENABLE) == TRUE
  EmbeddedPkg/SimpleTextInOutSerial/SimpleTextInOutSerial.inf
!else
  MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  MdeModulePkg/Universal/Console/TerminalDxe/TerminalDxe.inf
  MdeModulePkg/Universal/SerialDxe/SerialDxe.inf
!if $(TOKEN_CONSOLE_PREF_SUPPORT) == TRUE
  EmbeddedPkg/Drivers/ConsolePrefDxe/ConsolePrefDxe.inf
!endif
!endif
  MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf

!if $(DIAGS_KERNEL_ENABLE) == TRUE
  Edk2OverridePkg/ArmPkg/Drivers/ArmGic/ArmGicDxe.inf
!else
  ArmPkg/Drivers/ArmGic/ArmGicDxe.inf
!endif

  ArmPkg/Drivers/TimerDxe/TimerDxe.inf
  #MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf
  Silicon/CIX/Sky1/Drivers/GenericWatchdogDxe/GenericWatchdogDxe.inf
!if $(GLOBAL_WATCHDOG_ENABLE) == TRUE
  Silicon/CIX/Sky1/Drivers/GenericWatchdogDxe/GlobalWatchdogDxe.inf
!endif
  MdeModulePkg/Universal/EbcDxe/EbcDxe.inf
  #
  # FAT filesystem + GPT/MBR partitioning
  #
  MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf
  MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf
  FatPkg/EnhancedFatDxe/Fat.inf
!if $(DTB_UPDATE_ENABLE) == TRUE
  Platform/CIX/Sky1/Drivers/DtbUpdateDxe/DtbUpdateDxe.inf
!endif
!if $(RLOG_DUMP_ENABLE) == TRUE
  Platform/CIX/Sky1/Drivers/RamLogDumpDxe/RamLogDumpDxe.inf
!endif
  # FV Filesystem
!if $(FV_SIMPLE_FILE_SUPPORT) == TRUE
  MdeModulePkg/Universal/FvSimpleFileSystemDxe/FvSimpleFileSystemDxe.inf
!endif
  #
  # ACPI Support
  #
!if $(ACPI_ENABLE) == TRUE
  MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf
  MdeModulePkg/Universal/Acpi/AcpiPlatformDxe/AcpiPlatformDxe.inf
  Platform/CIX/Sky1/Drivers/AcpiSocTables/AcpiSocTables.inf
  Platform/CIX/Sky1/Drivers/AcpiSocDxe/AcpiSocDxe.inf

  DynamicTablesPkg/Drivers/DynamicTableFactoryDxe/DynamicTableFactoryDxe.inf {
    <LibraryClasses>
      NULL|Platform/CIX/Sky1/Library/Acpi/CIX/AcpiMadtLibCIX/AcpiMadtLibCIX.inf
      NULL|Platform/CIX/Sky1/Library/Acpi/CIX/AcpiSsdtCpuTopologyLibCIX/SsdtCpuTopologyLibCIX.inf
  }
  Platform/CIX/Sky1/Drivers/DynamicTableManagerDxe/DynamicTableManagerDxe.inf
  Platform/CIX/Sky1/Drivers/ConfigurationManagerDxe/ConfigurationManagerDxe.inf
!endif
  Platform/CIX/Sky1/Drivers/S3SaveStateDxe/S3SaveStateDxe.inf

  #
  # boot logo
  #
!if $(BOOT_LOGO_ENABLE) == TRUE
  Platform/CIX/Sky1/Drivers/Logo/LogoDxe.inf
!endif
  # SMBIOS/DMI
  #
!if $(SMBIOS_ENABLE) == TRUE
  MdeModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf
  ArmPkg/Universal/Smbios/ProcessorSubClassDxe/ProcessorSubClassDxe.inf
  Platform/CIX/Sky1/Drivers/CixSmbiosDxe/CixSmbiosDxe.inf
!endif
  #
  # RAM Disk
!if $(TOKEN_RAM_DISK_SUPPORT) == TRUE
  MdeModulePkg/Universal/Disk/RamDiskDxe/RamDiskDxe.inf
  Platform/CIX/Sky1/Drivers/CreateRamDiskDxe/CreateRamDiskDxe.inf
!endif

!if $(SOC_I2C_ENABLE) == TRUE
  Silicon/CIX/Sky1/Drivers/I2cDxe/I2cDxe.inf
  !if $(I2C_EC_ENABLE) == TRUE
  Platform/CIX/Sky1/Drivers/EcPlatformDxe/EcPlatformDxe.inf
  !endif
  !if $(I2C_HID_ENABLE) == TRUE
  Platform/CIX/Sky1/Drivers/I2cHidKeyboard/I2cHidKeyboard.inf
  Platform/CIX/Sky1/Drivers/I2cHidServiceDriver/I2cHidServiceDriver.inf
  Platform/CIX/Sky1/Drivers/I2cHidDeviceDxe/I2cHidDeviceDxe.inf
  !endif
!endif

!if $(SOC_SPI_ENABLE) == TRUE
  Silicon/CIX/Sky1/Drivers/SpiDxe/SpiDxe.inf
!endif

!if $(SOC_GPIO_INTR_ENABLE) == TRUE
  Silicon/CIX/Sky1/Drivers/GpioInterruptDxe/GpioInterruptDxe.inf
!endif

!if $(SOC_CDNSP_ENABLE) == TRUE || $(SOC_CDNSP_HOST_ENABLE) == TRUE
  Silicon/CIX/Sky1/Drivers/TypecPhyDxe/TypecPhyDxe.inf
  !if $(PLATFORM_PD_ENABLE) == TRUE
  Platform/CIX/Sky1/Drivers/PdDxe/PdDxe.inf {
    <LibraryClasses>
    PdLib|Platform/CIX/Sky1/Library/Rts5453hPdLib/Rts5453hPdLib.inf
  }
  !endif
!endif

!if $(SOC_USB_DEVICE_ENABLE) == TRUE
  Platform/CIX/Sky1/Drivers/UsbDeviceControlDxe/UsbDeviceControlDxe.inf
  Platform/CIX/Sky1/Application/Fastboot/Fastboot.inf {
    <LibraryClasses>
      BootLib|Platform/CIX/Sky1/Library/BootLib/BootLib.inf
      FlashUpdateLib|Platform/CIX/Sky1/Library/FlashUpdateLib/FlashUpdateLib.inf
      LibUfdt|Platform/CIX/Library/LibUfdt/LibUfdt.inf
      PartitionLib|Platform/CIX/Sky1/Library/PartitionLib/PartitionLib.inf
  }
  !if $(SOC_CDNSP_ENABLE) == TRUE
    Silicon/CIX/Sky1/Drivers/CdnspDxe/CdnspDxe.inf
  !endif
  Platform/CIX/Sky1/Drivers/LaunchFastbootDxe/LaunchFastbootDxe.inf
!endif

  MdeModulePkg/Bus/Pci/NonDiscoverablePciDeviceDxe/NonDiscoverablePciDeviceDxe.inf
!if $(SOC_CDNSP_HOST_ENABLE) == TRUE
  MdeModulePkg/Bus/Pci/UhciDxe/UhciDxe.inf
  MdeModulePkg/Bus/Pci/EhciDxe/EhciDxe.inf
  MdeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf
  MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf
  MdeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf
  MdeModulePkg/Bus/Usb/UsbMouseDxe/UsbMouseDxe.inf
  Silicon/CIX/Sky1/Drivers/UsbCHostDxe/UsbCHostDxe.inf
  Silicon/CIX/Sky1/Drivers/Usb32HostDxe/Usb32HostDxe.inf
  Silicon/CIX/Sky1/Drivers/Usb20HostDxe/Usb20HostDxe.inf
!endif

  # GMAC
!if $(SOC_GMAC_ENABLE) == TRUE
  Silicon/CIX/Sky1/Drivers/GmacSnpDxe/GmacSnpDxe.inf
!endif

!if $(SOC_HDA_ENABLE) == TRUE
  Silicon/CIX/Sky1/Drivers/HdaDxe/HdaDxe.inf
!endif

  #
  # PCI Support
  #
!if $(PCIE_HOST_ENABLE) == TRUE
  ArmPkg/Drivers/ArmPciCpuIo2Dxe/ArmPciCpuIo2Dxe.inf
  MdeModulePkg/Bus/Pci/PciHostBridgeDxe/PciHostBridgeDxe.inf
  Platform/CIX/Sky1/Drivers/Pci/PciBusDxe/PciBusDxe.inf
  EmbeddedPkg/Drivers/NonCoherentIoMmuDxe/NonCoherentIoMmuDxe.inf
!endif

!if $(SOC_GOP_ENABLE) == TRUE
  Silicon/CIX/Sky1/Drivers/DpuDxe/DpuDxe.inf {
    <LibraryClasses>
       DisplayLib|Silicon/CIX/Sky1/Library/DisplayLib/DisplayLib.inf
  }

  Silicon/CIX/Sky1/Drivers/LcdGraphicsOutputDxe/LcdGraphicsOutputDxe.inf {
    <LibraryClasses>
      DisplayLib|Silicon/CIX/Sky1/Library/DisplayLib/DisplayLib.inf
  }
!endif

  #
  # NVMe boot devices
  #
  Platform/CIX/Sky1/Drivers/NvmExpressDxe/NvmExpressDxe.inf

  # SCMI Driver
  Platform/CIX/Sky1/Drivers/ArmScmiDxe/ArmScmiDxe.inf
  # reset
  Silicon/CIX/Sky1/Drivers/ResetDxe/ResetDxe.inf
  # clock
  !if $(DIAGS_KERNEL_ENABLE) == TRUE
    CixDiagsKernelPkg/Sky1/Drivers/ClockDxe/ClockDxe.inf
  !else
    Silicon/CIX/Sky1/Drivers/ClockDxe/ClockDxe.inf
  !endif
  # power
!if $(DIAGS_KERNEL_ENABLE) == TRUE
  CixDiagsKernelPkg/Sky1/Drivers/PowerDxe/PowerDxe.inf
!else
  Silicon/CIX/Sky1/Drivers/PowerDxe/PowerDxe.inf
!endif
!if $(SETUP_DEBUG_CFG_ENABLE) == TRUE
  Platform/CIX/Sky1/Drivers/DebugConfigDxe/DebugConfigDxe.inf
!endif

  # Bds
  #
  MdeModulePkg/Universal/BootManagerPolicyDxe/BootManagerPolicyDxe.inf
  MdeModulePkg/Universal/BdsDxe/BdsDxe.inf
  MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf
  MdeModulePkg/Universal/DisplayEngineDxe/DisplayEngineDxe.inf
  MdeModulePkg/Universal/SetupBrowserDxe/SetupBrowserDxe.inf
!if $(TOKEN_SETUP_SUPPORT) == TRUE
  Platform/CIX/Sky1/Drivers/SetupManagerDxe/SetupManagerDxe.inf
!endif
    MdeModulePkg/Application/UiApp/UiApp.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/BootDiscoveryPolicyUiLib/BootDiscoveryPolicyUiLib.inf
!if $(TOKEN_SETUP_SUPPORT) == TRUE
      NULL|Platform/CIX/Sky1/Library/DeviceManagerUiLib/DeviceManagerUiLib.inf
!else
      NULL|MdeModulePkg/Library/DeviceManagerUiLib/DeviceManagerUiLib.inf
!endif
      NULL|MdeModulePkg/Library/BootManagerUiLib/BootManagerUiLib.inf
      NULL|MdeModulePkg/Library/BootMaintenanceManagerUiLib/BootMaintenanceManagerUiLib.inf
  }

!if $(CAPSULE_ENABLE)
  MdeModulePkg/Universal/EsrtDxe/EsrtDxe.inf
#  SignedCapsulePkg/Universal/SystemFirmwareUpdate/SystemFirmwareReportDxe.inf
  Platform/CIX/Sky1/Drivers/SystemFirmwareUpdate/SystemFirmwareReportDxe.inf
  Platform/CIX/Sky1/Drivers/ABLUpdate/SystemFirmwareReportDxe.inf
#  SignedCapsulePkg/Universal/SystemFirmwareUpdate/SystemFirmwareUpdateDxe.inf
  MdeModulePkg/Application/CapsuleApp/CapsuleApp.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  }
#  Platform/CIX/Sky1/Drivers/SystemFirmwareDescriptor/SystemFirmwareDescriptor.inf

!endif
  MdeModulePkg/Application/HelloWorld/HelloWorld.inf
!if $(FW_UPDATE_ENABLE) == TRUE
  # firmware update protocol
  Platform/CIX/Sky1/Drivers/FirmwareUpdateDxe/FwUpdateProtocolDxe.inf
!endif
!if $(PM_CONFIG_UPDATE_SUPPORT) == TRUE
  Platform/CIX/Sky1/Drivers/PmConfigUpdateDxe/PmConfigUpdateDxe.inf
!endif
!if $(MEM_CONFIG_UPDATE_SUPPORT) == TRUE
  Platform/CIX/Sky1/Drivers/MemConfigUpdateDxe/MemConfigUpdateDxe.inf
!endif
!if $(FW_CONFIG_UPDATE_SUPPORT) == TRUE
  Platform/CIX/Sky1/Drivers/FirmwareConfigUpdateDxe/FirmwareConfigUpdateDxe.inf
!endif
  Platform/CIX/Sky1/Drivers/FwVersionDxe/FwVersionDxe.inf
  Platform/CIX/Sky1/Drivers/FwVersionDxe/FwVersionProtocolTest.inf
  #
  # UEFI application (Shell Embedded Boot Loader)
  #
!if $(INCLUDE_TFTP_COMMAND) == TRUE
ShellPkg/DynamicCommand/TftpDynamicCommand/TftpDynamicCommand.inf {
  <PcdsFixedAtBuild>
    gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE
}
ShellPkg/DynamicCommand/HttpDynamicCommand/HttpDynamicCommand.inf {
  <PcdsFixedAtBuild>
    gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE
}
!endif

!if $(VARIABLE_SUPPORT) == "SPI" && $(STMM_SUPPORT) == TRUE
!else
  MdeModulePkg/Application/VariableInfo/VariableInfo.inf
!endif
!if $(COMPILE_SYSTEM_LOADER) == android
  Platform/CIX/Sky1/Application/LinuxLoader/LinuxLoader.inf {
    <LibraryClasses>
      FdtLib|EmbeddedPkg/Library/FdtLib/FdtLib.inf
      LibUfdt|Platform/CIX/Library/LibUfdt/LibUfdt.inf
      ArmSmcLib|ArmPkg/Library/ArmSmcLib/ArmSmcLib.inf
      BootLib|Platform/CIX/Sky1/Library/BootLib/BootLib.inf
      FlashUpdateLib|Platform/CIX/Sky1/Library/FlashUpdateLib/FlashUpdateLib.inf
      AvbLib|Platform/CIX/Sky1/Library/avb/AvbLib.inf
      DtbUpdateLib|Platform/CIX/Sky1/Library/DtbUpdateLib/DtbUpdateLib.inf
      PartitionLib|Platform/CIX/Sky1/Library/PartitionLib/PartitionLib.inf
  }
  Platform/CIX/Sky1/Application/ABSelect/ABSelect.inf {
    <LibraryClasses>
      LibUfdt|Platform/CIX/Library/LibUfdt/LibUfdt.inf
      PartitionLib|Platform/CIX/Sky1/Library/PartitionLib/PartitionLib.inf
      MiniFastbootLib|Platform/CIX/Sky1/Library/FastbootLib/MiniFastbootLib.inf
      AblAuthenticateLib|Platform/CIX/Sky1/Library/AblAuthenticateLib/AblAuthenticateLib.inf
      AvbLib|Platform/CIX/Sky1/Library/avb/AvbLib.inf
  }
  Platform/CIX/Sky1/Drivers/AndroidConfigDxe/AndroidConfigDxe.inf
!endif

!if $(PD_FW_UPDATE_ENABLE) == TRUE
  Platform/CIX/Sky1/Application/PdFwUpdate/PdFwUpdate.inf
!endif

!if $(I2S_MC_SUPPORT) == TRUE
  Silicon/CIX/Sky1/Drivers/I2sDxe/I2sDxe.inf
!endif


!if $(PI_TEST_SUPPORT) == TRUE
  Platform/ARM/Drivers/ArmPsciMpServicesDxe/ArmPsciMpServicesDxe.inf
!endif
!if $(POWER_BUTTON_ENABLE) == TRUE
  Platform/CIX/Sky1/Drivers/PowerButtonDxe/PowerButtonDxe.inf
!endif
