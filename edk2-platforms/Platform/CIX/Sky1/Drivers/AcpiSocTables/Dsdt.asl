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

  // GPIO MMIO regions for USB VBUS power control
  // GPI4: S5 GPIO Bank 0 (GPIO001-014, GPIO025-042)
  // GPI5: S5 GPIO Bank 1 (GPIO015-024)
  OperationRegion (GS50, SystemMemory, 0x16004000, 0x08)
  Field (GS50, DWordAcc, NoLock, Preserve) {
    GD50, 32,  // GPIO0_S5 Data Register (SWPORTA_DR)
    GR50, 32,  // GPIO0_S5 Direction Register (SWPORTA_DDR)
  }
  OperationRegion (GS51, SystemMemory, 0x16005000, 0x08)
  Field (GS51, DWordAcc, NoLock, Preserve) {
    GD51, 32,  // GPIO1_S5 Data Register (SWPORTA_DR)
    GR51, 32,  // GPIO1_S5 Direction Register (SWPORTA_DDR)
  }

  // Prepare To Sleep — called by Linux before reboot (S5) and shutdown
  // Drives USB VBUS GPIOs LOW so devices re-enumerate cleanly on next boot
  Method (_PTS, 1, NotSerialized) {
    If (LEqual (Arg0, 5)) {
      // Set direction bits to output (1) for our VBUS pins
      // Bank 0: GPIO040=BIT29, GPIO041=BIT30, GPIO042=BIT31
      Store (Or (GR50, 0xE0000000), GR50)
      // Bank 1: GPIO019=BIT4, GPIO020=BIT5, GPIO021=BIT6
      Store (Or (GR51, 0x00000070), GR51)

      // Clear data bits to drive VBUS LOW
      // Bank 0: clear bits 29-31
      Store (And (GD50, Not (0xE0000000)), GD50)
      // Bank 1: clear bits 4-6
      Store (And (GD51, Not (0x00000070)), GD51)

      // Small delay for USB devices to detect VBUS drop
      Sleep (50)
    }
  }

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
