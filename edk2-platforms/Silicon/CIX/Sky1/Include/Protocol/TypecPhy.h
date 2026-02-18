/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 **/

#ifndef __TYPEC_PHY_DXE__
#define __TYPEC_PHY_DXE__

typedef struct _TYPEC_PHY_PROTOCOL TYPEC_PHY_PROTOCOL;

#define RCSU_USB_TYPEC_CTRL_ADDRESS  0x16000420

typedef enum {
  TYPEC_ORIENTATION_NONE,
  TYPEC_ORIENTATION_NORMAL,
  TYPEC_ORIENTATION_REVERSE,
} TYPEC_ORIENTATION;

typedef enum {
  TYPEC_STATE_SAFE,
  TYPEC_STATE_USB,
  TYPEC_DP_STATE_C,
  TYPEC_DP_STATE_D,
  TYPEC_DP_STATE_E,
  TYPEC_DP_STATE_F,
  TYPEC_STATE_DISCONNECT,
} TYPEC_MODE;

typedef
  EFI_STATUS
(EFIAPI *TYPEC_SWITCH_SET)(
  IN CONST TYPEC_PHY_PROTOCOL *This,
  IN TYPEC_ORIENTATION Oritation
  );

typedef
  EFI_STATUS
(EFIAPI *TYPEC_MUX_SET)(
  IN CONST TYPEC_PHY_PROTOCOL *This,
  IN TYPEC_MODE Mode
  );

typedef
  EFI_STATUS
(EFIAPI *U3PHY_INIT)(
  IN CONST TYPEC_PHY_PROTOCOL *This
  );

typedef
  EFI_STATUS
(EFIAPI *U3PHY_EXIT)(
  IN CONST TYPEC_PHY_PROTOCOL *This
  );

typedef
  EFI_STATUS
(EFIAPI *DPPHY_POWER_ON)(
  IN CONST TYPEC_PHY_PROTOCOL *This
  );

typedef
  EFI_STATUS
(EFIAPI *DPPHY_POWER_OFF)(
  IN CONST TYPEC_PHY_PROTOCOL *This
  );

typedef
  EFI_STATUS
(EFIAPI *DPPHY_CONFIGURE)(
  IN CONST TYPEC_PHY_PROTOCOL *This
  );

typedef
  UINT8
(EFIAPI *DPPHY_GET_LANE_NUM)(
  IN CONST TYPEC_PHY_PROTOCOL *This
  );

typedef
  UINT8
(EFIAPI *GET_PHY_CONTROL_INDEX)(
  IN CONST TYPEC_PHY_PROTOCOL *This
  );
typedef
EFI_STATUS
(EFIAPI *ENTER_HOST_MODE)(
  IN CONST TYPEC_PHY_PROTOCOL  *This
  );

struct _TYPEC_PHY_PROTOCOL {
  UINT32                   PhyBaseAddress;
  TYPEC_SWITCH_SET         TypecSwitchSet;
  TYPEC_MUX_SET            TypecMuxSet;
  U3PHY_INIT               U3phyInit;
  U3PHY_EXIT               U3phyExit;
  DPPHY_POWER_ON           DpPhyPowerOn;
  DPPHY_POWER_OFF          DpPhyPowerOff;
  DPPHY_CONFIGURE          DpPhyConfigure;
  DPPHY_GET_LANE_NUM       DpPhyGetLaneNum;
  GET_PHY_CONTROL_INDEX    GetPhyControlIndex;
  ENTER_HOST_MODE          EnterHostMode;
};

#endif
