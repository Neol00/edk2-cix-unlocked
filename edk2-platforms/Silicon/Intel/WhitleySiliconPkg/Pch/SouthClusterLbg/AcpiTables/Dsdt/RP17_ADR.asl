/** @file

  @copyright
  Copyright 2015 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Method (_ADR, 0) {
  If (LNotEqual(RPAH,0)) {
    Return (RPAH)
  } Else {
    Return (0x001B0000)
  }
}
