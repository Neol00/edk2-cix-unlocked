/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <sky1-iomux.h>

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
            0x02, 0x00, 0x00, 0x24, /* GPIO144 pdb0 */
            0x02, 0x04, 0x00, 0x24, /* GPIO145 pdb1 */
            0x02, 0x08, 0x00, 0x24, /* GPIO146 pdb2 */
            0x02, 0x0c, 0x00, 0x24, /* GPIO147 pdb3 */
        })
        {
            SKY1_IOMUXC_GMAC1_RXD0,
            SKY1_IOMUXC_GMAC1_RXD1,
            SKY1_IOMUXC_GMAC1_RXD2,
            SKY1_IOMUXC_GMAC1_RXD3,
        }

    PinGroup ("pinctrl_fch_pwm0", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0x50, 0x00, 0x07
        })
        {
            SKY1_IOMUXC_DP2_VARY_BL
        }

    PinGroup ("pinctrl_fch_pwm1", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x01, 0x3c, 0x00, 0xb7
        })
        {
            SKY1_IOMUXC_UART0_TXD
        }

    PinGroup ("pinctrl_edp0", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0x48, 0x00, 0x24,
            0x00, 0x4c, 0x00, 0x24,
        })
        {
            SKY1_IOMUXC_DP2_DIGON,
            SKY1_IOMUXC_DP2_BLON,
        }

    PinGroup ("pinctrl_cam0_hw", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x01, 0x94, 0x00, 0x3c,
            0x01, 0x1c, 0x00, 0x9c,
            0x01, 0x28, 0x00, 0x1c,
            0x01, 0x2c, 0x00, 0x0c,
            0x01, 0x34, 0x00, 0x1c,
        })
        {
            SKY1_IOMUXC_CSI0_MCLK0,
            SKY1_IOMUXC_I2S3_DATA_IN1,
            SKY1_IOMUXC_I2S4_MCLK_LB,
            SKY1_IOMUXC_I2S4_SCK_LB,
            SKY1_IOMUXC_I2S4_DATA_IN_LB
        }

    PinGroup ("pinctrl_cam1_hw", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x01, 0x98, 0x00, 0x3c,
            0x01, 0x30, 0x00, 0x1c,
            0x01, 0x38, 0x00, 0x1c,
        })
        {
            SKY1_IOMUXC_CSI0_MCLK1,
            SKY1_IOMUXC_I2S4_WS_LB,
            SKY1_IOMUXC_I2S4_DATA_OUT_LB,
        }

    PinGroup ("pinctrl_cam2_hw", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x01, 0x9c, 0x00, 0x3c,
            0x01, 0x0c, 0x00, 0x9c,
            0x01, 0x18, 0x00, 0xbc,
            0x01, 0x14, 0x00, 0xbc,
            0x01, 0x08, 0x00, 0xbc,
        })
        {
            SKY1_IOMUXC_CSI1_MCLK0,
            SKY1_IOMUXC_I2S3_RWS,
            SKY1_IOMUXC_I2S3_DATA_IN0,
            SKY1_IOMUXC_I2S3_TWS,
            SKY1_IOMUXC_I2S3_RSCK
        }

    PinGroup ("pinctrl_cam3_hw", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x01, 0xa0, 0x00, 0x3c,
            0x01, 0x20, 0x00, 0xbc,
            0x01, 0x24, 0x00, 0xbc,
        })
        {
            SKY1_IOMUXC_CSI1_MCLK1,
            SKY1_IOMUXC_I2S3_DATA_OUT0,
            SKY1_IOMUXC_I2S3_DATA_OUT1,
        }

    PinGroup ("pinctrl_lt7911_hw", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x01, 0x1c, 0x00, 0x8c,
            0x01, 0x28, 0x00, 0x0c,
            0x01, 0x2c, 0x00, 0x0c,
            0x01, 0x34, 0x00, 0x1c,
        })
        {
            SKY1_IOMUXC_I2S3_DATA_IN1,
            SKY1_IOMUXC_I2S4_MCLK_LB,
            SKY1_IOMUXC_I2S4_SCK_LB,
            SKY1_IOMUXC_I2S4_DATA_IN_LB,
        }

    PinGroup ("gmac0", ResourceProducer, ,
        RawDataBuffer ()  // Vendor Data
        {   // mux reg offset, config value
            0x01, 0xa4, 0x00, 0x9c,
            0x01, 0xa8, 0x00, 0x9c,
            0x01, 0xac, 0x00, 0x9c,
            0x01, 0xb0, 0x00, 0x9c,
            0x01, 0xb4, 0x00, 0x9c,
            0x01, 0xb8, 0x00, 0x9c,
            0x01, 0xbc, 0x00, 0x94,
            0x01, 0xc0, 0x00, 0x94,
            0x01, 0xc4, 0x00, 0x94,
            0x01, 0xc8, 0x00, 0x94,
            0x01, 0xcc, 0x00, 0x94,
            0x01, 0xd0, 0x00, 0x94,
            0x01, 0xd4, 0x00, 0x94,
            0x01, 0xd8, 0x00, 0x9c,
            0x01, 0xdc, 0x00, 0x9c
        })
        {   // Pin list
            SKY1_IOMUXC_GMAC0_REFCLK_25M,
            SKY1_IOMUXC_GMAC0_TX_CTL,
            SKY1_IOMUXC_GMAC0_TXD0,
            SKY1_IOMUXC_GMAC0_TXD1,
            SKY1_IOMUXC_GMAC0_TXD2,
            SKY1_IOMUXC_GMAC0_TXD3,
            SKY1_IOMUXC_GMAC0_TX_CLK,
            SKY1_IOMUXC_GMAC0_RX_CTL,
            SKY1_IOMUXC_GMAC0_RXD0,
            SKY1_IOMUXC_GMAC0_RXD1,
            SKY1_IOMUXC_GMAC0_RXD2,
            SKY1_IOMUXC_GMAC0_RXD3,
            SKY1_IOMUXC_GMAC0_RX_CLK,
            SKY1_IOMUXC_GMAC0_MDC,
            SKY1_IOMUXC_GMAC0_MDIO
        }
    PinGroup ("gmac0-init", ResourceProducer, ,
        RawDataBuffer ()  // Vendor Data
        {   // mux reg offset, config value
            0x01, 0xd8, 0x00, 0x9c,
            0x01, 0xdc, 0x00, 0x9c
        })
        {   // Pin list
            SKY1_IOMUXC_GMAC0_MDC,
            SKY1_IOMUXC_GMAC0_MDIO
        }

    PinGroup ("pinctrl_fch_i2c0", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0x78, 0x00, 0x47,
            0x00, 0x7c, 0x00, 0x47
        })
        {
            SKY1_IOMUXC_I2C0_CLK,
            SKY1_IOMUXC_I2C0_SDA
        }

    PinGroup ("pinctrl_fch_i2c2", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0x88, 0x00, 0x5c,
            0x00, 0x8c, 0x00, 0x5c
        })
        {
            SKY1_IOMUXC_I2C2_SCL,
            SKY1_IOMUXC_I2C2_SDA
        }
/*
    PinGroup ("pinctrl_fch_i2c2_gpio", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0x88, 0x01, 0x5c,
            0x00, 0x8c, 0x01, 0x5c
        })
        {
            SKY1_IOMUXC_I2C2_SCL,
            SKY1_IOMUXC_I2C2_SDA
        }
*/

    PinGroup ("pinctrl_fch_spi1", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x01, 0xe8, 0x01, 0x5c,
            0x01, 0xec, 0x01, 0x5c,
            0x01, 0xf0, 0x01, 0x5c,
            0x01, 0xf4, 0x01, 0x5c,
            0x01, 0xf8, 0x01, 0x1c

        })
        {
            SKY1_IOMUXC_GMAC1_TXD0,
            SKY1_IOMUXC_GMAC1_TXD1,
            SKY1_IOMUXC_GMAC1_TXD2,
            SKY1_IOMUXC_GMAC1_TXD3,
            SKY1_IOMUXC_GMAC1_TX_CLK
        }

    PinGroup ("pinctrl_fch_uart0", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x01, 0x3c, 0x00, 0x37,
            0x01, 0x40, 0x00, 0x37,
            0x01, 0x44, 0x00, 0x37,
            0x01, 0x48, 0x00, 0x37
        })
        {
            SKY1_IOMUXC_UART0_TXD,
            SKY1_IOMUXC_UART0_RXD,
            SKY1_IOMUXC_UART0_CTS,
            SKY1_IOMUXC_UART0_RTS
        }

    PinGroup ("pinctrl_fch_uart1", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x01, 0x4c, 0x00, 0x37,
            0x01, 0x50, 0x00, 0x37,
            0x01, 0x54, 0x00, 0x37,
            0x01, 0x58, 0x00, 0x37
        })
        {
            SKY1_IOMUXC_UART1_TXD,
            SKY1_IOMUXC_UART1_RXD,
            SKY1_IOMUXC_UART1_CTS,
            SKY1_IOMUXC_UART1_RTS
        }

    PinGroup ("pinctrl_fch_uart2", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x01, 0x5c, 0x00, 0x27,
            0x01, 0x60, 0x00, 0x27
        })
        {
            SKY1_IOMUXC_UART2_TXD,
            SKY1_IOMUXC_UART2_RXD,
        }

    PinGroup ("pinctrl_hda", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0xa8, 0x00, 0x3c,
            0x00, 0xac, 0x00, 0x3c,
            0x00, 0xb0, 0x00, 0x3c,
            0x00, 0xb4, 0x00, 0x5c,
            0x00, 0xb8, 0x00, 0x5c,
            0x00, 0xbc, 0x00, 0x3c,
            0x00, 0xc0, 0x00, 0x3c
        })
        {
            SKY1_IOMUXC_HDA_BITCLK,
            SKY1_IOMUXC_HDA_RST_L,
            SKY1_IOMUXC_HDA_SDIN0,
            SKY1_IOMUXC_HDA_SDOUT0,
            SKY1_IOMUXC_HDA_SYNC,
            SKY1_IOMUXC_HDA_SDIN1,
            SKY1_IOMUXC_HDA_SDOUT1,
        }
    PinGroup ("pinctrl_substrate_i2s0", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0xa8, 0x00, 0xbc, /* I2S0_SCK */
            0x00, 0xac, 0x00, 0xbc, /* I2S0_DATA_IN */
            0x00, 0xb0, 0x00, 0xbc, /* I2S0_MCLK */
            0x00, 0xb4, 0x00, 0xdc, /* I2S0_DATA_OUT */
            0x00, 0xb8, 0x00, 0xdc, /* I2S0_WS */
        })
        {
            SKY1_IOMUXC_HDA_BITCLK,
            SKY1_IOMUXC_HDA_RST_L,
            SKY1_IOMUXC_HDA_SDIN0,
            SKY1_IOMUXC_HDA_SDOUT0,
            SKY1_IOMUXC_HDA_SYNC,
        }

    PinGroup ("pinctrl_substrate_i2s1", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0xc4, 0x00, 0x3c, /* I2S1_MCLK */
            0x00, 0xc8, 0x00, 0x3c, /* I2S1_SCLK */
            0x00, 0xcc, 0x00, 0x5c, /* I2S1_WS */
            0x00, 0xd0, 0x00, 0x3c, /* I2S1_DATA_IN */
            0x00, 0xd4, 0x00, 0x3c, /* I2S1_DATA_OUT */
        })
        {
            SKY1_IOMUXC_I2S1_MCLK,
            SKY1_IOMUXC_I2S1_SCK,
            SKY1_IOMUXC_I2S1_WS,
            SKY1_IOMUXC_I2S1_DATA_IN,
            SKY1_IOMUXC_I2S1_DATA_OUT,
        }
    PinGroup ("pinctrl_substrate_i2s2", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0xd8, 0x00, 0x3c, /* I2S2_MCLK */
            0x00, 0xdc, 0x00, 0x3c, /* I2S2_RSCK */
            0x00, 0xe0, 0x00, 0x5c, /* I2S2_RWS */
            0x00, 0xe4, 0x00, 0x3c, /* I2S2_TSCK */
            0x00, 0xe8, 0x00, 0x5c, /* I2S2_TWS */
            0x00, 0xec, 0x00, 0x3c, /* I2S2_DATA_IN0 */
            0x00, 0xf0, 0x00, 0x3c, /* I2S2_DATA_IN1 */
            0x00, 0xf4, 0x00, 0x5c, /* I2S2_DATA_OUT0 */
            0x00, 0xf8, 0x00, 0x5c, /* I2S2_DATA_OUT1 */
            0x00, 0xfc, 0x00, 0x5c, /* I2S2_DATA_OUT2 */
            0x01, 0x00, 0x00, 0x5c, /* I2S2_DATA_OUT3 */
        })
        {
            SKY1_IOMUXC_I2S2_MCLK,
            SKY1_IOMUXC_I2S2_RSCK,
            SKY1_IOMUXC_I2S2_RWS,
            SKY1_IOMUXC_I2S2_TSCK,
            SKY1_IOMUXC_I2S2_TWS,
            SKY1_IOMUXC_I2S2_DATA_IN0,
            SKY1_IOMUXC_I2S2_DATA_IN1,
            SKY1_IOMUXC_I2S2_DATA_OUT0,
            SKY1_IOMUXC_I2S2_DATA_OUT1,
            SKY1_IOMUXC_I2S2_DATA_OUT2,
            SKY1_IOMUXC_I2S2_DATA_OUT3,
        }

    PinGroup ("pinctrl_substrate_i2s3", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x01, 0x04, 0x00, 0x3c, /* i2s3_mclk */
            0x01, 0x08, 0x00, 0x3c, /* i2s3_rsck */
            0x01, 0x0c, 0x00, 0x5c, /* i2s3_rws */
            0x01, 0x10, 0x00, 0x3c, /* i2s3_tsck */
            0x01, 0x14, 0x00, 0x5c, /* i2s3_tws */
            0x01, 0x18, 0x00, 0x3c, /* i2s3_data_in0 */
            0x01, 0x1c, 0x00, 0x3c, /* i2s3_data_in1 */
            0x01, 0x20, 0x00, 0x5c, /* i2s3_data_out0 */
            0x01, 0x24, 0x00, 0x5c, /* i2s3_data_out1 */
        })
        {
            SKY1_IOMUXC_I2S3_MCLK,
            SKY1_IOMUXC_I2S3_RSCK,
            SKY1_IOMUXC_I2S3_RWS,
            SKY1_IOMUXC_I2S3_TSCK,
            SKY1_IOMUXC_I2S3_TWS,
            SKY1_IOMUXC_I2S3_DATA_IN0,
            SKY1_IOMUXC_I2S3_DATA_IN1,
            SKY1_IOMUXC_I2S3_DATA_OUT0,
            SKY1_IOMUXC_I2S3_DATA_OUT1,
        }
    PinGroup ("pinctrl_substrate_i2s4", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x01, 0x28, 0x00, 0x9c, /* i2s4_mclk_lb */
            0x01, 0x2c, 0x00, 0x9c, /* i2s4_sck_lb */
            0x01, 0x30, 0x00, 0x9c, /* i2s4_ws_lb */
            0x01, 0x34, 0x00, 0x9c, /* i2s4_data_in_lb */
            0x01, 0x38, 0x00, 0x9c, /* i2s4_data_out_lb */
        })
        {
            SKY1_IOMUXC_I2S4_MCLK_LB,
            SKY1_IOMUXC_I2S4_SCK_LB,
            SKY1_IOMUXC_I2S4_WS_LB,
            SKY1_IOMUXC_I2S4_DATA_IN_LB,
            SKY1_IOMUXC_I2S4_DATA_OUT_LB,
        }

    PinGroup ("pinctrl_substrate_i2s5", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0xdc, 0x01, 0x3c, /* i2s5_rsck_dbg */
            0x00, 0xe0, 0x01, 0x5c, /* i2s5_rws_dbg */
            0x00, 0xe4, 0x01, 0x3c, /* i2s5_tsck_dbg */
            0x00, 0xe8, 0x01, 0x3c, /* i2s5_tws_dbg */
            0x00, 0xec, 0x01, 0x3c, /* i2s5_data_in0_dbg */
            0x00, 0xf0, 0x01, 0x3c, /* i2s5_data_in1_dbg */
            0x00, 0xf4, 0x01, 0x5c, /* i2s9_data_out0_dbg */
            0x00, 0xf8, 0x01, 0x5c, /* i2s9_data_out1_dbg */
        })
        {
            SKY1_IOMUXC_I2S2_RSCK,
            SKY1_IOMUXC_I2S2_RWS,
            SKY1_IOMUXC_I2S2_TSCK,
            SKY1_IOMUXC_I2S2_TWS,
            SKY1_IOMUXC_I2S2_DATA_IN0,
            SKY1_IOMUXC_I2S2_DATA_IN1,
            SKY1_IOMUXC_I2S2_DATA_OUT0,
            SKY1_IOMUXC_I2S2_DATA_OUT1,
        }
    PinGroup ("pinctrl_substrate_i2s6", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0xdc, 0x01, 0xbc, /* i2s5_rsck_dbg */
            0x00, 0xe0, 0x01, 0xdc, /* i2s5_rws_dbg */
            0x00, 0xe4, 0x01, 0xbc, /* i2s5_tsck_dbg */
            0x00, 0xe8, 0x01, 0xbc, /* i2s5_tws_dbg */
            0x00, 0xec, 0x01, 0xbc, /* i2s5_data_in0_dbg */
            0x00, 0xf0, 0x01, 0xbc, /* i2s5_data_in1_dbg */
            0x00, 0xf4, 0x01, 0xdc, /* i2s9_data_out0_dbg */
            0x00, 0xf8, 0x01, 0xdc, /* i2s9_data_out1_dbg */
        })
        {
            SKY1_IOMUXC_I2S2_RSCK,
            SKY1_IOMUXC_I2S2_RWS,
            SKY1_IOMUXC_I2S2_TSCK,
            SKY1_IOMUXC_I2S2_TWS,
            SKY1_IOMUXC_I2S2_DATA_IN0,
            SKY1_IOMUXC_I2S2_DATA_IN1,
            SKY1_IOMUXC_I2S2_DATA_OUT0,
            SKY1_IOMUXC_I2S2_DATA_OUT1,
        }

    PinGroup ("pinctrl_substrate_i2s7", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x01, 0x08, 0x01, 0x3c, /* i2s7_rsck_dbg */
            0x01, 0x0c, 0x01, 0x5c, /* i2s7_rws_dbg */
            0x01, 0x10, 0x01, 0x3c, /* i2s7_tsck_dbg */
            0x01, 0x14, 0x01, 0x5c, /* i2s7_tws_dbg */
            0x01, 0x18, 0x01, 0x3c, /* i2s7_data_in0_dbg */
            0x01, 0x1c, 0x01, 0x3c, /* i2s7_data_in1_dbg */
            0x01, 0x20, 0x01, 0x5c, /* i2s7_data_out0_dbg */
            0x01, 0x24, 0x01, 0x5c, /* i2s7_data_out1_dbg */
        })
        {
            SKY1_IOMUXC_I2S3_RSCK,
            SKY1_IOMUXC_I2S3_RWS,
            SKY1_IOMUXC_I2S3_TSCK,
            SKY1_IOMUXC_I2S3_TWS,
            SKY1_IOMUXC_I2S3_DATA_IN0,
            SKY1_IOMUXC_I2S3_DATA_IN1,
            SKY1_IOMUXC_I2S3_DATA_OUT0,
            SKY1_IOMUXC_I2S3_DATA_OUT1,
        }

    PinGroup ("pinctrl_substrate_i2s8", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x01, 0x08, 0x01, 0xbc, /* i2s8_rsck_dbg */
            0x01, 0x0c, 0x01, 0xdc, /* i2s8_rws_dbg */
            0x01, 0x10, 0x01, 0xbc, /* i2s8_tsck_dbg */
            0x01, 0x14, 0x01, 0xdc, /* i2s8_tws_dbg */
            0x01, 0x18, 0x01, 0xbc, /* i2s8_data_in0_dbg */
            0x01, 0x1c, 0x01, 0xbc, /* i2s8_data_in1_dbg */
            0x01, 0x20, 0x01, 0xdc, /* i2s8_data_out0_dbg */
            0x01, 0x24, 0x01, 0xdc, /* i2s8_data_out1_dbg */
        })
        {
            SKY1_IOMUXC_I2S3_RSCK,
            SKY1_IOMUXC_I2S3_RWS,
            SKY1_IOMUXC_I2S3_TSCK,
            SKY1_IOMUXC_I2S3_TWS,
            SKY1_IOMUXC_I2S3_DATA_IN0,
            SKY1_IOMUXC_I2S3_DATA_IN1,
            SKY1_IOMUXC_I2S3_DATA_OUT0,
            SKY1_IOMUXC_I2S3_DATA_OUT1,
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
//            SKY1_IOMUXC_HDA_BITCLK,
//            SKY1_IOMUXC_HDA_RST_L,
//            SKY1_IOMUXC_HDA_SDIN0,
//            SKY1_IOMUXC_HDA_SDOUT0,
//            SKY1_IOMUXC_HDA_SYNC,
//            SKY1_IOMUXC_HDA_SDIN1,
//            SKY1_IOMUXC_HDA_SDOUT1,
//        }
    PinGroup ("pinctrl_alc5682_irq", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x02, 0x14, 0x00, 0x1c
        })
        {
            SKY1_IOMUXC_GMAC1_MDC
        }

    PinGroup ("pinctrl_fch_i3c0", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0x88, 0x00, 0xdc,
            0x00, 0x8c, 0x00, 0xdc,
            0x00, 0x90, 0x00, 0xdc
        })
        {
            SKY1_IOMUXC_I2C2_SCL,
            SKY1_IOMUXC_I2C2_SDA,
            SKY1_IOMUXC_I3C0_PUR_EN_L
        }

    PinGroup ("pinctrl_fch_i3c1", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0x94, 0x00, 0xdc,
            0x00, 0x98, 0x00, 0xdc,
            0x00, 0x9c, 0x00, 0xdc
        })
        {
            SKY1_IOMUXC_I2C3_CLK,
            SKY1_IOMUXC_I2C3_SDA,
            SKY1_IOMUXC_I3C1_PUR_EN_L
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
            0x00, 0x30, 0x00, 0x5c,
        })
        {
            SKY1_IOMUXC_GPIO13,
        }

    PinGroup ("i2c0_grp", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0x70, 0x00, 0x5c,
            0x00, 0x74, 0x00, 0x5c
        })
        {
            SKY1_IOMUXC_SFI_I2C0_SCL,
            SKY1_IOMUXC_SFI_I2C0_SDA
        }

    PinGroup ("i2c1_grp", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0x78, 0x00, 0x57,
            0x00, 0x7c, 0x00, 0x57
        })
        {
            SKY1_IOMUXC_SFI_I2C1_SCL,
            SKY1_IOMUXC_SFI_I2C1_SDA
        }

    PinGroup ("pinctrl_fch_spi0", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0xa8, 0x00, 0x5c,
            0x00, 0xac, 0x00, 0x5c,
            0x00, 0xb0, 0x00, 0x5c,
            0x00, 0xb4, 0x00, 0x5c,
            0x00, 0xb8, 0x00, 0x1c
        })
        {
            SKY1_IOMUXC_SPI1_MISO,
            SKY1_IOMUXC_SPI1_CS0,
            SKY1_IOMUXC_SPI1_CS1,
            SKY1_IOMUXC_SPI1_MOSI,
            SKY1_IOMUXC_SPI1_CLK
        }

    PinGroup ("pinctrl_fch_xspi", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0xf0, 0x00, 0xdc,
            0x00, 0xf4, 0x00, 0xdc,
            0x00, 0xf8, 0x00, 0xdc,
            0x00, 0xfc, 0x00, 0xdc,
            0x01, 0x00, 0x00, 0xdc,
            0x01, 0x04, 0x00, 0xdc
        })
        {
            SKY1_IOMUXC_SE_QSPI_CLK,
            SKY1_IOMUXC_SE_QSPI_CS_L,
            SKY1_IOMUXC_SE_QSPI_DATA0,
            SKY1_IOMUXC_SE_QSPI_DATA1,
            SKY1_IOMUXC_SE_QSPI_DATA2,
            SKY1_IOMUXC_SE_QSPI_DATA3
        }
    PinGroup ("pinctrl_usb0", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0xd4, 0x00, 0x44,
        })
        {
            SKY1_IOMUXC_USB_OC6_L,
        }
    PinGroup ("pinctrl_usb1", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0xd8, 0x00, 0x44,
        })
        {
            SKY1_IOMUXC_USB_OC7_L,
        }
    PinGroup ("pinctrl_usb2", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0xdc, 0x00, 0x44,
        })
        {
            SKY1_IOMUXC_USB_OC8_L,
        }
    PinGroup ("pinctrl_usb3", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0xe0, 0x00, 0x44,
        })
        {
            SKY1_IOMUXC_USB_OC9_L,
        }
    PinGroup ("pinctrl_usb4", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0xcc, 0x00, 0x44,
            0x00, 0xe8, 0x00, 0xa4,
        })
        {
            SKY1_IOMUXC_USB_OC4_L,
            SKY1_IOMUXC_DRIVE_VBUS4,
        }
    PinGroup ("pinctrl_usb5", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0xd0, 0x00, 0x44,
            0x00, 0xec, 0x00, 0xa4,
        })
        {
            SKY1_IOMUXC_USB_OC5_L,
            SKY1_IOMUXC_DRIVE_VBUS5,
        }
    PinGroup ("pinctrl_usb6", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0xbc, 0x00, 0x44,
        })
        {
            SKY1_IOMUXC_USB_OC0_L,
        }
    PinGroup ("pinctrl_usb7", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0xc0, 0x00, 0x44,
        })
        {
            SKY1_IOMUXC_USB_OC1_L,
        }
    PinGroup ("pinctrl_usb8", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0xc4, 0x00, 0x44,
        })
        {
            SKY1_IOMUXC_USB_OC2_L,
        }
    PinGroup ("pinctrl_usb9", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0xc8, 0x00, 0x44,
        })
        {
            SKY1_IOMUXC_USB_OC3_L,
        }

    PinGroup ("pinctrl_ra8900ce_irq", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0x28, 0x00, 0x44,
        })
        {
            SKY1_IOMUXC_GPIO11,
        }
    PinGroup ("pinctrl_pcie_x8_rc", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0x04, 0x00, 0x24,
        })
        {
            SKY1_IOMUXC_GPIO2,
        }
    PinGroup ("pinctrl_pcie_x4_rc", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0x0c, 0x00, 0x24,
        })
        {
            SKY1_IOMUXC_GPIO4,
        }
    PinGroup ("pinctrl_pcie_x2_rc", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0x08, 0x00, 0x24,
        })
        {
            SKY1_IOMUXC_GPIO3,
        }
    PinGroup ("pinctrl_pcie_x1_1_rc", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0x14, 0x00, 0x24,
        })
        {
            SKY1_IOMUXC_GPIO6,
        }
    PinGroup ("pinctrl_pcie_x1_0_rc", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0x10, 0x00, 0x24,
        })
        {
            SKY1_IOMUXC_GPIO5,
        }
    PinGroup ("wifi_vbat_gpio", ResourceProducer, ,
        RawDataBuffer ()
        {
            0x00, 0x30, 0x00, 0x44,
        })
        {
            SKY1_IOMUXC_GPIO13,
        }
  })
}
