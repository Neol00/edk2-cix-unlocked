/** @file

  @copyright
  Copyright 2015 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Method (_ADR, 0) {
  If (LNotEqual(RPAK,0)) {
    Return (RPAK)
  } Else {
    Return (0x001B0003)
  }
}
