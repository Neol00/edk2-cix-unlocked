## @file
#
#  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

!if $(DIAGS_KERNEL_ENABLE) == TRUE
  INF Edk2OverridePkg/MdeModulePkg/Core/Dxe/DxeMain.inf
!else
  INF MdeModulePkg/Core/Dxe/DxeMain.inf
!endif

  INF MdeModulePkg/Universal/PCD/Dxe/Pcd.inf
!if $(GLOBAL_WATCHDOG_ENABLE) == TRUE
  INF Silicon/CIX/Sky1/Drivers/GenericWatchdogDxe/GlobalWatchdogDxe.inf
!endif
  #
  # PI DXE Drivers producing Architectural Protocols (EFI Services)
  #
  INF ArmPkg/Drivers/CpuDxe/CpuDxe.inf
  INF MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf
  INF Platform/CIX/Sky1/Drivers/S3SaveStateDxe/S3SaveStateDxe.inf
!if $(CFG_PARAM_MANAGE_ENABLE) == TRUE
  INF Platform/CIX/Sky1/Drivers/PlatformConfigParamsManageDxe/PlatformConfigParamsManageDxe.inf
  INF Silicon/CIX/Sky1/Drivers/ConfigParamsManageDxe/ConfigParamsManageDxe.inf
!endif
!if $(TOKEN_SETUP_SUPPORT) == TRUE
  INF Platform/CIX/Sky1/Drivers/PlatformSetupVariableInitDxe/PlatformSetupVariableInitDxe.inf
!endif
  INF Silicon/CIX/Sky1/Drivers/SocDxe/SocDxe.inf
  INF Platform/CIX/Sky1/Drivers/PlatformDxe/PlatformDxe.inf
  INF MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf
  INF MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
!if $(SOC_PWR_CLK_RST_ENABLE) == TRUE
  # SCMI Driver
  INF Platform/CIX/Sky1/Drivers/ArmScmiDxe/ArmScmiDxe.inf
  # power
!if $(DIAGS_KERNEL_ENABLE) == TRUE
  INF CixDiagsKernelPkg/Sky1/Drivers/PowerDxe/PowerDxe.inf
!else
  INF Silicon/CIX/Sky1/Drivers/PowerDxe/PowerDxe.inf
!endif
  # clock
  !if $(DIAGS_KERNEL_ENABLE) == TRUE
    INF CixDiagsKernelPkg/Sky1/Drivers/ClockDxe/ClockDxe.inf
  !else
    INF Silicon/CIX/Sky1/Drivers/ClockDxe/ClockDxe.inf
  !endif
!endif
  # reset
  INF Silicon/CIX/Sky1/Drivers/ResetDxe/ResetDxe.inf

!if $(SOC_CDNSP_ENABLE) == TRUE || $(SOC_CDNSP_HOST_ENABLE) == TRUE
  INF Silicon/CIX/Sky1/Drivers/TypecPhyDxe/TypecPhyDxe.inf
  !if $(PLATFORM_PD_ENABLE) == TRUE
  INF Platform/CIX/Sky1/Drivers/PdDxe/PdDxe.inf
  !endif
!endif

!if $(SOC_USB_DEVICE_ENABLE) == TRUE
  !if $(SOC_CDNSP_ENABLE) == TRUE
    INF Silicon/CIX/Sky1/Drivers/CdnspDxe/CdnspDxe.inf
  !endif
    INF Platform/CIX/Sky1/Drivers/UsbDeviceControlDxe/UsbDeviceControlDxe.inf
    INF Platform/CIX/Sky1/Application/Fastboot/Fastboot.inf
    INF Platform/CIX/Sky1/Drivers/LaunchFastbootDxe/LaunchFastbootDxe.inf
!endif

  INF MdeModulePkg/Bus/Pci/NonDiscoverablePciDeviceDxe/NonDiscoverablePciDeviceDxe.inf

!if $(SOC_CDNSP_HOST_ENABLE) == TRUE
  INF MdeModulePkg/Bus/Pci/UhciDxe/UhciDxe.inf
  INF MdeModulePkg/Bus/Pci/EhciDxe/EhciDxe.inf
  INF MdeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf
  INF MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  INF MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf
  INF MdeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf
  INF MdeModulePkg/Bus/Usb/UsbMouseDxe/UsbMouseDxe.inf
  INF Silicon/CIX/Sky1/Drivers/UsbCHostDxe/UsbCHostDxe.inf
  INF Silicon/CIX/Sky1/Drivers/Usb32HostDxe/Usb32HostDxe.inf
  INF Silicon/CIX/Sky1/Drivers/Usb20HostDxe/Usb20HostDxe.inf
!endif

!if $(STMM_SUPPORT) == TRUE
  INF Platform/CIX/Sky1/Drivers/MmCommunicationDxe/ArmMmCommunication.inf
!endif
!if $(STMM_SUPPORT) == TRUE
  INF Silicon/CIX/Sky1/Drivers/NorFlashDxe/NorFlashSmmRuntimeDxe.inf
!else
  INF Silicon/CIX/Sky1/Drivers/NorFlashDxe/NorFlashDxe.inf
!endif
!if $(VARIABLE_SUPPORT) == "SPI" && $(STMM_SUPPORT) == FALSE
  INF MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteDxe.inf
!endif
!if $(VARIABLE_SUPPORT) == "SPI" && $(STMM_SUPPORT) == TRUE
  INF MdeModulePkg/Universal/Variable/RuntimeDxe/VariableSmmRuntimeDxe.inf
!else
  INF MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf
!endif
!if $(PCIE_HOST_ENABLE) == TRUE
  INF Silicon/CIX/Sky1/Drivers/PcieInitDxe/PcieInitDxe.inf
!endif
!if $(SECURE_BOOT_ENABLE) == TRUE
!include ArmPlatformPkg/SecureBootDefaultKeys.fdf.inc
  INF SecurityPkg/VariableAuthenticated/SecureBootConfigDxe/SecureBootConfigDxe.inf
  INF SecurityPkg/VariableAuthenticated/SecureBootDefaultKeysDxe/SecureBootDefaultKeysDxe.inf
  INF Platform/CIX/Sky1/Drivers/SecureBootDxe/SecureBootDxe.inf
!endif
  INF MdeModulePkg/Universal/MonotonicCounterRuntimeDxe/MonotonicCounterRuntimeDxe.inf
  INF EmbeddedPkg/ResetRuntimeDxe/ResetRuntimeDxe.inf
  INF EmbeddedPkg/RealTimeClockRuntimeDxe/RealTimeClockRuntimeDxe.inf
  INF EmbeddedPkg/MetronomeDxe/MetronomeDxe.inf
  INF MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf
  #
  # Multiple Console IO support
  #
!if $(SLT_ENABLE) == TRUE
  INF EmbeddedPkg/SimpleTextInOutSerial/SimpleTextInOutSerial.inf
!else
  INF MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  INF MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  INF MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  INF MdeModulePkg/Universal/Console/TerminalDxe/TerminalDxe.inf
  INF MdeModulePkg/Universal/SerialDxe/SerialDxe.inf
!if $(TOKEN_CONSOLE_PREF_SUPPORT) == TRUE
  INF EmbeddedPkg/Drivers/ConsolePrefDxe/ConsolePrefDxe.inf
!endif
!endif

!if $(DIAGS_KERNEL_ENABLE) == TRUE
  INF Edk2OverridePkg/ArmPkg/Drivers/ArmGic/ArmGicDxe.inf
!else
  INF ArmPkg/Drivers/ArmGic/ArmGicDxe.inf
!endif
  INF ArmPkg/Drivers/TimerDxe/TimerDxe.inf
!if $(POWER_BUTTON_ENABLE) == TRUE
  INF Platform/CIX/Sky1/Drivers/PowerButtonDxe/PowerButtonDxe.inf
!endif

#  INF MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf
  INF Silicon/CIX/Sky1/Drivers/GenericWatchdogDxe/GenericWatchdogDxe.inf
  INF MdeModulePkg/Universal/EbcDxe/EbcDxe.inf

!if $(SOC_GPIO_INTR_ENABLE) == TRUE
  INF Silicon/CIX/Sky1/Drivers/GpioInterruptDxe/GpioInterruptDxe.inf
!endif
!if $(SOC_GMAC_ENABLE) == TRUE
  INF Silicon/CIX/Sky1/Drivers/GmacSnpDxe/GmacSnpDxe.inf
!endif
!if $(DTB_UPDATE_ENABLE) == TRUE
  INF Platform/CIX/Sky1/Drivers/DtbUpdateDxe/DtbUpdateDxe.inf
!endif
!if $(RLOG_DUMP_ENABLE) == TRUE
  INF Platform/CIX/Sky1/Drivers/RamLogDumpDxe/RamLogDumpDxe.inf
!endif
!if $(SOC_I2C_ENABLE) == TRUE
  INF Silicon/CIX/Sky1/Drivers/I2cDxe/I2cDxe.inf
  !if $(I2C_EC_ENABLE) == TRUE
  INF Platform/CIX/Sky1/Drivers/EcPlatformDxe/EcPlatformDxe.inf
  !endif
  !if $(I2C_HID_ENABLE) == TRUE
  INF Platform/CIX/Sky1/Drivers/I2cHidKeyboard/I2cHidKeyboard.inf
  INF Platform/CIX/Sky1/Drivers/I2cHidServiceDriver/I2cHidServiceDriver.inf
  INF Platform/CIX/Sky1/Drivers/I2cHidDeviceDxe/I2cHidDeviceDxe.inf
  !endif
!endif

!if $(SOC_SPI_ENABLE) == TRUE
  INF Silicon/CIX/Sky1/Drivers/SpiDxe/SpiDxe.inf
!endif

!if $(SOC_HDA_ENABLE) == TRUE
  INF Silicon/CIX/Sky1/Drivers/HdaDxe/HdaDxe.inf
!endif

!if $(SOC_GOP_ENABLE) == TRUE
  #
  # GOP Support
  #

  INF Silicon/CIX/Sky1/Drivers/DpuDxe/DpuDxe.inf
  INF Silicon/CIX/Sky1/Drivers/LcdGraphicsOutputDxe/LcdGraphicsOutputDxe.inf

  #
  # TianoCore logo (splash screen)
  #
  #INF MdeModulePkg/Logo/LogoDxe.inf
!endif
!if $(AMD_GOP_DRIVER_SUPPORT) == TRUE
 FILE DRIVER = 27E5BA77-5839-400b-A7DA-5F5D78F0C539
 {
   SECTION PE32 = Platform/CIX/Sky1/Drivers/AmdGop/Arm64Gop_1_68.efi
   SECTION UI = "AmdGop"
 }
!endif

  #
  # x86 Option ROM emulator (Intel MultiArchUefiPkg / Unicorn)
  # Enables GPU Option ROM loading for NVIDIA, Intel, etc.
  #
!if $(ARCH) == AARCH64
  INF Emulator/X86EmulatorDxe/X86EmulatorDxe.inf
!endif

  #
  # FAT filesystem + GPT/MBR partitioning
  #
  INF MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  INF MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf
  INF FatPkg/EnhancedFatDxe/Fat.inf

!if $(NTFS_DRIVER_SUPPORT) == TRUE
 FILE DRIVER = D3E589F2-010A-4438-B4E5-7620C403D26F
 {
   SECTION PE32 = Platform/CIX/Sky1/Drivers/NTFS/NTFS.efi
   SECTION UI = "NTFS"
 }
!endif

!if $(EXT4_DRIVER_SUPPORT) == TRUE
!include Features/Ext4Pkg/Ext4.fdf.inc
!endif

!if $(REALTEK_LAN_DRIVER_SUPPORT) == TRUE
 FILE DRIVER = 785277C5-20B9-4940-8D9F-341AEAAB4DEF
 {
   SECTION PE32 = Platform/CIX/Sky1/Drivers/RealtekLan/RtkUndiDxe.efi
   SECTION UI = "Realtek"
 }
!endif
  INF MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf
  # FV FileSystem
!if $(FV_SIMPLE_FILE_SUPPORT) == TRUE
  INF MdeModulePkg/Universal/FvSimpleFileSystemDxe/FvSimpleFileSystemDxe.inf
!endif
!if $(TOKEN_RAM_DISK_SUPPORT) == TRUE
  INF MdeModulePkg/Universal/Disk/RamDiskDxe/RamDiskDxe.inf
  INF Platform/CIX/Sky1/Drivers/CreateRamDiskDxe/CreateRamDiskDxe.inf
!endif

!if $(COMPILE_SYSTEM_LOADER) == android
  INF Platform/CIX/Sky1/Application/LinuxLoader/LinuxLoader.inf
  INF Platform/CIX/Sky1/Application/ABSelect/ABSelect.inf
  INF Platform/CIX/Sky1/Drivers/AndroidConfigDxe/AndroidConfigDxe.inf
!endif
  #INF Platform/CIX/Sky1/Application/HelloWorld/HelloWorld.inf
  #INF MdeModulePkg/Application/HelloWorld/HelloWorld.inf
  #
  # ACPI Support
  #
!if $(ACPI_ENABLE) == TRUE
  INF MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf
  INF RuleOverride = ACPITABLE Platform/CIX/Sky1/Drivers/AcpiSocTables/AcpiSocTables.inf
  INF Platform/CIX/Sky1/Drivers/AcpiSocDxe/AcpiSocDxe.inf

  INF Platform/CIX/Sky1/Drivers/ConfigurationManagerDxe/ConfigurationManagerDxe.inf
  INF DynamicTablesPkg/Drivers/DynamicTableFactoryDxe/DynamicTableFactoryDxe.inf
  INF Platform/CIX/Sky1/Drivers/DynamicTableManagerDxe/DynamicTableManagerDxe.inf
!endif
  #
  # boot logo
  #
!if $(BOOT_LOGO_ENABLE) == TRUE
  INF Platform/CIX/Sky1/Drivers/Logo/LogoDxe.inf
!endif
  #
  # SMBIOS/DMI
  #
!if $(SMBIOS_ENABLE) == TRUE
  INF MdeModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf
  INF ArmPkg/Universal/Smbios/ProcessorSubClassDxe/ProcessorSubClassDxe.inf
  INF Platform/CIX/Sky1/Drivers/CixSmbiosDxe/CixSmbiosDxe.inf
!endif
  #
  # PCI Support
  #
!if $(PCIE_HOST_ENABLE) == TRUE
  INF ArmPkg/Drivers/ArmPciCpuIo2Dxe/ArmPciCpuIo2Dxe.inf
  INF MdeModulePkg/Bus/Pci/PciHostBridgeDxe/PciHostBridgeDxe.inf
  INF Platform/CIX/Sky1/Drivers/Pci/PciBusDxe/PciBusDxe.inf
  INF EmbeddedPkg/Drivers/NonCoherentIoMmuDxe/NonCoherentIoMmuDxe.inf
  #
  # NVMe boot devices
  #
  INF Platform/CIX/Sky1/Drivers/NvmExpressDxe/NvmExpressDxe.inf
!endif

!if $(CAPSULE_ENABLE) == TRUE
  INF  MdeModulePkg/Universal/EsrtDxe/EsrtDxe.inf
#  INF  SignedCapsulePkg/Universal/SystemFirmwareUpdate/SystemFirmwareReportDxe.inf
!if $(COMPILE_SYSTEM_LOADER) == android
  INF  Platform/CIX/Sky1/Drivers/ABLUpdate/SystemFirmwareReportDxe.inf
!else
  INF  Platform/CIX/Sky1/Drivers/SystemFirmwareUpdate/SystemFirmwareReportDxe.inf
!endif
  FILE FREEFORM = PCD(gEfiSignedCapsulePkgTokenSpaceGuid.PcdEdkiiPkcs7TestPublicKeyFileGuid) {
     SECTION RAW = BaseTools/Source/Python/Pkcs7Sign/TestRoot.cer
     SECTION UI = "Pkcs7TestRoot"
     }
!endif
!if $(FW_UPDATE_ENABLE) == TRUE
  # firmware update protocol
  INF Platform/CIX/Sky1/Drivers/FirmwareUpdateDxe/FwUpdateProtocolDxe.inf
  # FILE FREEFORM  = C0903BAC-CC0D-C264-B85B-70F1821A45C6 { # gEcFirmwareFileGuid
  #     SECTION RAW = Platform/CIX/Sky1/Drivers/FirmwareUpdateDxe/ec.bin
  # }
!endif
!if $(PM_CONFIG_UPDATE_SUPPORT) == TRUE
  INF Platform/CIX/Sky1/Drivers/PmConfigUpdateDxe/PmConfigUpdateDxe.inf
!endif
!if $(MEM_CONFIG_UPDATE_SUPPORT) == TRUE
  INF Platform/CIX/Sky1/Drivers/MemConfigUpdateDxe/MemConfigUpdateDxe.inf
!endif
!if $(FW_VERSION_ENABLE) == TRUE
  INF Platform/CIX/Sky1/Drivers/FwVersionDxe/FwVersionProtocolTest.inf
!endif
!if $(NETWORK_ENABLE) == TRUE
  #
  # Network Modules
  #
  !include NetworkPkg/Network.fdf.inc
!endif
  !if $(SETUP_DEBUG_CFG_ENABLE) == TRUE
    INF Platform/CIX/Sky1/Drivers/DebugConfigDxe/DebugConfigDxe.inf
  !endif
  # Bds
  #
  INF MdeModulePkg/Universal/BootManagerPolicyDxe/BootManagerPolicyDxe.inf
!if $(TOKEN_SETUP_SUPPORT) == TRUE
  INF Platform/CIX/Sky1/Drivers/SetupManagerDxe/SetupManagerDxe.inf
!endif
  INF MdeModulePkg/Application/UiApp/UiApp.inf
  INF MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf
  INF MdeModulePkg/Universal/DisplayEngineDxe/DisplayEngineDxe.inf
  INF MdeModulePkg/Universal/SetupBrowserDxe/SetupBrowserDxe.inf
  INF MdeModulePkg/Universal/BdsDxe/BdsDxe.inf
!if $(INCLUDE_TFTP_COMMAND) == TRUE
  INF ShellPkg/DynamicCommand/TftpDynamicCommand/TftpDynamicCommand.inf
  INF ShellPkg/DynamicCommand/HttpDynamicCommand/HttpDynamicCommand.inf
!endif
!if $(VARIABLE_SUPPORT) == "SPI" && $(STMM_SUPPORT) == TRUE
!else
  INF MdeModulePkg/Application/VariableInfo/VariableInfo.inf
!endif
!if $(I2S_MC_SUPPORT) == TRUE
  INF Silicon/CIX/Sky1/Drivers/I2sDxe/I2sDxe.inf
!endif
!if $(PI_TEST_SUPPORT) == TRUE
  INF Platform/ARM/Drivers/ArmPsciMpServicesDxe/ArmPsciMpServicesDxe.inf
!endif
