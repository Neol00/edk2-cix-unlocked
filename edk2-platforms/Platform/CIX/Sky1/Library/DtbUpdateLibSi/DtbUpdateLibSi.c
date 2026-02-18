/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 **/

#include "DtbUpdate.h"
#include <libfdt.h>
#include <fdt.h>
#include <Library/HwHarvestLib.h>
#include <Protocol/PlatformConfigParamsManageProtocol.h>

STATIC
INT32
fdt_alloc_phandle (
  IN VOID  *Fdt
  )
{
  INT32  Offset;
  INT32  Phandle;

  Phandle = 0;

  for (Offset = fdt_next_node (Fdt, -1, NULL); Offset >= 0;
       Offset = fdt_next_node (Fdt, Offset, NULL))
  {
    Phandle = MAX (Phandle, fdt_get_phandle (Fdt, Offset));
  }

  return Phandle + 1;
}

STATIC
INT32
fdt_set_phandle (
  IN VOID    *Fdt,
  IN UINT32  NodeOffset,
  IN UINT32  Phandle
  )
{
  INT32  Ret;

  Ret = fdt_setprop_cell (Fdt, NodeOffset, "phandle", Phandle);
  if (Ret < 0) {
    return Ret;
  }

  Ret = fdt_setprop_cell (Fdt, NodeOffset, "linux,phandle", Phandle);

  return Ret;
}

STATIC
INT32
fdt_create_phandle (
  IN VOID    *Fdt,
  IN UINT32  NodeOffset
  )
{
  INT32  Phandle, Ret;

  Phandle = fdt_get_phandle (Fdt, NodeOffset);
  if (Phandle == 0) {
    Phandle = fdt_alloc_phandle (Fdt);
    Ret     = fdt_set_phandle (Fdt, NodeOffset, Phandle);

    if (Ret < 0) {
      DEBUG (
             (
              DEBUG_INFO,
              "%a: Can't set phandle %d: %a\n",
              __FUNCTION__,
              Phandle,
              fdt_strerror (Ret)
             )
             );
      return 0;
    }
  }

  return Phandle;
}

UINT32
fdt_check_header_ext_si (
  VOID  *fdt
  )
{
  UINT64  fdt_start, fdt_end;
  UINT32  sum;

  fdt_start = (UINT64)fdt;

  if (fdt_start + fdt_totalsize (fdt) <= fdt_start) {
    return FDT_ERR_BADOFFSET;
  }

  fdt_end = fdt_start + fdt_totalsize (fdt);

  if (!(sum = ADD_OF (fdt_off_dt_struct (fdt), fdt_size_dt_struct (fdt)))) {
    return FDT_ERR_BADOFFSET;
  } else {
    if (CHECK_ADD64 (fdt_start, sum)) {
      return FDT_ERR_BADOFFSET;
    } else if (fdt_start + sum > fdt_end) {
      return FDT_ERR_BADOFFSET;
    }
  }

  if (!(sum = ADD_OF (fdt_off_dt_strings (fdt), fdt_size_dt_strings (fdt)))) {
    return FDT_ERR_BADOFFSET;
  } else {
    if (CHECK_ADD64 (fdt_start, sum)) {
      return FDT_ERR_BADOFFSET;
    } else if (fdt_start + sum > fdt_end) {
      return FDT_ERR_BADOFFSET;
    }
  }

  if (fdt_start + fdt_off_mem_rsvmap (fdt) > fdt_end) {
    return FDT_ERR_BADOFFSET;
  }

  return 0;
}

VOID
EnableDtbNode (
  IN  VOID         *fdt,
  IN  CONST CHAR8  *NodePath
  )
{
  INT32  Node;
  INT32  Rc;

  Node = fdt_path_offset (fdt, NodePath);
  if (Node < 0) {
    DEBUG (
           (DEBUG_ERROR, "%a: failed to locate DT path '%a': %a\n",
            __FUNCTION__, NodePath, fdt_strerror (Node))
           );
    return;
  }

  Rc = fdt_setprop_string (fdt, Node, "status", "okay");
  if (Rc < 0) {
    DEBUG ((DEBUG_ERROR, "%a: failed to set status to 'disabled' on '%a': %a\n", __FUNCTION__, NodePath, fdt_strerror (Rc)));
  }
}

STATIC
VOID
UpdateDtbNodeInterrupts (
  IN  VOID         *fdt,
  IN  CONST CHAR8  *NodePath
  )
{
  INT32  Node;
  INT32  Rc;
  UINT8  Interrupt[12] = { 0, 0, 0, 0, 0, 0, 0x1, 0x2A, 0, 0, 0, 0x4 };// INTID 298

  Node = fdt_path_offset (fdt, NodePath);
  if (Node < 0) {
    DEBUG (
           (DEBUG_ERROR, "%a: failed to locate DT path '%a': %a\n",
            __FUNCTION__, NodePath, fdt_strerror (Node))
           );
    return;
  }

  Rc = fdt_setprop (fdt, Node, "interrupts", Interrupt, 12);
  if (Rc < 0) {
    DEBUG ((DEBUG_ERROR, "%a: failed to update interrupts on '%a': %a\n", __FUNCTION__, NodePath, fdt_strerror (Rc)));
  }
}

VOID
DisableDtbNode (
  IN  VOID         *fdt,
  IN  CONST CHAR8  *NodePath
  )
{
  INT32  Node;
  INT32  Rc;

  Node = fdt_path_offset (fdt, NodePath);
  if (Node < 0) {
    DEBUG (
           (DEBUG_ERROR, "%a: failed to locate DT path '%a': %a\n",
            __FUNCTION__, NodePath, fdt_strerror (Node))
           );
    return;
  }

  Rc = fdt_setprop_string (fdt, Node, "status", "disabled");
  if (Rc < 0) {
    DEBUG ((DEBUG_ERROR, "%a: failed to set status to 'disabled' on '%a': %a\n", __FUNCTION__, NodePath, fdt_strerror (Rc)));
  }
}

VOID
NpuHarvest (
  IN  VOID         *fdt,
  IN  CONST CHAR8  *NodePath,
  IN  UINT8        HarvestInfoValue
  )
{
  INT32  Node;
  INT32  Rc;

  Node = fdt_path_offset (fdt, NodePath);
  if (Node < 0) {
    DEBUG (
           (DEBUG_ERROR, "%a: failed to locate DT path '%a': %a\n",
            __FUNCTION__, NodePath, fdt_strerror (Node))
           );
    return;
  }

  Rc = fdt_setprop_cell (fdt, Node, "core_mask", HarvestInfoValue);
  if (Rc < 0) {
    DEBUG ((DEBUG_ERROR, "%a: failed to set status to 'disabled' on '%a': %a\n", __FUNCTION__, NodePath, fdt_strerror (Rc)));
  }
}

EFI_STATUS
UpdateUART2Interrupts (
  IN  VOID  *fdt
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  UpdateDtbNodeInterrupts (fdt, "/soc@0/uart@040d0000");
  // InterruptsDtbNodeUpdate(fdt, "/soc@0/uart@040d0000",1,298);//cl21248 uart intid

  return Status;
}

STATIC
VOID
DbtNodePropRm (
  IN  VOID         *Fdt,
  IN  CONST CHAR8  *DeviceNodeName0,
  IN  CONST CHAR8  *DeviceNodeName1,
  IN  CONST CHAR8  *PropName
  )
{
  INT32  Node, NodeIdle;
  INT32  PhandleVal, Phandle[2] = { 0, 0 }, Error;

  NodeIdle = fdt_path_offset (Fdt, DeviceNodeName0);
  if (NodeIdle <= 0) {
    DEBUG (
           (
            DEBUG_INFO,
            "%a: Failed to find path %a: %a\n",
            __FUNCTION__,
            DeviceNodeName0,
            fdt_strerror (NodeIdle)
           )
           );
    return;
  }

  PhandleVal = fdt_get_phandle (Fdt, NodeIdle);
  if (PhandleVal <= 0) {
    PhandleVal = fdt_create_phandle (Fdt, NodeIdle);
    if (!PhandleVal) {
      DEBUG (
             (
              DEBUG_INFO,
              "%a: Failed to create Phandle %s: %a\n",
              __FUNCTION__,
              DeviceNodeName0,
              fdt_strerror (PhandleVal)
             )
             );
      return;
    }
  }

  DEBUG ((DEBUG_INFO, "%a: Phandle : %d\n", __FUNCTION__, PhandleVal));

  Node = fdt_path_offset (Fdt, DeviceNodeName1);
  if (Node <= 0) {
    DEBUG (
           (
            DEBUG_INFO,
            "%a: Failed to find path %s: %a\n",
            __FUNCTION__,
            DeviceNodeName1,
            fdt_strerror (Node)
           )
           );
    return;
  }

 #if 1
  Phandle[0] = cpu_to_fdt32 (PhandleVal);
  Error      = fdt_setprop (Fdt, Node, PropName, Phandle, sizeof (Phandle));
  if (Error != 0) {
    DEBUG (
           (
            DEBUG_INFO,
            "%a: Failed to delete property %a: %a\n",
            __FUNCTION__,
            PropName,
            fdt_strerror (Error)
           )
           );
    return;
  }

 #endif
}

VOID
UpdateDtbNodePropertyValue (
  IN  VOID         *fdt,
  IN  CONST CHAR8  *NodePath,
  IN  CONST CHAR8  *PropertyName,
  IN  UINT32       PropertyValue
  )
{
  INT32  Node;
  INT32  Rc;

  Node = fdt_path_offset (fdt, NodePath);
  if (Node < 0) {
    DebugPrint(DEBUG_ERROR, "%a: failed to locate DT path '%a': %a\n",
            __FUNCTION__, NodePath, fdt_strerror (Node));
    return;
  }

  DebugPrint (DEBUG_INFO, "set %a to 0x%X on '%a'\n", PropertyName, PropertyValue, NodePath);

  Rc = fdt_setprop_u32 (fdt, Node, PropertyName, PropertyValue);
  if (Rc < 0) {
    DebugPrint (DEBUG_ERROR, "%a: failed to set %a to 0x%X on '%a': %a\n", __FUNCTION__, PropertyName, PropertyValue, NodePath, fdt_strerror (Rc));
  }
}

VOID
UpdateDtbNodePropertyString (
  IN  VOID         *fdt,
  IN  CONST CHAR8  *NodePath,
  IN  CONST CHAR8  *PropertyName,
  IN  CONST CHAR8  *PropertyValue
  )
{
  INT32  Node;
  INT32  Rc;

  Node = fdt_path_offset (fdt, NodePath);
  if (Node < 0) {
    DEBUG (
           (DEBUG_ERROR, "%a: failed to locate DT path '%a': %a\n",
            __FUNCTION__, NodePath, fdt_strerror (Node))
           );
    return;
  }

  Rc = fdt_setprop_string (fdt, Node, PropertyName, PropertyValue);
  if (Rc < 0) {
    DEBUG ((DEBUG_ERROR, "%a: failed to set %a to %a on '%a': %a\n", __FUNCTION__, PropertyName, PropertyValue, NodePath, fdt_strerror (Rc)));
  }
}

EFI_STATUS
UpdateGopDtbStatus (
  IN  VOID  *fdt
  )
{
  CHAR8  *DpuEndPoints[] = {
  "/soc@0/disp-controller@14010000",
  "/soc@0/disp-controller@14080000",
  "/soc@0/disp-controller@140f0000",
  "/soc@0/disp-controller@14160000",
  "/soc@0/disp-controller@141d0000",
  };

  CHAR8  *DptxEndPoints[] = {
  "/soc@0/dp@14060000",
  "/soc@0/dp@140d0000",
  "/soc@0/dp@14140000",
  "/soc@0/dp@141b0000",
  "/soc@0/dp@14220000",
  };

  EFI_STATUS Status;
  CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL  *PlatformConfig;
  UINT8 GopDisplayPort = 0xFF;
  UINT8 Count = sizeof(DpuEndPoints) / sizeof(CHAR8 *);
  UINT8 Value = 0;

  Status = gBS->LocateProtocol (&gCixPlatformConfigParamsManageProtocolGuid, NULL, (VOID **)&PlatformConfig);
  if (EFI_ERROR (Status)) {
    DebugPrint(DEBUG_ERROR, "Locate %g %r, Update dts according to default value GopDisplayPort=0x%X\n",
                                          &gCixPlatformConfigParamsManageProtocolGuid, Status, GopDisplayPort);
  } else {
    GopDisplayPort = PlatformConfig->Data->GopDisplayPort;
    DebugPrint(DEBUG_INFO, "Update dts according to Platform Config Param GopDisplayPort=0x%X\n", GopDisplayPort);
  }

  for (UINT8 i = 0; i < Count; i++) {
    if(i == GopDisplayPort) {
      Value = 1;
    } else {
      Value = 0;
    }

    UpdateDtbNodePropertyValue(fdt, DpuEndPoints[i], "enabled_by_gop", Value);
    UpdateDtbNodePropertyValue(fdt, DptxEndPoints[i], "enabled_by_gop", Value);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
 UpdateTypecPhyDtbStatus(
  IN  VOID  *fdt
  )
{
  #define DEVICE_MODE 0
  #define USB_MODE    1
  #define DP_MODE     2
  #define DP_USB_MODE 3

  CHAR8  *TypecModeStrs[] = {
  "device",
  "usb",
  "dp-only",
  "dp+usb",
  };

  CHAR8  *TypecPhyEndPoints[] = {
  "/soc@0/usb-phy@9030000",
  "/soc@0/usb-phy@90A0000",
  "/soc@0/usb-phy@9110000",
  "/soc@0/usb-phy@9180000",
  };

  EFI_STATUS Status;
  CIX_PLATFORM_CONFIG_PARAMS_MANAGE_PROTOCOL  *PlatformConfig = NULL;
  UINT8 Count = sizeof(TypecPhyEndPoints) / sizeof(CHAR8 *);
  CHAR8 *Value;

  Status = gBS->LocateProtocol (&gCixPlatformConfigParamsManageProtocolGuid, NULL, (VOID **)&PlatformConfig);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "Locate %g %r, Update dts according to default value TypecMode=0x%X\n",
                                          &gCixPlatformConfigParamsManageProtocolGuid, Status, USB_MODE));
  }

  for (UINT8 i = 0; i < Count; i++) {
    UINT8 DisplayPortIdx = i;
    // skip display port 2, fixed at edp
    if(i > 1) {
      DisplayPortIdx += 1;
    }

    if(PlatformConfig == NULL) {
      // default set usb mode
      Value = TypecModeStrs[USB_MODE];
    } else {
      // if pd report dp or usb+dp, but no display on it, set usb mode
      if (PlatformConfig->Data->TypecMode[i] > USB_MODE &&
          PlatformConfig->Data->GopDisplayPort != DisplayPortIdx) {
        Value = TypecModeStrs[USB_MODE];
      } else {
        Value = TypecModeStrs[PlatformConfig->Data->TypecMode[i]];
      }
    }

    DEBUG((DEBUG_INFO, "Update typec phy [%d] endpoint: %a, phy-status: %a\n", i, TypecPhyEndPoints[i], Value));
    UpdateDtbNodePropertyString(fdt, TypecPhyEndPoints[i], "phy-status", Value);
  }

  return EFI_SUCCESS;
}

typedef struct {
    UINT8 HarvestStatus;
    UINT8 HarvestInfoValue;
} NpuCoreStatus;

NpuCoreStatus NpuStatus[] = {
  {3, 0},
  {2, 2},
  {1, 1},
  {0, 3}
};

EFI_STATUS
UpdateDtbStatus (
  IN  VOID  *fdt
  )
{
  UINT32 NpuHarvestStatus = 0;
  UINT32 HarvestInfoValue = 3;
  // PCIE
  if (IsIpHarvested (PcieX8)) {
    DisableDtbNode (fdt, DT_NODE_PCIEX8_RC);
  }

  // NPU
  NpuHarvestStatus = (IsIpHarvested (NpuCore0)<<1)|IsIpHarvested (NpuCore1_2);
  for(UINT32 i = 0; i < 4; i++) {
    if (NpuHarvestStatus == NpuStatus[i].HarvestStatus) {
      HarvestInfoValue = NpuStatus[i].HarvestInfoValue;
      break;
    }
  }
  NpuHarvest (fdt, DT_NODE_AIPU, HarvestInfoValue);

  // AUDIO
  if (IsIpHarvested (Audio)) {
    DisableDtbNode (fdt, DT_NODE_HIFI5);
    DisableDtbNode (fdt, DT_NODE_MBOX_AP2DSP);
    DisableDtbNode (fdt, DT_NODE_MBOX_DSP2AP);
    DisableDtbNode (fdt, DT_NODE_AUDSS_CRU);
    DisableDtbNode (fdt, DT_NODE_AUDSS_CLK);
    DisableDtbNode (fdt, DT_NODE_AUDSS_RST);
    DisableDtbNode (fdt, DT_NODE_AUDSS_DMAC);
    DisableDtbNode (fdt, DT_NODE_IPB_HDA);
    DisableDtbNode (fdt, DT_NODE_I2S0);
    DisableDtbNode (fdt, DT_NODE_I2S1);
    DisableDtbNode (fdt, DT_NODE_I2S2);
    DisableDtbNode (fdt, DT_NODE_I2S3);
    DisableDtbNode (fdt, DT_NODE_I2S4);
    DisableDtbNode (fdt, DT_NODE_I2S5);
    DisableDtbNode (fdt, DT_NODE_I2S6);
    DisableDtbNode (fdt, DT_NODE_I2S7);
    DisableDtbNode (fdt, DT_NODE_I2S8);
    DisableDtbNode (fdt, DT_NODE_I2S9);
    DisableDtbNode (fdt, DT_NODE_CODEC_ALC5682);
    DisableDtbNode (fdt, DT_NODE_SOUND);
  }

  // DPU
  if (IsIpHarvested (DpuCore0)) {
    DisableDtbNode (fdt, DT_NODE_DPU0);
    DisableDtbNode (fdt, DT_NODE_AEU0);
    DisableDtbNode (fdt, DT_NODE_DP0);
  }

  if (IsIpHarvested (DpuCore1)) {
    DisableDtbNode (fdt, DT_NODE_DPU1);
    DisableDtbNode (fdt, DT_NODE_AEU1);
    DisableDtbNode (fdt, DT_NODE_DP1);
  }

  if (IsIpHarvested (DpuCore2)) {
    DisableDtbNode (fdt, DT_NODE_DPU2);
    DisableDtbNode (fdt, DT_NODE_AEU2);
    DisableDtbNode (fdt, DT_NODE_DP2);
  }

  if (IsIpHarvested (DpuCore3)) {
    DisableDtbNode (fdt, DT_NODE_DPU3);
    DisableDtbNode (fdt, DT_NODE_AEU3);
    DisableDtbNode (fdt, DT_NODE_DP3);
  }

  if (IsIpHarvested (DpuCore4)) {
    DisableDtbNode (fdt, DT_NODE_DPU4);
    DisableDtbNode (fdt, DT_NODE_AEU4);
    DisableDtbNode (fdt, DT_NODE_DP4);
  }

  // ISP
  if (IsIpHarvested (Isp)) {
    DisableDtbNode (fdt, DT_NODE_ISP);
    DisableDtbNode (fdt, DT_NODE_ISP_CONFIG);
    DisableDtbNode (fdt, DT_NODE_ISP_ISPMEM);
    DisableDtbNode (fdt, DT_NODE_IMGSENSOR00);
    DisableDtbNode (fdt, DT_NODE_MOTOR0);
    DisableDtbNode (fdt, DT_NODE_IMGSENSOR01);
    DisableDtbNode (fdt, DT_NODE_IMGSENSOR02);
    DisableDtbNode (fdt, DT_NODE_IMGSENSOR03);
    DisableDtbNode (fdt, DT_NODE_CIX_VI_HW);
  }

  UpdateGopDtbStatus(fdt);
  UpdateTypecPhyDtbStatus(fdt);

  return EFI_SUCCESS;
}

EFI_STATUS
UpdateCpuDtbStatus (
  IN  VOID  *fdt
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32      CpuId, Index, CpuMask, CpuNum = 8;

  CpuMask = MmioRead32 (PMCTRL_S5_BASE_ADDRESS + 0x504) & 0xff;

  DbtNodePropRm (fdt, "/cpus/idle-states/cpu-sleep-1", "/cpus/cpu4@400", "cpu-idle-states");
  if (CpuMask == 0x0) {
    return Status;
  }

  for (Index = 0; Index < CpuNum; Index++) {
    CpuId   = CpuMask & 0x1;
    CpuMask = CpuMask >> 0x1;

    if (CpuId == 0x0) {
      continue;
    }

    CpuId = 4 + Index;
    switch (CpuId) {
      case 0:
        DbtNodePropRm (fdt, "/cpus/idle-states/cpu-standby-0", "/cpus/cpu0@0", "cpu-idle-states");
        break;
      case 4:
        DbtNodePropRm (fdt, "/cpus/idle-states/cpu-standby-0", "/cpus/cpu4@400", "cpu-idle-states");
        break;
      case 5:
        DbtNodePropRm (fdt, "/cpus/idle-states/cpu-standby-0", "/cpus/cpu5@500", "cpu-idle-states");
        break;
      case 6:
        DbtNodePropRm (fdt, "/cpus/idle-states/cpu-standby-0", "/cpus/cpu6@600", "cpu-idle-states");
        break;
      case 7:
        DbtNodePropRm (fdt, "/cpus/idle-states/cpu-standby-0", "/cpus/cpu7@700", "cpu-idle-states");
        break;
      case 8:
        DbtNodePropRm (fdt, "/cpus/idle-states/cpu-standby-0", "/cpus/cpu8@800", "cpu-idle-states");
        break;
      case 9:
        DbtNodePropRm (fdt, "/cpus/idle-states/cpu-standby-0", "/cpus/cpu9@900", "cpu-idle-states");
        break;
      case 10:
        DbtNodePropRm (fdt, "/cpus/idle-states/cpu-standby-0", "/cpus/cpu10@a00", "cpu-idle-states");
        break;
      case 11:
        DbtNodePropRm (fdt, "/cpus/idle-states/cpu-standby-0", "/cpus/cpu11@b00", "cpu-idle-states");
        break;
      default:
        break;
    }
  }

  return Status;
}

VOID
DumpCpuDtbStatus (
  VOID  *fdt
  )
{
  INT32         Node;
  const UINT32  *PropU32;
  INT32         len;

  Node    = fdt_path_offset (fdt, "/cpus/cpu4@400");
  PropU32 = fdt_getprop (fdt, Node, "cpu-idle-states", &len);
  DEBUG ((DEBUG_INFO, "cpu4 cpu-idle-states : 0x%x 0x%x \n", fdt32_to_cpu (PropU32[0]), fdt32_to_cpu (PropU32[1])));

  Node    = fdt_path_offset (fdt, "/cpus/cpu5@500");
  PropU32 = fdt_getprop (fdt, Node, "cpu-idle-states", &len);
  DEBUG ((DEBUG_INFO, "cpu5 cpu-idle-states : 0x%x 0x%x \n", fdt32_to_cpu (PropU32[0]), fdt32_to_cpu (PropU32[1])));

  Node    = fdt_path_offset (fdt, "/cpus/cpu6@600");
  PropU32 = fdt_getprop (fdt, Node, "cpu-idle-states", &len);
  DEBUG ((DEBUG_INFO, "cpu6 cpu-idle-states : 0x%x 0x%x \n", fdt32_to_cpu (PropU32[0]), fdt32_to_cpu (PropU32[1])));

  Node    = fdt_path_offset (fdt, "/cpus/cpu7@700");
  PropU32 = fdt_getprop (fdt, Node, "cpu-idle-states", &len);
  DEBUG ((DEBUG_INFO, "cpu7 cpu-idle-states : 0x%x 0x%x \n", fdt32_to_cpu (PropU32[0]), fdt32_to_cpu (PropU32[1])));

  Node    = fdt_path_offset (fdt, "/cpus/cpu8@800");
  PropU32 = fdt_getprop (fdt, Node, "cpu-idle-states", &len);
  DEBUG ((DEBUG_INFO, "cpu8 cpu-idle-states : 0x%x 0x%x \n", fdt32_to_cpu (PropU32[0]), fdt32_to_cpu (PropU32[1])));

  Node    = fdt_path_offset (fdt, "/cpus/cpu9@900");
  PropU32 = fdt_getprop (fdt, Node, "cpu-idle-states", &len);
  DEBUG ((DEBUG_INFO, "cpu9 cpu-idle-states : 0x%x 0x%x \n", fdt32_to_cpu (PropU32[0]), fdt32_to_cpu (PropU32[1])));

  Node    = fdt_path_offset (fdt, "/cpus/cpu10@a00");
  PropU32 = fdt_getprop (fdt, Node, "cpu-idle-states", &len);
  DEBUG ((DEBUG_INFO, "cpu10 cpu-idle-states : 0x%x 0x%x \n", fdt32_to_cpu (PropU32[0]), fdt32_to_cpu (PropU32[1])));

  Node    = fdt_path_offset (fdt, "/cpus/cpu11@b00");
  PropU32 = fdt_getprop (fdt, Node, "cpu-idle-states", &len);
  DEBUG ((DEBUG_INFO, "cpu11 cpu-idle-states : 0x%x 0x%x \n", fdt32_to_cpu (PropU32[0]), fdt32_to_cpu (PropU32[1])));
}

VOID
DumpDpuDtbStatus (
  VOID  *fdt
  )
{

  INT32        Node;
  CONST CHAR8  *ProptyValue;
  INT32        len;
  CHAR8  *DpuDtbNodes[] = {
  DT_NODE_DPU0,
  DT_NODE_AEU0,
  DT_NODE_DP0,
  DT_NODE_DPU1,
  DT_NODE_AEU1,
  DT_NODE_DP1,
  DT_NODE_DPU2,
  DT_NODE_AEU2,
  DT_NODE_DP2,
  DT_NODE_DPU3,
  DT_NODE_AEU3,
  DT_NODE_DP3,
  DT_NODE_DPU4,
  DT_NODE_AEU4,
  DT_NODE_DP4
  };

  UINT8        Count = sizeof(DpuDtbNodes)/sizeof(DpuDtbNodes[0]);
  for (UINT8 i = 0; i < Count; i++) {
    Node        = fdt_path_offset (fdt, DpuDtbNodes[i]);
    ProptyValue = fdt_getprop (fdt, Node, "status", &len);
    if((ProptyValue == NULL)||(len < 0)) {
      DEBUG ((DEBUG_ERROR, "%a fdt_getprop ERROR\n", __FUNCTION__));
    }else {
      DEBUG ((DEBUG_INFO, "%a : %a\n", DpuDtbNodes[i], ProptyValue));
    }
  }

}

VOID
DumpAudioDtbStatus (
  VOID  *fdt
  )
{
  INT32        Node;
  CONST CHAR8  *ProptyValue;
  INT32        len;
  CHAR8  *AudioDtbNodes[] = {
  DT_NODE_HIFI5,
  DT_NODE_MBOX_AP2DSP,
  DT_NODE_MBOX_DSP2AP,
  DT_NODE_AUDSS_CRU,
  DT_NODE_AUDSS_DMAC,
  DT_NODE_IPB_HDA,
  DT_NODE_I2S0,
  DT_NODE_I2S1,
  DT_NODE_I2S2,
  DT_NODE_I2S3,
  DT_NODE_I2S4,
  DT_NODE_I2S5,
  DT_NODE_I2S6,
  DT_NODE_I2S7,
  DT_NODE_I2S8,
  DT_NODE_I2S9,
  DT_NODE_SOUND,
  DT_NODE_CODEC_ALC5682
  };

  UINT8        Count = sizeof(AudioDtbNodes)/sizeof(AudioDtbNodes[0]);
  for (UINT8 i = 0; i < Count; i++) {
    Node        = fdt_path_offset (fdt, AudioDtbNodes[i]);
    ProptyValue = fdt_getprop (fdt, Node, "status", &len);
    if((ProptyValue == NULL)||(len < 0)) {
      DEBUG ((DEBUG_ERROR, "%a fdt_getprop ERROR\n", __FUNCTION__));
    }else {
      DEBUG ((DEBUG_INFO, "%a : %a\n", AudioDtbNodes[i], ProptyValue));
    }
  }


}

VOID
DumpNpuDtbStatus (
  VOID  *fdt
  )
{
  INT32        Node;
  const UINT32  *PropU32;
  INT32        len;

  Node        = fdt_path_offset (fdt, DT_NODE_AIPU);
  PropU32 = fdt_getprop (fdt, Node, "core_mask", &len);
  if((PropU32 == NULL)||(len < 0)) {
    DEBUG ((DEBUG_ERROR, "%a fdt_getprop ERROR\n", __FUNCTION__));
  }else {
    DEBUG ((DEBUG_INFO, "core_mask : 0x%x\n", fdt32_to_cpu (PropU32[0])));
  }

}

VOID
DumpIspDtbStatus (
  VOID  *fdt
  )
{

  CHAR8  *IspDtbNodes[] = {
  DT_NODE_ISP,
  DT_NODE_ISP_CONFIG,
  DT_NODE_ISP_ISPMEM,
  DT_NODE_MOTOR0,
  DT_NODE_IMGSENSOR00,
  DT_NODE_IMGSENSOR01,
  DT_NODE_IMGSENSOR02,
  DT_NODE_IMGSENSOR03,
  DT_NODE_CIX_VI_HW
  };
  INT32        Node;
  CONST CHAR8  *ProptyValue;
  INT32        len;
  UINT8        Count = sizeof(IspDtbNodes)/sizeof(IspDtbNodes[0]);
  for (UINT8 i = 0; i < Count; i++) {
    Node        = fdt_path_offset (fdt, IspDtbNodes[i]);
    ProptyValue = fdt_getprop (fdt, Node, "status", &len);
    if((ProptyValue == NULL)||(len < 0)) {
      DEBUG ((DEBUG_ERROR, "%a fdt_getprop ERROR\n", __FUNCTION__));
    }else {
      DEBUG ((DEBUG_INFO, "%a : %a\n", IspDtbNodes[i], ProptyValue));
    }
  }


}

VOID
DumpDtbStatusSi (
  VOID  *fdt
  )
{
  INT32        Node;
  CONST CHAR8  *ProptyValue;
  // const UINT32  *PropU32;
  INT32  len;

  DumpAudioDtbStatus (fdt);
  DumpDpuDtbStatus (fdt);
  DumpNpuDtbStatus (fdt);
  DumpIspDtbStatus (fdt);

  Node        = fdt_path_offset (fdt, DT_NODE_PCIEX8_RC);
  ProptyValue = fdt_getprop (fdt, Node, "status", &len);
  DEBUG ((DEBUG_INFO, "pcie X8 : %a\n", ProptyValue));

  // Node    = fdt_path_offset (fdt, "/soc@0/uart@040d0000");
  // PropU32 = fdt_getprop (fdt, Node, "interrupts", &len);
  // DEBUG ((DEBUG_INFO, "uart2 interrupts : 0x%x 0x%x 0x%x\n", fdt32_to_cpu (PropU32[0]), fdt32_to_cpu (PropU32[1]), fdt32_to_cpu (PropU32[2])));
}

EFI_STATUS
UpdateDtbSi (
  VOID   *fdt,
  UINTN  *ImageSize
  )
{
  EFI_STATUS  Status;
  INT32       ret;
  UINT32      PaddSize;

  ret = fdt_check_header (fdt) || fdt_check_header_ext_si (fdt);
  if (ret) {
    DEBUG ((EFI_D_ERROR, "ERROR: Invalid device tree header ...\n"));
    return EFI_NOT_FOUND;
  }

  /* Add padding to make space for new nodes and properties. */
  PaddSize = ADD_OF (fdt_totalsize (fdt), DTB_PAD_SIZE);

  ret = fdt_open_into (fdt, fdt, PaddSize);
  if (ret != 0) {
    DEBUG ((EFI_D_ERROR, "ERROR: Failed to move/resize dtb buffer ...\n"));
    return EFI_BAD_BUFFER_SIZE;
  }

  Status = UpdateDtbStatus (fdt);

  ret = fdt_pack (fdt);
  if (ret != 0) {
    DEBUG ((EFI_D_ERROR, "ERROR: Failed to pack dtb ...\n"));
    return EFI_ABORTED;
  }

  *ImageSize = fdt_totalsize (fdt);
  return EFI_SUCCESS;
}
