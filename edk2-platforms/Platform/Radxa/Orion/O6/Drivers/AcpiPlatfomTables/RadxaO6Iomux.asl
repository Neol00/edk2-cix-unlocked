/** @file

  Copyright 2026 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

include(pads-sky1.m4)

// MUX0 For iomux S0 pins
Device (MUX0) {
  Name (_HID, "CIXHA016")
  Name (_UID, 0x0)
  Name (_STA, 0xF)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x04170000, 0x1000)
    PinGroup ("pinctrl_sndcard", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO144_FUNC_GPIO144(PULL_DOWN|DS_LEVEL4),
            CIX_PAD_GPIO145_FUNC_GPIO145(PULL_DOWN|DS_LEVEL4),
            CIX_PAD_GPIO146_FUNC_GPIO146(PULL_DOWN|DS_LEVEL4),
            CIX_PAD_GPIO147_FUNC_GPIO147(PULL_DOWN|DS_LEVEL4),
        })
        {
            CIX_PAD_GPIO144,
            CIX_PAD_GPIO145,
            CIX_PAD_GPIO146,
            CIX_PAD_GPIO147,
        }

    PinGroup ("pinctrl_fch_pwm0", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_DP2_VARY_BL_FUNC_DP2_VARY_BL(DS_LEVEL7)
        })
        {
            CIX_PAD_DP2_VARY_BL
        }

    PinGroup ("pinctrl_fch_pwm1", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_UART0_TXD_FUNC_PWM0(PULL_DOWN|ST|DS_LEVEL7)
        })
        {
            CIX_PAD_UART0_TXD
        }

    PinGroup ("pinctrl_edp0", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_DP2_DIGON_FUNC_DP2_DIGON(PULL_DOWN|DS_LEVEL4), /* DP2_DIGON */
            CIX_PAD_DP2_BLON_FUNC_DP2_BLON(PULL_DOWN|DS_LEVEL4) /* DP2_BLON */
        })
        {
            CIX_PAD_DP2_DIGON,
            CIX_PAD_DP2_BLON,
        }

    PinGroup ("pinctrl_cam0_hw", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_I2S3_DATA_OUT0_FUNC_GPIO088(PULL_DOWN|ST|DS_LEVEL12),
            CIX_PAD_I2S3_DATA_OUT1_FUNC_GPIO089(PULL_DOWN|ST|DS_LEVEL12),
            CIX_PAD_I2S3_MCLK_FUNC_GPIO081(ST|DS_LEVEL12),
        })
        {
            CIX_PAD_I2S3_DATA_OUT0,
            CIX_PAD_I2S3_DATA_OUT1,
            CIX_PAD_I2S3_MCLK,
        }

    PinGroup ("pinctrl_cam1_hw", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_I2S3_RSCK_FUNC_GPIO082(PULL_DOWN|ST|DS_LEVEL12),
            CIX_PAD_I2S3_MCLK_FUNC_GPIO081(ST|DS_LEVEL12),
            CIX_PAD_I2S3_TWS_FUNC_GPIO085(PULL_DOWN|ST|DS_LEVEL12),
        })
        {
            CIX_PAD_I2S3_RSCK,
            CIX_PAD_I2S3_MCLK,
            CIX_PAD_I2S3_TWS,
        }


    PinGroup ("pinctrl_lt7911_hw", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_I2S3_DATA_IN1_FUNC_GPIO087(DS_LEVEL12),
            CIX_PAD_GPIO090_FUNC_GPIO090(DS_LEVEL12),
            CIX_PAD_GPIO091_FUNC_GPIO091(DS_LEVEL12),
            CIX_PAD_GPIO093_FUNC_GPIO093(ST|DS_LEVEL12),
        })
        {
            CIX_PAD_I2S3_DATA_IN1,
            CIX_PAD_GPIO090,
            CIX_PAD_GPIO091,
            CIX_PAD_GPIO093,
        }

/*
    PinGroup ("gmac0", ResourceProducer, ,
        RawDataBuffer ()  // Vendor Data
        {
            CIX_PAD_GPIO121_FUNC_GMAC0_REFCLK_25M(ST|DS_LEVEL12),
            CIX_PAD_GPIO122_FUNC_GMAC0_TX_CTL(ST|DS_LEVEL12),
            CIX_PAD_GPIO123_FUNC_GMAC0_TXD0(ST|DS_LEVEL12),
            CIX_PAD_GPIO124_FUNC_GMAC0_TXD1(ST|DS_LEVEL12),
            CIX_PAD_GPIO125_FUNC_GMAC0_TXD2(ST|DS_LEVEL12),
            CIX_PAD_GPIO126_FUNC_GMAC0_TXD3(ST|DS_LEVEL12),
            CIX_PAD_GPIO127_FUNC_GMAC0_TX_CLK(ST|DS_LEVEL4),
            CIX_PAD_GPIO128_FUNC_GMAC0_RX_CTL(ST|DS_LEVEL4),
            CIX_PAD_GPIO129_FUNC_GMAC0_RXD0(ST|DS_LEVEL4),
            CIX_PAD_GPIO130_FUNC_GMAC0_RXD1(ST|DS_LEVEL4),
            CIX_PAD_GPIO131_FUNC_GMAC0_RXD2(ST|DS_LEVEL4),
            CIX_PAD_GPIO132_FUNC_GMAC0_RXD3(ST|DS_LEVEL4),
            CIX_PAD_GPIO133_FUNC_GMAC0_RX_CLK(ST|DS_LEVEL4),
            CIX_PAD_GPIO134_FUNC_GMAC0_MDC(ST|DS_LEVEL12),
            CIX_PAD_GPIO135_FUNC_GMAC0_MDIO(ST|DS_LEVEL12)
        })
        {   // Pin list
            CIX_PAD_GPIO121,
            CIX_PAD_GPIO122,
            CIX_PAD_GPIO123,
            CIX_PAD_GPIO124,
            CIX_PAD_GPIO125,
            CIX_PAD_GPIO126,
            CIX_PAD_GPIO127,
            CIX_PAD_GPIO128,
            CIX_PAD_GPIO129,
            CIX_PAD_GPIO130,
            CIX_PAD_GPIO131,
            CIX_PAD_GPIO132,
            CIX_PAD_GPIO133,
            CIX_PAD_GPIO134,
            CIX_PAD_GPIO135,
        }
    PinGroup ("gmac0-init", ResourceProducer, ,
        RawDataBuffer ()  // Vendor Data
        {   // mux reg offset, config value
            CIX_PAD_GPIO134_FUNC_GMAC0_MDC(ST|DS_LEVEL12),
            CIX_PAD_GPIO135_FUNC_GMAC0_MDIO(ST|DS_LEVEL12)
        })
        {   // Pin list
            CIX_PAD_GPIO134,
            CIX_PAD_GPIO135,
        }
*/

    PinGroup ("pinctrl_fch_i2c0", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_I2C0_CLK_FUNC_I2C0_CLK(PULL_UP|DS_LEVEL7),
            CIX_PAD_I2C0_SDA_FUNC_I2C0_SDA(PULL_UP|DS_LEVEL7)
        })
        {
            CIX_PAD_I2C0_CLK,
            CIX_PAD_I2C0_SDA
        }

    PinGroup ("pinctrl_fch_i2c2", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_I2C2_SCL_FUNC_I2C2_SCL(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_I2C2_SDA_FUNC_I2C2_SDA(PULL_UP|ST|DS_LEVEL12)
        })
        {
            CIX_PAD_I2C2_SCL,
            CIX_PAD_I2C2_SDA
        }
/*
    PinGroup ("pinctrl_fch_i2c2_gpio", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0x88, 0x01, 0x5c,
            0x00, 0x8c, 0x01, 0x5c
        })
        {
            CIX_PAD_I2C2_SCL,
            CIX_PAD_I2C2_SDA
        }

    PinGroup ("pinctrl_fch_spi1", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO138_FUNC_SPI2_MISO(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_GPIO139_FUNC_SPI2_CS0(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_GPIO140_FUNC_SPI2_CS1(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_GPIO141_FUNC_SPI2_MOSI(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_GPIO142_FUNC_SPI2_CLK(ST|DS_LEVEL12)

        })
        {
            CIX_PAD_GPIO138,
            CIX_PAD_GPIO139,
            CIX_PAD_GPIO140,
            CIX_PAD_GPIO141,
            CIX_PAD_GPIO142,
        }
*/

    PinGroup ("pinctrl_fch_uart0", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_UART0_TXD_FUNC_UART0_TXD(PULL_DOWN|ST|DS_LEVEL7),
            CIX_PAD_UART0_RXD_FUNC_UART0_RXD(PULL_DOWN|ST|DS_LEVEL7),
            CIX_PAD_UART0_CTS_FUNC_UART0_CTS(PULL_DOWN|ST|DS_LEVEL7),
            CIX_PAD_UART0_RTS_FUNC_UART0_RTS(PULL_DOWN|ST|DS_LEVEL7)
        })
        {
            CIX_PAD_UART0_TXD,
            CIX_PAD_UART0_RXD,
            CIX_PAD_UART0_CTS,
            CIX_PAD_UART0_RTS
        }

    PinGroup ("pinctrl_fch_uart1", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_UART1_TXD_FUNC_UART1_TXD(PULL_DOWN|ST|DS_LEVEL7),
            CIX_PAD_UART1_RXD_FUNC_UART1_RXD(PULL_DOWN|ST|DS_LEVEL7),
            CIX_PAD_UART1_CTS_FUNC_UART1_CTS(PULL_DOWN|ST|DS_LEVEL7),
            CIX_PAD_UART1_RTS_FUNC_UART1_RTS(PULL_DOWN|ST|DS_LEVEL7)
        })
        {
            CIX_PAD_UART1_TXD,
            CIX_PAD_UART1_RXD,
            CIX_PAD_UART1_CTS,
            CIX_PAD_UART1_RTS
        }

    PinGroup ("pinctrl_fch_uart2", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_UART2_TXD_FUNC_UART2_TXD(PULL_DOWN|DS_LEVEL7),
            CIX_PAD_UART2_RXD_FUNC_UART2_RXD(PULL_DOWN|DS_LEVEL7)
        })
        {
            CIX_PAD_UART2_TXD,
            CIX_PAD_UART2_RXD,
        }

    PinGroup ("pinctrl_hda", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_HDA_BITCLK_FUNC_HDA_BITCLK(PULL_DOWN|ST|DS_LEVEL12),
            CIX_PAD_HDA_RST_L_FUNC_HDA_RST_L(PULL_DOWN|ST|DS_LEVEL12),
            CIX_PAD_HDA_SDIN0_FUNC_HDA_SDIN0(PULL_DOWN|ST|DS_LEVEL12),
            CIX_PAD_HDA_SDOUT0_FUNC_HDA_SDOUT0(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_HDA_SYNC_FUNC_HDA_SYNC(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_HDA_SDIN1_FUNC_HDA_SDIN1(PULL_DOWN|ST|DS_LEVEL12),
            CIX_PAD_HDA_SDOUT1_FUNC_HDA_SDOUT1(PULL_DOWN|ST|DS_LEVEL12),
        })
        {
            CIX_PAD_HDA_BITCLK,
            CIX_PAD_HDA_RST_L,
            CIX_PAD_HDA_SDIN0,
            CIX_PAD_HDA_SDOUT0,
            CIX_PAD_HDA_SYNC,
            CIX_PAD_HDA_SDIN1,
            CIX_PAD_HDA_SDOUT1,
        }
    PinGroup ("pinctrl_substrate_i2s0", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_HDA_BITCLK_FUNC_I2S0_SCK(PULL_DOWN|ST|DS_LEVEL12), /* I2S0_SCK */
            CIX_PAD_HDA_RST_L_FUNC_I2S0_DATA_IN(PULL_DOWN|ST|DS_LEVEL12), /* I2S0_DATA_IN */
            CIX_PAD_HDA_SDIN0_FUNC_I2S0_MCLK(PULL_DOWN|ST|DS_LEVEL12), /* I2S0_MCLK */
            CIX_PAD_HDA_SDOUT0_FUNC_I2S0_DATA_OUT(PULL_UP|ST|DS_LEVEL12), /* I2S0_DATA_OUT */
            CIX_PAD_HDA_SYNC_FUNC_I2S0_WS(PULL_UP|ST|DS_LEVEL12), /* I2S0_WS */
        })
        {
            CIX_PAD_HDA_BITCLK,
            CIX_PAD_HDA_RST_L,
            CIX_PAD_HDA_SDIN0,
            CIX_PAD_HDA_SDOUT0,
            CIX_PAD_HDA_SYNC,
        }

    PinGroup ("pinctrl_substrate_i2s1", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_I2S1_MCLK_FUNC_I2S1_MCLK(PULL_DOWN|ST|DS_LEVEL12), /* I2S1_MCLK */
            CIX_PAD_I2S1_SCK_FUNC_I2S1_SCK(PULL_DOWN|ST|DS_LEVEL12), /* I2S1_SCK */
            CIX_PAD_I2S1_WS_FUNC_I2S1_WS(PULL_UP|ST|DS_LEVEL12), /* I2S1_WS */
            CIX_PAD_I2S1_DATA_IN_FUNC_I2S1_DATA_IN(PULL_DOWN|ST|DS_LEVEL12), /* I2S1_DATA_IN */
            CIX_PAD_I2S1_DATA_OUT_FUNC_I2S1_DATA_OUT(PULL_DOWN|ST|DS_LEVEL12), /* I2S1_DATA_OUT */
        })
        {
            CIX_PAD_I2S1_MCLK,
            CIX_PAD_I2S1_SCK,
            CIX_PAD_I2S1_WS,
            CIX_PAD_I2S1_DATA_IN,
            CIX_PAD_I2S1_DATA_OUT,
        }
    PinGroup ("pinctrl_substrate_i2s2", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_I2S2_MCLK_FUNC_I2S2_MCLK(PULL_DOWN|ST|DS_LEVEL12), /* I2S2_MCLK */
            CIX_PAD_I2S2_RSCK_FUNC_I2S2_RSCK(PULL_DOWN|ST|DS_LEVEL12), /* I2S2_RSCK */
            CIX_PAD_I2S2_RWS_FUNC_I2S2_RWS(PULL_UP|ST|DS_LEVEL12), /* I2S2_RWS */
            CIX_PAD_I2S2_TSCK_FUNC_I2S2_TSCK(PULL_DOWN|ST|DS_LEVEL12), /* I2S2_TSCK */
            CIX_PAD_I2S2_TWS_FUNC_I2S2_TWS(PULL_UP|ST|DS_LEVEL12), /* I2S2_TWS */
            CIX_PAD_I2S2_DATA_IN0_FUNC_I2S2_DATA_IN0(PULL_DOWN|ST|DS_LEVEL12), /* I2S2_DATA_IN0 */
            CIX_PAD_I2S2_DATA_IN1_FUNC_I2S2_DATA_IN1(PULL_DOWN|ST|DS_LEVEL12), /* I2S2_DATA_IN1 */
            CIX_PAD_I2S2_DATA_OUT0_FUNC_I2S2_DATA_OUT0(PULL_UP|ST|DS_LEVEL12), /* I2S2_DATA_OUT0 */
            CIX_PAD_I2S2_DATA_OUT1_FUNC_I2S2_DATA_OUT1(PULL_UP|ST|DS_LEVEL12), /* I2S2_DATA_OUT1 */
            CIX_PAD_I2S2_DATA_OUT2_FUNC_I2S2_DATA_OUT2(PULL_UP|ST|DS_LEVEL12), /* I2S2_DATA_OUT2 */
            CIX_PAD_I2S2_DATA_OUT3_FUNC_I2S2_DATA_OUT3(PULL_UP|ST|DS_LEVEL12), /* I2S2_DATA_OUT3 */
        })
        {
            CIX_PAD_I2S2_MCLK,
            CIX_PAD_I2S2_RSCK,
            CIX_PAD_I2S2_RWS,
            CIX_PAD_I2S2_TSCK,
            CIX_PAD_I2S2_TWS,
            CIX_PAD_I2S2_DATA_IN0,
            CIX_PAD_I2S2_DATA_IN1,
            CIX_PAD_I2S2_DATA_OUT0,
            CIX_PAD_I2S2_DATA_OUT1,
            CIX_PAD_I2S2_DATA_OUT2,
            CIX_PAD_I2S2_DATA_OUT3,
        }

    PinGroup ("pinctrl_substrate_i2s3", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_I2S3_MCLK_FUNC_I2S3_MCLK(PULL_DOWN|ST|DS_LEVEL12), /* i2s3_mclk */
            CIX_PAD_I2S3_RSCK_FUNC_I2S3_RSCK(PULL_DOWN|ST|DS_LEVEL12), /* i2s3_rsck */
            CIX_PAD_I2S3_RWS_FUNC_I2S3_RWS(PULL_UP|ST|DS_LEVEL12), /* i2s3_rws */
            CIX_PAD_I2S3_TSCK_FUNC_I2S3_TSCK(PULL_DOWN|ST|DS_LEVEL12), /* i2s3_tsck */
            CIX_PAD_I2S3_TWS_FUNC_I2S3_TWS(PULL_UP|ST|DS_LEVEL12), /* i2s3_tws */
            CIX_PAD_I2S3_DATA_IN0_FUNC_I2S3_DATA_IN0(PULL_DOWN|ST|DS_LEVEL12), /* i2s3_data_in0 */
            CIX_PAD_I2S3_DATA_IN1_FUNC_I2S3_DATA_IN1(PULL_DOWN|ST|DS_LEVEL12), /* i2s3_data_in1 */
            CIX_PAD_I2S3_DATA_OUT0_FUNC_I2S3_DATA_OUT0(PULL_UP|ST|DS_LEVEL12), /* i2s3_data_out0 */
            CIX_PAD_I2S3_DATA_OUT1_FUNC_I2S3_DATA_OUT1(PULL_UP|ST|DS_LEVEL12), /* i2s3_data_out1 */
        })
        {
            CIX_PAD_I2S3_MCLK,
            CIX_PAD_I2S3_RSCK,
            CIX_PAD_I2S3_RWS,
            CIX_PAD_I2S3_TSCK,
            CIX_PAD_I2S3_TWS,
            CIX_PAD_I2S3_DATA_IN0,
            CIX_PAD_I2S3_DATA_IN1,
            CIX_PAD_I2S3_DATA_OUT0,
            CIX_PAD_I2S3_DATA_OUT1,
        }
    PinGroup ("pinctrl_substrate_i2s4", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO090_FUNC_I2S4_MCLK_LB(ST|DS_LEVEL12),   /* i2s4_mclk_lb */
            CIX_PAD_GPIO091_FUNC_I2S4_SCK_LB(ST|DS_LEVEL12),    /* i2s4_sck_lb */
            CIX_PAD_GPIO092_FUNC_I2S4_WS_LB(ST|DS_LEVEL12),     /* i2s4_ws_lb */
            CIX_PAD_GPIO093_FUNC_I2S4_DATA_IN_LB(ST|DS_LEVEL12),/* i2s4_data_in_lb */
            CIX_PAD_GPIO094_FUNC_I2S4_DATA_OUT_LB(ST|DS_LEVEL12),  /* i2s4_data_out_lb */
        })
        {
            CIX_PAD_GPIO090,
            CIX_PAD_GPIO091,
            CIX_PAD_GPIO092,
            CIX_PAD_GPIO093,
            CIX_PAD_GPIO094,
        }

    PinGroup ("pinctrl_substrate_i2s5", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_I2S2_RSCK_FUNC_I2S5_RSCK_DBG(PULL_DOWN|ST|DS_LEVEL12), /* i2s5_rsck_dbg */
            CIX_PAD_I2S2_RWS_FUNC_I2S5_RWS_DBG(PULL_UP|ST|DS_LEVEL12), /* i2s5_rws_dbg */
            CIX_PAD_I2S2_TSCK_FUNC_I2S5_TSCK_DBG(PULL_DOWN|ST|DS_LEVEL12), /* i2s5_tsck_dbg */
            CIX_PAD_I2S2_TWS_FUNC_I2S5_TWS_DBG(PULL_DOWN|ST|DS_LEVEL12), /* i2s5_tws_dbg */
            CIX_PAD_I2S2_DATA_IN0_FUNC_I2S5_DATA_IN0_DBG(PULL_DOWN|ST|DS_LEVEL12), /* i2s5_data_in0_dbg */
            CIX_PAD_I2S2_DATA_IN1_FUNC_I2S5_DATA_IN1_DBG(PULL_DOWN|ST|DS_LEVEL12), /* i2s5_data_in1_dbg */
            CIX_PAD_I2S2_DATA_OUT0_FUNC_I2S5_DATA_OUT0_DBG(PULL_UP|ST|DS_LEVEL12), /* i2s5_data_out0_dbg */
            CIX_PAD_I2S2_DATA_OUT1_FUNC_I2S5_DATA_OUT1_DBG(PULL_UP|ST|DS_LEVEL12), /* i2s5_data_out1_dbg */
        })
        {
            CIX_PAD_I2S2_RSCK,
            CIX_PAD_I2S2_RWS,
            CIX_PAD_I2S2_TSCK,
            CIX_PAD_I2S2_TWS,
            CIX_PAD_I2S2_DATA_IN0,
            CIX_PAD_I2S2_DATA_IN1,
            CIX_PAD_I2S2_DATA_OUT0,
            CIX_PAD_I2S2_DATA_OUT1,
        }
    PinGroup ("pinctrl_substrate_i2s6", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_I2S2_RSCK_FUNC_I2S6_RSCK_DBG(PULL_DOWN|ST|DS_LEVEL12), /* i2s6_rsck_dbg */
            CIX_PAD_I2S2_RWS_FUNC_I2S6_RWS_DBG(PULL_UP|ST|DS_LEVEL12), /* i2s6_rws_dbg */
            CIX_PAD_I2S2_TSCK_FUNC_I2S6_TSCK_DBG(PULL_DOWN|ST|DS_LEVEL12), /* i2s6_tsck_dbg */
            CIX_PAD_I2S2_TWS_FUNC_I2S6_TWS_DBG(PULL_DOWN|ST|DS_LEVEL12), /* i2s6_tws_dbg */
            CIX_PAD_I2S2_DATA_IN0_FUNC_I2S6_DATA_IN0_DBG(PULL_DOWN|ST|DS_LEVEL12), /* i2s6_data_in0_dbg */
            CIX_PAD_I2S2_DATA_IN1_FUNC_I2S6_DATA_IN1_DBG(PULL_DOWN|ST|DS_LEVEL12), /* i2s6_data_in1_dbg */
            CIX_PAD_I2S2_DATA_OUT0_FUNC_I2S6_DATA_OUT0_DBG(PULL_UP|ST|DS_LEVEL12), /* i2s6_data_out0_dbg */
            CIX_PAD_I2S2_DATA_OUT1_FUNC_I2S6_DATA_OUT1_DBG(PULL_UP|ST|DS_LEVEL12), /* i2s6_data_out1_dbg */
        })
        {
            CIX_PAD_I2S2_RSCK,
            CIX_PAD_I2S2_RWS,
            CIX_PAD_I2S2_TSCK,
            CIX_PAD_I2S2_TWS,
            CIX_PAD_I2S2_DATA_IN0,
            CIX_PAD_I2S2_DATA_IN1,
            CIX_PAD_I2S2_DATA_OUT0,
            CIX_PAD_I2S2_DATA_OUT1,
        }

    PinGroup ("pinctrl_substrate_i2s7", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_I2S3_RSCK_FUNC_I2S7_RSCK_DBG(PULL_DOWN|ST|DS_LEVEL12), /* i2s7_rsck_dbg */
            CIX_PAD_I2S3_RWS_FUNC_I2S7_RWS_DBG(PULL_UP|ST|DS_LEVEL12), /* i2s7_rws_dbg */
            CIX_PAD_I2S3_TSCK_FUNC_I2S7_TSCK_DBG(PULL_DOWN|ST|DS_LEVEL12), /* i2s7_tsck_dbg */
            CIX_PAD_I2S3_TWS_FUNC_I2S7_TWS_DBG(PULL_UP|ST|DS_LEVEL12), /* i2s7_tws_dbg */
            CIX_PAD_I2S3_DATA_IN0_FUNC_I2S7_DATA_IN0_DBG(PULL_DOWN|ST|DS_LEVEL12), /* i2s7_data_in0_dbg */
            CIX_PAD_I2S3_DATA_IN1_FUNC_I2S7_DATA_IN1_DBG(PULL_DOWN|ST|DS_LEVEL12), /* i2s7_data_in1_dbg */
            CIX_PAD_I2S3_DATA_OUT0_FUNC_I2S7_DATA_OUT0_DBG(PULL_UP|ST|DS_LEVEL12), /* i2s7_data_out0_dbg */
            CIX_PAD_I2S3_DATA_OUT1_FUNC_I2S7_DATA_OUT1_DBG(PULL_UP|ST|DS_LEVEL12), /* i2s7_data_out1_dbg */
        })
        {
            CIX_PAD_I2S3_RSCK,
            CIX_PAD_I2S3_RWS,
            CIX_PAD_I2S3_TSCK,
            CIX_PAD_I2S3_TWS,
            CIX_PAD_I2S3_DATA_IN0,
            CIX_PAD_I2S3_DATA_IN1,
            CIX_PAD_I2S3_DATA_OUT0,
            CIX_PAD_I2S3_DATA_OUT1,
        }

    PinGroup ("pinctrl_substrate_i2s8", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_I2S3_RSCK_FUNC_I2S8_RSCK_DBG(PULL_DOWN|ST|DS_LEVEL12), /* i2s8_rsck_dbg */
            CIX_PAD_I2S3_RWS_FUNC_I2S8_RWS_DBG(PULL_UP|ST|DS_LEVEL12), /* i2s8_rws_dbg */
            CIX_PAD_I2S3_TSCK_FUNC_I2S8_TSCK_DBG(PULL_DOWN|ST|DS_LEVEL12), /* i2s8_tsck_dbg */
            CIX_PAD_I2S3_TWS_FUNC_I2S8_TWS_DBG(PULL_UP|ST|DS_LEVEL12), /* i2s8_tws_dbg */
            CIX_PAD_I2S3_DATA_IN0_FUNC_I2S8_DATA_IN0_DBG(PULL_DOWN|ST|DS_LEVEL12), /* i2s8_data_in0_dbg */
            CIX_PAD_I2S3_DATA_IN1_FUNC_I2S8_DATA_IN1_DBG(PULL_DOWN|ST|DS_LEVEL12), /* i2s8_data_in1_dbg */
            CIX_PAD_I2S3_DATA_OUT0_FUNC_I2S8_DATA_OUT0_DBG(PULL_UP|ST|DS_LEVEL12), /* i2s8_data_out0_dbg */
            CIX_PAD_I2S3_DATA_OUT1_FUNC_I2S8_DATA_OUT1_DBG(PULL_UP|ST|DS_LEVEL12), /* i2s8_data_out1_dbg */
        })
        {
            CIX_PAD_I2S3_RSCK,
            CIX_PAD_I2S3_RWS,
            CIX_PAD_I2S3_TSCK,
            CIX_PAD_I2S3_TWS,
            CIX_PAD_I2S3_DATA_IN0,
            CIX_PAD_I2S3_DATA_IN1,
            CIX_PAD_I2S3_DATA_OUT0,
            CIX_PAD_I2S3_DATA_OUT1,
        }

//    PinGroup ("pinctrl_substrate_i2s9", ResourceProducer, ,
//        RawDataBuffer ()
//        {
//            0x00, 0xa8, 0x01, 0x3c, /* i2s9_rsck_dbg */
//            0x00, 0xac, 0x01, 0x3c, /* i2s9_data_in0_dbg */
//            0x00, 0xb0, 0x01, 0x3c, /* i2s9_tsck_dbg */
//            0x00, 0xb4, 0x01, 0x5c, /* i2s9_tws_dbg */
//            0x00, 0xb8, 0x01, 0x5c, /* i2s9_rws_dbg */
//            0x00, 0xbc, 0x01, 0x3c, /* i2s9_data_in1_dbg */
//            0x00, 0xc0, 0x01, 0x3c, /* i2s9_data_out0_dbg */
//        })
//        {
//            CIX_PAD_HDA_BITCLK,
//            CIX_PAD_HDA_RST_L,
//            CIX_PAD_HDA_SDIN0,
//            CIX_PAD_HDA_SDOUT0,
//            CIX_PAD_HDA_SYNC,
//            CIX_PAD_HDA_SDIN1,
//            CIX_PAD_HDA_SDOUT1,
//        }
    PinGroup ("pinctrl_alc5682_irq", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO149_FUNC_GPIO149(ST|DS_LEVEL12)
        })
        {
            CIX_PAD_GPIO149
        }

    PinGroup ("pinctrl_fch_i3c0", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_I2C2_SCL_FUNC_I3C0_SCL(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_I2C2_SDA_FUNC_I3C0_SDA(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_GPIO057_FUNC_I3C0_PUR_EN_L(PULL_UP|ST|DS_LEVEL12)
        })
        {
            CIX_PAD_I2C2_SCL,
            CIX_PAD_I2C2_SDA,
            CIX_PAD_GPIO057
        }

    PinGroup ("pinctrl_fch_i3c1", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_I2C3_CLK_FUNC_I3C1_CLK(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_I2C3_SDA_FUNC_I3C1_SDA(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_GPIO060_FUNC_I3C1_PUR_EN_L(PULL_UP|ST|DS_LEVEL12),
        })
        {
            CIX_PAD_I2C3_CLK,
            CIX_PAD_I2C3_SDA,
            CIX_PAD_GPIO060
        }
  })
}

// MUX1 For iomux S5 pins
Device (MUX1) {
  Name (_HID, "CIXHA017")
  Name (_UID, 0x1)
  Name (_STA, 0xF)
  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x16007000, 0x1000)
    PinGroup ("wifi_vbat_gpio", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO013_FUNC_GPIO013(PULL_UP|ST|DS_LEVEL12),
        })
        {
            CIX_PAD_GPIO013,
        }

    PinGroup ("i2c0_grp", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_SFI_I2C0_SCL_FUNC_SFI_I2C0_SCL(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_SFI_I2C0_SDA_FUNC_SFI_I2C0_SDA(PULL_UP|ST|DS_LEVEL12)
        })
        {
            CIX_PAD_SFI_I2C0_SCL,
            CIX_PAD_SFI_I2C0_SDA
        }

    PinGroup ("i2c1_grp", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_SFI_I2C1_SCL_FUNC_SFI_I2C1_SCL(PULL_UP|ST|DS_LEVEL7),
            CIX_PAD_SFI_I2C1_SDA_FUNC_SFI_I2C1_SDA(PULL_UP|ST|DS_LEVEL7)
        })
        {
            CIX_PAD_SFI_I2C1_SCL,
            CIX_PAD_SFI_I2C1_SDA
        }

    PinGroup ("pinctrl_fch_spi0", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_SPI1_MISO_FUNC_SPI1_MISO(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_SPI1_CS0_FUNC_SPI1_CS0(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_SPI1_CS1_FUNC_SPI1_CS1(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_SPI1_MOSI_FUNC_SPI1_MOSI(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_SPI1_CLK_FUNC_SPI1_CLK(ST|DS_LEVEL12)
        })
        {
            CIX_PAD_SPI1_MISO,
            CIX_PAD_SPI1_CS0,
            CIX_PAD_SPI1_CS1,
            CIX_PAD_SPI1_MOSI,
            CIX_PAD_SPI1_CLK
        }

    PinGroup ("pinctrl_fch_xspi", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_SE_QSPI_CLK_FUNC_QSPI_CLK(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_SE_QSPI_CS_L_FUNC_QSPI_CS_L(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_SE_QSPI_DATA0_FUNC_QSPI_DATA0(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_SE_QSPI_DATA1_FUNC_QSPI_DATA1(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_SE_QSPI_DATA2_FUNC_QSPI_DATA2(PULL_UP|ST|DS_LEVEL12),
            CIX_PAD_SE_QSPI_DATA3_FUNC_QSPI_DATA3(PULL_UP|ST|DS_LEVEL12)
        })
        {
            CIX_PAD_SE_QSPI_CLK,
            CIX_PAD_SE_QSPI_CS_L,
            CIX_PAD_SE_QSPI_DATA0,
            CIX_PAD_SE_QSPI_DATA1,
            CIX_PAD_SE_QSPI_DATA2,
            CIX_PAD_SE_QSPI_DATA3
        }
    PinGroup ("pinctrl_usb0", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO036_FUNC_GPIO036(PULL_UP|DS_LEVEL4),
            CIX_PAD_GPIO040_FUNC_GPIO040(PULL_DOWN|DS_LEVEL4),
        })
        {
            CIX_PAD_GPIO036,
            CIX_PAD_GPIO040,
        }
    PinGroup ("pinctrl_usb1", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO037_FUNC_GPIO037(PULL_UP|DS_LEVEL4),
        })
        {
            CIX_PAD_GPIO037,
        }
    PinGroup ("pinctrl_usb2", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO038_FUNC_GPIO038(PULL_UP|DS_LEVEL4),
        })
        {
            CIX_PAD_GPIO038,
        }
    PinGroup ("pinctrl_usb3", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO039_FUNC_GPIO039(PULL_UP|DS_LEVEL4),
        })
        {
            CIX_PAD_GPIO039,
        }
    PinGroup ("pinctrl_usb4", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO034_FUNC_GPIO034(PULL_UP|DS_LEVEL4),
            CIX_PAD_GPIO041_FUNC_GPIO041(PULL_DOWN|DS_LEVEL4),
        })
        {
            CIX_PAD_GPIO034,
            CIX_PAD_GPIO041,
        }
    PinGroup ("pinctrl_usb5", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO035_FUNC_GPIO035(PULL_UP|DS_LEVEL4),
            CIX_PAD_GPIO042_FUNC_GPIO042(PULL_DOWN|DS_LEVEL4),
        })
        {
            CIX_PAD_GPIO035,
            CIX_PAD_GPIO042,
        }
    PinGroup ("pinctrl_usb7", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO031_FUNC_GPIO031(PULL_UP|DS_LEVEL4),
        })
        {
            CIX_PAD_GPIO031,
        }
    PinGroup ("pinctrl_usb8", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO032_FUNC_GPIO032(PULL_UP|DS_LEVEL4),
        })
        {
            CIX_PAD_GPIO032,
        }
    PinGroup ("pinctrl_pcie_x8_rc", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0x04, 0x00, 0x24,
        })
        {
            CIX_PAD_GPIO002,
        }
    PinGroup ("pinctrl_pcie_x4_rc", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO004_FUNC_GPIO004(PULL_DOWN|DS_LEVEL4),
        })
        {
            CIX_PAD_GPIO004,
        }
    PinGroup ("pinctrl_pcie_x2_rc", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO005_FUNC_GPIO005(PULL_DOWN|DS_LEVEL4),
        })
        {
            CIX_PAD_GPIO005,
        }
    PinGroup ("pinctrl_pcie_x1_1_rc", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO003_FUNC_GPIO003(PULL_DOWN|DS_LEVEL4),
        })
        {
            CIX_PAD_GPIO003,
        }
    PinGroup ("pinctrl_pcie_x1_0_rc", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO006_FUNC_GPIO006(PULL_DOWN|DS_LEVEL4),
        })
        {
            CIX_PAD_GPIO006,
        }
    PinGroup ("vgfx_poweren_gpio", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_SFI_GPIO2_FUNC_GPIO017(PULL_UP|DS_LEVEL4),
        })
        {
            CIX_PAD_SFI_GPIO2,
        }
    PinGroup ("gbe1_poweren_gpio", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO010_FUNC_GPIO010(PULL_UP|DS_LEVEL4),
        })
        {
            CIX_PAD_GPIO010,
        }
    PinGroup ("gbe2_poweren_gpio", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO022_FUNC_GPIO022(PULL_UP|ST|DS_LEVEL4),
        })
        {
            CIX_PAD_GPIO022,
        }

    PinGroup ("pinctrl_hym8563_irq", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO011_FUNC_GPIO011(PULL_UP|DS_LEVEL4),
        })
        {
            CIX_PAD_GPIO011,
        }

    PinGroup ("vcc_ssd_pwren", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO012_FUNC_GPIO012(PULL_UP|DS_LEVEL4),
        })
        {
            CIX_PAD_GPIO012,
        }

    PinGroup ("gpio_leds", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_SFI_GPIO0_FUNC_GPIO015(PULL_UP|ST|DS_LEVEL7),
        })
        {
            CIX_PAD_SFI_GPIO0,
        }

    PinGroup ("wl_radio_disable_l", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO019_FUNC_GPIO019(PULL_UP|ST|DS_LEVEL4),
        })
        {
            CIX_PAD_GPIO019,
        }

    PinGroup ("bt_radio_disable_l", ResourceProducer, ,
        RawDataBuffer ()
        {
            CIX_PAD_GPIO020_FUNC_GPIO020(PULL_UP|ST|DS_LEVEL4),
        })
        {
            CIX_PAD_GPIO020,
        }
  })
}
