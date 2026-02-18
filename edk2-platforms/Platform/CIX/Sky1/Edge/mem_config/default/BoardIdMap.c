#include "MemConfigBinTool.h"
#include "BoardConfig.h"
#include "MemConfigurationTable.h"

#define ENTRY_COUNT 4

// PCBID[6][2:0]
#define RS600      4

// PCBID[7][5:3]
#define SAMSUNG_4G_SR_315   0
#define HYNIX_3G_SR_315     5
#define SAMSUNG_8G_DR_315   8
#define HYNIX_16G_DR_x8_315 10

//               PCBID[2:0]    PCBID[5:3]         PCBID[6]           PCBID[7]
#define PCB(b,d) (((b) & 7) + (((d) & 7) << 3) + (((b) & 8) << 3) + (((d) & 8) << 4))

MEM_CONFIG_BLOCK_BOARDID_MAP BoardIdMapBlock = {
  {
    .Signature      = MEM_CONFIG_BLOCK_BOARDID_MAP_SIGNAUTE,
    .BlockSize      = sizeof(MEM_CONFIG_BLOCK_HEADER) + sizeof(BOARD_ID_MAP_ENTRY) * ENTRY_COUNT,
    .BoardMask      = BOARD_ID_MASK_DEFAULT
  },
  {
    {PCB(RS600, HYNIX_3G_SR_315    ), RS600_12G_ID   },
    {PCB(RS600, SAMSUNG_4G_SR_315  ), RS600_16G_ID   },
    {PCB(RS600, SAMSUNG_8G_DR_315  ), RS600_32G_ID   },
    {PCB(RS600, HYNIX_16G_DR_x8_315), RS600_64G_x8_ID},
  }
};
