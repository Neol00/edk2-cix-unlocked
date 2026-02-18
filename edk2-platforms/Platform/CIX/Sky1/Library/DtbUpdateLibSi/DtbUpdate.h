/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 **/

#ifndef _DTS_UPDATE_DXE_H_
#define _DTS_UPDATE_DXE_H_

#include <Uefi.h>
#include <Uefi/UefiBaseType.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IoLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/SocInitLib.h>
#include <Guid/FileInfo.h>
#include <Guid/EventGroup.h>

#define DTB_PAD_SIZE  2048

#define ADD_OF(a, b)       (MAX_UINT32 - b > a) ? (a + b) : 0
#define CHECK_ADD64(a, b)  ((MAX_UINT64 - b < a) ? TRUE : FALSE)

#define DT_NODE_AIPU          "/soc@0/aipu@14260000"
#define DT_NODE_SMMU_MMHUB    "/iommu@0b1b0000"
#define DT_NODE_SMMU_PCIEHUB  "/iommu@0b010000"
#define DT_NODE_PCIEX8_RC     "/soc@0/pcie@0a010000"
#define DT_NODE_PCIEX4_RC     "/soc@0/pcie@0a070000"
#define DT_NODE_ITS           "/soc@0/interrupt-controller@0e001000/its@0e050000"
// s5_gpio0
#define DT_NODE_S5_GPIO0  "/soc@0/gpio-controller@16004000"
#define DT_NODE_VPU       "/soc@0/vpu@14230000"
// TYPE C
#define DT_NODE_USBSS_0             "/soc@0/usb@9000000"
#define DT_NODE_USBSS_0_CONTROLLER  "/soc@0/usb@9000000/usb-controller@9010000"
#define DT_NODE_USBSS_1             "/soc@0/usb@9070000"
#define DT_NODE_USBSS_1_CONTROLLER  "/soc@0/usb@9070000/usb-controller@9080000"
#define DT_NODE_USBSS_2             "/soc@0/usb@90E0000"
#define DT_NODE_USBSS_2_CONTROLLER  "/soc@0/usb@90E0000/usb-controller@90F0000"
#define DT_NODE_USBSS_3             "/soc@0/usb@9150000"
#define DT_NODE_USBSS_3_CONTROLLER  "/soc@0/usb@9150000/usb-controller@9160000"
// TYPE A
#define DT_NODE_USBSS_4             "/soc@0/usb@91c0300"
#define DT_NODE_USBSS_4_CONTROLLER  "/soc@0/usb@91c0300/usb-controller@91d0000"

#define DT_NODE_USBHS_0             "/soc@0/usb@9250000"
#define DT_NODE_USBHS_0_CONTROLLER  "/soc@0/usb@9250000/usb-controller@9260000"

// GMAC
#define DT_NODE_GMAC0  "/soc@0/ethernet@9320000"
#define DT_NODE_GMAC1  "/soc@0/ethernet@9330000"

// CSI
#define DT_NODE_CIX_BRIDEG0  "/soc@0/cix_bridge@142B0000"
#define DT_NODE_CIX_BRIDEG1  "/soc@0/cix_bridge@142C0000"
#define DT_NODE_CIX_BRIDEG2  "/soc@0/cix_bridge@14310000"
#define DT_NODE_CIX_BRIDEG3  "/soc@0/cix_bridge@14320000"

// audio
#define DT_NODE_HIFI5  "/soc@0/audio-dsp@07000000"
// mbox_ap2dsp: mailbox@70f0000
#define DT_NODE_MBOX_AP2DSP  "/soc@0/mailbox@70f0000"
// mbox_dsp2ap: mailbox@7100000
#define DT_NODE_MBOX_DSP2AP  "/soc@0/mailbox@7100000"
// audss_cru: system-controller@07110000
#define DT_NODE_AUDSS_CRU  "/soc@0/system-controller@07110000"
// audss_clk: clock-controller
#define DT_NODE_AUDSS_CLK  "/soc@0/system-controller@07110000/clock-controller"
// audss_rst: reset-controller
#define DT_NODE_AUDSS_RST  "/soc@0/system-controller@07110000/reset-controller"
// audss_dmac: dma-controller@07010000
#define DT_NODE_AUDSS_DMAC  "/soc@0/dma-controller@07010000"
// ipb_hda: ipb-hda@070c0000
#define DT_NODE_IPB_HDA  "/soc@0/ipb-hda@070c0000"
// i2s0: i2s@07020000
#define DT_NODE_I2S0  "/soc@0/i2s@07020000"
// i2s1: i2s@07030000
#define DT_NODE_I2S1  "/soc@0/i2s@07030000"
// i2s2: i2s@07040000
#define DT_NODE_I2S2  "/soc@0/i2s@07040000"
// i2s3: i2s@07050000
#define DT_NODE_I2S3  "/soc@0/i2s@07050000"
// i2s4: i2s@07060000
#define DT_NODE_I2S4  "/soc@0/i2s@07060000"
// i2s5: i2s@07070000
#define DT_NODE_I2S5  "/soc@0/i2s@07070000"
// i2s6: i2s@07080000
#define DT_NODE_I2S6  "/soc@0/i2s@07080000"
// i2s7: i2s@07090000
#define DT_NODE_I2S7  "/soc@0/i2s@07090000"
// i2s8: i2s@070a0000
#define DT_NODE_I2S8  "/soc@0/i2s@070a0000"
// i2s9: i2s@070b0000
#define DT_NODE_I2S9  "/soc@0/i2s@070b0000"
// codec_alc5682
#define DT_NODE_SOUND          "/sound"
#define DT_NODE_CODEC_ALC5682  "/sound/codec_alc5682"

// dpu0: disp-controller@14010000
// aeu0: aeu@14030000
// dp0: dp@14060000
#define DT_NODE_DPU0  "/soc@0/disp-controller@14010000"
#define DT_NODE_AEU0  "/soc@0/aeu@14030000"
#define DT_NODE_DP0   "/soc@0/dp@14060000"
// dpu1: disp-controller@14080000
// aeu1: aeu@140a0000
// dp1: dp@140d0000
#define DT_NODE_DPU1  "/soc@0/disp-controller@14080000"
#define DT_NODE_AEU1  "/soc@0/aeu@140a0000"
#define DT_NODE_DP1   "/soc@0/dp@140d0000"
// dpu2: disp-controller@140f0000
// aeu2: aeu@14110000
// dp2: dp@14140000
#define DT_NODE_DPU2  "/soc@0/disp-controller@140f0000"
#define DT_NODE_AEU2  "/soc@0/aeu@14110000"
#define DT_NODE_DP2   "/soc@0/dp@14140000"

// dpu3: disp-controller@14160000
// aeu3: aeu@14180000
// dp3: dp@141b0000
#define DT_NODE_DPU3  "/soc@0/disp-controller@14160000"
#define DT_NODE_AEU3  "/soc@0/aeu@14180000"
#define DT_NODE_DP3   "/soc@0/dp@141b0000"
// dpu4: disp-controller@141d0000
// aeu4: aeu@141f0000
// dp4: dp@14220000
#define DT_NODE_DPU4  "/soc@0/disp-controller@141d0000"
#define DT_NODE_AEU4  "/soc@0/aeu@141f0000"
#define DT_NODE_DP4   "/soc@0/dp@14220000"

// isp
// armcb_isp@14340000
// armcb_config
// armcb_ispmem
// imgsensor00@34
// motor0@40
// imgsensor02@36
// imgsensor01@38
// imgsensor03@3a
// cix_vi_hw
#define DT_NODE_ISP          "/soc@0/armcb_isp@14340000"
#define DT_NODE_ISP_CONFIG   "/armcb_config"
#define DT_NODE_ISP_ISPMEM   "/armcb_ispmem"
#define DT_NODE_IMGSENSOR00  "/soc@0/i2c@04010000/imgsensor00@34"
#define DT_NODE_MOTOR0       "/soc@0/i2c@04010000/motor0@40"
#define DT_NODE_IMGSENSOR01  "/soc@0/i2c@04040000/imgsensor01@38" // i2c3: i2c@04040000
#define DT_NODE_IMGSENSOR02  "/soc@0/i2c@04020000/imgsensor02@36" // i2c1: i2c@04020000
#define DT_NODE_IMGSENSOR03  "/soc@0/i2c@04050000/imgsensor03@3a" // i2c4: i2c@04050000
#define DT_NODE_CIX_VI_HW    "/cix_vi_hw"


VOID
DisableDtbNode (
  IN  VOID         *fdt,
  IN  CONST CHAR8  *NodePath
  );

VOID
EnableDtbNode (
  IN  VOID         *fdt,
  IN  CONST CHAR8  *NodePath
  );

EFI_STATUS
UpdateGpuDtbStatus (
  IN  VOID  *fdt
  );

EFI_STATUS
UpdateDpuDtbStatus (
  IN  VOID  *fdt
  );

EFI_STATUS
UpdateNpuDtbStatus (
  IN  VOID  *fdt
  );

EFI_STATUS
UpdateVpuDtbStatus (
  IN  VOID  *fdt
  );

#endif
