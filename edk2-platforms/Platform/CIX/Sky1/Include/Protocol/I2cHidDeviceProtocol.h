#/** @file I2cHidDeviceProtocol.h
#
#  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
#
#**/

#ifndef _I2C_MASTER_PROTOCOL_H_
#define _I2C_MASTER_PROTOCOL_H_

  #include <Uefi.h>
  #include <Pi/PiI2c.h>
  #include <Library/UefiLib.h>
  #include <IndustryStandard/Pci.h>
  #include <Library/BaseLib.h>
  #include <Library/MemoryAllocationLib.h>
  #include <Library/DebugLib.h>
  #include <Library/IoLib.h>
  #include <Library/UefiBootServicesTableLib.h>
  #include <Library/BaseMemoryLib.h>
  #include <Library/TimerLib.h>
  #include <Library/PcdLib.h>
  #include <Library/UefiRuntimeLib.h>
  #include <Protocol/I2cMaster.h>

#define CIX_I2C_HID_DEVICE_PATH_GUID  \
    { 0xdeb15fae, 0xc007, 0x4a82, { 0xb7, 0x2f, 0x2e, 0x6e, 0x39, 0xc0, 0x2f, 0xf5 } }

#define I2C_MASTER_VERSION  0x00000001

//
//  Frequency of I2C
//
#define I2C_100KHZ  (100 * 1000)
#define I2C_400KHZ  (400 * 1000)
#define I2C_1MHZ    (1000 * 1000)
#define I2C_3_4MHZ  (3400 * 1000)

#define I2C_BUS_PRIVATE_SIGNATURE  SIGNATURE_32('I','C','B','P')

#define I2C_BUS_PRIVATE_FROM_THIS(a) \
            CR (a, I2C_BUS_PRIVATE, I2cMaster, I2C_BUS_PRIVATE_SIGNATURE)

typedef struct _I2C_BUS_PRIVATE I2C_BUS_PRIVATE;

  #pragma pack(1)

typedef struct {
  VENDOR_DEVICE_PATH          DevicePath;
  UINT8                       Connect;
  EFI_GUID                    DeviceGuid;
  EFI_DEVICE_PATH_PROTOCOL    End;
} CIX_I2C_HOST_DEVICE_PATH;

typedef struct {
  UINT32            BaseAddress;
  UINT16            HidAddress;
  UINT16            InterruptPin;
  EFI_I2C_DEVICE    I2cDevice;
} I2C_HID_DEVICE_INFO;
//
// Declare a private data structure for this driver
//
struct _I2C_BUS_PRIVATE {
  UINT32                     Signature;
  UINT32                     Version;
  EFI_HANDLE                 Handle;
  UINT32                     I2cBase;
  UINT32                     HidAddress;
  EFI_GUID                   Guid;
  UINT16                     InterruptPin;
  EFI_I2C_MASTER_PROTOCOL    I2cMaster;
};

  #pragma pack()

extern EFI_GUID  gCixI2cHidDevicePathGuid;

#endif
