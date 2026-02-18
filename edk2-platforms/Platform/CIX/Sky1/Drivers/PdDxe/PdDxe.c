/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
  Copyright (c) 2021, ARM Limited. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "PdDxe.h"

#define ALERT_PIN_POLL_INTERVAL             10
#define ALERT_PIN_POLL_MAX_COUNT            1000 // max wait time 10ms (ALERT_PIN_POLL_INTERVAL * ALERT_PIN_POLL_MAX_COUNT)
#define PHY_SWITCH_DELAY_AFTER_PD_UPDATE    1000 * 1000 * 1.5
#define ALERT_TIMER_CALLBACK_INTERVAL       1000 * 100

PD_DEV_INFO  *mPdDevList = NULL;
UINT8        mPdDevCnt   = 0;

CHAR8  *TypecModeStrs[] = {
  "TYPEC_STATE_SAFE", "TYPEC_STATE_USB",  "TYPEC_DP_STATE_C", "TYPEC_DP_STATE_D",
  "TYPEC_DP_STATE_E", "TYPEC_DP_STATE_F", "TYPEC_STATE_DISCONNECT"
};
CHAR8  *TypecOrientationStrs[] = { "TYPEC_ORIENTATION_NONE", "TYPEC_ORIENTATION_NORMAL", "TYPEC_ORIENTATION_REVERSE" };

VOID
EFIAPI
TypecPhyTimerCallback (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  EFI_STATUS          Status;
  TYPEC_PHY_INFO      *PhyInfo   = (TYPEC_PHY_INFO *)Context;
  UINT8               TypcPhyIdx;
  TYPEC_PHY_PROTOCOL  *TypecPhy  = PhyInfo->TypecPhyProtocol;
  TYPEC_PORT_STATE    PortState  = PhyInfo->TypecPortState;

  TypcPhyIdx = PhyInfo->PhyControlIndex;

  DEBUG ((
    DEBUG_INFO,
    "\n\n[Typec phy %d timer callback enter]\n",
    TypcPhyIdx
    ));

  if (PortState.Mode == TYPEC_STATE_USB ||
      PortState.Mode == TYPEC_DP_STATE_D) {
    Status = TypecPhy->U3phyInit (TypecPhy);
    if (EFI_ERROR (Status)) {
      DebugPrint (
        DEBUG_ERROR,
        "%a: Typec phy %d, U3 Phy Init %r\n",
        __FUNCTION__,
        TypcPhyIdx,
        Status
        );
    }
  } else {
    Status = TypecPhy->U3phyExit (TypecPhy);
    if (EFI_ERROR (Status)) {
      DebugPrint (
        DEBUG_ERROR,
        "%a: Typec phy %d, U3 Phy Exit %r\n",
        __FUNCTION__,
        TypcPhyIdx,
        Status
        );
    }
  }

  DEBUG ((
    DEBUG_INFO,
    "[Typec phy %d timer callback exit]\n\n\n",
    TypcPhyIdx
    ));

  FreePool(Context);

  gBS->CloseEvent(Event);
}

EFI_STATUS
UpdateTypecPhy (
  IN UINT8             TypcPhyIdx,
  IN BOOLEAN           ReadyForIrq,
  IN TYPEC_PORT_STATE  PortState
  )
{
  EFI_STATUS          Status           = EFI_SUCCESS;
  UINTN               PhyCount         = 0;
  EFI_HANDLE          *PhyHandleBuffer = NULL;
  UINTN               PhyIndex         = 0;
  TYPEC_PHY_PROTOCOL  *TypecPhy;

  DEBUG ((
    DEBUG_INFO,
    "Update Typec phy %d, Mode: %a, Orientation: %a\n",
    TypcPhyIdx,
    TypecModeStrs[PortState.Mode],
    TypecOrientationStrs[PortState.Orientation]
    ));

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gCixTypecPhyProtocolGuid,
                  NULL,
                  &PhyCount,
                  &PhyHandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    DebugPrint (DEBUG_ERROR, "%a: Locate Handle Buffer %g state %r\n", __FUNCTION__, &gCixTypecPhyProtocolGuid, Status);
    return Status;
  }

  for (PhyIndex = 0; PhyIndex < PhyCount; PhyIndex++) {
    Status = gBS->HandleProtocol (PhyHandleBuffer[PhyIndex], &gCixTypecPhyProtocolGuid, (VOID **)&TypecPhy);
    if (EFI_ERROR (Status)) {
      DebugPrint (DEBUG_ERROR, "%a: Handle Protocol %g status: %r\n", __FUNCTION__, &gCixTypecPhyProtocolGuid, Status);
      goto exit;
    }

    if (TypecPhy->GetPhyControlIndex (TypecPhy) == TypcPhyIdx) {
      break;
    }
  }

  if (PhyIndex == PhyCount) {
    // no matched typc phy found
    DEBUG ((
      DEBUG_INFO,
      "Typec phy %d is not supported\n",
      TypcPhyIdx
      ));
    Status = EFI_UNSUPPORTED;
    goto exit;
  }

  Status = TypecPhy->TypecSwitchSet (TypecPhy, PortState.Orientation);
  if (EFI_ERROR (Status)) {
    DebugPrint (
      DEBUG_ERROR,
      "%a: Typec phy %d, Set Orientation %x %r\n",
      __FUNCTION__,
      TypcPhyIdx,
      PortState.Orientation,
      Status
      );

    goto exit;
  }

  Status = TypecPhy->TypecMuxSet (TypecPhy, PortState.Mode);
  if (EFI_ERROR (Status)) {
    DebugPrint (
      DEBUG_ERROR,
      "%a: Typec phy %d, Set Mode: %x %r\n",
      __FUNCTION__,
      TypcPhyIdx,
      PortState.Mode,
      Status
      );

    goto exit;
  }

  if (ReadyForIrq) {
    EFI_EVENT       TimerEvent;
    TYPEC_PHY_INFO  *PhyInfo = (TYPEC_PHY_INFO *)AllocatePool (sizeof (TYPEC_PHY_INFO));
    PhyInfo->PhyControlIndex  = TypcPhyIdx;
    PhyInfo->TypecPhyProtocol = TypecPhy;
    CopyMem ((void *)&(PhyInfo->TypecPortState), (void *)&PortState, sizeof (PortState));
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL | EVT_TIMER,
                    TPL_CALLBACK,
                    TypecPhyTimerCallback,
                    PhyInfo,
                    &TimerEvent
                    );
    Status = gBS->SetTimer (TimerEvent, TimerRelative, PHY_SWITCH_DELAY_AFTER_PD_UPDATE);
    DEBUG ((
      DEBUG_INFO,
      "Typec phy %d create timer callback %r\n",
      TypcPhyIdx,
      Status
      ));
  }

exit:
  if (PhyHandleBuffer) {
    FreePool (PhyHandleBuffer);
  }

  return Status;
}

VOID
PdAlertCallBack (
  IN VOID   *Context,
  IN UINTN  Size
  )
{
  EFI_STATUS      Status        = EFI_SUCCESS;
  ALERT_INTR_INFO *AlertInfo    = (ALERT_INTR_INFO *)Context;
  UINT8           *ConnectedIdx = AlertInfo->ConnectIdxs;
  UINT8           ConnectedCnt  = AlertInfo->ConnectCnt;

  DEBUG ((
    DEBUG_INFO,
    "\n\n[Pd Alert Interrupt Enter]\n"
    ));

  for (UINT8 Index = 0; Index < ConnectedCnt; Index++) {
    UINT8        PdDevIdx = ConnectedIdx[Index];
    PD_DEV_INFO  *PdDev   = &mPdDevList[PdDevIdx];

    PdEnterAlertInterrupt (PdDevIdx);

    Status = PdGetPortState (
               PdDevIdx,
               &PdDev->TypecPortState
               );
    if (EFI_ERROR (Status)) {
      DebugPrint (
        DEBUG_ERROR,
        "%a: Pd Device %d get port state %r\n",
        __FUNCTION__,
        PdDevIdx,
        Status
        );
    } else {
      DEBUG ((
        DEBUG_INFO,
        "Update Pd Device %d state, Mode: %a, Orientation: %a\n",
        PdDevIdx,
        TypecModeStrs[PdDev->TypecPortState.Mode],
        TypecOrientationStrs[PdDev->TypecPortState.Orientation]
        ));
      UpdateTypecPhy (
        PdDev->PhyControlIndex,
        PdDev->TypecPhyReadyForIRQ,
        PdDev->TypecPortState
        );
    }

    PdExitAlertInterrupt (PdDevIdx);
  }

  IO_INOUT_VALUE_SEL PinVal;
  UINT32 RetryCnt = 0;
  do {
    GpioGet(AlertInfo->AlertPin, &PinVal);
    RetryCnt ++;
    if (PinVal == INOUT_HIGH) {
        break;
    }
    MicroSecondDelay(ALERT_PIN_POLL_INTERVAL);
  } while (RetryCnt < ALERT_PIN_POLL_MAX_COUNT);

  if(RetryCnt == ALERT_PIN_POLL_MAX_COUNT) {
    DEBUG ((
        DEBUG_INFO,
        "Pd alert interrupt (gpio [%d]) not cleared in time\n",
        AlertInfo->AlertPin
        ));
  }

  DEBUG ((
    DEBUG_INFO,
    "[Pd Alert Interrupt Exit]\n\n\n"
    ));
}

VOID
EFIAPI
PdAlertTimerCallback (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  ALERT_INTR_INFO *AlertInfo = (ALERT_INTR_INFO *)Context;
  IO_INOUT_VALUE_SEL PinVal;

  GpioGet(AlertInfo->AlertPin, &PinVal);
  if (PinVal == INOUT_LOW) {
    PdAlertCallBack(AlertInfo, sizeof(ALERT_INTR_INFO));
  }

  gBS->SetTimer (Event, TimerRelative, ALERT_TIMER_CALLBACK_INTERVAL);
}

VOID
SortEnabledAlertPin (
  PD_DEV_INFO  *PdDevList,
  UINT8        PdDevCount,
  UINT32       **EnabledPins,
  UINT8        *EnabledCnt
  )
{
  UINT32  *AlertPins = (UINT32 *)AllocatePool (PdDevCount * sizeof (UINT32));
  UINT8   i, j;
  UINT8   SortedCnt = 0;

  *EnabledCnt = 0;

  for (i = 0; i < PdDevCount; i++) {
    if (!PdDevList[i].Enabled) {
      continue;
    }

    for (j = 0; j < SortedCnt; j++) {
      if (PdDevList[i].AlertPin == AlertPins[j]) {
        break;
      }
    }

    if (j == SortedCnt) {
      AlertPins[j] = PdDevList[i].AlertPin;
      SortedCnt++;
    }
  }

  if (SortedCnt > 0) {
    *EnabledPins = (UINT32 *)AllocatePool (SortedCnt * sizeof (UINT32));
    CopyMem (*EnabledPins, AlertPins, SortedCnt * sizeof (UINT32));
    *EnabledCnt = SortedCnt;
  }

  if (AlertPins) {
    FreePool (AlertPins);
  }
}

VOID
CollectConnectedDeviceIndex (
  PD_DEV_INFO  *PdDevList,
  UINT32       PdDevCount,
  UINT32       AlertPin,
  UINT8        **ConnectedIdx,
  UINT8        *ConnectedCnt
  )
{
  UINT8  *TmpIdxs  = (UINT8 *)AllocatePool (PdDevCount);
  UINT8  SortedCnt = 0;
  UINT8  Index;

  *ConnectedCnt = 0;

  for (Index = 0; Index < PdDevCount; Index++) {
    if (!PdDevList[Index].Enabled ||
        (PdDevList[Index].AlertPin != AlertPin))
    {
      continue;
    }

    TmpIdxs[SortedCnt] = Index;
    SortedCnt++;
  }

  if (SortedCnt > 0) {
    *ConnectedIdx = (UINT8 *)AllocatePool (SortedCnt);
    CopyMem (*ConnectedIdx, TmpIdxs, SortedCnt);
    *ConnectedCnt = SortedCnt;
  }

  if (TmpIdxs) {
    FreePool (TmpIdxs);
  }
}

UINT8
GetPdDeviceCount (
  VOID
  )
{
  return mPdDevCnt;
}

EFI_STATUS
SetPhyReadyForAlertIRQ (
  IN UINT8  PdDevIndex
  )
{
  if (!mPdDevList || (PdDevIndex >= mPdDevCnt)) {
    return EFI_UNSUPPORTED;
  }

  mPdDevList[PdDevIndex].TypecPhyReadyForIRQ = TRUE;

  return EFI_SUCCESS;
}

EFI_STATUS
GetPortState (
  IN      UINT8             PdDevIndex,
  IN OUT  TYPEC_PORT_STATE  *PortState
  )
{
  if (!mPdDevList || (PdDevIndex >= mPdDevCnt)) {
    return EFI_UNSUPPORTED;
  }

  PortState->Mode        = mPdDevList[PdDevIndex].TypecPortState.Mode;
  PortState->Orientation = mPdDevList[PdDevIndex].TypecPortState.Orientation;

  return EFI_SUCCESS;
}

STATIC CIX_FD_PROTOCOL  CixPdProtocol = {
  GetPdDeviceCount,
  SetPhyReadyForAlertIRQ,
  GetPortState
};

EFI_STATUS
RegisterAlertTimerCallback (
  VOID
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32      *EnabledPins = NULL;
  UINT8       EnabledCnt = 0;
  UINT8       Index = 0;

  if (!mPdDevList || (mPdDevCnt == 0)) {
    DebugPrint (DEBUG_ERROR, "%a: %d Input invalid parameter\n", __FUNCTION__, __LINE__);
    return EFI_INVALID_PARAMETER;
  }

  SortEnabledAlertPin (mPdDevList, mPdDevCnt, &EnabledPins, &EnabledCnt);

  DEBUG ((DEBUG_INFO, "Enabled alert pin nums:"));
  if (EnabledCnt) {
    for ( UINT8 i = 0; i < EnabledCnt; i++) {
      DEBUG ((DEBUG_INFO, " %d", EnabledPins[i]));
    }
  } else {
    DEBUG ((DEBUG_INFO, " not found"));
  }

  DEBUG ((DEBUG_INFO, "\n"));

  for (Index = 0; Index < EnabledCnt; Index++) {
    UINT8  *ConnectedIdx;
    UINT8  ConnectedCnt;

    CollectConnectedDeviceIndex (mPdDevList, mPdDevCnt, EnabledPins[Index], &ConnectedIdx, &ConnectedCnt);

    DEBUG ((DEBUG_INFO, "Alert Pin Num %d connected pd device indexes:", EnabledPins[Index]));
    if (ConnectedCnt) {
      for (UINT8 i = 0; i < ConnectedCnt; i++) {
        DEBUG ((DEBUG_INFO, " %d", ConnectedIdx[i]));
      }
    } else {
      DEBUG ((DEBUG_INFO, " not found"));
    }

    DEBUG ((DEBUG_INFO, "\n"));

    if (ConnectedCnt > 0) {
      EFI_EVENT TimerEvent;
      ALERT_INTR_INFO *AlertInfo = (ALERT_INTR_INFO *)AllocatePool(sizeof(ALERT_INTR_INFO));
      AlertInfo->AlertPin = EnabledPins[Index];
      AlertInfo->ConnectCnt = ConnectedCnt;
      AlertInfo->ConnectIdxs = ConnectedIdx;

      Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL | EVT_TIMER,
                    TPL_CALLBACK,
                    PdAlertTimerCallback,
                    AlertInfo,
                    &TimerEvent
                    );
      Status = gBS->SetTimer (TimerEvent, TimerRelative, ALERT_TIMER_CALLBACK_INTERVAL);
      DEBUG ((DEBUG_INFO, "%a: register alert pin num %d pd timer callback status: %r\n", __FUNCTION__, EnabledPins[Index], Status));
    }
  }

  if (EnabledPins) {
    FreePool (EnabledPins);
  }

  return Status;
}

/**
  Entrypoint of Pd Dxe Driver

  @param  ImageHandle[in]      The firmware allocated handle for the EFI image.
  @param  SystemTable[in]      A pointer to the EFI System Table.

  @retval EFI_SUCCESS          Pd Dxe update typec phy and register interrupt success.
  @retval Others               Unexpected error happened.

**/
EFI_STATUS
EFIAPI
PdDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status                 = EFI_SUCCESS;
  UINT8       *pPdDevAlertPins       = PcdGetPtr (PcdPdDevAlertPins);
  UINT8       *pTpyecDftModes        = PcdGetPtr (PcdTypecPortDefaultModes);
  UINT8       *pTpyecDftOrientations = PcdGetPtr (PcdTypecPortDefaultOrientations);

  POST_CODE (PdDxeStart);

  mPdDevCnt  = PcdGet8 (PcdPdDevCount);
  mPdDevList = (PD_DEV_INFO *)AllocatePool (mPdDevCnt * sizeof (PD_DEV_INFO));

  for (UINT8 PdDevIdx = 0; PdDevIdx < mPdDevCnt; PdDevIdx++) {
    PD_DEV_INFO  *PdDev = &mPdDevList[PdDevIdx];
    PdDev->Enabled             = IsPdDeviceValid (PdDevIdx);
    PdDev->PhyControlIndex     = PdDevIdx;    // default invalid value
    PdDev->AlertPin            = pPdDevAlertPins[PdDevIdx];
    PdDev->TypecPhyReadyForIRQ = FALSE;    // wait for usb host to set this flag true after reset finish

    if (PdDev->Enabled) {
      // get typec port state
      Status = PdGetPortState (
                 PdDevIdx,
                 &PdDev->TypecPortState
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((
          DEBUG_INFO,
          "Pd Device %d update port state fail, set default state\n",
          PdDevIdx
          ));

        // get port state fail, set default state
        PdDev->TypecPortState.Mode        = pTpyecDftModes[PdDevIdx];
        PdDev->TypecPortState.Orientation = pTpyecDftOrientations[PdDevIdx];
      }
    } else {
      // pd device is invalid
      DEBUG ((
        DEBUG_INFO,
        "Pd Device %d is invalid, set default state\n",
        PdDevIdx
        ));

      // no pd device connected
      PdDev->TypecPortState.Mode        = pTpyecDftModes[PdDevIdx];
      PdDev->TypecPortState.Orientation = pTpyecDftOrientations[PdDevIdx];
    }

    DebugPrint (
      DEBUG_INFO,
      "Update Pd Device %d state, Mode: %a, Orientation: %a\n",
      PdDevIdx,
      TypecModeStrs[PdDev->TypecPortState.Mode],
      TypecOrientationStrs[PdDev->TypecPortState.Orientation]
      );

    UpdateTypecPhy (
      PdDev->PhyControlIndex,
      FALSE,
      PdDev->TypecPortState
      );
  }

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gCixPdProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &CixPdProtocol
                  );
  if (EFI_ERROR (Status)) {
    DebugPrint (
      DEBUG_ERROR,
      "%a: failed to install pd protocol %r\n",
      __FUNCTION__,
      Status
      );
    return Status;
  }

  Status = RegisterAlertTimerCallback ();
  if (EFI_ERROR (Status)) {
    DebugPrint (
      DEBUG_ERROR,
      "%a: Register alert timer callback %r\n",
      __FUNCTION__,
      Status
      );
     return Status;
  }

  POST_CODE (PdDxeEnd);

  return EFI_SUCCESS;
}
