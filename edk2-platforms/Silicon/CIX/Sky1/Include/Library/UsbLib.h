/** UsbLib.h

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef USB_LIB_H
#define USB_LIB_H

#include <Base.h>
#include <UsbDpPhy.h>
#include <Protocol/NonDiscoverableDevice.h>

#define DEBUG_LOG_NONE  0

#define DUMP_REGISTER_INFO  0

#if DUMP_REGISTER_INFO == 1
#define DEBUG_HOST_REGISTER  DEBUG_ERROR
#else
#define DEBUG_HOST_REGISTER  DEBUG_LOG_NONE
#endif

#define BIT(nr)  (1UL << (nr))

#define CFG_APB_TIMEOUT_PSLVERR_EN              BIT(22)
#define CFG_APB_PSLVERR_EN                      BIT(23)
#define XEC_USBSSP_CHICKEN_BITS_3      0xA230
#define XEC_AXI_CAP_ADDR    0xA174
#define XEC_AXI_CTRL0_ADDR  0xA17c
#define XEC_AXI_CAP         BIT(31)
#define AXI_BMAX            0x07

#ifdef FPGA_BOARD
#define SOF_CLOCK_FREQUENCY  4000   // 4Mhz fpga setting
#define LPM_CLOCK_FREQUENCY  4000   // 4Mhz fpga setting
#else
#define SOF_CLOCK_FREQUENCY  8000   // 4Mhz normal setting
#define LPM_CLOCK_FREQUENCY  32     // 32k  setting
#endif

#define PMCTRL_S5_USB_MODE_STAP   0x16000424
#define PMCTRL_S5_USB_TYPEC_CTRL  0x16000420
#define XEC_CFG_3XPORT_MODE             0x2040
#define CFG_3XPORT_MODE_DIS_SSP         (~(1 << 31))
#define USB_DISABLED_U3_MASK         (0x7)


#define USB20ControlIndex0    2
#define USB20ControlIndex1    9
#define USB20ControlIndex2    8
#define USB20ControlIndex3    7
#define USB3DrdControlIndex0  5
#define USB3DrdControlIndex1  6
#define USBCDrdControlIndex0  0
#define USBCControlIndex0     1
#define USBCControlIndex1     4
#define USBCControlIndex2     3

typedef enum {
  USBTYPE_UNDEFINED = 0,
  USBTYPEA_USB20PHY = 1,
  USBTYPEA_USB32PHY = 2,
  USBTYPEC_USB32PHY = 3,
  USBTYPEC_USB20PHY = 4,
  USBT20ONLY_PHY    = 5
} USBPHY_TYPE;

typedef struct {
  UINT32     Offset;
  UINT32     Value;
  BOOLEAN    NeedCheck;
} USB_RCSU_CONF;

typedef struct {
  UINT32    SofClock;
  UINT32    LpmClock;
  UINT32    AxiClock;
  UINT32    ApbClock;
} USB_RCSU_CLOCK_RESOURCE;

typedef struct {
  UINT32    ControlPRestId;
  UINT32    ControlRestId;
} USB_RCSU_RESET_RESOURCE;

typedef struct {
  UINT64                     RcsuBase;
  UINT64                     OtgAddrBase;
  UINT64                     HostBase;
  UINT64                     Usb3PhyBase;
  UINT32                     RangeSize;

  USB_RCSU_CONF              *RcsuConf;
  UINT32                     RcsuLength;
  USB_RCSU_RESET_RESOURCE    *RcsuResetConf;
  USB_RCSU_CLOCK_RESOURCE    *RcsuClockConf;
  CDNSP_PHY                  *PhyInstance;
} USB_HOST_INFO;

VOID
UsbHostModifyAxCache (
  IN  UINTN  UsbReg
  );

VOID
UsbHostModifyTimerCount (
  IN  UINTN  Index
  );

VOID
UsbTypeAPhyInit (
  IN  UINTN  Index
  );

EFI_STATUS
GetUsbControlIndex (
  NON_DISCOVERABLE_DEVICE  *Device,
  UINT32                   *ControlIndex
  );

EFI_STATUS
UsbHostInit (
  UINT32  Index
  );

VOID
UsbTypeCPhyInit (
  IN  UINTN  Index
  );

USB_HOST_INFO *
GetUsbHostInfoArray (
  VOID
  );

VOID
DisableOverCurrentDetect (
  UINT32  Index
  );

EFI_STATUS
UsbTypeAPhyOverride (
  IN UINT32  Index
  );

#endif
