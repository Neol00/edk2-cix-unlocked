/** @file

  @copyright
  Copyright 2016 - 2020 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

DefinitionBlock ("EPRPPlatform10nm.asl","DSDT",2,"INTEL","EPRP10NM",3)
{
  #include "CommonPlatform10nm.asi"
  #include "PlatformPciTree10nm_EPRP.asi"
  #include "AMLUPD.asl"
  #include "DSDT.asl"
  #include "Pch.asl"       //This is in another package (PchPkg)
  #include "Platform.asl"
  #include "PlatformGpe10nm.asi"
  #include "IioPcieEdpcNotify10nm.asi"
} // end of DSDT
