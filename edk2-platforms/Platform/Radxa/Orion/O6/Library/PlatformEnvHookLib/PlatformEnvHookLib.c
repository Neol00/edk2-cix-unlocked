/**

  Copyright 2023 Cix Technology (Shanghai) Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/EcLib.h>
#include <Library/PinMuxLib.h>
#include <Library/PinMuxTableLib.h>
#include <Library/GpioLib.h>
#include <Library/GpioTableLib.h>
#include <Library/PlatformEnvHookLib.h>
#include <Library/TimerLib.h>
#include <Protocol/EcPlatformProtocol.h>
#include <Protocol/I2cDevicePath.h>
#include <Library/CixSipLib.h>
#include <Library/EcLib.h>
#include <Library/MailBoxLib.h>
#include <Guid/NetworkStackSetup.h>
#include <PlatformSetupVar.h>

// {3F7B73C7-FB70-4e91-86E7-34EAD76AC74D}
EFI_GUID  gEfiFarmEnableFlagGuid = {
  0x3f7b73c7, 0xfb70, 0x4e91, { 0x86, 0xe7, 0x34, 0xea, 0xd7, 0x6a, 0xc7, 0x4d }
};

GPIO_CFG  GpioCfgTable[] = {
  { 1,   INPUT,             INOUT_VALUE_DEFAULT, INTERRUPT_DISABLE,        EDGE_LOW               }, // EC_HID_INT_L
  { 2,   OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // SLT_X8_RST_L
  { 3,   OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // PERSTB2
  { 4,   OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // M2_NVME_RST_L
  { 5,   OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // M2_WLAN_RST_L
  { 6,   OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // LOM_RST_L
  { 7,   INPUT,             INOUT_VALUE_DEFAULT, INTERRUPT_DISABLE,        EDGE_LOW               }, // EC_ACPI_INT_L
  { 8,   OUTPUT,            INOUT_LOW,           INTERRUPT_TYPE_DEFAULT,   INTERRUPT_TYPE_DEFAULT }, // GPIO8(DEFAULT LOW, LED ON)
  { 9,   INPUT,             INOUT_VALUE_DEFAULT, INTERRUPT_DISABLE,        LEVEL_LOW              }, // I2C_INT_CPU_PD2
  { 10,  OUTPUT,            INOUT_HIGH,          INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // LOM_PWREN
  { 11,  INPUT,             INOUT_VALUE_DEFAULT, INTERRUPT_ENABLE_DEFAULT, LEVEL_LOW              }, // RTC_ALARM_L
  { 12,  OUTPUT,            INOUT_HIGH,          INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // M2_SSD_PWREN
  { 13,  OUTPUT,            INOUT_HIGH,          INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // WLAN_PWREN
  { 14,  INPUT,             INOUT_VALUE_DEFAULT, INTERRUPT_ENABLE_DEFAULT, LEVEL_LOW              }, // HP_MIC_DET#
  { 15,  OUTPUT,            INOUT_HIGH,          INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // PWR_LED_EN GPIO15 (DEFAULT HIGH, LED ON)
  { 16,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // SSD_LED_EN
  { 17,  OUTPUT,            INOUT_HIGH,          INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // VGFX_PWREN
  { 18,  INPUT,             INOUT_VALUE_DEFAULT, INTERRUPT_ENABLE_DEFAULT, LEVEL_LOW              }, // GPIO018_WAKE_SSD
  { 19,  OUTPUT,            INOUT_HIGH,          INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // WL_RADIO_DISABLE_L
  { 20,  OUTPUT,            INOUT_HIGH,          INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // BT_RADIO_DISABLE_L
  { 21,  OUTPUT,            INOUT_HIGH,          INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // USB02F_DRIVE_VBUS
  { 22,  OUTPUT,            INOUT_HIGH,          INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // 5GPHY2_PWR_EN
  { 30,  INPUT,             INOUT_VALUE_DEFAULT, INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GBE1_PEWAKE_L
  { 31,  INPUT,             INOUT_VALUE_DEFAULT, INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // SLT_X8_PEWAKE_L
  { 32,  INPUT,             INOUT_VALUE_DEFAULT, INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // WLAN_WAKE_L
  { 33,  INPUT,             INOUT_VALUE_DEFAULT, INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GBE2_PEWAKE_L
  { 40,  OUTPUT,            INOUT_HIGH,          INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // VBUS For USB Port6-7
  { 43,  OUTPUT,            INOUT_HIGH,          INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // BOOT_STRAP
  { 44,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO44 (DEFAULT LOW)
  { 45,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO45 (DEFAULT LOW)
  { 46,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO46 (DEFAULT LOW)
  { 55,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO55 (DEFAULT LOW)
  { 56,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO56 (DEFAULT LOW)
  { 71,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO71 (DEFAULT LOW)
  { 72,  OUTPUT,            INOUT_VALUE_DEFAULT, INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO72
  { 76,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO76 (DEFAULT LOW)
  { 78,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO78 (DEFAULT LOW)
  { 79,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO79 (DEFAULT LOW)
  { 80,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO80 (DEFAULT LOW)
  { 81,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO81 (DEFAULT LOW)
  { 82,  OUTPUT,            INOUT_HIGH,          INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // CAM2_PWDN
  { 83,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // CAM_PWREN
  { 84,  INPUT,             INOUT_VALUE_DEFAULT, INTERRUPT_ENABLE_DEFAULT, LEVEL_LOW              }, // TPM_INT_L
  { 85,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // CAM2_RST_L
  { 86,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // CAM1_PWDN
  { 87,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // CAM3_PWDN
  { 88,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // CAM0_RST_L
  { 89,  OUTPUT,            INOUT_HIGH,          INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // CAM3_PWDN
  { 90,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO90 (DEFAULT LOW)
  { 91,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO91 (DEFAULT LOW)
  { 92,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO92 (DEFAULT LOW)
  { 93,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO93 (DEFAULT LOW)
  { 94,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO94 (DEFAULT LOW)
  { 95,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO95 (DEFAULT LOW)
  { 96,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO96 (DEFAULT LOW)
  { 99,  OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO99 (DEFAULT LOW)
  { 100, OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO100 (DEFAULT LOW)
  { 129, OUTPUT,            INOUT_HIGH,          INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO129_TP_RST
  { 136, INPUT,             INOUT_VALUE_DEFAULT, INTERRUPT_ENABLE_DEFAULT, LEVEL_LOW              }, // TPNL_INT_L
  { 137, OUTPUT,            INOUT_VALUE_DEFAULT, INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO137
  { 138, OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO138 (DEFAULT LOW)
  { 139, OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO139 (DEFAULT LOW)
  { 140, OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO140 (DEFAULT LOW)
  { 141, OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO141 (DEFAULT LOW)
  { 142, OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // GPIO142 (DEFAULT LOW)
  { 143, INPUT,             INOUT_VALUE_DEFAULT, INTERRUPT_ENABLE_DEFAULT, LEVEL_LOW              }, // I2C_NVME_ALERT_L
  { 144, OUTPUT,            INOUT_LOW,           INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }, // AUDIO_PWR_EN
  // the end of GpioCfgTable, no more item allowed to add afterward
  { 0,   DIRECTION_DEFAULT, INOUT_VALUE_DEFAULT, INTERRUPT_ENABLE_DEFAULT, INTERRUPT_TYPE_DEFAULT }
};

PINMUX_CFG  PinMuxCfgTable[] = {
  { S5_DOMAIN,       IO_S5_USB_OC4_L,        IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // USBA_OC4_L
  { S5_DOMAIN,       IO_S5_USB_OC5_L,        IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // USBA_OC5_L
  { S5_DOMAIN,       IO_S5_USB_OC6_L,        IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // USBC_OC0_L
  { S5_DOMAIN,       IO_S5_USB_OC7_L,        IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // USBC_OC1_L
  { S5_DOMAIN,       IO_S5_USB_OC8_L,        IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // USBC_OC2_L
  { S5_DOMAIN,       IO_S5_USB_OC9_L,        IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // USBC_OC3_L
  //{ S5_DOMAIN,       IO_S5_USB_DRIVE_VBUS0,  IO_FUNC01,   PU_ENABLE,  PD_DISABLE, ST_DEFAULT, DRV_STREN_DEFAULT }, // USB_DRIVE_VBUS0
  { S5_DOMAIN,       IO_S5_USB_DRIVE_VBUS0,  IO_FUNC00,   PU_ENABLE,  PD_DISABLE, ST_DEFAULT, DRV_STREN_DEFAULT }, // GPIO VBUS For USB Port6-7
  { S5_DOMAIN,       IO_S5_USB_DRIVE_VBUS4,  IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // USB_DRIVE_VBUS4
  { S5_DOMAIN,       IO_S5_USB_DRIVE_VBUS5,  IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // USB_DRIVE_VBUS5
  { S0_DOMAIN,       IO_S0_GMAC1_RX_CTL,     IO_FUNC00,   PU_ENABLE,  PD_DISABLE, ST_DEFAULT, DRV_STREN_DEFAULT }, // I2C_NVME_ALERT_L
  { S0_DOMAIN,       IO_S0_GMAC1_RX_CLK,     IO_FUNC00,   PU_ENABLE,  PD_DISABLE, ST_DEFAULT, DRV_STREN_DEFAULT }, // I2C_WWAN_ALERT_L
  { S0_DOMAIN,       IO_S0_PM_GPIO0,         IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // PMIC0_I2C_INT_L
  { S0_DOMAIN,       IO_S0_PM_GPIO1,         IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // PMIC1_I2C_INT_L
  { S0_DOMAIN,       IO_S0_PM_GPIO2,         IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // PMIC2_I2C_INT_L
  // XSPI
  { S5_DOMAIN,       IO_S5_SE_QSPI_CLK,      IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT },
  { S5_DOMAIN,       IO_S5_SE_QSPI_CS_L,     IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT },
  { S5_DOMAIN,       IO_S5_SE_QSPI_DATA0,    IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT },
  { S5_DOMAIN,       IO_S5_SE_QSPI_DATA1,    IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT },
  { S5_DOMAIN,       IO_S5_SE_QSPI_DATA2,    IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT },
  { S5_DOMAIN,       IO_S5_SE_QSPI_DATA3,    IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT },
  { S5_DOMAIN,       IO_S5_GPIO001,          IO_FUNC00,   PU_DEFAULT, PD_DISABLE, ST_DEFAULT, DRV_STREN_DEFAULT }, // EC_HID_INT_L
  { S5_DOMAIN,       IO_S5_SFI_GPIO0,        IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // PWR_LED_EN
  { S5_DOMAIN,       IO_S5_SFI_GPIO1,        IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // SSD_LED_EN
  { S5_DOMAIN,       IO_S5_SFI_GPIO2,        IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // VGFX_PWREN
  { S5_DOMAIN,       IO_S5_SFI_GPIO3,        IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // GPIO018_WAKE_SSD
  { S5_DOMAIN,       IO_S5_SFI_GPIO4,        IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // WL_RADIO_DISABLE_L
  { S5_DOMAIN,       IO_S5_SFI_GPIO5,        IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // BT_RADIO_DISABLE_L
  { S5_DOMAIN,       IO_S5_SFI_GPIO6,        IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // USB02F_DRIVE_VBUS
  { S5_DOMAIN,       IO_S5_SFI_GPIO7,        IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // 5GPHY2_PWR_EN
  { S0_DOMAIN,       IO_S0_I2C2_SCL,         IO_FUNC02,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // GPOIO55
  { S0_DOMAIN,       IO_S0_I2C2_SDA,         IO_FUNC02,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // GPOIO56
  { S0_DOMAIN,       IO_S0_I3C0_PUR_EN_L,    IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // I3C0_PUR_EN_L
  { S0_DOMAIN,       IO_S0_I3C1_PUR_EN_L,    IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // I3C1_PUR_EN_L
  { S0_DOMAIN,       IO_S0_I2S2_RSCK,        IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // GPIO71
  { S0_DOMAIN,       IO_S0_I2S2_DATA_IN1,    IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // GPIO76
  { S0_DOMAIN,       IO_S0_I2S2_DATA_OUT1,   IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // GPIO78
  { S0_DOMAIN,       IO_S0_I2S2_DATA_OUT2,   IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // GPIO79
  { S0_DOMAIN,       IO_S0_I2S2_DATA_OUT3,   IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // GPIO80
  { S0_DOMAIN,       IO_S0_I2S3_MCLK,        IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // GPIO81
  { S0_DOMAIN,       IO_S0_I2S3_RSCK,        IO_FUNC01,   PU_ENABLE,  PD_DISABLE, ST_DEFAULT, DRV_STREN_DEFAULT }, // CAM2_PWDN
  { S0_DOMAIN,       IO_S0_I2S3_RWS,         IO_FUNC01,   PU_DISABLE, PD_ENABLE,  ST_DEFAULT, DRV_STREN_DEFAULT }, // CAM2_PWREN
  { S0_DOMAIN,       IO_S0_I2S3_TSCK,        IO_FUNC01,   PU_ENABLE,  PD_DISABLE, ST_DEFAULT, DRV_STREN_DEFAULT }, // TPM_INT_L
  { S0_DOMAIN,       IO_S0_I2S3_TWS,         IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // CAM2_RST_L
  { S0_DOMAIN,       IO_S0_I2S3_DATA_IN0,    IO_FUNC01,   PU_DISABLE, PD_ENABLE,  ST_DEFAULT, DRV_STREN_DEFAULT }, // CAM3_PWREN
  { S0_DOMAIN,       IO_S0_I2S3_DATA_IN1,    IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // CAM0_RST_L
  { S0_DOMAIN,       IO_S0_I2S3_DATA_OUT0,   IO_FUNC01,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // CAM3_RST_L
  { S0_DOMAIN,       IO_S0_I2S3_DATA_OUT1,   IO_FUNC01,   PU_ENABLE,  PD_DISABLE, ST_DEFAULT, DRV_STREN_DEFAULT }, // CAM3_PWDN
  { S0_DOMAIN,       IO_S0_I2S4_MCLK_LB,     IO_FUNC00,   PU_ENABLE,  PD_DISABLE, ST_DEFAULT, DRV_STREN_DEFAULT }, // CAM0_PWDN
  { S0_DOMAIN,       IO_S0_I2S4_SCK_LB,      IO_FUNC00,   PU_DISABLE, PD_ENABLE,  ST_DEFAULT, DRV_STREN_DEFAULT }, // CAM0_PWREN
  { S0_DOMAIN,       IO_S0_I2S4_WS_LB,       IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // CAM1_RST_L
  { S0_DOMAIN,       IO_S0_I2S4_DATA_IN_LB,  IO_FUNC00,   PU_DISABLE, PD_ENABLE,  ST_DEFAULT, DRV_STREN_DEFAULT }, // CAM1_PWREN
  { S0_DOMAIN,       IO_S0_I2S4_DATA_OUT_LB, IO_FUNC00,   PU_ENABLE,  PD_DISABLE, ST_DEFAULT, DRV_STREN_DEFAULT }, // CAM1_PWDN
  { S0_DOMAIN,       IO_S0_UART0_TXD,        IO_FUNC02,   PU_ENABLE,  PD_DISABLE, ST_DEFAULT, DRV_STREN_DEFAULT }, // GPIO95
  { S0_DOMAIN,       IO_S0_UART0_RXD,        IO_FUNC02,   PU_ENABLE,  PD_DISABLE, ST_DEFAULT, DRV_STREN_DEFAULT }, // GPIO96
  { S0_DOMAIN,       IO_S0_UART1_TXD,        IO_FUNC02,   PU_ENABLE,  PD_DISABLE, ST_DEFAULT, DRV_STREN_DEFAULT }, // GPIO99
  { S0_DOMAIN,       IO_S0_UART1_RXD,        IO_FUNC02,   PU_ENABLE,  PD_DISABLE, ST_DEFAULT, DRV_STREN_DEFAULT }, // GPIO100
  // NON_GMAC1
  { S0_DOMAIN,       IO_S0_GMAC1_REFCLK_25M, IO_FUNC00,   PU_ENABLE,  PD_DISABLE, ST_DEFAULT, DRV_STREN_DEFAULT }, // TPNL_INT_L
  { S0_DOMAIN,       IO_S0_GMAC1_TX_CTL,     IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // NFC_INT_L
  { S0_DOMAIN,       IO_S0_GMAC1_TXD0,       IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // GPIO138
  { S0_DOMAIN,       IO_S0_GMAC1_TXD1,       IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // GPIO139
  { S0_DOMAIN,       IO_S0_GMAC1_TXD2,       IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // GPIO140
  { S0_DOMAIN,       IO_S0_GMAC1_TXD3,       IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // GPIO141
  { S0_DOMAIN,       IO_S0_GMAC1_TX_CLK,     IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // GPIO142
  { S0_DOMAIN,       IO_S0_GMAC1_RXD0,       IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // PA0_PDB
  { S0_DOMAIN,       IO_S0_GMAC1_RXD1,       IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // PA1_PDB
  { S0_DOMAIN,       IO_S0_GMAC1_RXD2,       IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // PA2_PDB
  { S0_DOMAIN,       IO_S0_GMAC1_RXD3,       IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // PA3_PDB
  { S0_DOMAIN,       IO_S0_GMAC1_MDC,        IO_FUNC00,   PU_ENABLE,  PD_DISABLE, ST_DEFAULT, DRV_STREN_DEFAULT }, // I2S0_INT
  { S0_DOMAIN,       IO_S0_GMAC1_MDIO,       IO_FUNC00,   PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }, // I2S2_INT

  // the end of PinMuxCfgTable, no more item allowed to add afterward
  { UNDEFINE_DOMAIN, IO_S0_S5_UNDEFINED,     IO_UNDEFINE, PU_DEFAULT, PD_DEFAULT, ST_DEFAULT, DRV_STREN_DEFAULT }
};

EFI_STATUS
EFIAPI
PlatformEnvHookLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  return Status;
}

EFI_STATUS
EFIAPI
InitGpio (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  // Reset power signal incase of forced shutdown
  // Based on edk2-platforms/Platform/Radxa/Library/ArmPsciResetSystemLib/ArmPsciResetSystemLib.c
  GpioConfig(FixedPcdGet8(PcdPcieRootPort0PeResetPin), OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  GpioConfig(FixedPcdGet8(PcdPcieRootPort1PeResetPin), OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  GpioConfig(FixedPcdGet8(PcdPcieRootPort2PeResetPin), OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  GpioConfig(FixedPcdGet8(PcdPcieRootPort3PeResetPin), OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  GpioConfig(FixedPcdGet8(PcdPcieRootPort4PeResetPin), OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);

  GpioConfig(10, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT); //  output low of LOM_PWREN
  GpioConfig(12, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT); //  output low of M2_SSD_PWREN
  GpioConfig(13, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT); //  output low of WLAN_PWREN
  GpioConfig(17, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT); //  output low of VGFX_PWREN
  GpioConfig(21, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT); //  output low of USB02F_DRIVE_VBUS
  GpioConfig(22, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT); //  output low of 5GPHY2_PWR_EN
  GpioConfig(40, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT); //  output low of USB2_DRIVE_VBUS
  GpioConfig(41, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT); //  output low of USB_DRIVE_VBUS4
  GpioConfig(42, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT); //  output low of USB_DRIVE_VBUS5
  GpioConfig(81, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT); //  output low of CAM_PWREN

  // Wait 100ms to make sure the device is off
  MicroSecondDelay(100 * 1000);

  // Continue with the rest of GPIO initialization
  GpioInit (GpioCfgTable, ARRAY_SIZE (GpioCfgTable));

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
InitPinmux (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  PinMuxInit (PinMuxCfgTable, ARRAY_SIZE (PinMuxCfgTable));

  return EFI_SUCCESS;
}

VOID
EFIAPI
InstallRtcProtocol (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_HANDLE                *Handle;
  UINTN                     HandleNum;
  UINTN                     Index;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiI2cMasterProtocolGuid,
                  NULL,
                  &HandleNum,
                  &Handle
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "gEfiI2cMasterProtocolGuid not found yet, status: %r\n", Status));
  } else {
    for (Index = 0; Index < HandleNum; Index++) {
      Status = gBS->HandleProtocol (
                      Handle[Index],
                      &gEfiDevicePathProtocolGuid,
                      (VOID **)&DevicePath
                      );
      if (EFI_ERROR (Status)) {
        continue;
      } else {
        if ((DevicePath->Type == HARDWARE_DEVICE_PATH) && (DevicePath->SubType == HW_VENDOR_DP)) {
          if (CompareGuid (&(((VENDOR_DEVICE_PATH *)DevicePath)->Guid), &gCixI2cDevicePathGuid) &&
              (((I2C_DEVICE_PATH *)DevicePath)->Bus == FixedPcdGet8 (PcdRtc8563I2cBus)))
          {
            //
            // Install the ra8900ce I2C Master protocol on this handle so the RTC driver
            // can identify it as the I2C master it can invoke directly, rather than
            // through the I2C driver stack (which cannot be used at runtime)
            //
            Status = gBS->InstallProtocolInterface (
                            &Handle[Index],
                            &gPcf8563RealTimeClockLibI2cMasterProtocolGuid,
                            EFI_NATIVE_INTERFACE,
                            NULL
                            );
            ASSERT_EFI_ERROR (Status);
            if (Event != NULL) {
              gBS->CloseEvent (Event);
            }
          }
        }
      }
    }

    gBS->FreePool (Handle);
  }

  return;
}

EFI_STATUS
EFIAPI
UpdatePcdDmaDeviceLimit (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  EFI_STATUS  Status;
  UINT64      SystemMemSize  = 0;
  UINT64      DmaDeviceLimit = 0;

 #ifdef UPDATE_MEMORY_TOP
  UINT32  *pMemSizeAddr = (UINT32 *)(FixedPcdGet64 (PcdMemInfoBaseAddr) + 0x10);
  SystemMemSize   = *pMemSizeAddr;
  SystemMemSize <<= 20;
 #else
  SystemMemSize = FixedPcdGet64 (PcdSystemMemorySize);
 #endif
  DEBUG ((DEBUG_INFO, "[%a] SystemMemSize :%lx\n", __FUNCTION__, SystemMemSize));
  DmaDeviceLimit += (SystemMemSize + 0x80000000);
  DmaDeviceLimit -= 1;
  Status          = PcdSet64S (PcdDmaDeviceLimit, DmaDeviceLimit);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "PcdDmaDeviceLimit set fail status:%r\n", Status));
  } else {
    DEBUG ((DEBUG_INFO, "PcdDmaDeviceLimit set success\n"));
  }

  return Status;
}

EFI_STATUS
EFIAPI
WakeupSourceInit (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  DEBUG ((DEBUG_INFO, "%a\n",__func__));

  /*usb3 typec ports*/
  WakeupCfg(USB_C_SSP_0_HOST_IRQ,FALSE);
  WakeupCfg(USB_C_SSP_1_HOST_IRQ,FALSE);
  WakeupCfg(USB_C_SSP_2_HOST_IRQ,FALSE);
  WakeupCfg(USB_C_SSP_3_HOST_IRQ,FALSE);

  /*usb3 typea ports*/
  WakeupCfg(USB_SSP_0_HOST_IRQ,FALSE);
  WakeupCfg(USB_SSP_1_HOST_IRQ,FALSE);

  /*usb2 ports*/
  WakeupCfg(USB2_0_HOST_IRQ,FALSE);
  WakeupCfg(USB2_1_HOST_IRQ,FALSE);
  WakeupCfg(USB2_2_HOST_IRQ,FALSE);
  WakeupCfg(USB2_3_HOST_IRQ,FALSE);
  return Status;
}

EFI_STATUS
EFIAPI
OnboardDevicePowerOff (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  EC_PARAMS_GPIO  GpioInfo;

  DEBUG ((DEBUG_INFO, "[%a] EntryPoint\n", __FUNCTION__));

  if (ConfigData == NULL) {
    DebugPrint (DEBUG_ERROR, "ConfigData point is NULL");
    return EFI_OUT_OF_RESOURCES;
  }

  if (ConfigData->PlatConfig->GfxPower == 0) {
    // GPIO17
    DEBUG ((DEBUG_INFO, "GPIO17 Output Low GFX Power Off\n"));
    GpioConfig (17, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  } else {
    // GPIO17
    DEBUG ((DEBUG_INFO, "GPIO17 Output Low GFX Power On\n"));
    GpioConfig (17, OUTPUT, INOUT_HIGH, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  }

  if (ConfigData->PlatConfig->TouchPanelPower == 0) {
    // EC GPIO203
    DEBUG ((DEBUG_INFO, "EC GPIO203 Output Low TouchPanelPower Power Off\n"));
    GpioInfo.GpioNum = 203;
    GpioInfo.GpioVal = 0;
    SetGpio (&GpioInfo);
  } else {
    // EC GPIO203
    DEBUG ((DEBUG_INFO, "EC GPIO203 Output High TouchPanelPower Power On\n"));
    GpioInfo.GpioNum = 203;
    GpioInfo.GpioVal = 1;
    SetGpio (&GpioInfo);
  }

  if (ConfigData->PlatConfig->TpmPower == 0) {
    // EC GPIO74
    DEBUG ((DEBUG_INFO, "EC GPIO74 Output Low TpmPower Power Off\n"));
    GpioInfo.GpioNum = 74;
    GpioInfo.GpioVal = 0;
    SetGpio (&GpioInfo);
  } else {
    // EC GPIO74
    DEBUG ((DEBUG_INFO, "EC GPIO74 Output High TpmPower Power On\n"));
    GpioInfo.GpioNum = 74;
    GpioInfo.GpioVal = 1;
    SetGpio (&GpioInfo);
  }

  if (ConfigData->PlatConfig->WwanPower == 0) {
    // EC GPIO215
    DEBUG ((DEBUG_INFO, "EC GPIO215 Output Low WwanPower Power Off\n"));
    GpioInfo.GpioNum = 215;
    GpioInfo.GpioVal = 0;
    SetGpio (&GpioInfo);
  } else {
    // EC GPIO215
    DEBUG ((DEBUG_INFO, "EC GPIO215 Output High WwanPower Power On\n"));
    GpioInfo.GpioNum = 215;
    GpioInfo.GpioVal = 1;
    SetGpio (&GpioInfo);
  }

  if (ConfigData->PlatConfig->PcieX2SlotPower == 0) {
    // EC GPIO214
    DEBUG ((DEBUG_INFO, "EC GPIO214 Output Low PcieX2SlotPower Power Off\n"));
    GpioInfo.GpioNum = 214;
    GpioInfo.GpioVal = 0;
    SetGpio (&GpioInfo);
  } else {
    // EC GPIO214
    DEBUG ((DEBUG_INFO, "EC GPIO214 Output High PcieX2SlotPower Power On\n"));
    GpioInfo.GpioNum = 214;
    GpioInfo.GpioVal = 1;
    SetGpio (&GpioInfo);
  }

  if (ConfigData->PlatConfig->FingerPrintPower == 0) {
    // EC GPIO107
    DEBUG ((DEBUG_INFO, "EC GPIO107 Output Low FingerPrintPower Power Off\n"));
    GpioInfo.GpioNum = 107;
    GpioInfo.GpioVal = 0;
    SetGpio (&GpioInfo);
  } else {
    // EC GPIO107
    DEBUG ((DEBUG_INFO, "EC GPIO107 Output High FingerPrintPower Power On\n"));
    GpioInfo.GpioNum = 107;
    GpioInfo.GpioVal = 1;
    SetGpio (&GpioInfo);
  }

  if (ConfigData->PlatConfig->OnBoardLanPower == 0) {
    // GPIO010
    DEBUG ((DEBUG_INFO, "GPIO010 Output Low OnBoardLan0Power Power Off\n"));
    GpioConfig (10, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
    // GPIO022
    DEBUG ((DEBUG_INFO, "GPIO022 Output Low OnBoardLan1Power Power Off\n"));
    GpioConfig (22, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  } else {
    // GPIO010
    DEBUG ((DEBUG_INFO, "GPIO010 Output High OnBoardLan0Power Power On\n"));
    GpioConfig (10, OUTPUT, INOUT_HIGH, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
    // GPIO022
    DEBUG ((DEBUG_INFO, "GPIO022 Output High OnBoardLan1Power Power On\n"));
    GpioConfig (22, OUTPUT, INOUT_HIGH, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  }

  if (ConfigData->PlatConfig->WlanPower == 0) {
    // GPIO13
    DEBUG ((DEBUG_INFO, "GPIO13 Output Low WlanPower Power Off\n"));
    GpioConfig (13, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  } else {
    // GPIO13
    DEBUG ((DEBUG_INFO, "GPIO13 Output High WlanPower Power On\n"));
    GpioConfig (13, OUTPUT, INOUT_HIGH, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  }

  if (ConfigData->PlatConfig->M2SsdPower == 0) {
    // GPIO12
    DEBUG ((DEBUG_INFO, "GPIO12 Output Low M2SsdPower Power Off\n"));
    GpioConfig (12, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  } else {
    // GPIO12
    DEBUG ((DEBUG_INFO, "GPIO12 Output High M2SsdPower Power On\n"));
    GpioConfig (12, OUTPUT, INOUT_HIGH, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  }

  if (ConfigData->PlatConfig->IspCamera0Power == 0) {
    // GPIO91
    DEBUG ((DEBUG_INFO, "GPIO91 Output Low IspCamera0Power Power Off\n"));
    GpioConfig (91, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  } else {
    // GPIO91
    DEBUG ((DEBUG_INFO, "GPIO91 Output High IspCamera0Power Power On\n"));
    GpioConfig (91, OUTPUT, INOUT_HIGH, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  }

  if (ConfigData->PlatConfig->IspCamera1Power == 0) {
    // GPIO93
    DEBUG ((DEBUG_INFO, "GPIO93 Output Low IspCamera1Power Power Off\n"));
    GpioConfig (93, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  } else {
    // GPIO93
    DEBUG ((DEBUG_INFO, "GPIO93 Output High IspCamera1Power Power On\n"));
    GpioConfig (93, OUTPUT, INOUT_HIGH, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  }

  if (ConfigData->PlatConfig->IspCamera2Power == 0) {
    // GPIO83
    DEBUG ((DEBUG_INFO, "GPIO83 Output Low IspCamera2Power Power Off\n"));
    GpioConfig (83, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  } else {
    // GPIO83
    DEBUG ((DEBUG_INFO, "GPIO83 Output High IspCamera2Power Power On\n"));
    GpioConfig (83, OUTPUT, INOUT_HIGH, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  }

  if (ConfigData->PlatConfig->IspCamera3Power == 0) {
    // GPIO86
    DEBUG ((DEBUG_INFO, "GPIO86 Output Low IspCamera3Power Power Off\n"));
    GpioConfig (86, OUTPUT, INOUT_LOW, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  } else {
    // GPIO86
    DEBUG ((DEBUG_INFO, "GPIO86 Output High IspCamera3Power Power On\n"));
    GpioConfig (86, OUTPUT, INOUT_HIGH, INTERRUPT_DISABLE, INTERRUPT_TYPE_DEFAULT);
  }

  return Status;
}

EFI_STATUS
EFIAPI
SetStateAfterG3 (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  EFI_STATUS                         Status;
  EC_PARAMS_EC_AUTO_POWER_ON  AutoPowerOnInfo;

  AutoPowerOnInfo.EcAutoPowerOnFlg = ConfigData->PlatConfig->StateAfterG3;
  Status                               = SetECAutoPowerOn (&AutoPowerOnInfo);
  if (EFI_ERROR (Status)) {
    DebugPrint (DEBUG_ERROR, "SetECAutoPowerOn Status:%r\n", Status);
  }

  return Status;
}

EFI_STATUS
EFIAPI
RtcWakupEnable (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  if (ConfigData->PlatConfig->RtcWakeup) {
    GpioConfig (11, INPUT, INOUT_VALUE_DEFAULT, INTERRUPT_ENABLE, LEVEL_LOW);
  } else {
    GpioConfig (11, INPUT, INOUT_VALUE_DEFAULT, INTERRUPT_DISABLE, LEVEL_LOW);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FarmFunctionControl (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  EFI_STATUS                Status = EFI_SUCCESS;
  UINTN                     VarSize;
  NETWORK_STACK             NetworkStack;
  PLATFORM_SETUP_DATA       PlatformSetupVar;
  EC_RESPONSE_FARM_ID       FarmIdInfo;
  UINT8                     FarmEnableFlag;

  Status = GetFarmId (&FarmIdInfo);
  if (EFI_ERROR (Status)) {
    DebugPrint (DEBUG_ERROR, "[%a] [%d]Get Farm Id info fail.\n", __FUNCTION__, __LINE__);
    return Status;
  }

  if (FarmIdInfo.Id == 0) {
    DebugPrint (DEBUG_INFO, "Farm Function Enable.\n");
    VarSize = sizeof (NETWORK_STACK);
    Status  = gRT->GetVariable (
                     NETWORK_STACK_VAR,
                     &gEfiNetworkStackSetupGuid,
                     NULL,
                     &VarSize,
                     &NetworkStack
                     );
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "[%a] [%d]Get Network Stack Variable fail.\n", __FUNCTION__, __LINE__);
      return Status;
    }

    NetworkStack.Enable   = 1;
    NetworkStack.Ipv4Pxe  = 1;
    NetworkStack.Ipv6Pxe  = 1;
    NetworkStack.Ipv4Http = 1;
    NetworkStack.Ipv6Http = 1;
    Status                = gRT->SetVariable (
                                   NETWORK_STACK_VAR,
                                   &gEfiNetworkStackSetupGuid,
                                   EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                   sizeof (NETWORK_STACK),
                                   &NetworkStack
                                   );
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "[%a] [%d]Set Network Stack Variable fail.\n", __FUNCTION__, __LINE__);
      return Status;
    }

    VarSize = sizeof (PLATFORM_SETUP_DATA);
    Status  = gRT->GetVariable (
                     PLATFORM_SETUP_VAR,
                     &gPlatformSetupVariableGuid,
                     NULL,
                     &VarSize,
                     &PlatformSetupVar
                     );
    PlatformSetupVar.SocWatchdogTimer = 0;
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "[%a] [%d]Get Platform Setup Variable fail.\n", __FUNCTION__, __LINE__);
      return Status;
    }

    Status = gRT->SetVariable (
                    PLATFORM_SETUP_VAR,
                    &gPlatformSetupVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof (PLATFORM_SETUP_DATA),
                    &PlatformSetupVar
                    );
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "[%a] [%d]Set Platform Setup Variable fail.\n", __FUNCTION__, __LINE__);
      return Status;
    }

    ConfigData->SocConfig->S5.SocWatchdogTimer = PlatformSetupVar.SocWatchdogTimer; // Update ConfigData parameter
    FarmEnableFlag                             = 1;
    Status                                     = gRT->SetVariable (
                                                        L"FarmEnableFlag",
                                                        &gEfiFarmEnableFlagGuid,
                                                        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                                        sizeof (UINT8),
                                                        &FarmEnableFlag
                                                        );
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "[%a] [%d]Set Farm Enable Flag Variable fail.\n", __FUNCTION__, __LINE__);
      return Status;
    }
  } else {
    DebugPrint (DEBUG_INFO, "Farm Function Disable.\n");
    VarSize =  sizeof (UINT8);
    Status  = gRT->GetVariable (
                     L"FarmEnableFlag",
                     &gEfiFarmEnableFlagGuid,
                     NULL,
                     &VarSize,
                     &FarmEnableFlag
                     );
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "[%a] [%d]Get Farm Enable Flag Variable Status%r.\n", __FUNCTION__, __LINE__, Status);
      return Status;
    }

    if (FarmEnableFlag == 1) {
      VarSize = sizeof (NETWORK_STACK);
      Status  = gRT->GetVariable (
                       NETWORK_STACK_VAR,
                       &gEfiNetworkStackSetupGuid,
                       NULL,
                       &VarSize,
                       &NetworkStack
                       );
      if (EFI_ERROR (Status)) {
        DebugPrint (DEBUG_ERROR, "[%a] [%d]Get Network Stack Variable fail.\n", __FUNCTION__, __LINE__);
        return Status;
      }

      NetworkStack.Enable   = FixedPcdGet8 (PcdNetworkStackSupport);
      NetworkStack.Ipv4Pxe  = FixedPcdGet8 (PcdIPv4PXESupport);
      NetworkStack.Ipv6Pxe  = FixedPcdGet8 (PcdIPv6PXESupport);
      NetworkStack.Ipv4Http = FixedPcdGet8 (PcdIPv4HttpSupport);
      NetworkStack.Ipv6Http = FixedPcdGet8 (PcdIPv6HttpSupport);
      Status                = gRT->SetVariable (
                                     NETWORK_STACK_VAR,
                                     &gEfiNetworkStackSetupGuid,
                                     EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                     sizeof (NETWORK_STACK),
                                     &NetworkStack
                                     );
      if (EFI_ERROR (Status)) {
        DebugPrint (DEBUG_ERROR, "[%a] [%d]Set Network Stack Variable fail.\n", __FUNCTION__, __LINE__);
        return Status;
      }

      VarSize = sizeof (PLATFORM_SETUP_DATA);
      Status  = gRT->GetVariable (
                       PLATFORM_SETUP_VAR,
                       &gPlatformSetupVariableGuid,
                       NULL,
                       &VarSize,
                       &PlatformSetupVar
                       );
      PlatformSetupVar.SocWatchdogTimer = FixedPcdGet8 (PcdSocWatchdogTimer);
      if (EFI_ERROR (Status)) {
        DebugPrint (DEBUG_ERROR, "[%a] [%d]Get Platform Setup Variable fail.\n", __FUNCTION__, __LINE__);
        return Status;
      }

      Status = gRT->SetVariable (
                      PLATFORM_SETUP_VAR,
                      &gPlatformSetupVariableGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                      sizeof (PLATFORM_SETUP_DATA),
                      &PlatformSetupVar
                      );
      if (EFI_ERROR (Status)) {
        DebugPrint (DEBUG_ERROR, "[%a] [%d]Set Platform Setup Variable fail.\n", __FUNCTION__, __LINE__);
        return Status;
      }

      ConfigData->SocConfig->S5.SocWatchdogTimer = PlatformSetupVar.SocWatchdogTimer; // Update ConfigData parameter
      FarmEnableFlag                             = 0;
      Status                                     = gRT->SetVariable (
                                                          L"FarmEnableFlag",
                                                          &gEfiFarmEnableFlagGuid,
                                                          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                                          sizeof (UINT8),
                                                          &FarmEnableFlag
                                                          );
      if (EFI_ERROR (Status)) {
        DebugPrint (DEBUG_ERROR, "[%a] [%d]Set Farm Enable Flag Variable fail.\n", __FUNCTION__, __LINE__);
        return Status;
      }
    }
  }

  return Status;
}

#ifdef STMM_SUPPORT
EFI_STATUS
EFIAPI
FenceFchXspiHost (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  EFI_STATUS                     Status;
  MBOX_GASKET_FENCING_PARAMETER  Params;
  MBOX_GASKET_FENCING_RESPONSE   Respon;

  Params.Id = FCH_XSPI_RANGE_ID;
  Status    = MboxEnableGasketFencing (&Params, &Respon);
  DEBUG ((DEBUG_INFO, "[%a] enable FCH XSPI gasket fencing status %r, response error code %d\n", __FUNCTION__, Status, Respon.ErrCode));

  return Status;
}
#endif

VOID
EFIAPI
InitEcDefaultSetting (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  EC_PLATFORM_PROTOCOL        *Ec;
  EC_PARAM                    Params;
  ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData;

  ConfigData = (ENV_HOOK_PARAMS_DATA_BLOCK *)Context;

  Status = gBS->LocateProtocol (&gCixEcPlatformProtocolGuid, NULL, (VOID **)&Ec);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "EC platform not ready.\n"));
    return;
  }

  switch (ConfigData->PlatConfig->EcFanMode) {
    case 0: // auto mode
      Params.ThermalAutoFanCtl.FanAutoFlg = EC_FAN_MODE_AUTO;
      break;
    case 1: // performance mode
      Params.ThermalAutoFanCtl.FanAutoFlg = EC_FAN_MODE_PERF;
      break;
    case 2: // quiet mode
      Params.ThermalAutoFanCtl.FanAutoFlg = EC_FAN_MODE_MUTE;
      break;
    default:
      DEBUG ((DEBUG_INFO, "Undefined EC fan mode! mode=%d \n", ConfigData->PlatConfig->EcFanMode));
      return;
  }

  Status = Ec->Transfer (Ec, EC_COMMAND_SET_THERMAL_AUTO_FAN_CTL, &Params, NULL);
  DEBUG ((DEBUG_INFO, "EC Set Fan control mode Status:%r\n", Status));
}

STATIC PLATFORM_ENV_INIT_TABLE  mPlatformEnvInitTable[] = {
  { NULL,                        NULL,                 InitPinmux              },
  { NULL,                        NULL,                 InitGpio                },
  { NULL,                        NULL,                 UpdatePcdDmaDeviceLimit },
  { NULL,                        NULL,                 WakeupSourceInit        },
  { NULL,                        NULL,                 OnboardDevicePowerOff   },
  { NULL,                        NULL,                 SetStateAfterG3         },
  { NULL,                        NULL,                 RtcWakupEnable          },
  { NULL,                        NULL,                 FarmFunctionControl     },
  { &gEfiI2cMasterProtocolGuid,  InstallRtcProtocol,   NULL                    },
  { &gCixEcPlatformProtocolGuid, InitEcDefaultSetting, NULL                    },
#ifdef STMM_SUPPORT
  { NULL,                        NULL,                 FenceFchXspiHost        },
 #endif
  // add platform initialization routines on ENV phase BEFORE this line, and they were invoked from top to down.
  { NULL,                        NULL,                 NULL                    }
};

STATIC UINT32  mPlatformEnvInitTableSize = ARRAY_SIZE (mPlatformEnvInitTable);

VOID
EFIAPI
PlatformEnvHook (
  IN OUT ENV_HOOK_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32      Index  = 0;
  VOID        *Registration;

  if (ConfigData == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: platform ENV hook routine failed to get config data\n", __FUNCTION__));
    return;
  }

  while (Index < mPlatformEnvInitTableSize) {
    if (mPlatformEnvInitTable[Index].Guid == NULL) {
      if (mPlatformEnvInitTable[Index].Init != NULL) {
        Status = mPlatformEnvInitTable[Index].Init (ConfigData);
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "%a: platform ENV hook routine failed at index %d, status %r\n", __FUNCTION__, Index, Status));
        }
      }
    } else {
      EfiCreateProtocolNotifyEvent (
        mPlatformEnvInitTable[Index].Guid,
        TPL_CALLBACK,
        mPlatformEnvInitTable[Index].Notify,
        ConfigData,
        &Registration
        );
    }

    Index++;
  }
}
