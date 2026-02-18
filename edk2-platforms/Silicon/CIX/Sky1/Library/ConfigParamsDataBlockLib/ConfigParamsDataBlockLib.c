/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "ConfigParamsDataBlockInternal.h"

CONFIG_PARAMS_DATA_BLOCK  mConfigParamsDataBlock = {
  {
    // CONFIG_PARAMS_CPU
    FixedPcdGet8 (PcdAcpiCpuLpiState),
  },
  {
    // CONFIG_PARAMS_DSU
  },
  {
    // CONFIG_PARAMS_GIC
  },
  {
    // CONFIG_PARAMS_MMU
  },
  {
    // CONFIG_PARAMS_DDRC
  },
  {
    // CONFIG_PARAMS_PCIE
    {
      FixedPcdGetBool (PcdPcieRootPort0Enable),
      FixedPcdGetBool (PcdPcieRootPort1Enable),
      FixedPcdGetBool (PcdPcieRootPort2Enable),
      FixedPcdGetBool (PcdPcieRootPort3Enable),
      FixedPcdGetBool (PcdPcieRootPort4Enable),
    },
    {
      FixedPcdGet8 (PcdPcieRootPort0LaneNum),
      FixedPcdGet8 (PcdPcieRootPort1LaneNum),
      FixedPcdGet8 (PcdPcieRootPort2LaneNum),
      FixedPcdGet8 (PcdPcieRootPort3LaneNum),
      FixedPcdGet8 (PcdPcieRootPort4LaneNum),
    },
    {
      FixedPcdGet8 (PcdPcieRootPort0MaxSpeed),
      FixedPcdGet8 (PcdPcieRootPort1MaxSpeed),
      FixedPcdGet8 (PcdPcieRootPort2MaxSpeed),
      FixedPcdGet8 (PcdPcieRootPort3MaxSpeed),
      FixedPcdGet8 (PcdPcieRootPort4MaxSpeed),
    },
    {
      FixedPcdGet8 (PcdPcieRootPort0SpeedGen),
      FixedPcdGet8 (PcdPcieRootPort1SpeedGen),
      FixedPcdGet8 (PcdPcieRootPort2SpeedGen),
      FixedPcdGet8 (PcdPcieRootPort3SpeedGen),
      FixedPcdGet8 (PcdPcieRootPort4SpeedGen),
    },
    {
      FixedPcdGet8 (PcdPcieRootPort0MaxAspmSupport),
      FixedPcdGet8 (PcdPcieRootPort1MaxAspmSupport),
      FixedPcdGet8 (PcdPcieRootPort2MaxAspmSupport),
      FixedPcdGet8 (PcdPcieRootPort3MaxAspmSupport),
      FixedPcdGet8 (PcdPcieRootPort4MaxAspmSupport),
    },
    {
      FixedPcdGet8 (PcdPcieRootPort0AspmSupport),
      FixedPcdGet8 (PcdPcieRootPort1AspmSupport),
      FixedPcdGet8 (PcdPcieRootPort2AspmSupport),
      FixedPcdGet8 (PcdPcieRootPort3AspmSupport),
      FixedPcdGet8 (PcdPcieRootPort4AspmSupport),
    },
    {
      FixedPcdGet8 (PcdPcieRootPort0MaxPayload),
      FixedPcdGet8 (PcdPcieRootPort1MaxPayload),
      FixedPcdGet8 (PcdPcieRootPort2MaxPayload),
      FixedPcdGet8 (PcdPcieRootPort3MaxPayload),
      FixedPcdGet8 (PcdPcieRootPort4MaxPayload),
    },
    {
      FixedPcdGet8 (PcdPcieRootPort0MaxReadRequest),
      FixedPcdGet8 (PcdPcieRootPort1MaxReadRequest),
      FixedPcdGet8 (PcdPcieRootPort2MaxReadRequest),
      FixedPcdGet8 (PcdPcieRootPort3MaxReadRequest),
      FixedPcdGet8 (PcdPcieRootPort4MaxReadRequest),
    },
    {
      FixedPcdGet8 (PcdPcieRootPort0L1SubStates),
      FixedPcdGet8 (PcdPcieRootPort1L1SubStates),
      FixedPcdGet8 (PcdPcieRootPort2L1SubStates),
      FixedPcdGet8 (PcdPcieRootPort3L1SubStates),
      FixedPcdGet8 (PcdPcieRootPort4L1SubStates),
    },
    {
      0,
      0,
      0,
      0,
      0,
    },
    {
      FixedPcdGetBool (PcdPcieRootPort0DevicePower),
      FixedPcdGetBool (PcdPcieRootPort1DevicePower),
      FixedPcdGetBool (PcdPcieRootPort2DevicePower),
      FixedPcdGetBool (PcdPcieRootPort3DevicePower),
      FixedPcdGetBool (PcdPcieRootPort4DevicePower),
    },
    {
      FixedPcdGet8 (PcdPcieRootPort0PeResetPin),
      FixedPcdGet8 (PcdPcieRootPort1PeResetPin),
      FixedPcdGet8 (PcdPcieRootPort2PeResetPin),
      FixedPcdGet8 (PcdPcieRootPort3PeResetPin),
      FixedPcdGet8 (PcdPcieRootPort4PeResetPin),
    },
    {
      FixedPcdGet8 (PcdPcieRootPort0DtiEnable),
      FixedPcdGet8 (PcdPcieRootPort1DtiEnable),
      FixedPcdGet8 (PcdPcieRootPort2DtiEnable),
      FixedPcdGet8 (PcdPcieRootPort3DtiEnable),
      FixedPcdGet8 (PcdPcieRootPort4DtiEnable),
    },
  },
  {
    // CONFIG_PARAMS_DPU
    {
      FixedPcdGetBool (PcdDP0Enable),
      FixedPcdGetBool (PcdDP1Enable),
      FixedPcdGetBool (PcdDP2Enable),
      FixedPcdGetBool (PcdDP3Enable),
      FixedPcdGetBool (PcdDP4Enable),
    },
    {
      FixedPcdGet8 (PcdDPPriority0),
      FixedPcdGet8 (PcdDPPriority1),
      FixedPcdGet8 (PcdDPPriority2),
      FixedPcdGet8 (PcdDPPriority3),
      FixedPcdGet8 (PcdDPPriority4),
    },
  },
  {
    // CONFIG_PARAMS_NPU
  },
  {
    // CONFIG_PARAMS_VPU
  },
  {
    // CONFIG_PARAMS_ISP
  },
  {
    // CONFIG_PARAMS_CSI
  },
  {
    // CONFIG_PARAMS_USB_20
    {
      FixedPcdGetBool (PcdUsb2Control0Enable),
      FixedPcdGet8 (PcdUsb2Control0MaxSpeed),
    },
    {
      FixedPcdGetBool (PcdUsb2Control1Enable),
      FixedPcdGet8 (PcdUsb2Control1MaxSpeed),
    },
    {
      FixedPcdGetBool (PcdUsb2Control2Enable),
      FixedPcdGet8 (PcdUsb2Control2MaxSpeed),
    },
    {
      FixedPcdGetBool (PcdUsb2Control3Enable),
      FixedPcdGet8 (PcdUsb2Control3MaxSpeed),
    },
  },
  {
    // CONFIG_PARAMS_USB_32_DRD
    {
      FixedPcdGetBool (PcdUsb3Control0Enable),
      FixedPcdGet8 (PcdUsb3Control0MaxSpeed),
      FixedPcdGetBool (PcdUsb3Control0DataRole),
    },
    {
      FixedPcdGetBool (PcdUsb3Control1Enable),
      FixedPcdGet8 (PcdUsb3Control1MaxSpeed),
      FixedPcdGetBool (PcdUsb3Control1DataRole),
    },
  },
  {
    // CONFIG_PARAMS_USB_TYPEC_DRD
    {
      FixedPcdGetBool (PcdUsbCDrdControl0Enable),
      FixedPcdGet8 (PcdUsbCDrdControl0MaxSpeed),
      FixedPcdGetBool (PcdUsbCDrdControl0DataRole),
    },
  },
  {
    // CONFIG_PARAMS_USB_TYPE_C
    {
      FixedPcdGetBool (PcdUsbCControl0Enable),
      FixedPcdGet8 (PcdUsbCControl0MaxSpeed),
    },
    {
      FixedPcdGetBool (PcdUsbCControl1Enable),
      FixedPcdGet8 (PcdUsbCControl1MaxSpeed),
    },
    {
      FixedPcdGetBool (PcdUsbCControl2Enable),
      FixedPcdGet8 (PcdUsbCControl2MaxSpeed),
    },
  },
  {
    // CONFIG_PARAMS_GMAC
    {
      FixedPcdGetBool (PcdGmac0Enable),         // Enable
      FixedPcdGet32 (PcdGmacApbClockMHz),       // ApbClockMHz
      FixedPcdGet32 (PcdGmac0ClockMHz),         // ClockMHz
      FixedPcdGet8 (PcdGmac0QueueEnMap),        // QueueEnMap
      FixedPcdGet8 (PcdGmac0Queue0SegmentNum),  // Queue0SegmentNum
      FixedPcdGet8 (PcdGmac0Queue1SegmentNum),  // Queue1SegmentNum
      FixedPcdGet8 (PcdGmac0Queue2SegmentNum),  // Queue2SegmentNum
      FixedPcdGet8 (PcdGmac0Queue3SegmentNum),  // Queue3SegmentNum
      FixedPcdGet8 (PcdGmac0Queue4SegmentNum),  // Queue4SegmentNum
      FixedPcdGet8 (PcdGmac0Queue5SegmentNum),  // Queue5SegmentNum
      FixedPcdGet8 (PcdGmac0Queue6SegmentNum),  // Queue6SegmentNum
      FixedPcdGet8 (PcdGmac0Queue7SegmentNum),  // Queue7SegmentNum
      FixedPcdGetBool (PcdGmac0DuplexEn),       // DuplexEn
      FixedPcdGetBool (PcdGmac0RxPauseEn),      // RxPauseEn
      FixedPcdGetBool (PcdGmac0IntEn),          // IntEn
      FixedPcdGet64 (PcdGmac0MacAddress)        // MacAddress
    },
    {
      FixedPcdGetBool (PcdGmac1Enable),         // Enable
      FixedPcdGet32 (PcdGmacApbClockMHz),       // ApbClockMHz
      FixedPcdGet32 (PcdGmac1ClockMHz),         // ClockMHz
      FixedPcdGet8 (PcdGmac1QueueEnMap),        // QueueEnMap
      FixedPcdGet8 (PcdGmac1Queue0SegmentNum),  // Queue0SegmentNum
      FixedPcdGet8 (PcdGmac1Queue1SegmentNum),  // Queue1SegmentNum
      FixedPcdGet8 (PcdGmac1Queue2SegmentNum),  // Queue2SegmentNum
      FixedPcdGet8 (PcdGmac1Queue3SegmentNum),  // Queue3SegmentNum
      FixedPcdGet8 (PcdGmac1Queue4SegmentNum),  // Queue4SegmentNum
      FixedPcdGet8 (PcdGmac1Queue5SegmentNum),  // Queue5SegmentNum
      FixedPcdGet8 (PcdGmac1Queue6SegmentNum),  // Queue6SegmentNum
      FixedPcdGet8 (PcdGmac1Queue7SegmentNum),  // Queue7SegmentNum
      FixedPcdGetBool (PcdGmac1DuplexEn),       // DuplexEn
      FixedPcdGetBool (PcdGmac1RxPauseEn),      // RxPauseEn
      FixedPcdGetBool (PcdGmac1IntEn),          // IntEn
      FixedPcdGet64 (PcdGmac1MacAddress)        // MacAddress
    },
  },
  {
    // CONFIG_PARAMS_SENSOR_FUSION
  },
  {
    // CONFIG_PARAMS_AUDIO
    FixedPcdGetBool (PcdAudioHdaEn),        // HdaEn
  },
  {
    // CONFIG_PARAMS_LSIO_FCH
    {
      // CONFIG_PARAMS_LSIO_FCH_I2C
      {
        FixedPcdGetBool (PcdI2c0En),       // Enable
        FixedPcdGet32 (PcdI2c0BusFreq)     // BusFreq
      },
      {
        FixedPcdGetBool (PcdI2c1En),       // Enable
        FixedPcdGet32 (PcdI2c1BusFreq)     // BusFreq
      },
      {
        FixedPcdGetBool (PcdI2c2En),       // Enable
        FixedPcdGet32 (PcdI2c2BusFreq)     // BusFreq
      },
      {
        FixedPcdGetBool (PcdI2c3En),       // Enable
        FixedPcdGet32 (PcdI2c3BusFreq)     // BusFreq
      },
      {
        FixedPcdGetBool (PcdI2c4En),       // Enable
        FixedPcdGet32 (PcdI2c4BusFreq)     // BusFreq
      },
      {
        FixedPcdGetBool (PcdI2c5En),       // Enable
        FixedPcdGet32 (PcdI2c5BusFreq)     // BusFreq
      },
      {
        FixedPcdGetBool (PcdI2c6En),       // Enable
        FixedPcdGet32 (PcdI2c6BusFreq)     // BusFreq
      },
      {
        FixedPcdGetBool (PcdI2c7En),       // Enable
        FixedPcdGet32 (PcdI2c7BusFreq)     // BusFreq
      },
    },
    {
      // CONFIG_PARAMS_LSIO_FCH_SPI
      {
        FixedPcdGetBool (PcdSpi0En),       // Enable
        FixedPcdGet32 (PcdSpi0BusFreqMHz), // BusFreq
        FixedPcdGet8 (PcdSpi0ChipSelect),  // ChipSelect
        FixedPcdGet8 (PcdSpi0Mode)         // Mode
      },
      {
        FixedPcdGetBool (PcdSpi1En),       // Enable
        FixedPcdGet32 (PcdSpi1BusFreqMHz), // BusFreq
        FixedPcdGet8 (PcdSpi1ChipSelect),  // ChipSelect
        FixedPcdGet8 (PcdSpi1Mode)         // Mode
      },
    }
  },
  {
    // CONFIG_PARAMS_S5_SS
    FixedPcdGet8 (PcdSocWatchdogTimer),
  },
  {
    // CONFIG_PARAMS_MISC
  }
};

CONFIG_PARAMS_DATA_ENTRY  mConfigDataEntryTable[] = {
  { PARAMS_DATA_GMAC0_ENABLE_ID,               PARAMS_DATA_OFFSET_SIZE (Gmac[0].Enable),              PARAMS_DATA_BOOLEAN_TYPE,      L"GMAC0 enable",                 L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_GMAC0_APBCLOCKHZ_ID,           PARAMS_DATA_OFFSET_SIZE (Gmac[0].ApbClockMHz),         PARAMS_DATA_INTEGER_TYPE,      L"GMAC0 APB clock(MHz)",         L"(dec)"                                                                           },
  { PARAMS_DATA_GMAC0_CLOCKMHZ_ID,             PARAMS_DATA_OFFSET_SIZE (Gmac[0].ClockMHz),            PARAMS_DATA_MULTI_OPTION_TYPE, L"GMAC0 clock",                  L"10:10MHz, 100:100MHz, 1000:1000MHz"                                              },
  { PARAMS_DATA_GMAC0_QUEUEENMAP_ID,           PARAMS_DATA_OFFSET_SIZE (Gmac[0].QueueEnMap),          PARAMS_DATA_INTEGER_TYPE,      L"GMAC0 queue enable bitmap",    L"(hex)"                                                                           },
  { PARAMS_DATA_GMAC0_QUEUE0SEGMENTNUM_ID,     PARAMS_DATA_OFFSET_SIZE (Gmac[0].Queue0SegmentNum),    PARAMS_DATA_INTEGER_TYPE,      L"GMAC0 queue 0 segment number", L"(hex)"                                                                           },
  { PARAMS_DATA_GMAC0_QUEUE1SEGMENTNUM_ID,     PARAMS_DATA_OFFSET_SIZE (Gmac[0].Queue1SegmentNum),    PARAMS_DATA_INTEGER_TYPE,      L"GMAC0 queue 1 segment number", L"(hex)"                                                                           },
  { PARAMS_DATA_GMAC0_QUEUE2SEGMENTNUM_ID,     PARAMS_DATA_OFFSET_SIZE (Gmac[0].Queue2SegmentNum),    PARAMS_DATA_INTEGER_TYPE,      L"GMAC0 queue 2 segment number", L"(hex)"                                                                           },
  { PARAMS_DATA_GMAC0_QUEUE3SEGMENTNUM_ID,     PARAMS_DATA_OFFSET_SIZE (Gmac[0].Queue3SegmentNum),    PARAMS_DATA_INTEGER_TYPE,      L"GMAC0 queue 3 segment number", L"(hex)"                                                                           },
  { PARAMS_DATA_GMAC0_QUEUE4SEGMENTNUM_ID,     PARAMS_DATA_OFFSET_SIZE (Gmac[0].Queue4SegmentNum),    PARAMS_DATA_INTEGER_TYPE,      L"GMAC0 queue 4 segment number", L"(hex)"                                                                           },
  { PARAMS_DATA_GMAC0_QUEUE5SEGMENTNUM_ID,     PARAMS_DATA_OFFSET_SIZE (Gmac[0].Queue5SegmentNum),    PARAMS_DATA_INTEGER_TYPE,      L"GMAC0 queue 5 segment number", L"(hex)"                                                                           },
  { PARAMS_DATA_GMAC0_QUEUE6SEGMENTNUM_ID,     PARAMS_DATA_OFFSET_SIZE (Gmac[0].Queue6SegmentNum),    PARAMS_DATA_INTEGER_TYPE,      L"GMAC0 queue 6 segment number", L"(hex)"                                                                           },
  { PARAMS_DATA_GMAC0_QUEUE7SEGMENTNUM_ID,     PARAMS_DATA_OFFSET_SIZE (Gmac[0].Queue7SegmentNum),    PARAMS_DATA_INTEGER_TYPE,      L"GMAC0 queue 7 segment number", L"(hex)"                                                                           },
  { PARAMS_DATA_GMAC0_DUPLEXEN_ID,             PARAMS_DATA_OFFSET_SIZE (Gmac[0].DuplexEn),            PARAMS_DATA_BOOLEAN_TYPE,      L"GMAC0 duplex enable",          L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_GMAC0_RXPAUSEEN_ID,            PARAMS_DATA_OFFSET_SIZE (Gmac[0].RxPauseEn),           PARAMS_DATA_BOOLEAN_TYPE,      L"GMAC0 RX pause enable",        L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_GMAC0_INTEN_ID,                PARAMS_DATA_OFFSET_SIZE (Gmac[0].IntEn),               PARAMS_DATA_BOOLEAN_TYPE,      L"GMAC0 interrupt enable",       L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_GMAC0_MACADDRESS_ID,           PARAMS_DATA_OFFSET_SIZE (Gmac[0].MacAddress),          PARAMS_DATA_INTEGER_TYPE,      L"GMAC0 MAC address",            L"(hex)"                                                                           },
  { PARAMS_DATA_GMAC1_ENABLE_ID,               PARAMS_DATA_OFFSET_SIZE (Gmac[1].Enable),              PARAMS_DATA_BOOLEAN_TYPE,      L"GMAC1 enable",                 L"0:disable,1:enable"                                                              },
  { PARAMS_DATA_GMAC1_APBCLOCKHZ_ID,           PARAMS_DATA_OFFSET_SIZE (Gmac[1].ApbClockMHz),         PARAMS_DATA_INTEGER_TYPE,      L"GMAC1 APB clock(MHz)",         L"(dec)"                                                                           },
  { PARAMS_DATA_GMAC1_CLOCKMHZ_ID,             PARAMS_DATA_OFFSET_SIZE (Gmac[1].ClockMHz),            PARAMS_DATA_MULTI_OPTION_TYPE, L"GMAC1 clock",                  L"10:10MHz, 100:100MHz, 1000:1000MHz"                                              },
  { PARAMS_DATA_GMAC1_QUEUEENMAP_ID,           PARAMS_DATA_OFFSET_SIZE (Gmac[1].QueueEnMap),          PARAMS_DATA_INTEGER_TYPE,      L"GMAC1 queue enable bitmap",    L"(hex)"                                                                           },
  { PARAMS_DATA_GMAC1_QUEUE0SEGMENTNUM_ID,     PARAMS_DATA_OFFSET_SIZE (Gmac[1].Queue0SegmentNum),    PARAMS_DATA_INTEGER_TYPE,      L"GMAC1 queue 0 segment number", L"(hex)"                                                                           },
  { PARAMS_DATA_GMAC1_QUEUE1SEGMENTNUM_ID,     PARAMS_DATA_OFFSET_SIZE (Gmac[1].Queue1SegmentNum),    PARAMS_DATA_INTEGER_TYPE,      L"GMAC1 queue 1 segment number", L"(hex)"                                                                           },
  { PARAMS_DATA_GMAC1_QUEUE2SEGMENTNUM_ID,     PARAMS_DATA_OFFSET_SIZE (Gmac[1].Queue2SegmentNum),    PARAMS_DATA_INTEGER_TYPE,      L"GMAC1 queue 2 segment number", L"(hex)"                                                                           },
  { PARAMS_DATA_GMAC1_QUEUE3SEGMENTNUM_ID,     PARAMS_DATA_OFFSET_SIZE (Gmac[1].Queue3SegmentNum),    PARAMS_DATA_INTEGER_TYPE,      L"GMAC1 queue 3 segment number", L"(hex)"                                                                           },
  { PARAMS_DATA_GMAC1_QUEUE4SEGMENTNUM_ID,     PARAMS_DATA_OFFSET_SIZE (Gmac[1].Queue4SegmentNum),    PARAMS_DATA_INTEGER_TYPE,      L"GMAC1 queue 4 segment number", L"(hex)"                                                                           },
  { PARAMS_DATA_GMAC1_QUEUE5SEGMENTNUM_ID,     PARAMS_DATA_OFFSET_SIZE (Gmac[1].Queue5SegmentNum),    PARAMS_DATA_INTEGER_TYPE,      L"GMAC1 queue 5 segment number", L"(hex)"                                                                           },
  { PARAMS_DATA_GMAC1_QUEUE6SEGMENTNUM_ID,     PARAMS_DATA_OFFSET_SIZE (Gmac[1].Queue6SegmentNum),    PARAMS_DATA_INTEGER_TYPE,      L"GMAC1 queue 6 segment number", L"(hex)"                                                                           },
  { PARAMS_DATA_GMAC1_QUEUE7SEGMENTNUM_ID,     PARAMS_DATA_OFFSET_SIZE (Gmac[1].Queue7SegmentNum),    PARAMS_DATA_INTEGER_TYPE,      L"GMAC1 queue 7 segment number", L"(hex)"                                                                           },
  { PARAMS_DATA_GMAC1_DUPLEXEN_ID,             PARAMS_DATA_OFFSET_SIZE (Gmac[1].DuplexEn),            PARAMS_DATA_BOOLEAN_TYPE,      L"GMAC1 duplex enable",          L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_GMAC1_RXPAUSEEN_ID,            PARAMS_DATA_OFFSET_SIZE (Gmac[1].RxPauseEn),           PARAMS_DATA_BOOLEAN_TYPE,      L"GMAC1 RX pause enable",        L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_GMAC1_INTEN_ID,                PARAMS_DATA_OFFSET_SIZE (Gmac[1].IntEn),               PARAMS_DATA_BOOLEAN_TYPE,      L"GMAC1 interrupt enable",       L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_GMAC1_MACADDRESS_ID,           PARAMS_DATA_OFFSET_SIZE (Gmac[1].MacAddress),          PARAMS_DATA_INTEGER_TYPE,      L"GMAC1 MAC address",            L"(hex)"                                                                           },
  { PARAMS_DATA_AUDIO_HDAEN_ID,                PARAMS_DATA_OFFSET_SIZE (Audio.HdaEn),                 PARAMS_DATA_BOOLEAN_TYPE,      L"audio HDA enable",             L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_LSIO_FCH_I2C0_ENABLE_ID,       PARAMS_DATA_OFFSET_SIZE (Fch.I2c[0].Enable),           PARAMS_DATA_BOOLEAN_TYPE,      L"FCH I2C0 enable",              L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_LSIO_FCH_I2C0_BUSFREQ_ID,      PARAMS_DATA_OFFSET_SIZE (Fch.I2c[0].BusFreq),          PARAMS_DATA_MULTI_OPTION_TYPE, L"FCH I2C0 bus frequency",       L"100000:100KHz, 400000:400KHz, 1000000:1MHz"                                      },
  { PARAMS_DATA_LSIO_FCH_I2C1_ENABLE_ID,       PARAMS_DATA_OFFSET_SIZE (Fch.I2c[1].Enable),           PARAMS_DATA_BOOLEAN_TYPE,      L"FCH I2C1 enable",              L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_LSIO_FCH_I2C1_BUSFREQ_ID,      PARAMS_DATA_OFFSET_SIZE (Fch.I2c[1].BusFreq),          PARAMS_DATA_MULTI_OPTION_TYPE, L"FCH I2C1 bus frequency",       L"100000:100KHz, 400000:400KHz, 1000000:1MHz"                                      },
  { PARAMS_DATA_LSIO_FCH_I2C2_ENABLE_ID,       PARAMS_DATA_OFFSET_SIZE (Fch.I2c[2].Enable),           PARAMS_DATA_BOOLEAN_TYPE,      L"FCH I2C2 enable",              L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_LSIO_FCH_I2C2_BUSFREQ_ID,      PARAMS_DATA_OFFSET_SIZE (Fch.I2c[2].BusFreq),          PARAMS_DATA_MULTI_OPTION_TYPE, L"FCH I2C2 bus frequency",       L"100000:100KHz, 400000:400KHz, 1000000:1MHz"                                      },
  { PARAMS_DATA_LSIO_FCH_I2C3_ENABLE_ID,       PARAMS_DATA_OFFSET_SIZE (Fch.I2c[3].Enable),           PARAMS_DATA_BOOLEAN_TYPE,      L"FCH I2C3 enable",              L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_LSIO_FCH_I2C3_BUSFREQ_ID,      PARAMS_DATA_OFFSET_SIZE (Fch.I2c[3].BusFreq),          PARAMS_DATA_MULTI_OPTION_TYPE, L"FCH I2C3 bus frequency",       L"100000:100KHz, 400000:400KHz, 1000000:1MHz"                                      },
  { PARAMS_DATA_LSIO_FCH_I2C4_ENABLE_ID,       PARAMS_DATA_OFFSET_SIZE (Fch.I2c[4].Enable),           PARAMS_DATA_BOOLEAN_TYPE,      L"FCH I2C4 enable",              L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_LSIO_FCH_I2C4_BUSFREQ_ID,      PARAMS_DATA_OFFSET_SIZE (Fch.I2c[4].BusFreq),          PARAMS_DATA_MULTI_OPTION_TYPE, L"FCH I2C4 bus frequency",       L"100000:100KHz, 400000:400KHz, 1000000:1MHz"                                      },
  { PARAMS_DATA_LSIO_FCH_I2C5_ENABLE_ID,       PARAMS_DATA_OFFSET_SIZE (Fch.I2c[5].Enable),           PARAMS_DATA_BOOLEAN_TYPE,      L"FCH I2C5 enable",              L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_LSIO_FCH_I2C5_BUSFREQ_ID,      PARAMS_DATA_OFFSET_SIZE (Fch.I2c[5].BusFreq),          PARAMS_DATA_MULTI_OPTION_TYPE, L"FCH I2C5 bus frequency",       L"100000:100KHz, 400000:400KHz, 1000000:1MHz"                                      },
  { PARAMS_DATA_LSIO_FCH_I2C6_ENABLE_ID,       PARAMS_DATA_OFFSET_SIZE (Fch.I2c[6].Enable),           PARAMS_DATA_BOOLEAN_TYPE,      L"FCH I2C6 enable",              L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_LSIO_FCH_I2C6_BUSFREQ_ID,      PARAMS_DATA_OFFSET_SIZE (Fch.I2c[6].BusFreq),          PARAMS_DATA_MULTI_OPTION_TYPE, L"FCH I2C6 bus frequency",       L"100000:100KHz, 400000:400KHz, 1000000:1MHz"                                      },
  { PARAMS_DATA_LSIO_FCH_I2C7_ENABLE_ID,       PARAMS_DATA_OFFSET_SIZE (Fch.I2c[7].Enable),           PARAMS_DATA_BOOLEAN_TYPE,      L"FCH I2C7 enable",              L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_LSIO_FCH_I2C7_BUSFREQ_ID,      PARAMS_DATA_OFFSET_SIZE (Fch.I2c[7].BusFreq),          PARAMS_DATA_MULTI_OPTION_TYPE, L"FCH I2C7 bus frequency",       L"100000:100KHz, 400000:400KHz, 1000000:1MHz"                                      },
  { PARAMS_DATA_LSIO_FCH_SPI0_ENABLE_ID,       PARAMS_DATA_OFFSET_SIZE (Fch.Spi[0].Enable),           PARAMS_DATA_BOOLEAN_TYPE,      L"FCH SPI0 enable",              L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_LSIO_FCH_SPI0_BUSFREQ_ID,      PARAMS_DATA_OFFSET_SIZE (Fch.Spi[0].BusFreq),          PARAMS_DATA_MULTI_OPTION_TYPE, L"FCH SPI0 bus frequency(MHz)",  L"5:5MHz, 10:10MHz, 20:20MHz, 25:25MHz"                                            },
  { PARAMS_DATA_LSIO_FCH_SPI0_CS_ID,           PARAMS_DATA_OFFSET_SIZE (Fch.Spi[0].ChipSelect),       PARAMS_DATA_MULTI_OPTION_TYPE, L"FCH SPI0 chip select",         L"1:chip 0 selected, 2:chip 1 selected, 15:no_chip selected"                       },
  { PARAMS_DATA_LSIO_FCH_SPI0_MODE_ID,         PARAMS_DATA_OFFSET_SIZE (Fch.Spi[0].Mode),             PARAMS_DATA_MULTI_OPTION_TYPE, L"FCH SPI0 mode",                L"0:CPOL-0 CPHA-0, 1:CPOL-0 CPHA-1, 2:CPOL-1 CPHA-0, 3:CPOL-1 CPHA-1"              },
  { PARAMS_DATA_LSIO_FCH_SPI1_ENABLE_ID,       PARAMS_DATA_OFFSET_SIZE (Fch.Spi[1].Enable),           PARAMS_DATA_BOOLEAN_TYPE,      L"FCH SPI1 enable",              L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_LSIO_FCH_SPI1_BUSFREQ_ID,      PARAMS_DATA_OFFSET_SIZE (Fch.Spi[1].BusFreq),          PARAMS_DATA_MULTI_OPTION_TYPE, L"FCH SPI1 bus frequency(MHz)",  L"5:5MHz, 10:10MHz, 20:20MHz, 25:25MHz"                                            },
  { PARAMS_DATA_LSIO_FCH_SPI1_CS_ID,           PARAMS_DATA_OFFSET_SIZE (Fch.Spi[1].ChipSelect),       PARAMS_DATA_MULTI_OPTION_TYPE, L"FCH SPI1 chip select",         L"1:chip 0 selected, 2:chip 1 selected, 15:no_chip selected"                       },
  { PARAMS_DATA_LSIO_FCH_SPI1_MODE_ID,         PARAMS_DATA_OFFSET_SIZE (Fch.Spi[1].Mode),             PARAMS_DATA_MULTI_OPTION_TYPE, L"FCH SPI1 mode",                L"0:CPOL-0 CPHA-0, 1:CPOL-0 CPHA-1, 2:CPOL-1 CPHA-0, 3:CPOL-1 CPHA-1"              },
  { PARAMS_DATA_PCIE_RP0_ID,                   PARAMS_DATA_OFFSET_SIZE (Pcie.PcieRpEnable[0]),        PARAMS_DATA_BOOLEAN_TYPE,      L"PCI Express Root Port 1",      L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_PCIE_RP1_ID,                   PARAMS_DATA_OFFSET_SIZE (Pcie.PcieRpEnable[1]),        PARAMS_DATA_BOOLEAN_TYPE,      L"PCI Express Root Port 2",      L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_PCIE_RP2_ID,                   PARAMS_DATA_OFFSET_SIZE (Pcie.PcieRpEnable[2]),        PARAMS_DATA_BOOLEAN_TYPE,      L"PCI Express Root Port 3",      L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_PCIE_RP3_ID,                   PARAMS_DATA_OFFSET_SIZE (Pcie.PcieRpEnable[3]),        PARAMS_DATA_BOOLEAN_TYPE,      L"PCI Express Root Port 4",      L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_PCIE_RP4_ID,                   PARAMS_DATA_OFFSET_SIZE (Pcie.PcieRpEnable[4]),        PARAMS_DATA_BOOLEAN_TYPE,      L"PCI Express Root Port 5",      L"0:disable, 1:enable"                                                             },
  { PARAMS_DATA_PCIE_RP0_WIDTH_ID,             PARAMS_DATA_OFFSET_SIZE (Pcie.PcieWidth[0]),           PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port0 Bandwidth",         L"0:x1, 1:x2, 2:x4, 3:x8"                                                          },
  { PARAMS_DATA_PCIE_RP1_WIDTH_ID,             PARAMS_DATA_OFFSET_SIZE (Pcie.PcieWidth[1]),           PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port1 Bandwidth",         L"0:x1, 1:x2, 2:x4"                                                                },
  { PARAMS_DATA_PCIE_RP2_WIDTH_ID,             PARAMS_DATA_OFFSET_SIZE (Pcie.PcieWidth[2]),           PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port2 Bandwidth",         L"0:x1, 1:x2"                                                                      },
  { PARAMS_DATA_PCIE_RP3_WIDTH_ID,             PARAMS_DATA_OFFSET_SIZE (Pcie.PcieWidth[3]),           PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port3 Bandwidth",         L"0:x1"                                                                            },
  { PARAMS_DATA_PCIE_RP4_WIDTH_ID,             PARAMS_DATA_OFFSET_SIZE (Pcie.PcieWidth[4]),           PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port4 Bandwidth",         L"0:x1"                                                                            },
  { PARAMS_DATA_PCIE_RP0_MAX_SPEED_ID,         PARAMS_DATA_OFFSET_SIZE (Pcie.PcieMaxSpeed[0]),        PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port0 Max Speed",         L"0:GEN1, 1:GEN2, 2:GEN3, 3:GEN4"                                                  },
  { PARAMS_DATA_PCIE_RP1_MAX_SPEED_ID,         PARAMS_DATA_OFFSET_SIZE (Pcie.PcieMaxSpeed[1]),        PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port1 Max Speed",         L"0:GEN1, 1:GEN2, 2:GEN3, 3:GEN4"                                                  },
  { PARAMS_DATA_PCIE_RP2_MAX_SPEED_ID,         PARAMS_DATA_OFFSET_SIZE (Pcie.PcieMaxSpeed[2]),        PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port2 Max Speed",         L"0:GEN1, 1:GEN2, 2:GEN3, 3:GEN4"                                                  },
  { PARAMS_DATA_PCIE_RP3_MAX_SPEED_ID,         PARAMS_DATA_OFFSET_SIZE (Pcie.PcieMaxSpeed[3]),        PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port3 Max Speed",         L"0:GEN1, 1:GEN2, 2:GEN3, 3:GEN4"                                                  },
  { PARAMS_DATA_PCIE_RP4_MAX_SPEED_ID,         PARAMS_DATA_OFFSET_SIZE (Pcie.PcieMaxSpeed[4]),        PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port4 Max Speed",         L"0:GEN1, 1:GEN2, 2:GEN3, 3:GEN4"                                                  },
  { PARAMS_DATA_PCIE_RP0_TARGET_LINK_ID,       PARAMS_DATA_OFFSET_SIZE (Pcie.PcieTargetLinkSpeed[0]), PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port0 Target Link Speed", L"1:2.5 GT/s, 2:5 GT/s, 3:8 GT/s, 4:16 GT/s"                                       },
  { PARAMS_DATA_PCIE_RP1_TARGET_LINK_ID,       PARAMS_DATA_OFFSET_SIZE (Pcie.PcieTargetLinkSpeed[1]), PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port1 Target Link Speed", L"1:2.5 GT/s, 2:5 GT/s, 3:8 GT/s, 4:16 GT/s"                                       },
  { PARAMS_DATA_PCIE_RP2_TARGET_LINK_ID,       PARAMS_DATA_OFFSET_SIZE (Pcie.PcieTargetLinkSpeed[2]), PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port2 Target Link Speed", L"1:2.5 GT/s, 2:5 GT/s, 3:8 GT/s, 4:16 GT/s"                                       },
  { PARAMS_DATA_PCIE_RP3_TARGET_LINK_ID,       PARAMS_DATA_OFFSET_SIZE (Pcie.PcieTargetLinkSpeed[3]), PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port3 Target Link Speed", L"1:2.5 GT/s, 2:5 GT/s, 3:8 GT/s, 4:16 GT/s"                                       },
  { PARAMS_DATA_PCIE_RP4_TARGET_LINK_ID,       PARAMS_DATA_OFFSET_SIZE (Pcie.PcieTargetLinkSpeed[4]), PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port4 Target Link Speed", L"1:2.5 GT/s, 2:5 GT/s, 3:8 GT/s, 4:16 GT/s"                                       },
  { PARAMS_DATA_PCIE_RP0_MAX_ASPM_ID,          PARAMS_DATA_OFFSET_SIZE (Pcie.PcieAspmMaxSupport[0]),  PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port0 ASPM Max Support",  L"0:disable, 1:L0ss, 2:L1, 3:L0sL1"                                                },
  { PARAMS_DATA_PCIE_RP1_MAX_ASPM_ID,          PARAMS_DATA_OFFSET_SIZE (Pcie.PcieAspmMaxSupport[1]),  PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port1 ASPM Max Support",  L"0:disable, 1:L0ss, 2:L1, 3:L0sL1"                                                },
  { PARAMS_DATA_PCIE_RP2_MAX_ASPM_ID,          PARAMS_DATA_OFFSET_SIZE (Pcie.PcieAspmMaxSupport[2]),  PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port2 ASPM Max Support",  L"0:disable, 1:L0ss, 2:L1, 3:L0sL1"                                                },
  { PARAMS_DATA_PCIE_RP3_MAX_ASPM_ID,          PARAMS_DATA_OFFSET_SIZE (Pcie.PcieAspmMaxSupport[3]),  PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port3 ASPM Max Support",  L"0:disable, 1:L0ss, 2:L1, 3:L0sL1"                                                },
  { PARAMS_DATA_PCIE_RP4_MAX_ASPM_ID,          PARAMS_DATA_OFFSET_SIZE (Pcie.PcieAspmMaxSupport[4]),  PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port4 ASPM Max Support",  L"0:disable, 1:L0ss, 2:L1, 3:L0sL1"                                                },
  { PARAMS_DATA_PCIE_RP0_ASPM_ID,              PARAMS_DATA_OFFSET_SIZE (Pcie.PcieAspm[0]),            PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port0 ASPM",              L"0:disable, 1:L0ss, 2:L1, 3:L0sL1"                                                },
  { PARAMS_DATA_PCIE_RP1_ASPM_ID,              PARAMS_DATA_OFFSET_SIZE (Pcie.PcieAspm[1]),            PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port1 ASPM",              L"0:disable, 1:L0ss, 2:L1, 3:L0sL1"                                                },
  { PARAMS_DATA_PCIE_RP2_ASPM_ID,              PARAMS_DATA_OFFSET_SIZE (Pcie.PcieAspm[2]),            PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port2 ASPM",              L"0:disable, 1:L0ss, 2:L1, 3:L0sL1"                                                },
  { PARAMS_DATA_PCIE_RP3_ASPM_ID,              PARAMS_DATA_OFFSET_SIZE (Pcie.PcieAspm[3]),            PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port3 ASPM",              L"0:disable, 1:L0ss, 2:L1, 3:L0sL1"                                                },
  { PARAMS_DATA_PCIE_RP4_ASPM_ID,              PARAMS_DATA_OFFSET_SIZE (Pcie.PcieAspm[4]),            PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port4 ASPM",              L"0:disable, 1:L0ss, 2:L1, 3:L0sL1"                                                },
  { PARAMS_DATA_PCIE_RP0_MAX_PAYLOAD_ID,       PARAMS_DATA_OFFSET_SIZE (Pcie.PcieMaxPayload[0]),      PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port0 Max Payload",       L"0:128 Bytes, 1:256 Bytes, 2:512 Bytes, 3:1024 Bytes, 4:2048 Bytes, 5:4096 Bytes" },
  { PARAMS_DATA_PCIE_RP1_MAX_PAYLOAD_ID,       PARAMS_DATA_OFFSET_SIZE (Pcie.PcieMaxPayload[1]),      PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port1 Max Payload",       L"0:128 Bytes, 1:256 Bytes, 2:512 Bytes, 3:1024 Bytes, 4:2048 Bytes, 5:4096 Bytes" },
  { PARAMS_DATA_PCIE_RP2_MAX_PAYLOAD_ID,       PARAMS_DATA_OFFSET_SIZE (Pcie.PcieMaxPayload[2]),      PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port2 Max Payload",       L"0:128 Bytes, 1:256 Bytes, 2:512 Bytes, 3:1024 Bytes, 4:2048 Bytes, 5:4096 Bytes" },
  { PARAMS_DATA_PCIE_RP3_MAX_PAYLOAD_ID,       PARAMS_DATA_OFFSET_SIZE (Pcie.PcieMaxPayload[3]),      PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port3 Max Payload",       L"0:128 Bytes, 1:256 Bytes, 2:512 Bytes, 3:1024 Bytes, 4:2048 Bytes, 5:4096 Bytes" },
  { PARAMS_DATA_PCIE_RP4_MAX_PAYLOAD_ID,       PARAMS_DATA_OFFSET_SIZE (Pcie.PcieMaxPayload[4]),      PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port4 Max Payload",       L"0:128 Bytes, 1:256 Bytes, 2:512 Bytes, 3:1024 Bytes, 4:2048 Bytes, 5:4096 Bytes" },
  { PARAMS_DATA_PCIE_RP0_MAX_READ_REQUEST_ID,  PARAMS_DATA_OFFSET_SIZE (Pcie.PcieMaxReadRequest[0]),  PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port0 Max Read Request",  L"0:128 Bytes, 1:256 Bytes, 2:512 Bytes, 3:1024 Bytes, 4:2048 Bytes, 5:4096 Bytes" },
  { PARAMS_DATA_PCIE_RP1_MAX_READ_REQUEST_ID,  PARAMS_DATA_OFFSET_SIZE (Pcie.PcieMaxReadRequest[1]),  PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port1 Max Read Request",  L"0:128 Bytes, 1:256 Bytes, 2:512 Bytes, 3:1024 Bytes, 4:2048 Bytes, 5:4096 Bytes" },
  { PARAMS_DATA_PCIE_RP2_MAX_READ_REQUEST_ID,  PARAMS_DATA_OFFSET_SIZE (Pcie.PcieMaxReadRequest[2]),  PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port2 Max Read Request",  L"0:128 Bytes, 1:256 Bytes, 2:512 Bytes, 3:1024 Bytes, 4:2048 Bytes, 5:4096 Bytes" },
  { PARAMS_DATA_PCIE_RP3_MAX_READ_REQUEST_ID,  PARAMS_DATA_OFFSET_SIZE (Pcie.PcieMaxReadRequest[3]),  PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port3 Max Read Request",  L"0:128 Bytes, 1:256 Bytes, 2:512 Bytes, 3:1024 Bytes, 4:2048 Bytes, 5:4096 Bytes" },
  { PARAMS_DATA_PCIE_RP4_MAX_READ_REQUEST_ID,  PARAMS_DATA_OFFSET_SIZE (Pcie.PcieMaxReadRequest[4]),  PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port4 Max Read Request",  L"0:128 Bytes, 1:256 Bytes, 2:512 Bytes, 3:1024 Bytes, 4:2048 Bytes, 5:4096 Bytes" },
  { PARAMS_DATA_PCIE_RP0_LINK_STATUS_ID,       PARAMS_DATA_OFFSET_SIZE (Pcie.PcieLinkUpStatus[0]),    PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port0 Linkup Status",     L"0:FALSE, 1:TRUE"                                                                 },
  { PARAMS_DATA_PCIE_RP1_LINK_STATUS_ID,       PARAMS_DATA_OFFSET_SIZE (Pcie.PcieLinkUpStatus[1]),    PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port1 Linkup Status",     L"0:FALSE, 1:TRUE"                                                                 },
  { PARAMS_DATA_PCIE_RP2_LINK_STATUS_ID,       PARAMS_DATA_OFFSET_SIZE (Pcie.PcieLinkUpStatus[2]),    PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port2 Linkup Status",     L"0:FALSE, 1:TRUE"                                                                 },
  { PARAMS_DATA_PCIE_RP3_LINK_STATUS_ID,       PARAMS_DATA_OFFSET_SIZE (Pcie.PcieLinkUpStatus[3]),    PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port3 Linkup Status",     L"0:FALSE, 1:TRUE"                                                                 },
  { PARAMS_DATA_PCIE_RP4_LINK_STATUS_ID,       PARAMS_DATA_OFFSET_SIZE (Pcie.PcieLinkUpStatus[4]),    PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port4 Linkup Status",     L"0:FALSE, 1:TRUE"                                                                 },
  { PARAMS_DATA_PCIE_RP0_DTI_ENABLE_ID,        PARAMS_DATA_OFFSET_SIZE (Pcie.PcieDtiEnable[0]),       PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port0 DTI Enable",        L"0:Disable, 1:Enable"                                                             },
  { PARAMS_DATA_PCIE_RP1_DTI_ENABLE_ID,        PARAMS_DATA_OFFSET_SIZE (Pcie.PcieDtiEnable[1]),       PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port1 DTI Enable",        L"0:Disable, 1:Enable"                                                             },
  { PARAMS_DATA_PCIE_RP2_DTI_ENABLE_ID,        PARAMS_DATA_OFFSET_SIZE (Pcie.PcieDtiEnable[2]),       PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port2 DTI Enable",        L"0:Disable, 1:Enable"                                                             },
  { PARAMS_DATA_PCIE_RP3_DTI_ENABLE_ID,        PARAMS_DATA_OFFSET_SIZE (Pcie.PcieDtiEnable[3]),       PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port3 DTI Enable",        L"0:Disable, 1:Enable"                                                             },
  { PARAMS_DATA_PCIE_RP4_DTI_ENABLE_ID,        PARAMS_DATA_OFFSET_SIZE (Pcie.PcieDtiEnable[4]),       PARAMS_DATA_MULTI_OPTION_TYPE, L"PCIE Port4 DTI Enable",        L"0:Disable, 1:Enable"                                                             },
  { PARAMS_DATA_USB_20_0_ENABLE_ID,            PARAMS_DATA_OFFSET_SIZE (Usb20[0].Enable),             PARAMS_DATA_BOOLEAN_TYPE,      L"USB2.0 Control0 Enable",       L"0:false, 1:true"                                                                 },
  { PARAMS_DATA_USB_20_1_ENABLE_ID,            PARAMS_DATA_OFFSET_SIZE (Usb20[1].Enable),             PARAMS_DATA_BOOLEAN_TYPE,      L"USB2.0 Control1 Enable",       L"0:false, 1:true"                                                                 },
  { PARAMS_DATA_USB_20_2_ENABLE_ID,            PARAMS_DATA_OFFSET_SIZE (Usb20[2].Enable),             PARAMS_DATA_BOOLEAN_TYPE,      L"USB2.0 Control2 Enable",       L"0:false, 1:true"                                                                 },
  { PARAMS_DATA_USB_20_3_ENABLE_ID,            PARAMS_DATA_OFFSET_SIZE (Usb20[3].Enable),             PARAMS_DATA_BOOLEAN_TYPE,      L"USB2.0 Control3 Enable",       L"0:false, 1:true"                                                                 },
  { PARAMS_DATA_USB_32_DRD_0_ENABLE_ID,        PARAMS_DATA_OFFSET_SIZE (Usb32Drd[0].Enable),          PARAMS_DATA_BOOLEAN_TYPE,      L"USB3.0 Control0 Enable",       L"0:false, 1:true"                                                                 },
  { PARAMS_DATA_USB_32_DRD_1_ENABLE_ID,        PARAMS_DATA_OFFSET_SIZE (Usb32Drd[1].Enable),          PARAMS_DATA_BOOLEAN_TYPE,      L"USB3.0 Control1 Enable",       L"0:false, 1:true"                                                                 },
  { PARAMS_DATA_USB_TYPE_C_DRD_0_ENABLE_ID,    PARAMS_DATA_OFFSET_SIZE (UsbCDrd[0].Enable),           PARAMS_DATA_BOOLEAN_TYPE,      L"USBC Drd Control Enable",      L"0:false, 1:true"                                                                 },
  { PARAMS_DATA_USB_TYPE_C_HOST_0_ENABLE_ID,   PARAMS_DATA_OFFSET_SIZE (UsbC[0].Enable),              PARAMS_DATA_BOOLEAN_TYPE,      L"USBC host Control0 Enable",    L"0:false, 1:true"                                                                 },
  { PARAMS_DATA_USB_TYPE_C_HOST_1_ENABLE_ID,   PARAMS_DATA_OFFSET_SIZE (UsbC[1].Enable),              PARAMS_DATA_BOOLEAN_TYPE,      L"USBC host Control1 Enable",    L"0:false, 1:true"                                                                 },
  { PARAMS_DATA_USB_TYPE_C_HOST_2_ENABLE_ID,   PARAMS_DATA_OFFSET_SIZE (UsbC[2].Enable),              PARAMS_DATA_BOOLEAN_TYPE,      L"USBC host Control2 Enable",    L"0:false, 1:true"                                                                 },
  { PARAMS_DATA_USB_20_0_MAXSPEED_ID,          PARAMS_DATA_OFFSET_SIZE (Usb20[0].MaxSpeed),           PARAMS_DATA_MULTI_OPTION_TYPE, L"USB2.0 Control0 Max Speed",    L"3:high, 2:full"                                                                  },
  { PARAMS_DATA_USB_20_1_MAXSPEED_ID,          PARAMS_DATA_OFFSET_SIZE (Usb20[1].MaxSpeed),           PARAMS_DATA_MULTI_OPTION_TYPE, L"USB2.0 Control1 Max Speed",    L"3:high, 2:full"                                                                  },
  { PARAMS_DATA_USB_20_2_MAXSPEED_ID,          PARAMS_DATA_OFFSET_SIZE (Usb20[2].MaxSpeed),           PARAMS_DATA_MULTI_OPTION_TYPE, L"USB2.0 Control2 Max Speed",    L"3:high, 2:full"                                                                  },
  { PARAMS_DATA_USB_20_3_MAXSPEED_ID,          PARAMS_DATA_OFFSET_SIZE (Usb20[3].MaxSpeed),           PARAMS_DATA_MULTI_OPTION_TYPE, L"USB2.0 Control3 Max Speed",    L"3:high, 2:full"                                                                  },
  { PARAMS_DATA_USB_32_DRD_0_DATAROLE_ID,      PARAMS_DATA_OFFSET_SIZE (Usb32Drd[0].MaxSpeed),        PARAMS_DATA_MULTI_OPTION_TYPE, L"USB3.0 Control0 Max Speed",    L"3:high, 2:full, 4:super, 5:super plus"                                           },
  { PARAMS_DATA_USB_32_DRD_1_DATAROLE_ID,      PARAMS_DATA_OFFSET_SIZE (Usb32Drd[1].MaxSpeed),        PARAMS_DATA_MULTI_OPTION_TYPE, L"USB3.0 Control1 Max Speed",    L"3:high, 2:full, 4:super, 5:super plus"                                           },
  { PARAMS_DATA_USB_TYPE_C_DRD_0_MAXSPEED_ID,  PARAMS_DATA_OFFSET_SIZE (UsbCDrd[0].MaxSpeed),         PARAMS_DATA_MULTI_OPTION_TYPE, L"USBC Drd Control Max Speed",   L"3:high, 2:full, 4:super, 5:super plus"                                           },
  { PARAMS_DATA_USB_TYPE_C_HOST_0_MAXSPEED_ID, PARAMS_DATA_OFFSET_SIZE (UsbC[0].MaxSpeed),            PARAMS_DATA_MULTI_OPTION_TYPE, L"USBC host Control0 Max Speed", L"3:high, 2:full, 4:super, 5:super plus"                                           },
  { PARAMS_DATA_USB_TYPE_C_HOST_1_MAXSPEED_ID, PARAMS_DATA_OFFSET_SIZE (UsbC[1].MaxSpeed),            PARAMS_DATA_MULTI_OPTION_TYPE, L"USBC host Control1 Max Speed", L"3:high, 2:full, 4:super, 5:super plus"                                           },
  { PARAMS_DATA_USB_TYPE_C_HOST_2_MAXSPEED_ID, PARAMS_DATA_OFFSET_SIZE (UsbC[2].MaxSpeed),            PARAMS_DATA_MULTI_OPTION_TYPE, L"USBC host Control2 Max Speed", L"3:high, 2:full, 4:super, 5:super plus"                                           },
  { PARAMS_DATA_USB_32_DRD_0_DATAROLE_ID,      PARAMS_DATA_OFFSET_SIZE (Usb32Drd[0].DataRole),        PARAMS_DATA_BOOLEAN_TYPE,      L"USB3.0 Control0 data role",    L"0:host, 1:device"                                                                },
  { PARAMS_DATA_USB_32_DRD_1_DATAROLE_ID,      PARAMS_DATA_OFFSET_SIZE (Usb32Drd[1].DataRole),        PARAMS_DATA_BOOLEAN_TYPE,      L"USB3.0 Control1 data role",    L"0:host, 1:device"                                                                },
  { PARAMS_DATA_USB_TYPE_C_DRD_0_DATAROLE,     PARAMS_DATA_OFFSET_SIZE (UsbCDrd[0].DataRole),         PARAMS_DATA_BOOLEAN_TYPE,      L"USBC Drd Control data role",   L"0:host, 1:device"                                                                },
  { PARAMS_DATA_SOC_WATCH_DOG_TIMER_ID,        PARAMS_DATA_OFFSET_SIZE (S5.SocWatchdogTimer),         PARAMS_DATA_BOOLEAN_TYPE,      L"SOC Watchdog  Timer",          L"0:Disable, 1:Enable"                                                             },
  { PARAMS_DATA_CPU_LPI_STATE_ID,              PARAMS_DATA_OFFSET_SIZE (Cpu.LpiState),                PARAMS_DATA_MULTI_OPTION_TYPE, L"CPU LPI State ",               L"0:Disable, 1:LPI0, 2:LPI1, 3:LPI2"                                               },
  { PARAMS_DATA_DPU0_ENABLE_ID,                PARAMS_DATA_OFFSET_SIZE (Dpu.DpEnable[0]),             PARAMS_DATA_MULTI_OPTION_TYPE, L"Dp0 Enable",                   L"0:Disable, 1:Enable"                                                             },
  { PARAMS_DATA_DPU1_ENABLE_ID,                PARAMS_DATA_OFFSET_SIZE (Dpu.DpEnable[1]),             PARAMS_DATA_MULTI_OPTION_TYPE, L"Dp1 Enable",                   L"0:Disable, 1:Enable"                                                             },
  { PARAMS_DATA_DPU2_ENABLE_ID,                PARAMS_DATA_OFFSET_SIZE (Dpu.DpEnable[2]),             PARAMS_DATA_MULTI_OPTION_TYPE, L"Dp2 Enable",                   L"0:Disable, 1:Enable"                                                             },
  { PARAMS_DATA_DPU3_ENABLE_ID,                PARAMS_DATA_OFFSET_SIZE (Dpu.DpEnable[3]),             PARAMS_DATA_MULTI_OPTION_TYPE, L"Dp3 Enable",                   L"0:Disable, 1:Enable"                                                             },
  { PARAMS_DATA_DPU4_ENABLE_ID,                PARAMS_DATA_OFFSET_SIZE (Dpu.DpEnable[4]),             PARAMS_DATA_MULTI_OPTION_TYPE, L"Dp4 Enable",                   L"0:Disable, 1:Enable"                                                             },
  { PARAMS_DATA_DPU_PRIORITY_0_ID,             PARAMS_DATA_OFFSET_SIZE (Dpu.DpPriority[0]),           PARAMS_DATA_MULTI_OPTION_TYPE, L"Display Priority0",            L"0:Dp0, 1:Dp1, 2:Dp2, 3:Dp3, 4:Dp4"                                               },
  { PARAMS_DATA_DPU_PRIORITY_1_ID,             PARAMS_DATA_OFFSET_SIZE (Dpu.DpPriority[1]),           PARAMS_DATA_MULTI_OPTION_TYPE, L"Display Priority1",            L"0:Dp0, 1:Dp1, 2:Dp2, 3:Dp3, 4:Dp4"                                               },
  { PARAMS_DATA_DPU_PRIORITY_2_ID,             PARAMS_DATA_OFFSET_SIZE (Dpu.DpPriority[2]),           PARAMS_DATA_MULTI_OPTION_TYPE, L"Display Priority2",            L"0:Dp0, 1:Dp1, 2:Dp2, 3:Dp3, 4:Dp4"                                               },
  { PARAMS_DATA_DPU_PRIORITY_3_ID,             PARAMS_DATA_OFFSET_SIZE (Dpu.DpPriority[3]),           PARAMS_DATA_MULTI_OPTION_TYPE, L"Display Priority3",            L"0:Dp0, 1:Dp1, 2:Dp2, 3:Dp3, 4:Dp4"                                               },
  { PARAMS_DATA_DPU_PRIORITY_4_ID,             PARAMS_DATA_OFFSET_SIZE (Dpu.DpPriority[4]),           PARAMS_DATA_MULTI_OPTION_TYPE, L"Display Priority4",            L"0:Dp0, 1:Dp1, 2:Dp2, 3:Dp3, 4:Dp4"                                               },
};

UINT32  mConfigDataEntryNum = sizeof (mConfigDataEntryTable) / sizeof (CONFIG_PARAMS_DATA_ENTRY);

EFI_STATUS
EFIAPI
ParseConfigDataOption (
  IN  CHAR16                      *String,
  OUT UINT32                      *Num,
  OUT CONFIG_PARAMS_DATA_OPTIONS  *Options
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  CHAR16      *Head, *Tail;
  CHAR16      *Str;
  UINT32      Index;
  UINTN       Location, Size;

  if ((String == NULL) || (Num == NULL) || (Options == NULL)) {
    Status = EFI_INVALID_PARAMETER;
  }

  Str = AllocateCopyPool (StrSize (String), String);
  if (Str == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Head = Str;
  Tail = Str;
  Size = StrSize (Str);

  Index    = 0;
  Location = 0;
  while ((Index < MAX_PARAMS_OPTION_NUM) && (Head != NULL) && (Location < Size)) {
    Head = StrStr (Tail, L":");
    if (Head == NULL) {
      FreePool (Str);
      return EFI_UNSUPPORTED;
    }

    *Head     = L'\0';
    Location += StrSize (Tail);
    if ((StrStr (Tail, L"0x") == NULL) && (StrStr (Tail, L"0X") == NULL)) {
      Options[Index].Value = StrDecimalToUint64 (Tail);
    } else {
      Options[Index].Value = StrHexToUint64 (Tail);
    }

    Tail = Head + 1;
    Head = StrStr (Tail, L",");
    if (Head == NULL) {
      Location += StrSize (Tail);
      if (Location != Size) {
        FreePool (Str);
        return EFI_UNSUPPORTED;
      }
    } else {
      *Head     = L'\0';
      Location += StrSize (Tail);
    }

    Status = StrCpyS (Options[Index].String, MAX_PARAMS_OPTION_STRING_SIZE, Tail);
    if (EFI_ERROR (Status)) {
      FreePool (Str);
      return Status;
    }

    if (Head != NULL) {
      Tail = Head + 1;
    }

    Index++;
  }

  *Num = Index;

  if ((Index == MAX_PARAMS_OPTION_NUM) && (Location < Size)) {
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}
