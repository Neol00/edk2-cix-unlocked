/** I2c.h
  Header defining the constant, base address amd function for I2C controller

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __I2C_DEVICE_PATH_PROTOCOL__
#define __I2C_DEVICE_PATH_PROTOCOL__

#pragma pack(1)
typedef struct {
  VENDOR_DEVICE_PATH          Vendor;
  UINT64                      MmioBase;
  UINT8                       Bus;
  EFI_DEVICE_PATH_PROTOCOL    End;
} I2C_DEVICE_PATH;
#pragma pack()

#endif //__I2C_H__
