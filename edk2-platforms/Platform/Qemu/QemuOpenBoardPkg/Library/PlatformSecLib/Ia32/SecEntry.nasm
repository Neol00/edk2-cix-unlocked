;------------------------------------------------------------------------------
;  @file SecEntry
;  Sec entry implementation
;
;  Copyright (c) 2022 Theo Jehl
;  SPDX-License-Identifier: BSD-2-Clause-Patent
;
;------------------------------------------------------------------------------

CODE_SEG equ CodeSegDescriptor - GDT_START
DATA_SEG equ DataSegDescriptor - GDT_START

extern ASM_PFX(SecStartup)

extern ASM_PFX(PcdGet32 (PcdTemporaryRamBase))
extern ASM_PFX(PcdGet32 (PcdTemporaryRamSize))

SECTION .text

BITS 16
align 4
global ASM_PFX(_ModuleEntryPoint)
ASM_PFX(_ModuleEntryPoint):
  cli
  ; Save the BIST in mm0
  movd mm0, eax
  mov esi, GDT_Descriptor
  db 66h
  lgdt [cs:si]

  mov     eax, cr0
  or      eax, 1
  mov     cr0, eax

  mov ax, DATA_SEG
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  mov esi, ProtectedModeEntryLinearAddress

  jmp dword far [cs:si]

BITS 32
align 4
ProtectedModeEntry:
  PROTECTED_MODE equ $

  mov ecx, DWORD [ASM_PFX(PcdGet32 (PcdTemporaryRamBase))]
  mov edx, DWORD [ASM_PFX(PcdGet32 (PcdTemporaryRamSize))]

  ; Initialize the stack at the end of base + size
  mov esp, ecx
  add esp, edx

  ; Push 1 CPU, will be probed later with Qemu FW CFG device
  push 1
  ; For now, we push the BIST once
  movd eax, mm0
  push eax
  ; Code in PlatformSecLib will look up this information we've just pushed
  ;  ================= TOP OF MEMORY ======================
  ;                    Count of BISTs
  ;                    BISTs[1..n]
  ;  ================= REST OF MEMORY =====================
  ; Each BIST is always a DWORD in size

  mov edi, 0xFFFFFFFC         ;BFV

  push DWORD [edi]            ;Passes BFV

  push ecx                    ;Passes RAM size

  push edx                    ;Passes RAM base

  call ASM_PFX(SecStartup)

align 8
NULL_SEGMENT    equ $ - GDT_START
GDT_START:

NullSegDescriptor:
  dd 0x0
  dd 0x0

  CODE_SEL        equ $ - GDT_START

CodeSegDescriptor:
  dw 0xFFFF
  dw 0x0
  db 0x0
  db 0x9B
  db 0xCF
  db 0x0

  DATA_SEL        equ $ - GDT_START

DataSegDescriptor:
  dw 0xFFFF
  dw 0x0
  db 0x0
  db 0x93
  db 0xCF
  db 0x0

GDT_END:

GDT_Descriptor:
  dw GDT_END - GDT_START - 1
  dd GDT_START

ProtectedModeEntryLinearAddress:
ProtectedModeEntryLinear:
  DD      ProtectedModeEntry  ; Offset of our 32 bit code
  DW      CODE_SEL
