/** I2cDxe.h
  Header defining the constant, base address amd function for I2C controller

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __I2C_DXE_H__
#define __I2C_DXE_H__

#include <Library/UefiLib.h>
#include <Uefi.h>

#include <Protocol/I2cMaster.h>
#include <Protocol/I2cDevicePath.h>

#define I2C_CONTROLLER_SIGNATURE  SIGNATURE_32 ('I', '2', 'C', 'C')
#define I2C_CONTROLLER_FROM_THIS(a)  CR ((a), I2C_CONTROLLER_CONTEXT,     \
                                            I2cMaster, I2C_CONTROLLER_SIGNATURE)

typedef struct {
  UINT32                     Signature;
  EFI_HANDLE                 Controller;
  I2C_HOST_DESCRIPTOR        Descriptor;
  EFI_I2C_MASTER_PROTOCOL    I2cMaster;
  I2C_DEVICE_PATH            DevicePath;
} I2C_CONTROLLER_CONTEXT;

#endif //__I2C_DXE_H__
