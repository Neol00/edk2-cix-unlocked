## @file
# Common DSC content to begin Stage 4 enabling
#
# @copyright
# Copyright (C) 2022 Theo Jehl
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
##


[LibraryClasses]
  !if $(SMM_REQUIRED) == TRUE
    SpiFlashCommonLib     | IntelSiliconPkg/Library/SmmSpiFlashCommonLib/SmmSpiFlashCommonLib.inf
  !endif

[LibraryClasses.Common.DXE_SMM_DRIVER]
  LockBoxLib              | MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxSmmLib.inf
  SmmCpuPlatformHookLib   | OvmfPkg/Library/SmmCpuPlatformHookLibQemu/SmmCpuPlatformHookLibQemu.inf
  SmmCpuFeaturesLib       | OvmfPkg/Library/SmmCpuFeaturesLib/SmmCpuFeaturesLib.inf

[Components.$(DXE_ARCH)]
  OvmfPkg/AcpiPlatformDxe/AcpiPlatformDxe.inf
  MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf
  OvmfPkg/SataControllerDxe/SataControllerDxe.inf
  MdeModulePkg/Bus/Ata/AtaAtapiPassThru/AtaAtapiPassThru.inf
  MdeModulePkg/Bus/Ata/AtaBusDxe/AtaBusDxe.inf
  MdeModulePkg/Bus/Pci/UhciDxe/UhciDxe.inf
  MdeModulePkg/Bus/Pci/EhciDxe/EhciDxe.inf
  MdeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf
  MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  MdeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf

  !if $(SMM_REQUIRED) == TRUE
    OvmfPkg/SmmAccess/SmmAccess2Dxe.inf
    OvmfPkg/SmmControl2Dxe/SmmControl2Dxe.inf
    MdeModulePkg/Core/PiSmmCore/PiSmmIpl.inf
    MdeModulePkg/Core/PiSmmCore/PiSmmCore.inf

    MdeModulePkg/Universal/LockBox/SmmLockBox/SmmLockBox.inf
    UefiCpuPkg/PiSmmCpuDxeSmm/PiSmmCpuDxeSmm.inf

    MdeModulePkg/Universal/ReportStatusCodeRouter/Smm/ReportStatusCodeRouterSmm.inf
    MdeModulePkg/Universal/StatusCodeHandler/Smm/StatusCodeHandlerSmm.inf
    UefiCpuPkg/CpuIo2Smm/CpuIo2Smm.inf
    MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteSmm.inf
    IntelSiliconPkg/Feature/Flash/SpiFvbService/SpiFvbServiceSmm.inf
  !endif

  #
  # SMBIOS Support
  #
  MdeModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf {
    <LibraryClasses>
      NULL | OvmfPkg/Library/SmbiosVersionLib/DetectSmbiosVersionLib.inf
  }
  OvmfPkg/SmbiosPlatformDxe/SmbiosPlatformDxe.inf
