/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved

**/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <PlatformSetupVar.h>
#include <Library/ConfigParamsDataBlockLib.h>
#include <Library/PlatformConfigParamsDataBlockLib.h>
#include <Library/CpuInfoLib.h>
#include <Protocol/ConfigParamsManageProtocol.h>
#include <Protocol/PlatformConfigParamsManageProtocol.h>
#include <Guid/NetworkStackSetup.h>
#include <Guid/DtPlatformFormSet.h>

// DT/ACPI preference constants (from EmbeddedPkg DtPlatformDxe)
#define DT_ACPI_SELECT_DT    0x0
#define DT_ACPI_SELECT_ACPI  0x1
#define DT_ACPI_VARIABLE_NAME  L"DtAcpiPref"

typedef struct {
  UINT8    Pref;
  UINT8    Reserved[3];
} DT_ACPI_VARSTORE_DATA;

EFI_STATUS
EFIAPI
CheckCpuShareInfo (
  )
{
  EFI_STATUS           Status = EFI_SUCCESS;
  UINTN                VarSize;
  PLATFORM_SETUP_DATA  PlatformSetupVar;
  UINT32               CpuShareInfo;
  UINT8                CpuCoreNum;

  VarSize = sizeof (PLATFORM_SETUP_DATA);

  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VAR,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &PlatformSetupVar
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a EfiGetVariable failed: %r\n", __FUNCTION__, Status));
    return Status;
  }

  GetCpuShareInfo (&CpuShareInfo);
  if (CpuShareInfo != PlatformSetupVar.CpuShareInfo) {
    GetValidCpuCoreNum (&CpuCoreNum);
    PlatformSetupVar.CpuCoreNum        = CpuCoreNum;
    PlatformSetupVar.CpuShareInfo      = CpuShareInfo;
    PlatformSetupVar.CpuCoreEnable[0]  = 1;        // cpu core 0 is boot core and never be disabled.
    PlatformSetupVar.CpuCoreEnable[1]  = FixedPcdGetBool (PcdCpuCore1En);
    PlatformSetupVar.CpuCoreEnable[2]  = FixedPcdGetBool (PcdCpuCore2En);
    PlatformSetupVar.CpuCoreEnable[3]  = FixedPcdGetBool (PcdCpuCore3En);
    PlatformSetupVar.CpuCoreEnable[4]  = FixedPcdGetBool (PcdCpuCore4En);
    PlatformSetupVar.CpuCoreEnable[5]  = FixedPcdGetBool (PcdCpuCore5En);
    PlatformSetupVar.CpuCoreEnable[6]  = FixedPcdGetBool (PcdCpuCore6En);
    PlatformSetupVar.CpuCoreEnable[7]  = FixedPcdGetBool (PcdCpuCore7En);
    PlatformSetupVar.CpuCoreEnable[8]  = FixedPcdGetBool (PcdCpuCore8En);
    PlatformSetupVar.CpuCoreEnable[9]  = FixedPcdGetBool (PcdCpuCore9En);
    PlatformSetupVar.CpuCoreEnable[10] = FixedPcdGetBool (PcdCpuCore10En);
    PlatformSetupVar.CpuCoreEnable[11] = FixedPcdGetBool (PcdCpuCore11En);

    Status = gRT->SetVariable (
                    PLATFORM_SETUP_VAR,
                    &gPlatformSetupVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof (PLATFORM_SETUP_DATA),
                    &PlatformSetupVar
                    );
    ASSERT_EFI_ERROR (Status);
  }

  return Status;
}

BOOLEAN
IsRtcPowerfailure (
  )
{
  EFI_STATUS                                  Status;
  PLATFORM_CONFIG_PARAMS_DATA_BLOCK           *ConfigData = NULL;
  CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL  *ConfigManage;

  Status = gBS->LocateProtocol (&gCixPlatformConfigParamsManageProtocolGuid, NULL, (VOID **)&ConfigManage);
  if (!EFI_ERROR (Status)) {
    ConfigData = ConfigManage->Data;
    if (ConfigData != NULL) {
      DEBUG ((DEBUG_INFO, "RTC voltage drop status : %a\n", ConfigData->RtcVoltDrop ? "TRUE" : "FALSE"));
      return ConfigData->RtcVoltDrop;
    }
  }

  return FALSE;
}

VOID
EFIAPI
UpdateConfigParams (
  CONFIG_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  EFI_STATUS           Status;
  UINTN                VarSize;
  PLATFORM_SETUP_DATA  PlatformSetupVar;

  VarSize = sizeof (PLATFORM_SETUP_DATA);

  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VAR,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &PlatformSetupVar
                  );
  if (!EFI_ERROR (Status) && !IsRtcPowerfailure ()) {
    for (UINT8 i = 0; i < MAX_PCIE_PORT_NUM; i++) {
      ConfigData->Pcie.PcieRpEnable[i]        = PlatformSetupVar.PcieRpEnable[i];
      ConfigData->Pcie.PcieWidth[i]           = PlatformSetupVar.PcieWidth[i];
      ConfigData->Pcie.PcieMaxSpeed[i]        = PlatformSetupVar.PcieMaxSpeed[i];
      ConfigData->Pcie.PcieTargetLinkSpeed[i] = PlatformSetupVar.PcieTargetLinkSpeed[i];
      ConfigData->Pcie.PcieAspmMaxSupport[i]  = PlatformSetupVar.PcieAspmMaxSupport[i];
      ConfigData->Pcie.PcieAspm[i]            = PlatformSetupVar.PcieAspm[i];
      ConfigData->Pcie.PcieMaxPayload[i]      = PlatformSetupVar.PcieMaxPayload[i];
      ConfigData->Pcie.PcieMaxReadRequest[i]  = PlatformSetupVar.PcieMaxReadRequest[i];
      ConfigData->Pcie.PcieL1Substates[i]     = PlatformSetupVar.PcieL1Substates[i];
      ConfigData->Pcie.PcieDtiEnable[i]       = PlatformSetupVar.PcieDtiEnable[i];
    }

    for (UINT8 i = 0; i < MAX_I2C_CTRL_NUM; i++) {
      ConfigData->Fch.I2c[i].Enable  = PlatformSetupVar.I2cEnable[i];
      ConfigData->Fch.I2c[i].BusFreq = PlatformSetupVar.I2cBusFreq[i];
    }

    ConfigData->Usb20[0].Enable     = PlatformSetupVar.Usb2Control0Enable;
    ConfigData->Usb20[1].Enable     = PlatformSetupVar.Usb2Control1Enable;
    ConfigData->Usb20[2].Enable     = PlatformSetupVar.Usb2Control2Enable;
    ConfigData->Usb20[3].Enable     = PlatformSetupVar.Usb2Control3Enable;
    ConfigData->Usb32Drd[0].Enable  = PlatformSetupVar.Usb3Control0Enable;
    ConfigData->Usb32Drd[1].Enable  = PlatformSetupVar.Usb3Control1Enable;
    ConfigData->UsbCDrd[0].Enable   = PlatformSetupVar.UsbCDrdControl0Enable;
    ConfigData->UsbC[0].Enable      = PlatformSetupVar.UsbCControl0Enable;
    ConfigData->UsbC[1].Enable      = PlatformSetupVar.UsbCControl1Enable;
    ConfigData->UsbC[2].Enable      = PlatformSetupVar.UsbCControl2Enable;
    ConfigData->UsbCDrd[0].DataRole = PlatformSetupVar.UsbCDrdControl0DataRole;

    ConfigData->Gmac[0].Enable     =  PlatformSetupVar.GmacEnable[0];
    ConfigData->Gmac[1].Enable     =  PlatformSetupVar.GmacEnable[1];
    ConfigData->Gmac[0].MacAddress =  PlatformSetupVar.GmacMacAddr[0];
    ConfigData->Gmac[1].MacAddress =  PlatformSetupVar.GmacMacAddr[1];

    ConfigData->S5.SocWatchdogTimer = PlatformSetupVar.SocWatchdogTimer;

    ConfigData->Cpu.LpiState = PlatformSetupVar.CpuLpiState;
  }
}

VOID
EFIAPI
UpdatePlatformConfigParams (
  PLATFORM_CONFIG_PARAMS_DATA_BLOCK  *ConfigData
  )
{
  EFI_STATUS           Status;
  UINTN                VarSize;
  PLATFORM_SETUP_DATA  PlatformSetupVar;
  SYSTEM_TABLE         SystemTableVar;
  UINTN                SystemTableVarSize;

  VarSize = sizeof (PLATFORM_SETUP_DATA);

  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VAR,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &PlatformSetupVar
                  );
  if (!EFI_ERROR (Status) && !IsRtcPowerfailure ()) {
    ConfigData->DtbMenuEntry     = PlatformSetupVar.DtbMenuEntry;
    ConfigData->GfxPower         = PlatformSetupVar.GfxPower;
    ConfigData->TouchPanelPower  = PlatformSetupVar.TouchPanelPower;
    ConfigData->TpmPower         = PlatformSetupVar.TpmPower;
    ConfigData->WwanPower        = PlatformSetupVar.WwanPower;
    ConfigData->PcieX2SlotPower  = PlatformSetupVar.PcieX2SlotPower;
    ConfigData->FingerPrintPower = PlatformSetupVar.FingerPrintPower;
    ConfigData->WlanPower        = PlatformSetupVar.WlanPower;
    ConfigData->M2SsdPower       = PlatformSetupVar.M2SsdPower;
    ConfigData->OnBoardLanPower  = PlatformSetupVar.OnBoardLanPower;
    ConfigData->IspCamera0Power  = PlatformSetupVar.IspCamera0Power;
    ConfigData->IspCamera1Power  = PlatformSetupVar.IspCamera1Power;
    ConfigData->IspCamera2Power  = PlatformSetupVar.IspCamera2Power;
    ConfigData->IspCamera3Power  = PlatformSetupVar.IspCamera3Power;
    ConfigData->StateAfterG3     = PlatformSetupVar.StateAfterG3;
    ConfigData->RtcWakeup        = PlatformSetupVar.RtcWakeup;
    ConfigData->LightSensorCtrl  = PlatformSetupVar.LightSensorCtrl;
    ConfigData->SpcrEnable       = PlatformSetupVar.SpcrEnable;
    ConfigData->EcFanMode        = PlatformSetupVar.EcFanMode;
  }

  SystemTableVarSize = sizeof (SYSTEM_TABLE);

  Status = gRT->GetVariable (
                  SYSTEM_TABLE_VAR,
                  &gCixGlobalVariableGuid,
                  NULL,
                  &SystemTableVarSize,
                  &SystemTableVar
                  );
  if (!EFI_ERROR (Status) && !IsRtcPowerfailure ()) {
    ConfigData->SystemTableSelect = SystemTableVar.SystemTableSelect;
  }
}

VOID
EFIAPI
ConfigParamsUpdateCallBack (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                         Status      = EFI_SUCCESS;
  CONFIG_PARAMS_DATA_BLOCK           *ConfigData = NULL;
  CIX_CONFIG_PARAMS_MANAGE_PROTOCOL  *ConfigManage;

  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  Status = gBS->LocateProtocol (&gCixConfigParamsManageProtocolGuid, NULL, (VOID **)&ConfigManage);
  if (!EFI_ERROR (Status)) {
    ConfigData = ConfigManage->Data;
    if (ConfigData != NULL) {
      UpdateConfigParams (ConfigData);
    }
  } else {
    DEBUG ((DEBUG_ERROR, "%a LocateProtocol failed: %r\n", __FUNCTION__, Status));
    return;
  }
}

VOID
EFIAPI
PlatformConfigParamsUpdateCallBack (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                                  Status      = EFI_SUCCESS;
  PLATFORM_CONFIG_PARAMS_DATA_BLOCK           *ConfigData = NULL;
  CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL  *ConfigManage;

  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  Status = gBS->LocateProtocol (&gCixPlatformConfigParamsManageProtocolGuid, NULL, (VOID **)&ConfigManage);
  if (!EFI_ERROR (Status)) {
    ConfigData = ConfigManage->Data;
    if (ConfigData != NULL) {
      UpdatePlatformConfigParams (ConfigData);
    }
  } else {
    DEBUG ((DEBUG_ERROR, "%a LocateProtocol failed: %r\n", __FUNCTION__, Status));
    return;
  }
}

VOID
EFIAPI
ConstructSetupVariable (
  PLATFORM_SETUP_DATA  *PlatformSetupVar
  )
{
  UINT8   CpuCoreNum;
  UINT32  CpuShareInfo;

  GetValidCpuCoreNum (&CpuCoreNum);
  GetCpuShareInfo (&CpuShareInfo);

  PlatformSetupVar->PcieRpEnable[0]        = FixedPcdGetBool (PcdPcieRootPort0Enable);
  PlatformSetupVar->PcieWidth[0]           = FixedPcdGet8 (PcdPcieRootPort0LaneNum);
  PlatformSetupVar->PcieMaxSpeed[0]        = FixedPcdGet8 (PcdPcieRootPort0MaxSpeed);
  PlatformSetupVar->PcieTargetLinkSpeed[0] = FixedPcdGet8 (PcdPcieRootPort0SpeedGen);
  PlatformSetupVar->PcieAspmMaxSupport[0]  = FixedPcdGet8 (PcdPcieRootPort0MaxAspmSupport);
  PlatformSetupVar->PcieAspm[0]            = FixedPcdGet8 (PcdPcieRootPort0AspmSupport);
  PlatformSetupVar->PcieMaxPayload[0]      = FixedPcdGet8 (PcdPcieRootPort0MaxPayload);
  PlatformSetupVar->PcieMaxReadRequest[0]  = FixedPcdGet8 (PcdPcieRootPort0MaxReadRequest);
  PlatformSetupVar->PcieDtiEnable[0]       = FixedPcdGet8 (PcdPcieRootPort0DtiEnable);

  PlatformSetupVar->PcieRpEnable[1]        = FixedPcdGetBool (PcdPcieRootPort1Enable);
  PlatformSetupVar->PcieWidth[1]           = FixedPcdGet8 (PcdPcieRootPort1LaneNum);
  PlatformSetupVar->PcieMaxSpeed[1]        = FixedPcdGet8 (PcdPcieRootPort1MaxSpeed);
  PlatformSetupVar->PcieTargetLinkSpeed[1] = FixedPcdGet8 (PcdPcieRootPort1SpeedGen);
  PlatformSetupVar->PcieAspmMaxSupport[1]  = FixedPcdGet8 (PcdPcieRootPort1MaxAspmSupport);
  PlatformSetupVar->PcieAspm[1]            = FixedPcdGet8 (PcdPcieRootPort1AspmSupport);
  PlatformSetupVar->PcieMaxPayload[1]      = FixedPcdGet8 (PcdPcieRootPort1MaxPayload);
  PlatformSetupVar->PcieMaxReadRequest[1]  = FixedPcdGet8 (PcdPcieRootPort1MaxReadRequest);
  PlatformSetupVar->PcieDtiEnable[1]       = FixedPcdGet8 (PcdPcieRootPort1DtiEnable);

  PlatformSetupVar->PcieRpEnable[2]        = FixedPcdGetBool (PcdPcieRootPort2Enable);
  PlatformSetupVar->PcieWidth[2]           = FixedPcdGet8 (PcdPcieRootPort2LaneNum);
  PlatformSetupVar->PcieMaxSpeed[2]        = FixedPcdGet8 (PcdPcieRootPort2MaxSpeed);
  PlatformSetupVar->PcieTargetLinkSpeed[2] = FixedPcdGet8 (PcdPcieRootPort2SpeedGen);
  PlatformSetupVar->PcieAspmMaxSupport[2]  = FixedPcdGet8 (PcdPcieRootPort2MaxAspmSupport);
  PlatformSetupVar->PcieAspm[2]            = FixedPcdGet8 (PcdPcieRootPort2AspmSupport);
  PlatformSetupVar->PcieMaxPayload[2]      = FixedPcdGet8 (PcdPcieRootPort2MaxPayload);
  PlatformSetupVar->PcieMaxReadRequest[2]  = FixedPcdGet8 (PcdPcieRootPort2MaxReadRequest);
  PlatformSetupVar->PcieDtiEnable[2]       = FixedPcdGet8 (PcdPcieRootPort2DtiEnable);

  PlatformSetupVar->PcieRpEnable[3]        = FixedPcdGetBool (PcdPcieRootPort3Enable);
  PlatformSetupVar->PcieWidth[3]           = FixedPcdGet8 (PcdPcieRootPort3LaneNum);
  PlatformSetupVar->PcieMaxSpeed[3]        = FixedPcdGet8 (PcdPcieRootPort3MaxSpeed);
  PlatformSetupVar->PcieTargetLinkSpeed[3] = FixedPcdGet8 (PcdPcieRootPort3SpeedGen);
  PlatformSetupVar->PcieAspmMaxSupport[3]  = FixedPcdGet8 (PcdPcieRootPort3MaxAspmSupport);
  PlatformSetupVar->PcieAspm[3]            = FixedPcdGet8 (PcdPcieRootPort3AspmSupport);
  PlatformSetupVar->PcieMaxPayload[3]      = FixedPcdGet8 (PcdPcieRootPort3MaxPayload);
  PlatformSetupVar->PcieMaxReadRequest[3]  = FixedPcdGet8 (PcdPcieRootPort3MaxReadRequest);
  PlatformSetupVar->PcieDtiEnable[3]       = FixedPcdGet8 (PcdPcieRootPort3DtiEnable);

  PlatformSetupVar->PcieRpEnable[4]        = FixedPcdGetBool (PcdPcieRootPort4Enable);
  PlatformSetupVar->PcieWidth[4]           = FixedPcdGet8 (PcdPcieRootPort4LaneNum);
  PlatformSetupVar->PcieMaxSpeed[4]        = FixedPcdGet8 (PcdPcieRootPort4MaxSpeed);
  PlatformSetupVar->PcieTargetLinkSpeed[4] = FixedPcdGet8 (PcdPcieRootPort4SpeedGen);
  PlatformSetupVar->PcieAspmMaxSupport[4]  = FixedPcdGet8 (PcdPcieRootPort4MaxAspmSupport);
  PlatformSetupVar->PcieAspm[4]            = FixedPcdGet8 (PcdPcieRootPort4AspmSupport);
  PlatformSetupVar->PcieMaxPayload[4]      = FixedPcdGet8 (PcdPcieRootPort4MaxPayload);
  PlatformSetupVar->PcieMaxReadRequest[4]  = FixedPcdGet8 (PcdPcieRootPort4MaxReadRequest);
  PlatformSetupVar->PcieDtiEnable[4]       = FixedPcdGet8 (PcdPcieRootPort4DtiEnable);

  PlatformSetupVar->I2cEnable[0]  = FixedPcdGetBool (PcdI2c0En);
  PlatformSetupVar->I2cEnable[1]  = FixedPcdGetBool (PcdI2c1En);
  PlatformSetupVar->I2cEnable[2]  = FixedPcdGetBool (PcdI2c2En);
  PlatformSetupVar->I2cEnable[3]  = FixedPcdGetBool (PcdI2c3En);
  PlatformSetupVar->I2cEnable[4]  = FixedPcdGetBool (PcdI2c4En);
  PlatformSetupVar->I2cEnable[5]  = FixedPcdGetBool (PcdI2c5En);
  PlatformSetupVar->I2cEnable[6]  = FixedPcdGetBool (PcdI2c6En);
  PlatformSetupVar->I2cEnable[7]  = FixedPcdGetBool (PcdI2c7En);
  PlatformSetupVar->I2cBusFreq[0] = FixedPcdGet32 (PcdI2c0BusFreq);
  PlatformSetupVar->I2cBusFreq[1] = FixedPcdGet32 (PcdI2c1BusFreq);
  PlatformSetupVar->I2cBusFreq[2] = FixedPcdGet32 (PcdI2c2BusFreq);
  PlatformSetupVar->I2cBusFreq[3] = FixedPcdGet32 (PcdI2c3BusFreq);
  PlatformSetupVar->I2cBusFreq[4] = FixedPcdGet32 (PcdI2c4BusFreq);
  PlatformSetupVar->I2cBusFreq[5] = FixedPcdGet32 (PcdI2c5BusFreq);
  PlatformSetupVar->I2cBusFreq[6] = FixedPcdGet32 (PcdI2c6BusFreq);
  PlatformSetupVar->I2cBusFreq[7] = FixedPcdGet32 (PcdI2c7BusFreq);

  PlatformSetupVar->GmacEnable[0]  = FixedPcdGetBool (PcdGmac0Enable);
  PlatformSetupVar->GmacEnable[1]  = FixedPcdGetBool (PcdGmac1Enable);
  PlatformSetupVar->GmacMacAddr[0] = FixedPcdGetBool (PcdGmac0MacAddress);
  PlatformSetupVar->GmacMacAddr[1] = FixedPcdGetBool (PcdGmac1MacAddress);

  PlatformSetupVar->MemBreakPoint   = FixedPcdGet8 (PcdMemBreakPoint);
  PlatformSetupVar->MemEyeScan      = FixedPcdGet8 (PcdMemEyeScan);
  PlatformSetupVar->MemFreq         = FixedPcdGet16 (PcdMemFreq);
  PlatformSetupVar->MemHarvesting   = FixedPcdGet8 (PcdMemHarvesting);
  PlatformSetupVar->WckAlwaysOn     = FixedPcdGet8 (PcdMemWckAlwaysOn);
  PlatformSetupVar->DataMask        = FixedPcdGet8 (PcdMemDataMask);
  PlatformSetupVar->RfmEn           = FixedPcdGet8 (PcdMemRfmEn);
  PlatformSetupVar->AutoPrechargeEn = FixedPcdGet8 (PcdMemAutoPrechargeEn);
  PlatformSetupVar->PbrEn           = FixedPcdGet8 (PcdMemPbrEn);
  PlatformSetupVar->MbistEn         = FixedPcdGet8 (PcdMemMbistEn);
  PlatformSetupVar->MbistMode       = FixedPcdGet8 (PcdMemMbistMode);
  PlatformSetupVar->MemWrLEcc       = FixedPcdGet8 (PcdMemWrLEcc);
  PlatformSetupVar->MemRdLEcc       = FixedPcdGet8 (PcdMemRdLEcc);
  PlatformSetupVar->PortPriority    = FixedPcdGet8 (PcdMemPortPriority);
  PlatformSetupVar->BdwP0Override   = 0;
  PlatformSetupVar->BdwP1Override   = 0;
  PlatformSetupVar->BdwP0           = FixedPcdGet8 (PcdMemBdwP0);
  PlatformSetupVar->BdwP1           = FixedPcdGet8 (PcdMemBdwP1);
  PlatformSetupVar->MemRPriorityP0  = FixedPcdGet8 (PcdMemRPriorityP0);
  PlatformSetupVar->MemWPriorityP0  = FixedPcdGet8 (PcdMemWPriorityP0);
  PlatformSetupVar->MemBdwOvflowP0  = FixedPcdGet8 (PcdMemBdwOvflowP0);
  PlatformSetupVar->MemRPriorityP1  = FixedPcdGet8 (PcdMemRPriorityP1);
  PlatformSetupVar->MemWPriorityP1  = FixedPcdGet8 (PcdMemWPriorityP1);
  PlatformSetupVar->MemBdwOvflowP1  = FixedPcdGet8 (PcdMemBdwOvflowP1);
  PlatformSetupVar->MemBreakPoint   = FixedPcdGet8 (PcdMemBreakPoint);
  PlatformSetupVar->MemEyeScan      = FixedPcdGet8 (PcdMemEyeScan);
  PlatformSetupVar->MemIEcc         = FixedPcdGet8 (PcdMemIEcc);

  PlatformSetupVar->StateAfterG3   = FixedPcdGet8 (PcdStateAfterG3);
  PlatformSetupVar->PrimaryDisplay = FixedPcdGet8 (PcdPrimaryDisplay);
  PlatformSetupVar->DtbMenuEntry   = FixedPcdGet8 (PcdDtbMenuEntry);

  PlatformSetupVar->BiosReset             = FixedPcdGet8 (PcdBiosReset);
  PlatformSetupVar->SocWatchdogTimer      = FixedPcdGet8 (PcdSocWatchdogTimer);
  PlatformSetupVar->VddSocVoltage         = FixedPcdGet16 (PcdVddSocVoltage);
  PlatformSetupVar->VddGpuVoltage         = FixedPcdGet16 (PcdVddGpuVoltage);
  PlatformSetupVar->VddDpuVoltage         = FixedPcdGet16 (PcdVddDpuVoltage);
  PlatformSetupVar->VddCpuBigCore0Voltage = FixedPcdGet16 (PcdVddCpuBigCore0Voltage);
  PlatformSetupVar->VddCpuBigCore1Voltage = FixedPcdGet16 (PcdVddCpuBigCore1Voltage);
  PlatformSetupVar->VddCpuMidCore0Voltage = FixedPcdGet16 (PcdVddCpuMidCore0Voltage);
  PlatformSetupVar->VddCpuMidCore1Voltage = FixedPcdGet16 (PcdVddCpuMidCore1Voltage);
  PlatformSetupVar->VddCpuLitCoreVoltage  = FixedPcdGet16 (PcdVddCpuLitCoreVoltage);
  PlatformSetupVar->GfxPower              = FixedPcdGet8 (PcdGfxPower);
  PlatformSetupVar->TouchPanelPower       = FixedPcdGet8 (PcdTouchPanelPower);
  PlatformSetupVar->TpmPower              = FixedPcdGet8 (PcdTpmPower);
  PlatformSetupVar->WwanPower             = FixedPcdGet8 (PcdWwanPower);
  PlatformSetupVar->PcieX2SlotPower       = FixedPcdGet8 (PcdPcieX2SlotPower);
  PlatformSetupVar->FingerPrintPower      = FixedPcdGet8 (PcdFingerPrintPower);
  PlatformSetupVar->WlanPower             = FixedPcdGet8 (PcdWlanPower);
  PlatformSetupVar->M2SsdPower            = FixedPcdGet8 (PcdM2SsdPower);
  PlatformSetupVar->OnBoardLanPower       = FixedPcdGet8 (PcdOnBoardLanPower);
  PlatformSetupVar->IspCamera0Power       = FixedPcdGet8 (PcdIspCamera0Power);
  PlatformSetupVar->IspCamera1Power       = FixedPcdGet8 (PcdIspCamera1Power);
  PlatformSetupVar->IspCamera2Power       = FixedPcdGet8 (PcdIspCamera2Power);
  PlatformSetupVar->IspCamera3Power       = FixedPcdGet8 (PcdIspCamera3Power);
  PlatformSetupVar->CpuCoreClkGating      = FixedPcdGet8 (PcdCpuCoreClkGating);
  PlatformSetupVar->DsuClkGating          = FixedPcdGet8 (PcdDsuClkGating);
  PlatformSetupVar->GicdClkGating         = FixedPcdGet8 (PcdGicdClkGating);
  PlatformSetupVar->Ci700ClkGating        = FixedPcdGet8 (PcdCi700ClkGating);
  PlatformSetupVar->SysNi700ClkGating     = FixedPcdGet8 (PcdSysNi700ClkGating);
  PlatformSetupVar->MmNi700ClkGating      = FixedPcdGet8 (PcdMmNi700ClkGating);
  PlatformSetupVar->PcieNi700ClkGating    = FixedPcdGet8 (PcdPcieNi700ClkGating);
  PlatformSetupVar->SmnNi700ClkGating     = FixedPcdGet8 (PcdSmnNi700ClkGating);
  PlatformSetupVar->GpuClkGating          = FixedPcdGet8 (PcdGpuClkGating);
  PlatformSetupVar->Dpu0ClkGating         = FixedPcdGet8 (PcdDpu0ClkGating);
  PlatformSetupVar->Dpu1ClkGating         = FixedPcdGet8 (PcdDpu1ClkGating);
  PlatformSetupVar->Dpu2ClkGating         = FixedPcdGet8 (PcdDpu2ClkGating);
  PlatformSetupVar->Dpu3ClkGating         = FixedPcdGet8 (PcdDpu3ClkGating);
  PlatformSetupVar->Dpu4ClkGating         = FixedPcdGet8 (PcdDpu4ClkGating);
  PlatformSetupVar->VpuClkGating          = FixedPcdGet8 (PcdVpuClkGating);
  PlatformSetupVar->RtcWakeup             = FixedPcdGet8 (PcdRtcWakeup);
  PlatformSetupVar->LightSensorCtrl       = FixedPcdGet8 (PcdLightSensorCtrl);
  PlatformSetupVar->SpcrEnable            = FixedPcdGetBool (PcdAcpiSpcrEnable);
  PlatformSetupVar->EcFanMode             = FixedPcdGetBool (PcdEcDefaultFanMode);

  PlatformSetupVar->Usb2Control0Enable      = FixedPcdGetBool (PcdUsb2Control0Enable);
  PlatformSetupVar->Usb2Control1Enable      = FixedPcdGetBool (PcdUsb2Control1Enable);
  PlatformSetupVar->Usb2Control2Enable      = FixedPcdGetBool (PcdUsb2Control2Enable);
  PlatformSetupVar->Usb2Control3Enable      = FixedPcdGetBool (PcdUsb2Control3Enable);
  PlatformSetupVar->Usb3Control0Enable      = FixedPcdGetBool (PcdUsb3Control0Enable);
  PlatformSetupVar->Usb3Control1Enable      = FixedPcdGetBool (PcdUsb3Control1Enable);
  PlatformSetupVar->UsbCDrdControl0Enable   = FixedPcdGetBool (PcdUsbCDrdControl0Enable);
  PlatformSetupVar->UsbCControl0Enable      = FixedPcdGetBool (PcdUsbCControl0Enable);
  PlatformSetupVar->UsbCControl1Enable      = FixedPcdGetBool (PcdUsbCControl1Enable);
  PlatformSetupVar->UsbCControl2Enable      = FixedPcdGetBool (PcdUsbCControl2Enable);
  PlatformSetupVar->UsbCDrdControl0DataRole = FixedPcdGetBool (PcdUsbCDrdControl0DataRole);
  PlatformSetupVar->CpuCoreEnable[0]        = 1; // cpu core 0 is boot core and never be disabled.
  PlatformSetupVar->CpuCoreEnable[1]        = FixedPcdGetBool (PcdCpuCore1En);
  PlatformSetupVar->CpuCoreEnable[2]        = FixedPcdGetBool (PcdCpuCore2En);
  PlatformSetupVar->CpuCoreEnable[3]        = FixedPcdGetBool (PcdCpuCore3En);
  PlatformSetupVar->CpuCoreEnable[4]        = FixedPcdGetBool (PcdCpuCore4En);
  PlatformSetupVar->CpuCoreEnable[5]        = FixedPcdGetBool (PcdCpuCore5En);
  PlatformSetupVar->CpuCoreEnable[6]        = FixedPcdGetBool (PcdCpuCore6En);
  PlatformSetupVar->CpuCoreEnable[7]        = FixedPcdGetBool (PcdCpuCore7En);
  PlatformSetupVar->CpuCoreEnable[8]        = FixedPcdGetBool (PcdCpuCore8En);
  PlatformSetupVar->CpuCoreEnable[9]        = FixedPcdGetBool (PcdCpuCore9En);
  PlatformSetupVar->CpuCoreEnable[10]       = FixedPcdGetBool (PcdCpuCore10En);
  PlatformSetupVar->CpuCoreEnable[11]       = FixedPcdGetBool (PcdCpuCore11En);
  PlatformSetupVar->CpuCoreNum              = CpuCoreNum;
  PlatformSetupVar->CpuShareInfo            = CpuShareInfo;
  PlatformSetupVar->CpuLpiState             = FixedPcdGet8 (PcdAcpiCpuLpiState);

  //
  // Stock PM OPP table defaults (from opp_config_custom.h / pmic_config.h)
  //
  // Domain 0: GPU Core (7 visible entries, +1 hidden safe sustained at OPP[0])
  PlatformSetupVar->PmOppFreq[0] = 250;  PlatformSetupVar->PmOppVolt[0] = 800;
  PlatformSetupVar->PmOppFreq[1] = 350;  PlatformSetupVar->PmOppVolt[1] = 800;
  PlatformSetupVar->PmOppFreq[2] = 600;  PlatformSetupVar->PmOppVolt[2] = 800;
  PlatformSetupVar->PmOppFreq[3] = 800;  PlatformSetupVar->PmOppVolt[3] = 800;
  PlatformSetupVar->PmOppFreq[4] = 1000; PlatformSetupVar->PmOppVolt[4] = 800;
  PlatformSetupVar->PmOppFreq[5] = 1100; PlatformSetupVar->PmOppVolt[5] = 800;
  PlatformSetupVar->PmOppFreq[6] = 1100; PlatformSetupVar->PmOppVolt[6] = 800;
  // Domain 1: GPU Top (6 visible entries, +1 hidden safe sustained at OPP[0])
  PlatformSetupVar->PmOppFreq[13] = 250;  PlatformSetupVar->PmOppVolt[13] = 800;
  PlatformSetupVar->PmOppFreq[14] = 350;  PlatformSetupVar->PmOppVolt[14] = 800;
  PlatformSetupVar->PmOppFreq[15] = 600;  PlatformSetupVar->PmOppVolt[15] = 800;
  PlatformSetupVar->PmOppFreq[16] = 800;  PlatformSetupVar->PmOppVolt[16] = 800;
  PlatformSetupVar->PmOppFreq[17] = 1000; PlatformSetupVar->PmOppVolt[17] = 800;
  PlatformSetupVar->PmOppFreq[18] = 1000; PlatformSetupVar->PmOppVolt[18] = 800;
  // Domain 2: CPU Little (2 entries)
  PlatformSetupVar->PmOppFreq[26] = 800;  PlatformSetupVar->PmOppVolt[26] = 790;
  PlatformSetupVar->PmOppFreq[27] = 1800; PlatformSetupVar->PmOppVolt[27] = 790;
  // Domain 3: CPU Big G0 (7 entries)
  PlatformSetupVar->PmOppFreq[39] = 800;  PlatformSetupVar->PmOppVolt[39] = 790;
  PlatformSetupVar->PmOppFreq[40] = 1200; PlatformSetupVar->PmOppVolt[40] = 800;
  PlatformSetupVar->PmOppFreq[41] = 1500; PlatformSetupVar->PmOppVolt[41] = 810;
  PlatformSetupVar->PmOppFreq[42] = 1800; PlatformSetupVar->PmOppVolt[42] = 820;
  PlatformSetupVar->PmOppFreq[43] = 2200; PlatformSetupVar->PmOppVolt[43] = 830;
  PlatformSetupVar->PmOppFreq[44] = 2400; PlatformSetupVar->PmOppVolt[44] = 860;
  PlatformSetupVar->PmOppFreq[45] = 2600; PlatformSetupVar->PmOppVolt[45] = 930;
  // Domain 4: CPU Big G1 (7 entries)
  PlatformSetupVar->PmOppFreq[52] = 800;  PlatformSetupVar->PmOppVolt[52] = 790;
  PlatformSetupVar->PmOppFreq[53] = 1200; PlatformSetupVar->PmOppVolt[53] = 800;
  PlatformSetupVar->PmOppFreq[54] = 1500; PlatformSetupVar->PmOppVolt[54] = 810;
  PlatformSetupVar->PmOppFreq[55] = 1800; PlatformSetupVar->PmOppVolt[55] = 820;
  PlatformSetupVar->PmOppFreq[56] = 2200; PlatformSetupVar->PmOppVolt[56] = 830;
  PlatformSetupVar->PmOppFreq[57] = 2400; PlatformSetupVar->PmOppVolt[57] = 860;
  PlatformSetupVar->PmOppFreq[58] = 2600; PlatformSetupVar->PmOppVolt[58] = 930;
  // Domain 5: CPU Mid G0 (7 entries)
  PlatformSetupVar->PmOppFreq[65] = 800;  PlatformSetupVar->PmOppVolt[65] = 790;
  PlatformSetupVar->PmOppFreq[66] = 1200; PlatformSetupVar->PmOppVolt[66] = 800;
  PlatformSetupVar->PmOppFreq[67] = 1500; PlatformSetupVar->PmOppVolt[67] = 810;
  PlatformSetupVar->PmOppFreq[68] = 1800; PlatformSetupVar->PmOppVolt[68] = 820;
  PlatformSetupVar->PmOppFreq[69] = 2000; PlatformSetupVar->PmOppVolt[69] = 820;
  PlatformSetupVar->PmOppFreq[70] = 2200; PlatformSetupVar->PmOppVolt[70] = 860;
  PlatformSetupVar->PmOppFreq[71] = 2400; PlatformSetupVar->PmOppVolt[71] = 930;
  // Domain 6: CPU Mid G1 (7 entries)
  PlatformSetupVar->PmOppFreq[78] = 800;  PlatformSetupVar->PmOppVolt[78] = 790;
  PlatformSetupVar->PmOppFreq[79] = 1200; PlatformSetupVar->PmOppVolt[79] = 800;
  PlatformSetupVar->PmOppFreq[80] = 1500; PlatformSetupVar->PmOppVolt[80] = 810;
  PlatformSetupVar->PmOppFreq[81] = 1800; PlatformSetupVar->PmOppVolt[81] = 820;
  PlatformSetupVar->PmOppFreq[82] = 2000; PlatformSetupVar->PmOppVolt[82] = 820;
  PlatformSetupVar->PmOppFreq[83] = 2200; PlatformSetupVar->PmOppVolt[83] = 860;
  PlatformSetupVar->PmOppFreq[84] = 2400; PlatformSetupVar->PmOppVolt[84] = 930;
  // Domain 7: DSU (2 entries)
  PlatformSetupVar->PmOppFreq[91] = 400;  PlatformSetupVar->PmOppVolt[91] = 790;
  PlatformSetupVar->PmOppFreq[92] = 1300; PlatformSetupVar->PmOppVolt[92] = 790;
  // Domain 8: NPU (4 visible entries, +1 hidden safe sustained at OPP[0], no voltage)
  PlatformSetupVar->PmOppFreq[104] = 400;
  PlatformSetupVar->PmOppFreq[105] = 600;
  PlatformSetupVar->PmOppFreq[106] = 800;
  PlatformSetupVar->PmOppFreq[107] = 1200;
  // Domain 9: VPU (6 visible entries, +1 hidden safe sustained at OPP[0], no voltage)
  PlatformSetupVar->PmOppFreq[117] = 150;
  PlatformSetupVar->PmOppFreq[118] = 300;
  PlatformSetupVar->PmOppFreq[119] = 480;
  PlatformSetupVar->PmOppFreq[120] = 600;
  PlatformSetupVar->PmOppFreq[121] = 800;
  PlatformSetupVar->PmOppFreq[122] = 1200;
  // Domain 10: CI700 (1 entry, no voltage)
  PlatformSetupVar->PmOppFreq[130] = 1500;
  // Domain 11: MMHUB (3 visible entries, +1 hidden safe sustained at OPP[0], no voltage)
  PlatformSetupVar->PmOppFreq[143] = 350;
  PlatformSetupVar->PmOppFreq[144] = 600;
  PlatformSetupVar->PmOppFreq[145] = 750;
  // TDP defaults (mW)
  PlatformSetupVar->PmTdp[0] = 2400;
  PlatformSetupVar->PmTdp[1] = 6700;
  PlatformSetupVar->PmTdp[2] = 6500;
  PlatformSetupVar->PmTdp[3] = 8000;
  PlatformSetupVar->PmTdp[4] = 8200;
  PlatformSetupVar->PmTdp[5] = 5500;
  PlatformSetupVar->PmTdp[6] = 12000;
  PlatformSetupVar->PmTdp[7] = 9000;
}

EFI_STATUS
PlatformSetupVariableInit (
  VOID
  )
{
  EFI_STATUS           Status = EFI_SUCCESS;
  UINTN                VarSize;
  PLATFORM_SETUP_DATA  PlatformSetupVar;

  VarSize = sizeof (PLATFORM_SETUP_DATA);

  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VAR,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &PlatformSetupVar
                  );
  if (EFI_ERROR (Status) || IsRtcPowerfailure ()) {
    //
    // Variable does not exist yet - create it
    //
    ZeroMem (&PlatformSetupVar, VarSize);
    ConstructSetupVariable (&PlatformSetupVar);

    Status = gRT->SetVariable (
                    PLATFORM_SETUP_VAR,
                    &gPlatformSetupVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof (PLATFORM_SETUP_DATA),
                    &PlatformSetupVar
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "%a: EfiSetVariable failed - %r\n", __FUNCTION__, Status));
    }
  } else {
    DEBUG ((DEBUG_INFO, "%a: EfiGetVariable Success - %r\n", __FUNCTION__, Status));

    //
    // Migration: if an existing variable has uninitialized PM fields (all zeros
    // from a prior BIOS version), populate them with stock defaults and rewrite.
    //
    if (PlatformSetupVar.PmTdp[0] == 0 && PlatformSetupVar.PmOppFreq[0] == 0) {
      // Domain 0: GPU Core
      PlatformSetupVar.PmOppFreq[0] = 250;  PlatformSetupVar.PmOppVolt[0] = 800;
      PlatformSetupVar.PmOppFreq[1] = 350;  PlatformSetupVar.PmOppVolt[1] = 800;
      PlatformSetupVar.PmOppFreq[2] = 600;  PlatformSetupVar.PmOppVolt[2] = 800;
      PlatformSetupVar.PmOppFreq[3] = 800;  PlatformSetupVar.PmOppVolt[3] = 800;
      PlatformSetupVar.PmOppFreq[4] = 1000; PlatformSetupVar.PmOppVolt[4] = 800;
      PlatformSetupVar.PmOppFreq[5] = 1100; PlatformSetupVar.PmOppVolt[5] = 800;
      PlatformSetupVar.PmOppFreq[6] = 1100; PlatformSetupVar.PmOppVolt[6] = 800;
      // Domain 1: GPU Top
      PlatformSetupVar.PmOppFreq[13] = 250;  PlatformSetupVar.PmOppVolt[13] = 800;
      PlatformSetupVar.PmOppFreq[14] = 350;  PlatformSetupVar.PmOppVolt[14] = 800;
      PlatformSetupVar.PmOppFreq[15] = 600;  PlatformSetupVar.PmOppVolt[15] = 800;
      PlatformSetupVar.PmOppFreq[16] = 800;  PlatformSetupVar.PmOppVolt[16] = 800;
      PlatformSetupVar.PmOppFreq[17] = 1000; PlatformSetupVar.PmOppVolt[17] = 800;
      PlatformSetupVar.PmOppFreq[18] = 1000; PlatformSetupVar.PmOppVolt[18] = 800;
      // Domain 2: CPU Little
      PlatformSetupVar.PmOppFreq[26] = 800;  PlatformSetupVar.PmOppVolt[26] = 790;
      PlatformSetupVar.PmOppFreq[27] = 1800; PlatformSetupVar.PmOppVolt[27] = 790;
      // Domain 3: CPU Big G0
      PlatformSetupVar.PmOppFreq[39] = 800;  PlatformSetupVar.PmOppVolt[39] = 790;
      PlatformSetupVar.PmOppFreq[40] = 1200; PlatformSetupVar.PmOppVolt[40] = 800;
      PlatformSetupVar.PmOppFreq[41] = 1500; PlatformSetupVar.PmOppVolt[41] = 810;
      PlatformSetupVar.PmOppFreq[42] = 1800; PlatformSetupVar.PmOppVolt[42] = 820;
      PlatformSetupVar.PmOppFreq[43] = 2200; PlatformSetupVar.PmOppVolt[43] = 830;
      PlatformSetupVar.PmOppFreq[44] = 2400; PlatformSetupVar.PmOppVolt[44] = 860;
      PlatformSetupVar.PmOppFreq[45] = 2600; PlatformSetupVar.PmOppVolt[45] = 930;
      // Domain 4: CPU Big G1
      PlatformSetupVar.PmOppFreq[52] = 800;  PlatformSetupVar.PmOppVolt[52] = 790;
      PlatformSetupVar.PmOppFreq[53] = 1200; PlatformSetupVar.PmOppVolt[53] = 800;
      PlatformSetupVar.PmOppFreq[54] = 1500; PlatformSetupVar.PmOppVolt[54] = 810;
      PlatformSetupVar.PmOppFreq[55] = 1800; PlatformSetupVar.PmOppVolt[55] = 820;
      PlatformSetupVar.PmOppFreq[56] = 2200; PlatformSetupVar.PmOppVolt[56] = 830;
      PlatformSetupVar.PmOppFreq[57] = 2400; PlatformSetupVar.PmOppVolt[57] = 860;
      PlatformSetupVar.PmOppFreq[58] = 2600; PlatformSetupVar.PmOppVolt[58] = 930;
      // Domain 5: CPU Mid G0
      PlatformSetupVar.PmOppFreq[65] = 800;  PlatformSetupVar.PmOppVolt[65] = 790;
      PlatformSetupVar.PmOppFreq[66] = 1200; PlatformSetupVar.PmOppVolt[66] = 800;
      PlatformSetupVar.PmOppFreq[67] = 1500; PlatformSetupVar.PmOppVolt[67] = 810;
      PlatformSetupVar.PmOppFreq[68] = 1800; PlatformSetupVar.PmOppVolt[68] = 820;
      PlatformSetupVar.PmOppFreq[69] = 2000; PlatformSetupVar.PmOppVolt[69] = 820;
      PlatformSetupVar.PmOppFreq[70] = 2200; PlatformSetupVar.PmOppVolt[70] = 860;
      PlatformSetupVar.PmOppFreq[71] = 2400; PlatformSetupVar.PmOppVolt[71] = 930;
      // Domain 6: CPU Mid G1
      PlatformSetupVar.PmOppFreq[78] = 800;  PlatformSetupVar.PmOppVolt[78] = 790;
      PlatformSetupVar.PmOppFreq[79] = 1200; PlatformSetupVar.PmOppVolt[79] = 800;
      PlatformSetupVar.PmOppFreq[80] = 1500; PlatformSetupVar.PmOppVolt[80] = 810;
      PlatformSetupVar.PmOppFreq[81] = 1800; PlatformSetupVar.PmOppVolt[81] = 820;
      PlatformSetupVar.PmOppFreq[82] = 2000; PlatformSetupVar.PmOppVolt[82] = 820;
      PlatformSetupVar.PmOppFreq[83] = 2200; PlatformSetupVar.PmOppVolt[83] = 860;
      PlatformSetupVar.PmOppFreq[84] = 2400; PlatformSetupVar.PmOppVolt[84] = 930;
      // Domain 7: DSU
      PlatformSetupVar.PmOppFreq[91] = 400;  PlatformSetupVar.PmOppVolt[91] = 790;
      PlatformSetupVar.PmOppFreq[92] = 1300; PlatformSetupVar.PmOppVolt[92] = 790;
      // Domain 8: NPU
      PlatformSetupVar.PmOppFreq[104] = 400;
      PlatformSetupVar.PmOppFreq[105] = 600;
      PlatformSetupVar.PmOppFreq[106] = 800;
      PlatformSetupVar.PmOppFreq[107] = 1200;
      // Domain 9: VPU
      PlatformSetupVar.PmOppFreq[117] = 150;
      PlatformSetupVar.PmOppFreq[118] = 300;
      PlatformSetupVar.PmOppFreq[119] = 480;
      PlatformSetupVar.PmOppFreq[120] = 600;
      PlatformSetupVar.PmOppFreq[121] = 800;
      PlatformSetupVar.PmOppFreq[122] = 1200;
      // Domain 10: CI700
      PlatformSetupVar.PmOppFreq[130] = 1500;
      // Domain 11: MMHUB
      PlatformSetupVar.PmOppFreq[143] = 350;
      PlatformSetupVar.PmOppFreq[144] = 600;
      PlatformSetupVar.PmOppFreq[145] = 750;
      // TDP defaults (mW)
      PlatformSetupVar.PmTdp[0] = 2400;
      PlatformSetupVar.PmTdp[1] = 6700;
      PlatformSetupVar.PmTdp[2] = 6500;
      PlatformSetupVar.PmTdp[3] = 8000;
      PlatformSetupVar.PmTdp[4] = 8200;
      PlatformSetupVar.PmTdp[5] = 5500;
      PlatformSetupVar.PmTdp[6] = 12000;
      PlatformSetupVar.PmTdp[7] = 9000;

      DEBUG ((DEBUG_INFO, "%a: Migrating PM defaults into existing variable\n", __FUNCTION__));
      gRT->SetVariable (
             PLATFORM_SETUP_VAR,
             &gPlatformSetupVariableGuid,
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
             sizeof (PLATFORM_SETUP_DATA),
             &PlatformSetupVar
             );
    }
  }

  return Status;
}

EFI_STATUS
NetworkStackVariableInit (
  VOID
  )
{
  EFI_STATUS     Status = EFI_SUCCESS;
  UINTN          VarSize;
  NETWORK_STACK  NetworkStack;

  VarSize = sizeof (NETWORK_STACK);

  Status = gRT->GetVariable (
                  NETWORK_STACK_VAR,
                  &gEfiNetworkStackSetupGuid,
                  NULL,
                  &VarSize,
                  &NetworkStack
                  );
  if (EFI_ERROR (Status) || IsRtcPowerfailure ()) {
    ZeroMem (&NetworkStack, VarSize);
    //
    // Variable does not exist yet - create it
    //
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
      DEBUG ((DEBUG_INFO, "%a: EfiSetVariable failed - %r\n", __FUNCTION__, Status));
    }
  } else {
    DEBUG ((DEBUG_INFO, "%a: EfiGetVariable Success - %r\n", __FUNCTION__, Status));
  }

  return Status;
}

EFI_STATUS
SystemTableVariableInit (
  VOID
  )
{
  EFI_STATUS              Status;
  UINTN                   VarSize;
  SYSTEM_TABLE            SystemTableVar;
  DT_ACPI_VARSTORE_DATA   DtAcpiPref;
  UINT8                   DerivedSelect;

  //
  // Read the authoritative DtAcpiPref variable (set by DtPlatformDxe under
  // Advanced settings).  Sync SystemTableVar to match so that downstream
  // consumers (ConfigParamsDataBlock, Platform.hfr suppressif) stay consistent.
  //
  VarSize = sizeof (DtAcpiPref);
  Status = gRT->GetVariable (
                  DT_ACPI_VARIABLE_NAME,
                  &gDtPlatformFormSetGuid,
                  NULL,
                  &VarSize,
                  &DtAcpiPref
                  );
  if (!EFI_ERROR (Status)) {
    // DtAcpiPref: 0 = DT, 1 = ACPI
    // SystemTableSelect: 0 = ACPI, 1 = DT  (inverted)
    DerivedSelect = (DtAcpiPref.Pref == DT_ACPI_SELECT_DT) ? 1 : 0;
  } else {
    DerivedSelect = FixedPcdGet8 (PcdSystemTableSelect);
  }

  VarSize = sizeof (SYSTEM_TABLE);
  Status = gRT->GetVariable (
                  SYSTEM_TABLE_VAR,
                  &gCixGlobalVariableGuid,
                  NULL,
                  &VarSize,
                  &SystemTableVar
                  );

  if (EFI_ERROR (Status) || IsRtcPowerfailure () ||
      SystemTableVar.SystemTableSelect != DerivedSelect) {
    SystemTableVar.SystemTableSelect = DerivedSelect;
    Status = gRT->SetVariable (
                    SYSTEM_TABLE_VAR,
                    &gCixGlobalVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof (SYSTEM_TABLE),
                    &SystemTableVar
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "%a: SetVariable failed - %r\n", __FUNCTION__, Status));
    }
  }

  return Status;
}

EFI_STATUS
EFIAPI
PlatformSetupVariableInitDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                  Status      = EFI_SUCCESS;
  CONFIG_PARAMS_DATA_BLOCK                    *ConfigData = NULL;
  CIX_CONFIG_PARAMS_MANAGE_PROTOCOL           *ConfigManage;
  PLATFORM_CONFIG_PARAMS_DATA_BLOCK           *PlatformConfigData = NULL;
  CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL  *PlatformConfigManage;
  VOID                                        *Registration;
  EFI_HANDLE                                  Handle;

  Status = PlatformSetupVariableInit ();

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "%a: PlatformSetupVariableInit failed - %r\n", __FUNCTION__, Status));
    return Status;
  } else {
    DEBUG ((DEBUG_INFO, "%a: PlatformSetupVariableInit Success - %r\n", __FUNCTION__, Status));
  }

  Status = NetworkStackVariableInit ();

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "%a: NetworkStackVariableInit failed - %r\n", __FUNCTION__, Status));
    return Status;
  } else {
    DEBUG ((DEBUG_INFO, "%a: NetworkStackVariableInit Success - %r\n", __FUNCTION__, Status));
  }

  Status = SystemTableVariableInit ();

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "%a: SystemTableVariableInit failed - %r\n", __FUNCTION__, Status));
    return Status;
  } else {
    DEBUG ((DEBUG_INFO, "%a: SystemTableVariableInit Success - %r\n", __FUNCTION__, Status));
  }

  //
  Status = gBS->LocateProtocol (&gCixConfigParamsManageProtocolGuid, NULL, (VOID **)&ConfigManage);
  if (!EFI_ERROR (Status)) {
    ConfigData = ConfigManage->Data;
    if (ConfigData != NULL) {
      UpdateConfigParams (ConfigData);
    }
  } else {
    DEBUG ((DEBUG_INFO, "%a: locate gCixConfigParamsManageProtocolGuid failed - %r\n", __FUNCTION__, Status));

    EfiCreateProtocolNotifyEvent (
      &gCixConfigParamsManageProtocolGuid,
      TPL_CALLBACK,
      ConfigParamsUpdateCallBack,
      NULL,
      &Registration
      );
  }

  Status = gBS->LocateProtocol (&gCixPlatformConfigParamsManageProtocolGuid, NULL, (VOID **)&PlatformConfigManage);
  if (!EFI_ERROR (Status)) {
    PlatformConfigData = PlatformConfigManage->Data;
    if (PlatformConfigData != NULL) {
      UpdatePlatformConfigParams (PlatformConfigData);
    }
  } else {
    DEBUG ((DEBUG_INFO, "%a: locate gCixPlatformConfigParamsManageProtocolGuid failed - %r\n", __FUNCTION__, Status));

    EfiCreateProtocolNotifyEvent (
      &gCixPlatformConfigParamsManageProtocolGuid,
      TPL_CALLBACK,
      PlatformConfigParamsUpdateCallBack,
      NULL,
      &Registration
      );
  }

  Status = CheckCpuShareInfo ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "%a: CheckCpuShareInfo failed - %r\n", __FUNCTION__, Status));
  }

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gCixSetupVarInitCompleteProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );

  return Status;
}
