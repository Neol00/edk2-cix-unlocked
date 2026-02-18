/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

// Power Engine Plugin
Device (PEP0)
{
  Name (_HID, "CIXHA026")
  Name (_UID, 0x0)

  Method (_STA) {
    Return (0xF)
  }
}