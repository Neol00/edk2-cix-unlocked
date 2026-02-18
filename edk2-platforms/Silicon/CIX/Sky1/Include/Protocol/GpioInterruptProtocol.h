/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef _GPIO_INTERRUPT_PROTOCOL_H_
#define _GPIO_INTERRUPT_PROTOCOL_H_

#include "InterruptId.h"
#include "IoConfig.h"
//
// Protocol GUID
//
// F32750F7-854F-44C2-BBF3-13994CDAC697

#define CIX_GPIO_INTERRUPT_PROTOCOL_GGUID \
  { 0xF32750F7, 0x854F, 0x44C2, { 0xBB, 0xF3, 0x13, 0x99, 0x4C, 0xDA, 0xC6, 0x97 } }

typedef struct _GPIO_INTERRUPT_PROTOCOL GPIO_INTERRUPT_PROTOCOL;

typedef
VOID
(EFIAPI *GPIO_INTERRUPT_CALLBACK)(
  IN VOID   *Context,
  IN UINTN  Size
  );

typedef struct _GPIO_INTERRUPT_HANDLER {
  GPIO_INTERRUPT_CALLBACK    Callback;
  VOID                       *Context;
  UINTN                      Size;
} GPIO_INTERRUPT_HANDLER;

typedef
  EFI_STATUS
(EFIAPI *GPIO_INTERRUPT_IRQ_REGISTER)(
  IN  GPIO_INTERRUPT_PROTOCOL *This,
  IN  UINT32                  Num,
  IN  IO_INTERRUPT_TYPE_SEL   IntrType,
  IN  GPIO_INTERRUPT_HANDLER  *Handler
  );

typedef
  EFI_STATUS
(EFIAPI *GPIO_INTERRUPT_IRQ_UNREGISTER)(
  IN  GPIO_INTERRUPT_PROTOCOL *This,
  IN  UINT32                  Num
  );

struct _GPIO_INTERRUPT_PROTOCOL {
  GPIO_INTERRUPT_IRQ_REGISTER      Register;
  GPIO_INTERRUPT_IRQ_UNREGISTER    Unregister;
};

extern EFI_GUID  gCixGpioInterruptProtocolGuid;

#endif
