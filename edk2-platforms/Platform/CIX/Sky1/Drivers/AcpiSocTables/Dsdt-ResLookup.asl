/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Protocol/sky1-reset.h>
#include <Protocol/sky1-reset-fch.h>
#include <Protocol/sky1-reset-audss.h>

Device (REST) { /* resource table */
  Name (_HID, "CIXA1019")
  Name (_UID, 0x0)
  Name (_STA, 0xF)

  // reserved memory lookup table
  Name (RSVL, Package() {
    // format: <base>, <size>, <type,"no-map"/"reusable">, <dev,ref>
    Package() {0xd0000000, 0x700000, "no-map", \_SB.DMA1},
    Package() {0xd0700000, 0x700000, "no-map", \_SB.HDA},
    Package() {0xcde08000, 0x100000, "no-map", \_SB.DSP},
  })

  // reset lookup table
  Name (RSTL, Package() {
    // format: <provider,ref>, <index>, <user,ref>, [<con-id>]
  })

  // resource name lookup table
  Name (IRQL, Package() {
    // format: <provider,ref>, <type>, <index>, <name>
    // type: include/linux/ioport.h
  })

  // device link lookup table
  Name (DLKL, Package() {
    // format: <supplier,ref>, <consumer,ref>, <flags>
  })
}

