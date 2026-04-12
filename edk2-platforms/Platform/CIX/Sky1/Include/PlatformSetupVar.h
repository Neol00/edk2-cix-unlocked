/**
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PLATFORM_SETUP_VAR_H_
#define _PLATFORM_SETUP_VAR_H_

#define PLATFORM_SETUP_VAR  L"PlatformSetupVar"
#define SYSTEM_TABLE_VAR    L"SystemTableVar"
#define MAX_CPU_CORE_NUM    12
#define MAX_PCIE_PORT_NUM   5
#define MAX_I2C_CTRL_NUM    8
#define MAX_USB_PORT_NUM    10
#define MAX_GMAC_PORT_NUM   2
#define MAX_DPU_PORT_NUM    5
#pragma pack(1)
typedef struct {
  UINT8     PcieRpEnable[MAX_PCIE_PORT_NUM];
  UINT8     PcieWidth[MAX_PCIE_PORT_NUM];
  UINT8     PcieMaxSpeed[MAX_PCIE_PORT_NUM];
  UINT8     PcieTargetLinkSpeed[MAX_PCIE_PORT_NUM];
  UINT8     PcieAspmMaxSupport[MAX_PCIE_PORT_NUM];
  UINT8     PcieAspm[MAX_PCIE_PORT_NUM];
  UINT8     PcieMaxPayload[MAX_PCIE_PORT_NUM];
  UINT8     PcieMaxReadRequest[MAX_PCIE_PORT_NUM];
  UINT8     PcieL1Substates[MAX_PCIE_PORT_NUM];
  UINT8     PcieDtiEnable[MAX_PCIE_PORT_NUM];
  UINT8     I2cEnable[MAX_I2C_CTRL_NUM];
  UINT32    I2cBusFreq[MAX_I2C_CTRL_NUM];
  UINT8     GmacEnable[MAX_GMAC_PORT_NUM];
  UINT64    GmacMacAddr[MAX_GMAC_PORT_NUM];
  UINT8     Usb2Control0Enable;
  UINT8     Usb2Control1Enable;
  UINT8     Usb2Control2Enable;
  UINT8     Usb2Control3Enable;
  UINT8     Usb3Control0Enable;
  UINT8     Usb3Control1Enable;
  UINT8     UsbCDrdControl0Enable;
  UINT8     UsbCControl0Enable;
  UINT8     UsbCControl1Enable;
  UINT8     UsbCControl2Enable;
  UINT8     UsbCDrdControl0DataRole;
  UINT16    MemFreq;
  UINT8     MemBreakPoint;
  UINT8     MemEyeScan;
  UINT8     MemHarvesting;
  UINT8     WckAlwaysOn;
  UINT8     DataMask;
  UINT8     RfmEn;
  UINT8     AutoPrechargeEn;
  UINT8     PbrEn;
  UINT8     MbistEn;
  UINT8     MbistMode;
  UINT8     MemWrLEcc;
  UINT8     MemRdLEcc;
  UINT8     PortPriority;
  UINT8     BdwP0Override;
  UINT8     BdwP0;
  UINT8     BdwP1Override;
  UINT8     BdwP1;
  UINT8     MemRPriorityP0Override;
  UINT8     MemRPriorityP0;
  UINT8     MemWPriorityP0Override;
  UINT8     MemWPriorityP0;
  UINT8     MemBdwOvflowP0;
  UINT8     MemRPriorityP1Override;
  UINT8     MemRPriorityP1;
  UINT8     MemWPriorityP1Override;
  UINT8     MemWPriorityP1;
  UINT8     MemBdwOvflowP1;
  UINT8     MemIEcc;
  UINT8     StateAfterG3;
  UINT8     PrimaryDisplay;
  UINT8     DtbMenuEntry;
  UINT8     BiosReset;
  UINT8     SocWatchdogTimer;
  UINT16    VddSocVoltage;
  UINT16    VddGpuVoltage;
  UINT16    VddDpuVoltage;
  UINT16    VddCpuBigCore0Voltage;
  UINT16    VddCpuBigCore1Voltage;
  UINT16    VddCpuMidCore0Voltage;
  UINT16    VddCpuMidCore1Voltage;
  UINT16    VddCpuLitCoreVoltage;
  UINT8     GfxPower;
  UINT8     TouchPanelPower;
  UINT8     TpmPower;
  UINT8     WwanPower;
  UINT8     PcieX2SlotPower;
  UINT8     FingerPrintPower;
  UINT8     WlanPower;
  UINT8     M2SsdPower;
  UINT8     OnBoardLanPower;
  UINT8     IspCamera0Power;
  UINT8     IspCamera1Power;
  UINT8     IspCamera2Power;
  UINT8     IspCamera3Power;
  UINT8     CpuCoreClkGating;
  UINT8     DsuClkGating;
  UINT8     GicdClkGating;
  UINT8     Ci700ClkGating;
  UINT8     SysNi700ClkGating;
  UINT8     MmNi700ClkGating;
  UINT8     PcieNi700ClkGating;
  UINT8     SmnNi700ClkGating;
  UINT8     GpuClkGating;
  UINT8     Dpu0ClkGating;
  UINT8     Dpu1ClkGating;
  UINT8     Dpu2ClkGating;
  UINT8     Dpu3ClkGating;
  UINT8     Dpu4ClkGating;
  UINT8     VpuClkGating;
  UINT8     CpuCoreNum;
  UINT8     CpuCoreEnable[MAX_CPU_CORE_NUM];
  UINT32    CpuShareInfo;
  UINT8     RtcWakeup;
  UINT8     LightSensorCtrl;
  UINT8     CpuLpiState;
  UINT8     SpcrEnable;
  UINT8     Reserved0;
  UINT8     EcFanMode;

  //
  // Power Management OPP Table Configuration
  //
  UINT8     PmConfigEnable;              // Reserved (always enabled), kept for layout compatibility

  //
  // Per-OPP-entry frequency (MHz) and voltage (mV) for all 12 DVFS domains.
  // Flat array layout: index = domain * 13 + entry. 0 = use stock value.
  // Domain indices match DVFS_ELEMENT_IDX_* (0=GPU_CORE..11=MMHUB).
  // Max 13 entries per domain (DOMAIN_MAX_OPP_ENTRIES).
  // Total: 12 domains * 13 entries = 156 elements.
  //
  UINT16    PmOppFreq[156];              // MHz, 0=stock
  UINT16    PmOppVolt[156];              // mV, 0=stock

  //
  // Per-rail TDP power cap (mW, 0=stock)
  // Indices match DPM_EDP_* (0=CPU_LIT..7=SOC)
  //
  UINT16    PmTdp[8];

  //
  // Reserved (formerly SPT thermal controls, unused by firmware)
  //
  UINT8     PmSptFastPowerRsvd;          // Reserved for struct layout compat
  UINT8     PmSptSlowPowerRsvd;          // Reserved for struct layout compat

  //
  // SoC voltage offset in mV, applied to the fixed SoC rail via delta_mV.
  // Range: 0 to 500. Polarity controls sign: 0 = positive, 1 = negative.
  //
  UINT8     PmSocVoltagePolarity;          // 0 = positive (+), 1 = negative (-)
  UINT16    PmSocVoltageOffset;

  //
  // eDP panel support: 0 = disabled (default), 1 = enabled
  //
  UINT8     EdpSupport;
} PLATFORM_SETUP_DATA;

typedef struct {
  UINT8    SystemTableSelect;
} SYSTEM_TABLE;

#pragma pack()

#endif
