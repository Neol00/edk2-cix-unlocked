/** @file

  @copyright
  Copyright 2020 - 2021 Intel Corporation. <BR>
  Copyright (c) 2021 - 2022, American Megatrends International LLC. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "PeiBoardInit.h"
#include <Library/UbaGpioUpdateLib.h>

#include <Library/GpioLib.h>
#include <Library/UbaGpioInitLib.h>
#include <GpioPinsSklH.h>
#include <Library/PcdLib.h>

//
// Board     : Aowanda
//
static GPIO_INIT_CONFIG mGpioTableAowanda [] =
  {
// Group A AWD
    {GPIO_SKL_H_GPP_A0,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_0_PU_IRQ_ESPI_ALERT1_N
    {GPIO_SKL_H_GPP_A1,  { GpioPadModeNative3, GpioHostOwnDefault,    GpioDirInOut,    GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_1_ESPI_IO0
    {GPIO_SKL_H_GPP_A2,  { GpioPadModeNative3, GpioHostOwnDefault,    GpioDirInOut,    GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_2_ESPI_IO1
    {GPIO_SKL_H_GPP_A3,  { GpioPadModeNative3, GpioHostOwnDefault,    GpioDirInOut,    GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_3_ESPI_IO2
    {GPIO_SKL_H_GPP_A4,  { GpioPadModeNative3, GpioHostOwnDefault,    GpioDirInOut,    GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_4_ESPI_IO3
    {GPIO_SKL_H_GPP_A5,  { GpioPadModeNative3, GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_5_ESPI_CS0_N
    {GPIO_SKL_H_GPP_A6,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_6_PU_ESPI_CS1_N
    {GPIO_SKL_H_GPP_A7,  { GpioPadModeNative3, GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_7_IRQ_ESPI_ALERT0_N
    {GPIO_SKL_H_GPP_A8,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_8_PU_LPC_CLKRUN_N
    {GPIO_SKL_H_GPP_A9,  { GpioPadModeNative3, GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_9_ESPI_CLK
    {GPIO_SKL_H_GPP_A10, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_10_TP_PCH_GPP_A10
    {GPIO_SKL_H_GPP_A11, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_11_PU_LPC_PME_N
    {GPIO_SKL_H_GPP_A12, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_12_PU_IRQ_PCH_SCI_WHEA_N
    {GPIO_SKL_H_GPP_A13, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_13_TP_PCH_GPP_A13
    {GPIO_SKL_H_GPP_A14, { GpioPadModeNative3, GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_14_RST_ESPI_RESET_N
    {GPIO_SKL_H_GPP_A15, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_15_PU_SUSACK_N
    {GPIO_SKL_H_GPP_A16, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_16_TP_PCH_GPP_A16
    {GPIO_SKL_H_GPP_A17, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_17_TP_PCH_GPP_A17
    {GPIO_SKL_H_GPP_A18, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_18_TP_PCH_GPP_A18
    //ME recovery jumper
    //{GPIO_SKL_H_GPP_A19, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_19_FM_ME_RCVR_N
    {GPIO_SKL_H_GPP_A20, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_20_TP_PCH_GPP_A20
    {GPIO_SKL_H_GPP_A21, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_21_TP_PCH_GPP_A21
    {GPIO_SKL_H_GPP_A22, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_22_TP_PCH_GPP_A22
    {GPIO_SKL_H_GPP_A23, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_A_23_TP_PCH_GPP_A23

// Group B AWD
    {GPIO_SKL_H_GPP_B0,  { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_0_FM_PCH_CORE_VID<0>
    {GPIO_SKL_H_GPP_B1,  { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_1_FM_PCH_CORE_VID<1>
    {GPIO_SKL_H_GPP_B2,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_2_PU_PCH_VRALERT_N
    {GPIO_SKL_H_GPP_B3,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_3_FM_BIOS_ENTER_SETUP_N
    {GPIO_SKL_H_GPP_B4,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_4_FM_BIOS_POST_START_N
    {GPIO_SKL_H_GPP_B5,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_5_FM_OCP3_BIF_READY
    {GPIO_SKL_H_GPP_B6,  { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_6_FM_CLKREQ_M2_SSD1_N
    {GPIO_SKL_H_GPP_B7,  { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_7_FM_CLKREQ_M2_SSD2_N
    {GPIO_SKL_H_GPP_B8,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_8_PU_GPP_PCH_B8
    {GPIO_SKL_H_GPP_B9,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_9_FM_BOARD_REV_ID2
    {GPIO_SKL_H_GPP_B10, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_10_FM_TPM_PRSNT_N
    {GPIO_SKL_H_GPP_B11, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_11_FM_PMBUS_ALERT_BUF_EN_N
    {GPIO_SKL_H_GPP_B12, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_12_TP_PCH_GPP_B12
    {GPIO_SKL_H_GPP_B13, { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_13_RST_PLTRST_N
    {GPIO_SKL_H_GPP_B14, { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_14_FM_PCH_BIOS_RCVR_SPKR
    {GPIO_SKL_H_GPP_B15, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_15_FM_CPU_ERR0_PCH_N
    {GPIO_SKL_H_GPP_B16, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_16_FM_CPU_ERR1_PCH_N
    {GPIO_SKL_H_GPP_B17, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_17_FM_CPU_ERR2_PCH_N
    {GPIO_SKL_H_GPP_B18, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_18_FM_NO_REBOOT
    {GPIO_SKL_H_GPP_B19, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_19_FM_BOARD_SKU_ID5
    {GPIO_SKL_H_GPP_B20, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_20_FM_BIOS_POST_CMPLT_N
    {GPIO_SKL_H_GPP_B21, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_21_FM_FAST_PROCHOT_EN_N
    {GPIO_SKL_H_GPP_B22, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_22_FM_OCP3_FRU
    {GPIO_SKL_H_GPP_B23, { GpioPadModeNative2, GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_B_23_FM_PCH_BMC_THRMTRIP_EXI_STRAP_N

// Group C AWD
    {GPIO_SKL_H_GPP_C0,  { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirInOut,    GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_0_SMB4_HOST_STBY_BMC_LVC3_SCL
    {GPIO_SKL_H_GPP_C1,  { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirInOut,    GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_1_SMB4_HOST_STBY_BMC_LVC3_SDA
    {GPIO_SKL_H_GPP_C2,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_2_PU_PCH_TLS_ENABLE_STRAP_PCH
    {GPIO_SKL_H_GPP_C3,  { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirInOut,    GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_3_SMB6_SMLINK0_STBY_LVC3_SCL
    {GPIO_SKL_H_GPP_C4,  { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirInOut,    GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_4_SMB6_SMLINK0_STBY_LVC3_SDA
    {GPIO_SKL_H_GPP_C5,  { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_5_IRQ_SML0_ALERT_N
    {GPIO_SKL_H_GPP_C6,  { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirInOut,    GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_6_SMB8_PMBUS_SML1_STBY_LVC3_SCL
    {GPIO_SKL_H_GPP_C7,  { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirInOut,    GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_7_SMB8_PMBUS_SML1_STBY_LVC3_SDA
    {GPIO_SKL_H_GPP_C8,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_8_FM_PASSWORD_CLEAR_N
    {GPIO_SKL_H_GPP_C9,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_9_FM_MFG_MODE
    {GPIO_SKL_H_GPP_C10, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_10_FM_SATA_RAID_KEY
    {GPIO_SKL_H_GPP_C11, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_11_TP_PCH_GPP_C11
    {GPIO_SKL_H_GPP_C12, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_12_FM_BOARD_REV_ID0
    {GPIO_SKL_H_GPP_C13, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_13_FM_BOARD_REV_ID1
    {GPIO_SKL_H_GPP_C14, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_14_FM_BMC_PCH_SCI_LPC_N
    {GPIO_SKL_H_GPP_C15, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_15_FM_RISER1_ID_0
    {GPIO_SKL_H_GPP_C16, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_16_FM_RISER1_ID_1
    {GPIO_SKL_H_GPP_C17, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_17_FM_RISER1_ID_2
    {GPIO_SKL_H_GPP_C18, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_18_FM_RISER2_ID_0
    {GPIO_SKL_H_GPP_C19, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_19_FM_RISER2_ID_1
    // ME PROCHOT
    //{GPIO_SKL_H_GPP_C20, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_20_FM_THROTTLE_N
    {GPIO_SKL_H_GPP_C21, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_21_RST_PCH_PCIE_SMB_MUX_NX1
    {GPIO_SKL_H_GPP_C22, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_22_IRQ_BMC_PCH_SMI_LPC_N
    {GPIO_SKL_H_GPP_C23, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_C_23_FM_CPU_CATERR_DLY_LVT3_R_N

// Group D AWD
    {GPIO_SKL_H_GPP_D0,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntLevel | GpioIntNmi, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_0_IRQ_BMC_PCH_NMI
    {GPIO_SKL_H_GPP_D1,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_1_TP_PCH_GPP_D1
    {GPIO_SKL_H_GPP_D2,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_2_TP_PCH_GPP_D2
    {GPIO_SKL_H_GPP_D3,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_3_TP_PCH_GPP_D3
    {GPIO_SKL_H_GPP_D4,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_4_FM_PLD_PCH_DATA
    {GPIO_SKL_H_GPP_D5,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_5_FM_OCP3_BIF0
    {GPIO_SKL_H_GPP_D6,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_6_FM_OCP3_BIF1
    {GPIO_SKL_H_GPP_D7,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_7_FM_OCP3_BIF2
    {GPIO_SKL_H_GPP_D8,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_8_TP_PCH_GPP_D8
    {GPIO_SKL_H_GPP_D9,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_9_IRQ_FORCE_NM_THROTTLE_N
    {GPIO_SKL_H_GPP_D10, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_10_TP_PCH_GPP_D10
    {GPIO_SKL_H_GPP_D11, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_11_IRQ_LOM_ALERT_N
    {GPIO_SKL_H_GPP_D12, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_12_PU_PCH_GPP_D12
    {GPIO_SKL_H_GPP_D13, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_13_PU_PCH_GPP_D13
    {GPIO_SKL_H_GPP_D14, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_14_PU_PCH_GPP_D14
    {GPIO_SKL_H_GPP_D15, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_15_PU_PCH_GPP_D15
    {GPIO_SKL_H_GPP_D16, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_16_FM_ME_PFR_1
    {GPIO_SKL_H_GPP_D17, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_17_FM_ME_PFR_2
    {GPIO_SKL_H_GPP_D18, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_18_PU_PCH_GPP_D18
    {GPIO_SKL_H_GPP_D19, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_19_FM_PS_PWROK_DLY_SEL_R
    {GPIO_SKL_H_GPP_D20, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_20_FM_OCP3_PRSNTB0_N
    {GPIO_SKL_H_GPP_D21, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_21_FM_OCP3_PRSNTB1_N
    {GPIO_SKL_H_GPP_D22, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_22_FM_OCP3_PRSNTB2_N
    {GPIO_SKL_H_GPP_D23, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_D_23_FM_OCP3_PRSNTB3_N

// Group E AWD
    {GPIO_SKL_H_GPP_E0,  { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_E_0_FM_M2_SSD1_PEDET
    // ME Heartbeat
    //{GPIO_SKL_H_GPP_E1,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_E_1_FM_ME_HEARTBEAT_N
    {GPIO_SKL_H_GPP_E2,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_E_2_TP_PCH_GPP_E2
    {GPIO_SKL_H_GPP_E3,  { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_E_3_FM_ADR_TRIGGER_R_N
    {GPIO_SKL_H_GPP_E4,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_E_4_FM_HSC_TIMER_EXP_N
    {GPIO_SKL_H_GPP_E5,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_E_5_TP_PCH_GPP_E5
    {GPIO_SKL_H_GPP_E6,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_E_6_TP_PCH_GPP_E6
    {GPIO_SKL_H_GPP_E7,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_E_7_FM_ADR_SMI_GPIO_R_N
    {GPIO_SKL_H_GPP_E8,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_E_8_TP_PCH_GPP_E8
    {GPIO_SKL_H_GPP_E9,  { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_E_9_OC_PCH_USB_P01_N
    {GPIO_SKL_H_GPP_E10, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_E_10_TP_PCH_GPP_E10
    {GPIO_SKL_H_GPP_E11, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_E_11_PU_PCH_GPP_E11
    {GPIO_SKL_H_GPP_E12, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_E_12_IRQ_UV_DETECT_N

// Group F AWD
    {GPIO_SKL_H_GPP_F0,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_0_IRQ_OC_DETECT_N
    {GPIO_SKL_H_GPP_F1,  { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_1_FM_M2_SSD2_PEDET
    {GPIO_SKL_H_GPP_F2,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_2_FM_PCIE_SLOT1_PRSNT_N
    {GPIO_SKL_H_GPP_F3,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_3_FM_PCIE_SLOT2_PRSNT_N
    {GPIO_SKL_H_GPP_F4,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_4_FM_PCIE_SLOT3_PRSNT_N
    {GPIO_SKL_H_GPP_F5,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_5_IRQ_TPM_SPI_N
    {GPIO_SKL_H_GPP_F6,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_6_FM_EDSFF0_PRSNT_N
    {GPIO_SKL_H_GPP_F7,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_7_FM_EDSFF1_PRSNT_N
    {GPIO_SKL_H_GPP_F8,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_8_FM_EDSFF2_PRSNT_N
    {GPIO_SKL_H_GPP_F9,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_9_FM_EDSFF3_PRSNT_N
    {GPIO_SKL_H_GPP_F10, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_10_PU_PCH_GPP_F10
    {GPIO_SKL_H_GPP_F11, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_11_PU_PCH_GPP_F11
    {GPIO_SKL_H_GPP_F12, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_12_PU_PCH_GPP_F12
    {GPIO_SKL_H_GPP_F13, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_13_PU_PCH_GPP_F13
    {GPIO_SKL_H_GPP_F14, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_14_PU_PCH_GPP_F14
    {GPIO_SKL_H_GPP_F15, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_15_FM_FORCE_ADR_N
    {GPIO_SKL_H_GPP_F16, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_16_PU_PCH_GPP_F16
    {GPIO_SKL_H_GPP_F17, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_17_PU_PCH_GPP_F17
    {GPIO_SKL_H_GPP_F18, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_18_PU_PCH_GPP_F18
    {GPIO_SKL_H_GPP_F19, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_19_PU_PCH_GPP_F19
    {GPIO_SKL_H_GPP_F20, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_20_PU_PCH_GPP_F20
    {GPIO_SKL_H_GPP_F21, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_21_PU_PCH_GPP_F21
    {GPIO_SKL_H_GPP_F22, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_22_PU_PCH_GPP_F22
    {GPIO_SKL_H_GPP_F23, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_F_23_PU_PCH_GPP_F23

// Group G AWD
    {GPIO_SKL_H_GPP_G0,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_0_TP_PCH_GPP_G0
    {GPIO_SKL_H_GPP_G1,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_1_TP_PCH_GPP_G1
    {GPIO_SKL_H_GPP_G2,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_2_TP_PCH_GPP_G2
    {GPIO_SKL_H_GPP_G3,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_3_TP_PCH_GPP_G3
    {GPIO_SKL_H_GPP_G4,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_4_TP_PCH_GPP_G4
    {GPIO_SKL_H_GPP_G5,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_5_TP_PCH_GPP_G5
    {GPIO_SKL_H_GPP_G6,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_6_TP_PCH_GPP_G6
    {GPIO_SKL_H_GPP_G7,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_7_TP_PCH_GPP_G7
    {GPIO_SKL_H_GPP_G8,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_8_TP_PCH_GPP_G8
    {GPIO_SKL_H_GPP_G9,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_9_TP_PCH_GPP_G9
    {GPIO_SKL_H_GPP_G10, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_10_TP_PCH_GPP_G10
    {GPIO_SKL_H_GPP_G11, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_11_TP_PCH_GPP_G11
    {GPIO_SKL_H_GPP_G12, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_12_FM_BOARD_SKU_ID0
    {GPIO_SKL_H_GPP_G13, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_13_FM_BOARD_SKU_ID1
    {GPIO_SKL_H_GPP_G14, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_14_FM_BOARD_SKU_ID2
    {GPIO_SKL_H_GPP_G15, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_15_FM_BOARD_SKU_ID3
    {GPIO_SKL_H_GPP_G16, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_16_FM_BOARD_SKU_ID4
    {GPIO_SKL_H_GPP_G17, { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_17_FM_ADR_COMPLETE
    {GPIO_SKL_H_GPP_G18, { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_18_IRQ_NMI_EVENT_N
    {GPIO_SKL_H_GPP_G19, { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_19_IRQ_SMI_ACTIVE_N
    {GPIO_SKL_H_GPP_G20, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_20_IRQ_SML1_PMBUS_PCH_ALERT_N
    {GPIO_SKL_H_GPP_G21, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_21_PU_PCH_GPP_G21
    {GPIO_SKL_H_GPP_G22, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_22_TP_PCH_GPP_G22
    {GPIO_SKL_H_GPP_G23, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_G_23_TP_PCH_GPP_G23

// Group H AWD
    {GPIO_SKL_H_GPP_H0,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_0_TP_PCH_GPP_H0
    {GPIO_SKL_H_GPP_H1,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_1_TP_PCH_GPP_H1
    {GPIO_SKL_H_GPP_H2,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_2_TP_PCH_GPP_H2
    {GPIO_SKL_H_GPP_H3,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_3_M2_SSD1_PRSNT_N
    {GPIO_SKL_H_GPP_H4,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_4_M2_SSD2_PRSNT_N
    {GPIO_SKL_H_GPP_H5,  { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_5_FM_OCP3_CLK0_EN_R_N
    {GPIO_SKL_H_GPP_H6,  { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_6_FM_OCP3_CLK1_EN_R_N
    {GPIO_SKL_H_GPP_H7,  { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_7_FM_OCP3_CLK2_EN_R_N
    {GPIO_SKL_H_GPP_H8,  { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_8_FM_OCP3_CLK3_EN_R_N
    {GPIO_SKL_H_GPP_H9,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_9_TP_PCH_GPP_H9
    {GPIO_SKL_H_GPP_H10, { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_10_SMB11_SMLINK2_STBY_LVC3_SCL
    {GPIO_SKL_H_GPP_H11, { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirInOut,    GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_11_SMB11_SMLINK2_STBY_LVC3_SDA
    {GPIO_SKL_H_GPP_H12, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_12_FM_ESPI_FLASH_MODE
    {GPIO_SKL_H_GPP_H13, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_13_PU_PCH_GPP_H13
    {GPIO_SKL_H_GPP_H14, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_14_PU_PCH_GPP_H14
    {GPIO_SKL_H_GPP_H15, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_15_PU_ADR_TIMER_HOLD_OFF_N
    {GPIO_SKL_H_GPP_H16, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_16_PU_PCH_GPP_H16
    {GPIO_SKL_H_GPP_H17, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_17_PU_PCH_GPP_H17
    {GPIO_SKL_H_GPP_H18, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_18_FM_LT_KEY_DOWNGRADE_N
    {GPIO_SKL_H_GPP_H19, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_19_TP_PCH_GPP_H19
    {GPIO_SKL_H_GPP_H20, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_20_TP_PCH_GPP_H20
    {GPIO_SKL_H_GPP_H21, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_21_TP_PCH_GPP_H21
    {GPIO_SKL_H_GPP_H22, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_22_TP_PCH_GPP_H22
    {GPIO_SKL_H_GPP_H23, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_H_23_TP_PCH_GPP_H23

// Group I AWD
    {GPIO_SKL_H_GPP_I0,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_I_0_TP_PCH_GPP_I0
    {GPIO_SKL_H_GPP_I1,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_I_1_TP_PCH_GPP_I1
    {GPIO_SKL_H_GPP_I2,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_I_2_TP_PCH_GPP_I2
    {GPIO_SKL_H_GPP_I3,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_I_3_TP_PCH_GPP_I3
    {GPIO_SKL_H_GPP_I4,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_I_4_TP_PCH_GPP_I4
    {GPIO_SKL_H_GPP_I5,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_I_5_TP_PCH_GPP_I5
    {GPIO_SKL_H_GPP_I6,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_I_6_TP_PCH_GPP_I6
    {GPIO_SKL_H_GPP_I7,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_I_7_TP_PCH_GPP_I7
    {GPIO_SKL_H_GPP_I8,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_I_8_TP_PCH_GPP_I8
    {GPIO_SKL_H_GPP_I9,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_I_9_TP_PCH_GPP_I9
    {GPIO_SKL_H_GPP_I10, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_I_10_TP_PCH_GPP_I10
//    {GPIO_SKL_H_GPP_I11, { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_I_11_PD_3P3_RCOMP

// Group GPD AWD
    {GPIO_SKL_H_GPD0,    { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPD_0_FM_FIVRBREAK_N
    {GPIO_SKL_H_GPD1,    { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPD_1_PU_ACPRESENT_PCH
    {GPIO_SKL_H_GPD2,    { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPD_2_IRQ_HSC_FAULT_N
    {GPIO_SKL_H_GPD3,    { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPD_3_FM_PCH_PWRBTN_N
    {GPIO_SKL_H_GPD4,    { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPD_4_FM_SLPS3_N
    {GPIO_SKL_H_GPD5,    { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPD_5_FM_SLPS4_N
    {GPIO_SKL_H_GPD6,    { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPD_6_TP_SLPA_N
    {GPIO_SKL_H_GPD7,    { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPD_7_TP_GPD_7
    {GPIO_SKL_H_GPD8,    { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPD_8_TP_CLK_33K_PCH_SUSCLK
    {GPIO_SKL_H_GPD9,    { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPD_9_TP_GPD_9
    {GPIO_SKL_H_GPD10,   { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirOut,      GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPD_10_TP_SLPS5_N
    //{GPIO_SKL_H_GPD10,   { GpioPadModeGpio,    GpioHostOwnGpio,     GpioDirIn,    GpioOutDefault, GpioIntDis, GpioResetPwrGood,    GpioTermNone,    GpioPadConfigLock}},//GPD_10_TP_GPD_10_SLPS5_N
    {GPIO_SKL_H_GPD11,   { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPD_11_FM_GBE_LOM_DISABLE_N

// Group J AWD
    {GPIO_SKL_H_GPP_J0,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_0_TP_GPP_J_IO<0>
    {GPIO_SKL_H_GPP_J1,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_1_TP_GPP_J_IO<1>
    {GPIO_SKL_H_GPP_J2,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_2_TP_GPP_J_IO<2>
    {GPIO_SKL_H_GPP_J3,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_3_TP_GPP_J_IO<3>
    {GPIO_SKL_H_GPP_J4,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_4_TP_GPP_J_IO<4>
    {GPIO_SKL_H_GPP_J5,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_5_TP_GPP_J_IO<5>
    {GPIO_SKL_H_GPP_J6,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_6_TP_GPP_J_IO<6>
    {GPIO_SKL_H_GPP_J7,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_7_TP_GPP_J_IO<7>
    {GPIO_SKL_H_GPP_J8,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_8_TP_GPP_J_IO<8>
    {GPIO_SKL_H_GPP_J9,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_9_TP_GPP_J_IO<9>
    {GPIO_SKL_H_GPP_J10, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_10_TP_GPP_J_IO<10>
    {GPIO_SKL_H_GPP_J11, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_11_TP_GPP_J_IO<11>
    {GPIO_SKL_H_GPP_J12, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_12_TP_GPP_J_IO<12>
    {GPIO_SKL_H_GPP_J13, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_13_TP_GPP_J_IO<13>
    {GPIO_SKL_H_GPP_J14, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_14_TP_GPP_J_IO<14>
    {GPIO_SKL_H_GPP_J15, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_15_TP_GPP_J_IO<15>
    {GPIO_SKL_H_GPP_J16, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_16_TP_GPP_J_IO<16>
    {GPIO_SKL_H_GPP_J17, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_17_TP_GPP_J_IO<17>
    {GPIO_SKL_H_GPP_J18, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_18_TP_GPP_J_IO<18>
    {GPIO_SKL_H_GPP_J19, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_19_TP_GPP_J_IO<19>
    {GPIO_SKL_H_GPP_J20, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_20_TP_GPP_J_IO<20>
    {GPIO_SKL_H_GPP_J21, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_21_TP_GPP_J_IO<21>
    {GPIO_SKL_H_GPP_J22, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_22_TP_GPP_J_IO<22>
    {GPIO_SKL_H_GPP_J23, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_J_23_TP_GPP_J_IO<23>

// Group K AWD
    {GPIO_SKL_H_GPP_K0,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_K_0_TP_PCH_GPP_K0
    {GPIO_SKL_H_GPP_K1,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_K_1_TP_PCH_GPP_K1
    {GPIO_SKL_H_GPP_K2,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_K_2_TP_PCH_GPP_K2
    {GPIO_SKL_H_GPP_K3,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_K_3_TP_PCH_GPP_K3
    {GPIO_SKL_H_GPP_K4,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_K_4_TP_PCH_GPP_K4
    {GPIO_SKL_H_GPP_K5,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_K_5_TP_PCH_GPP_K5
    {GPIO_SKL_H_GPP_K6,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_K_6_TP_PCH_GPP_K6
    {GPIO_SKL_H_GPP_K7,  { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_K_7_FM_PCH_GBE_DEBUG_EN
    {GPIO_SKL_H_GPP_K8,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_K_8_PD_RMII_PCH_CONN_ARB_IN
    {GPIO_SKL_H_GPP_K9,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,      GpioOutHigh,    GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_K_9_PD_RMII_PCH_ARB_OUT
    {GPIO_SKL_H_GPP_K10, { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_K_10_RST_PCIE_PCH_PERST_N
//    {GPIO_SKL_H_GPP_K11, { GpioPadModeNative1, GpioHostOwnDefault,    GpioDirIn,       GpioOutLow,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_K_11_PD_1P8_3P3_RCOMP

// Group L AWD
    {GPIO_SKL_H_GPP_L2,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,       GpioOutHigh,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_L_2_TP_GPP_L_IO<2>
    {GPIO_SKL_H_GPP_L3,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,       GpioOutHigh,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_L_3_TP_GPP_L_IO<3>
    {GPIO_SKL_H_GPP_L4,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,       GpioOutHigh,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_L_4_TP_GPP_L_IO<4>
    {GPIO_SKL_H_GPP_L5,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,       GpioOutHigh,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_L_5_TP_GPP_L_IO<5>
    {GPIO_SKL_H_GPP_L6,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,       GpioOutHigh,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_L_6_TP_GPP_L_IO<6>
    {GPIO_SKL_H_GPP_L7,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,       GpioOutHigh,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_L_7_TP_GPP_L_IO<7>
    {GPIO_SKL_H_GPP_L8,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,       GpioOutHigh,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_L_8_TP_GPP_L_IO<8>
    {GPIO_SKL_H_GPP_L9,  { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,       GpioOutHigh,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_L_9_TP_GPP_L_IO<9>
    {GPIO_SKL_H_GPP_L10, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,       GpioOutHigh,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_L_10_TP_GPP_L_IO<10>
    {GPIO_SKL_H_GPP_L11, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,       GpioOutHigh,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_L_11_TP_GPP_L_IO<11>
    {GPIO_SKL_H_GPP_L12, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,       GpioOutHigh,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_L_12_TP_GPP_L_IO<12>
    {GPIO_SKL_H_GPP_L13, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,       GpioOutHigh,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_L_13_TP_GPP_L_IO<13>
    {GPIO_SKL_H_GPP_L14, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,       GpioOutHigh,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_L_14_TP_GPP_L_IO<14>
    {GPIO_SKL_H_GPP_L15, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,       GpioOutHigh,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_L_15_TP_GPP_L_IO<15>
    {GPIO_SKL_H_GPP_L16, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,       GpioOutHigh,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_L_16_TP_GPP_L_IO<16>
    {GPIO_SKL_H_GPP_L17, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,       GpioOutHigh,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_L_17_TP_GPP_L_IO<17>
    {GPIO_SKL_H_GPP_L18, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,       GpioOutHigh,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_L_18_TP_GPP_L_IO<18>
    {GPIO_SKL_H_GPP_L19, { GpioPadModeGpio,    GpioHostOwnDefault,    GpioDirOut,       GpioOutHigh,     GpioIntDis, GpioResetPwrGood, GpioTermNone, GpioPadConfigLock}},//GPP_L_19_TP_GPP_L_IO<19>
};

EFI_STATUS
TypeAowandaInstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
)
{
  EFI_STATUS                            Status;

  Status = UbaConfigPpi->AddData (
                                 UbaConfigPpi,
                                 &gPlatformGpioInitDataGuid,
                                 &mGpioTableAowanda,
                                 sizeof(mGpioTableAowanda)
                                 );
  Status = PcdSet32S (PcdOemSku_GPIO_TABLE_SIZE, sizeof (mGpioTableAowanda));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
