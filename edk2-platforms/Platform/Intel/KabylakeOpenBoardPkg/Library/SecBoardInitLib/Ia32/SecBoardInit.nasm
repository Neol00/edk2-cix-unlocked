;; @file
; Kaby Lake board SEC initialization.
;
; Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;;

SECTION .text

global  ASM_PFX(BoardBeforeTempRamInit)
ASM_PFX(BoardBeforeTempRamInit):
        ;
        ; This hook is called before FSP TempRamInit API call
        ; ESI, EDI need to be preserved
        ; ESP contains return address
        ;
        jmp     esp
