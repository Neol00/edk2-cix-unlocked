/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <IndustryStandard/Acpi.h>
#include <Library/AcpiLib.h>
#include <Library/PcdLib.h>
#include <Protocol/ClockId.h>
#include <Protocol/sky1-reset.h>
#include <Protocol/sky1-reset-fch.h>
#include <Protocol/sky1-reset-audss.h>
#include <AcpiRamVariable.h>
#include "Include/MemoryMap.h"
#include "Include/InterruptId.h"
#include "Include/DMACommon.h"
#include "CommonDefines.h"

#ifndef LINUX_ACPI_CONFIG_OVERRIDE
#include "DefaultLinuxAcpiConfig.h"
#else
#include "LinuxAcpiConfig.h"
#endif

#define RESOURCE_MEM 0x0200
#define RESOURCE_IRQ 0x0400

DefinitionBlock("DsdtTable.aml", "DSDT", 5, "CIXTEK", "SKY1EDK2", 1) {
  Scope(_SB) {
    include("Dsdt-Debug.asl")
    include("Dsdt-CPU.asl")
    include("Dsdt-iomux.asl")
    include("Dsdt-Fch-Uart.asl")
    include("Dsdt-dst.asl")
    include("Dsdt-PDC.asl")
    include("Dsdt-Mailbox.asl")
    include("Dsdt-Clock.asl")
    include("Dsdt-ResLookup.asl")
    include("Dsdt-Reset.asl")
    include("Dsdt-Gmac.asl")
    include("Dsdt-Thermal.asl")
    include("Dsdt-ScmiMailbox.asl")
    include("Dsdt-Audss.asl")
    include("Dsdt-Gpio.asl")
    include("Dsdt-Pwm.asl")
    //include("Dsdt-Wdt.asl")
    include("Dsdt-Timer.asl")
    include("Dsdt-HDA.asl")
    include("Dsdt-Dsp.asl")
    include("Dsdt-Dma.asl")
    include("Dsdt-Xspi.asl")
    include("Dsdt-I2c.asl")
    include("Dsdt-Spi.asl")
    include("Dsdt-I3c.asl")
    include("Dsdt-Pcie.asl")
    include("Dsdt-CdnsPcie.asl")
    include("Dsdt-CdnsPciePwr.asl")
    include("Dsdt-Vpu.asl")
    include("Dsdt-Dpu.asl")
    include("Dsdt-GPU.asl")
    include("Dsdt-NPU.asl")
    include("Dsdt-I2s.asl")
    include("Dsdt-USB.asl")
    include("Dsdt-SUSB.asl")
    include("Dsdt-ISP.asl")
    include("Dsdt-CSI-DMA.asl")
    include("Dsdt-AcpiRam.asl")
    include("Dsdt-Tee.asl")
    include("Dsdt-Misc.asl")
    include("Dsdt-PEP.asl")
  }
}
