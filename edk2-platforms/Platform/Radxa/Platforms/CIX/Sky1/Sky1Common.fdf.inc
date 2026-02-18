## @file
#
#  Copyright 2024 Radxa Computer (Shenzhen) Co., Ltd. All Rights Reserved.
#  Copyright (c) 2018, Linaro Ltd. All rights reserved.
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

  INF Platform/Radxa/Platforms/CIX/Sky1/Drivers/SystemInfoDxe/SystemInfoDxe.inf

!if $(TOKEN_SETUP_SUPPORT) == FALSE
  INF Platform/CIX/Sky1/Drivers/PlatformSetupVariableInitDxe/PlatformSetupVariableInitDxe.inf
  INF Platform/Radxa/Platforms/CIX/Sky1/Drivers/PlatformConfigDxe/PlatformConfigDxe.inf
!endif

  INF EmbeddedPkg/Drivers/DtPlatformDxe/DtPlatformDxe.inf

!if $(VARIABLE_SUPPORT) == "SPI" && $(STMM_SUPPORT) == TRUE
  INF MdeModulePkg/Application/VariableInfo/VariableInfo.inf
!endif

!if $(TOKEN_RAM_DISK_SUPPORT) == FALSE
  INF MdeModulePkg/Universal/Disk/RamDiskDxe/RamDiskDxe.inf
!endif
