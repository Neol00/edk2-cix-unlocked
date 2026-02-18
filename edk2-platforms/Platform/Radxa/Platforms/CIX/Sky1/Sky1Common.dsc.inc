[PcdsFixedAtBuild.common]

!if $(DEBUG_ON_UART3) == FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x040D0000 #0x091e1000 #0x0a051000
!else
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x040E0000
!endif

  # Default State After G3 is Power On
  gCixTokenSpaceGuid.PcdStateAfterG3|0x01

  # SPCR use UART2
  gArmPlatformTokenSpaceGuid.PcdSerialDbgRegisterBase|0x040D0000
  gArmPlatformTokenSpaceGuid.PcdSerialDbgInterrupt|330

[Components.common]

!if $(DEBUG_ON_UART3) == FALSE
  MdeModulePkg/Universal/SerialDxe/SerialDxe.inf
!else
  MdeModulePkg/Universal/SerialDxe/SerialDxe.inf {
    <PcdsFixedAtBuild>
      gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x040D0000
  }
!endif

  Platform/Radxa/Platforms/CIX/Sky1/Drivers/SystemInfoDxe/SystemInfoDxe.inf

!if $(TOKEN_SETUP_SUPPORT) == FALSE
  Platform/CIX/Sky1/Drivers/PlatformSetupVariableInitDxe/PlatformSetupVariableInitDxe.inf
  Platform/Radxa/Platforms/CIX/Sky1/Drivers/PlatformConfigDxe/PlatformConfigDxe.inf
!endif

  EmbeddedPkg/Drivers/DtPlatformDxe/DtPlatformDxe.inf {
    <LibraryClasses>
      DtPlatformDtbLoaderLib|EmbeddedPkg/Library/DxeDtPlatformDtbLoaderLibDefault/DxeDtPlatformDtbLoaderLibDefault.inf
  }

!if $(VARIABLE_SUPPORT) == "SPI" && $(STMM_SUPPORT) == TRUE
  MdeModulePkg/Application/VariableInfo/VariableInfo.inf
!endif

!if $(TOKEN_RAM_DISK_SUPPORT) == FALSE
  MdeModulePkg/Universal/Disk/RamDiskDxe/RamDiskDxe.inf
!endif
