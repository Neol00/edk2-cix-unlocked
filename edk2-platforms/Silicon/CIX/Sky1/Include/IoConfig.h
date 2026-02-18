/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef __IO_CONFIG_H__
#define __IO_CONFIG_H__

typedef enum _IO_DOMAIN {
  S0_DOMAIN       = 0,
  S5_DOMAIN       = 1,
  UNDEFINE_DOMAIN = 0xFF
} IO_DOMAIN;

typedef enum _IO_PULL_UP_SEL {
  PU_DISABLE = 0,
  PU_ENABLE  = 1,
  PU_DEFAULT = 0xFF
} IO_PULL_UP_SEL;

typedef enum _IO_PULL_DOWN_SEL {
  PD_DISABLE = 0,
  PD_ENABLE  = 1,
  PD_DEFAULT = 0xFF
} IO_PULL_DOWN_SEL;

typedef enum _IO_SCH_TRIG_SEL {
  ST_DISABLE = 0,
  ST_ENABLE  = 1,
  ST_DEFAULT = 0xFF
} IO_SCH_TRIG_SEL;

typedef enum _IO_DRV_STREN_SEL {
  DRV_STREN_0       = 1,
  DRV_STREN_1       = 2,
  DRV_STREN_2       = 4,
  DRV_STREN_3       = 8,
  DRV_STREN_DEFAULT = 0xFF
} IO_DRV_STREN_SEL;

typedef enum _IO_GPIO_BANK_SEL {
  GPIO_BANK0         = 0, // S5 GPIO BANK 0
  GPIO_BANK1         = 1, // S5 GPIO BANK 1
  GPIO_BANK2         = 2, // S5 GPIO BANK 2, reserved
  GPIO_BANK3         = 3, // S0 GPIO BANK 0
  GPIO_BANK4         = 4, // S0 GPIO BANK 1
  GPIO_BANK5         = 5, // S0 GPIO BANK 2
  GPIO_BANK6         = 6, // S0 GPIO BANK 3
  UNDEFINE_GPIO_BANK = 0xFF
} IO_GPIO_BANK_SEL;

// typedef enum _IO_MODE_SEL {
//   GPIO_MODE = 0,
//   BYPASS_MODE  = 1,
//   DEFAULT_MODE = 0xFF
// } IO_MODE_SEL;

typedef enum _IO_DIRECTION_SEL {
  OUTPUT            = 0,
  INPUT             = 1,
  DIRECTION_DEFAULT = 0xFF
} IO_DIRECTION_SEL;

// typedef enum _IO_OUTPUT_ENABLE_SEL {
//   OUTPUT_DISABLE = 0,
//   OUTPUT_ENABLE = 1,
//   OUTPUT_ENABLE_DEFAULT = 0xFF
// } IO_OUTPUT_ENABLE_SEL;

typedef enum _IO_OUTPUT_VALUE_SEL {
  INOUT_LOW           = 0,
  INOUT_HIGH          = 1,
  INOUT_VALUE_DEFAULT = 0xFF
} IO_INOUT_VALUE_SEL;

typedef enum _IO_INTERRUPT_ENABLE_SEL {
  INTERRUPT_DISABLE        = 0,
  INTERRUPT_ENABLE         = 1,
  INTERRUPT_ENABLE_DEFAULT = 0xFF
} IO_INTERRUPT_ENABLE_SEL;

typedef enum _IO_INTERRUPT_TYPE_SEL {
  LEVEL_HIGH             = 0,
  LEVEL_LOW              = 1,
  EDGE_HIGH              = 2,
  EDGE_LOW               = 3,
  ANY_EDGE               = 4,
  INTERRUPT_TYPE_DEFAULT = 0xFF
} IO_INTERRUPT_TYPE_SEL;

#define IO_FUNC00    0
#define IO_FUNC01    1
#define IO_FUNC02    2
#define IO_FUNC03    3
#define IO_UNDEFINE  0xFF

// S0 Domain
#define IO_S0_GPIO043           0x000
#define IO_S0_GPIO044           0x004
#define IO_S0_GPIO045           0x008
#define IO_S0_GPIO046           0x00C
#define IO_S0_RESET_IN_L        0x010
#define IO_S0_PLT_RESET_L       0x014
#define IO_S0_THERMTRIP_L       0x018
#define IO_S0_PROCHOT_L         0x01C
#define IO_S0_PM_I2C0_CLK       0x020
#define IO_S0_PM_I2C0_DATA      0x024
#define IO_S0_PM_I2C1_CLK       0x028
#define IO_S0_PM_I2C1_DATA      0x02C
#define IO_S0_PM_I2C2_CLK       0x030
#define IO_S0_PM_I2C2_DATA      0x034
#define IO_S0_PM_I2C3_CLK       0x038
#define IO_S0_PM_I2C3_DATA      0x03C
#define IO_S0_STRAP0            0x040
#define IO_S0_STRAP1            0x044
#define IO_S0_DP2_DIGON         0x048
#define IO_S0_DP2_BLON          0x04C
#define IO_S0_DP2_VARY_BL       0x050
#define IO_S0_I2C7_SCL          0x054
#define IO_S0_I2C7_SDA          0x058
#define IO_S0_UART6_CSU_SE_TXD  0x05C
#define IO_S0_CLK_REQ1_L        0x060
#define IO_S0_CLK_REQ3_L        0x064
#define IO_S0_I2C5_SCL          0x068
#define IO_S0_I2C5_SDA          0x06C
#define IO_S0_I2C6_SCL          0x070
#define IO_S0_I2C6_SDA          0x074
#define IO_S0_I2C0_CLK          0x078
#define IO_S0_I2C0_SDA          0x07C
#define IO_S0_I2C1_CLK          0x080
#define IO_S0_I2C1_SDA          0x084
#define IO_S0_I2C2_SCL          0x088
#define IO_S0_I2C2_SDA          0x08C
#define IO_S0_I3C0_PUR_EN_L     0x090
#define IO_S0_I2C3_CLK          0x094
#define IO_S0_I2C3_SDA          0x098
#define IO_S0_I3C1_PUR_EN_L     0x09C
#define IO_S0_I2C4_CLK          0x0A0
#define IO_S0_I2C4_SDA          0x0A4
#define IO_S0_HDA_BITCLK        0x0A8
#define IO_S0_HDA_RST_L         0x0AC
#define IO_S0_HDA_SDIN0         0x0B0
#define IO_S0_HDA_SDOUT0        0x0B4
#define IO_S0_HDA_SYNC          0x0B8
#define IO_S0_HDA_SDIN1         0x0BC
#define IO_S0_HDA_SDOUT1        0x0C0
#define IO_S0_I2S1_MCLK         0x0C4
#define IO_S0_I2S1_SCK          0x0C8
#define IO_S0_I2S1_WS           0x0CC
#define IO_S0_I2S1_DATA_IN      0x0D0
#define IO_S0_I2S1_DATA_OUT     0x0D4
#define IO_S0_I2S2_MCLK         0x0D8
#define IO_S0_I2S2_RSCK         0x0DC
#define IO_S0_I2S2_RWS          0x0E0
#define IO_S0_I2S2_TSCK         0x0E4
#define IO_S0_I2S2_TWS          0x0E8
#define IO_S0_I2S2_DATA_IN0     0x0EC
#define IO_S0_I2S2_DATA_IN1     0x0F0
#define IO_S0_I2S2_DATA_OUT0    0x0F4
#define IO_S0_I2S2_DATA_OUT1    0x0F8
#define IO_S0_I2S2_DATA_OUT2    0x0FC
#define IO_S0_I2S2_DATA_OUT3    0x100
#define IO_S0_I2S3_MCLK         0x104
#define IO_S0_I2S3_RSCK         0x108
#define IO_S0_I2S3_RWS          0x10C
#define IO_S0_I2S3_TSCK         0x110
#define IO_S0_I2S3_TWS          0x114
#define IO_S0_I2S3_DATA_IN0     0x118
#define IO_S0_I2S3_DATA_IN1     0x11C
#define IO_S0_I2S3_DATA_OUT0    0x120
#define IO_S0_I2S3_DATA_OUT1    0x124
#define IO_S0_I2S4_MCLK_LB      0x128
#define IO_S0_I2S4_SCK_LB       0x12C
#define IO_S0_I2S4_WS_LB        0x130
#define IO_S0_I2S4_DATA_IN_LB   0x134
#define IO_S0_I2S4_DATA_OUT_LB  0x138
#define IO_S0_UART0_TXD         0x13C
#define IO_S0_UART0_RXD         0x140
#define IO_S0_UART0_CTS         0x144
#define IO_S0_UART0_RTS         0x148
#define IO_S0_UART1_TXD         0x14C
#define IO_S0_UART1_RXD         0x150
#define IO_S0_UART1_CTS         0x154
#define IO_S0_UART1_RTS         0x158
#define IO_S0_UART2_TXD         0x15C
#define IO_S0_UART2_RXD         0x160
#define IO_S0_UART3_TXD         0x164
#define IO_S0_UART3_RXD         0x168
#define IO_S0_UART3_CTS         0x16C
#define IO_S0_UART3_RTS         0x170
#define IO_S0_UART4_CSU_PM_TXD  0x174
#define IO_S0_UART4_CSU_PM_RXD  0x178
#define IO_S0_UART5_CSU_SE_TXD  0x17C
#define IO_S0_UART5_CSU_SE_RXD  0x180
#define IO_S0_UART6_CSU_SE_RXD  0x184
#define IO_S0_CLK_REQ0_L        0x188
#define IO_S0_CLK_REQ2_L        0x18C
#define IO_S0_CLK_REQ4_L        0x190
#define IO_S0_CSI0_MCLK0        0x194
#define IO_S0_CSI0_MCLK1        0x198
#define IO_S0_CSI1_MCLK0        0x19C
#define IO_S0_CSI1_MCLK1        0x1A0
#define IO_S0_GMAC0_REFCLK_25M  0x1A4
#define IO_S0_GMAC0_TX_CTL      0x1A8
#define IO_S0_GMAC0_TXD0        0x1AC
#define IO_S0_GMAC0_TXD1        0x1B0
#define IO_S0_GMAC0_TXD2        0x1B4
#define IO_S0_GMAC0_TXD3        0x1B8
#define IO_S0_GMAC0_TX_CLK      0x1BC
#define IO_S0_GMAC0_RX_CTL      0x1C0
#define IO_S0_GMAC0_RXD0        0x1C4
#define IO_S0_GMAC0_RXD1        0x1C8
#define IO_S0_GMAC0_RXD2        0x1CC
#define IO_S0_GMAC0_RXD3        0x1D0
#define IO_S0_GMAC0_RX_CLK      0x1D4
#define IO_S0_GMAC0_MDC         0x1D8
#define IO_S0_GMAC0_MDIO        0x1DC
#define IO_S0_GMAC1_REFCLK_25M  0x1E0
#define IO_S0_GMAC1_TX_CTL      0x1E4
#define IO_S0_GMAC1_TXD0        0x1E8
#define IO_S0_GMAC1_TXD1        0x1EC
#define IO_S0_GMAC1_TXD2        0x1F0
#define IO_S0_GMAC1_TXD3        0x1F4
#define IO_S0_GMAC1_TX_CLK      0x1F8
#define IO_S0_GMAC1_RX_CTL      0x1FC
#define IO_S0_GMAC1_RXD0        0x200
#define IO_S0_GMAC1_RXD1        0x204
#define IO_S0_GMAC1_RXD2        0x208
#define IO_S0_GMAC1_RXD3        0x20C
#define IO_S0_GMAC1_RX_CLK      0x210
#define IO_S0_GMAC1_MDC         0x214
#define IO_S0_GMAC1_MDIO        0x218
#define IO_S0_PM_GPIO0          0x21C
#define IO_S0_PM_GPIO1          0x220
#define IO_S0_PM_GPIO2          0x224

// S5 Domain
#define IO_S5_GPIO001          0x000
#define IO_S5_GPIO002          0x004
#define IO_S5_GPIO003          0x008
#define IO_S5_GPIO004          0x00C
#define IO_S5_GPIO005          0x010
#define IO_S5_GPIO006          0x014
#define IO_S5_GPIO007          0x018
#define IO_S5_GPIO008          0x01C
#define IO_S5_GPIO009          0x020
#define IO_S5_GPIO010          0x024
#define IO_S5_GPIO011          0x028
#define IO_S5_GPIO012          0x02C
#define IO_S5_GPIO013          0x030
#define IO_S5_GPIO014          0x034
#define IO_S5_RSMRST_L         0x038
#define IO_S5_SRST_L           0x03C
#define IO_S5_SLP_S3_L         0x040
#define IO_S5_SLP_S5_L         0x044
#define IO_S5_PWRGD            0x048
#define IO_S5_PWROK            0x04C
#define IO_S5_PWRBTN_L         0x050
#define IO_S5_VDD_DDRIO_GATE   0x054
#define IO_S5_JTAG_GPIO_L      0x058
#define IO_S5_JTAG_TCK         0x05C
#define IO_S5_JTAG_TDI         0x060
#define IO_S5_JTAG_TDO         0x064
#define IO_S5_JTAG_TMS         0x068
#define IO_S5_JTAG_TRST_L      0x06C
#define IO_S5_SFI_I2C0_SCL     0x070
#define IO_S5_SFI_I2C0_SDA     0x074
#define IO_S5_SFI_I2C1_SCL     0x078
#define IO_S5_SFI_I2C1_SDA     0x07C
#define IO_S5_SFI_GPIO0        0x080
#define IO_S5_SFI_GPIO1        0x084
#define IO_S5_SFI_GPIO2        0x088
#define IO_S5_SFI_GPIO3        0x08C
#define IO_S5_SFI_GPIO4        0x090
#define IO_S5_SFI_GPIO5        0x094
#define IO_S5_SFI_GPIO6        0x098
#define IO_S5_SFI_GPIO7        0x09C
#define IO_S5_SFI_GPIO8        0x0A0
#define IO_S5_SFI_GPIO9        0x0A4
#define IO_S5_SPI1_MISO        0x0A8
#define IO_S5_SPI1_CS0         0x0AC
#define IO_S5_SPI1_CS1         0x0B0
#define IO_S5_SPI1_MOSI        0x0B4
#define IO_S5_SPI1_CLK         0x0B8
#define IO_S5_USB_OC0_L        0x0BC
#define IO_S5_USB_OC1_L        0x0C0
#define IO_S5_USB_OC2_L        0x0C4
#define IO_S5_USB_OC3_L        0x0C8
#define IO_S5_USB_OC4_L        0x0CC
#define IO_S5_USB_OC5_L        0x0D0
#define IO_S5_USB_OC6_L        0x0D4
#define IO_S5_USB_OC7_L        0x0D8
#define IO_S5_USB_OC8_L        0x0DC
#define IO_S5_USB_OC9_L        0x0E0
#define IO_S5_USB_DRIVE_VBUS0  0x0E4
#define IO_S5_USB_DRIVE_VBUS4  0x0E8
#define IO_S5_USB_DRIVE_VBUS5  0x0EC
#define IO_S5_SE_QSPI_CLK      0x0F0
#define IO_S5_SE_QSPI_CS_L     0x0F4
#define IO_S5_SE_QSPI_DATA0    0x0F8
#define IO_S5_SE_QSPI_DATA1    0x0FC
#define IO_S5_SE_QSPI_DATA2    0x100
#define IO_S5_SE_QSPI_DATA3    0x104

#define IO_S0_S5_UNDEFINED  0xFFFF

// S5 GPIO0
#define BITMAP_GPIO001  BIT0
#define BITMAP_GPIO002  BIT1
#define BITMAP_GPIO003  BIT2
#define BITMAP_GPIO004  BIT3
#define BITMAP_GPIO005  BIT4
#define BITMAP_GPIO006  BIT5
#define BITMAP_GPIO007  BIT6
#define BITMAP_GPIO008  BIT7
#define BITMAP_GPIO009  BIT8
#define BITMAP_GPIO010  BIT9
#define BITMAP_GPIO011  BIT10
#define BITMAP_GPIO012  BIT11
#define BITMAP_GPIO013  BIT12
#define BITMAP_GPIO014  BIT13
#define BITMAP_GPIO025  BIT14
#define BITMAP_GPIO026  BIT15
#define BITMAP_GPIO027  BIT16
#define BITMAP_GPIO028  BIT17
#define BITMAP_GPIO029  BIT18
#define BITMAP_GPIO030  BIT19
#define BITMAP_GPIO031  BIT20
#define BITMAP_GPIO032  BIT21
#define BITMAP_GPIO033  BIT22
#define BITMAP_GPIO034  BIT23
#define BITMAP_GPIO035  BIT24
#define BITMAP_GPIO036  BIT25
#define BITMAP_GPIO037  BIT26
#define BITMAP_GPIO038  BIT27
#define BITMAP_GPIO039  BIT28
#define BITMAP_GPIO040  BIT29
#define BITMAP_GPIO041  BIT30
#define BITMAP_GPIO042  BIT31

#define NUM_OF_GPIO0  32

// S5 GPIO1

#define BITMAP_GPIO015  BIT0
#define BITMAP_GPIO016  BIT1
#define BITMAP_GPIO017  BIT2
#define BITMAP_GPIO018  BIT3
#define BITMAP_GPIO019  BIT4
#define BITMAP_GPIO020  BIT5
#define BITMAP_GPIO021  BIT6
#define BITMAP_GPIO022  BIT7
#define BITMAP_GPIO023  BIT8
#define BITMAP_GPIO024  BIT9

#define NUM_OF_GPIO1  10
#define NUM_OF_GPIO2  0

// S0 GPIO0
#define BITMAP_GPIO043  BIT0
#define BITMAP_GPIO044  BIT1
#define BITMAP_GPIO045  BIT2
#define BITMAP_GPIO046  BIT3
#define BITMAP_GPIO047  BIT4
#define BITMAP_GPIO048  BIT5
#define BITMAP_GPIO049  BIT6
#define BITMAP_GPIO050  BIT7
#define BITMAP_GPIO051  BIT8
#define BITMAP_GPIO052  BIT9
#define BITMAP_GPIO053  BIT10
#define BITMAP_GPIO054  BIT11
#define BITMAP_GPIO055  BIT12
#define BITMAP_GPIO056  BIT13
#define BITMAP_GPIO057  BIT14
#define BITMAP_GPIO058  BIT15
#define BITMAP_GPIO059  BIT16
#define BITMAP_GPIO060  BIT17
#define BITMAP_GPIO061  BIT18
#define BITMAP_GPIO062  BIT19
#define BITMAP_GPIO063  BIT20
#define BITMAP_GPIO064  BIT21
#define BITMAP_GPIO065  BIT22
#define BITMAP_GPIO066  BIT23
#define BITMAP_GPIO067  BIT24
#define BITMAP_GPIO068  BIT25
#define BITMAP_GPIO069  BIT26
#define BITMAP_GPIO070  BIT27
#define BITMAP_GPIO071  BIT28
#define BITMAP_GPIO072  BIT29
#define BITMAP_GPIO073  BIT30
#define BITMAP_GPIO074  BIT31

#define NUM_OF_GPIO3  32

// S0 GPIO1
#define BITMAP_GPIO075  BIT0
#define BITMAP_GPIO076  BIT1
#define BITMAP_GPIO077  BIT2
#define BITMAP_GPIO078  BIT3
#define BITMAP_GPIO079  BIT4
#define BITMAP_GPIO080  BIT5
#define BITMAP_GPIO081  BIT6
#define BITMAP_GPIO082  BIT7
#define BITMAP_GPIO083  BIT8
#define BITMAP_GPIO084  BIT9
#define BITMAP_GPIO085  BIT10
#define BITMAP_GPIO086  BIT11
#define BITMAP_GPIO087  BIT12
#define BITMAP_GPIO088  BIT13
#define BITMAP_GPIO089  BIT14
#define BITMAP_GPIO090  BIT15
#define BITMAP_GPIO091  BIT16
#define BITMAP_GPIO092  BIT17
#define BITMAP_GPIO093  BIT18
#define BITMAP_GPIO094  BIT19
#define BITMAP_GPIO095  BIT20
#define BITMAP_GPIO096  BIT21
#define BITMAP_GPIO097  BIT22
#define BITMAP_GPIO098  BIT23
#define BITMAP_GPIO099  BIT24
#define BITMAP_GPIO100  BIT25
#define BITMAP_GPIO101  BIT26
#define BITMAP_GPIO102  BIT27
#define BITMAP_GPIO103  BIT28
#define BITMAP_GPIO104  BIT29
#define BITMAP_GPIO105  BIT30
#define BITMAP_GPIO106  BIT31

#define NUM_OF_GPIO4  32

// S0 GPIO2
#define BITMAP_GPIO107  BIT0
#define BITMAP_GPIO108  BIT1
#define BITMAP_GPIO109  BIT2
#define BITMAP_GPIO110  BIT3
#define BITMAP_GPIO111  BIT4
#define BITMAP_GPIO112  BIT5
#define BITMAP_GPIO113  BIT6
#define BITMAP_GPIO114  BIT7
#define BITMAP_GPIO115  BIT8
#define BITMAP_GPIO116  BIT9
#define BITMAP_GPIO117  BIT10
#define BITMAP_GPIO118  BIT11
#define BITMAP_GPIO119  BIT12
#define BITMAP_GPIO120  BIT13
#define BITMAP_GPIO121  BIT14
#define BITMAP_GPIO122  BIT15
#define BITMAP_GPIO123  BIT16
#define BITMAP_GPIO124  BIT17
#define BITMAP_GPIO125  BIT18
#define BITMAP_GPIO126  BIT19
#define BITMAP_GPIO127  BIT20
#define BITMAP_GPIO128  BIT21
#define BITMAP_GPIO129  BIT22
#define BITMAP_GPIO130  BIT23
#define BITMAP_GPIO131  BIT24
#define BITMAP_GPIO132  BIT25
#define BITMAP_GPIO133  BIT26
#define BITMAP_GPIO134  BIT27
#define BITMAP_GPIO135  BIT28
#define BITMAP_GPIO136  BIT29
#define BITMAP_GPIO137  BIT30
#define BITMAP_GPIO138  BIT31

#define NUM_OF_GPIO5  32

// S0 GPIO3
#define BITMAP_GPIO139    BIT0
#define BITMAP_GPIO140    BIT1
#define BITMAP_GPIO141    BIT2
#define BITMAP_GPIO142    BIT3
#define BITMAP_GPIO143    BIT4
#define BITMAP_GPIO144    BIT5
#define BITMAP_GPIO145    BIT6
#define BITMAP_GPIO146    BIT7
#define BITMAP_GPIO147    BIT8
#define BITMAP_GPIO148    BIT9
#define BITMAP_GPIO149    BIT10
#define BITMAP_GPIO150    BIT11
#define BITMAP_GPIO151    BIT12
#define BITMAP_GPIO152    BIT13
#define BITMAP_GPIO153    BIT14
#define BITMAP_DP2_BLON   BIT15
#define BITMAP_DP2_DIGON  BIT16

#define NUM_OF_GPIO6  17

#define BITMAP_UNDEFINED  0

#define NUM_OF_GPIOS  (NUM_OF_GPIO0 + NUM_OF_GPIO1 + NUM_OF_GPIO2 + NUM_OF_GPIO3 + NUM_OF_GPIO4 + NUM_OF_GPIO5 + NUM_OF_GPIO6)

#define GPIO_ACTIVE_HIGH  0
#define GPIO_ACTIVE_LOW   1

#endif
