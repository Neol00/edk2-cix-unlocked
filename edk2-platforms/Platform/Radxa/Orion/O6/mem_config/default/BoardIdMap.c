#include "MemConfigBinTool.h"
#include "BoardConfig.h"
#include "MemConfigurationTable.h"

#define ENTRY_COUNT 12

// PCBID[6][2:0]
#define RS600  4

// PCBID[7][5:3]
#define DRAM_4G_SR_315       0
#define DRAM_2G_SR_315       2
#define DRAM_3G_SR_315       5
#define DRAM_8G_DR_315_x8_HS 6
#define DRAM_6G_SR_315_HS    7
#define DRAM_8G_DR_315_LO    8
#define DRAM_12G_DR_315_HS   9
#define DRAM_16G_DR_x8_315   10
#define DRAM_8G_DR_315       11
#define DRAM_16G_RAYSON_315  12
#define DRAM_4G_DR_315_HS    13
#define DRAM_8G_HYNIX_315    14

//               PCBID[2:0]    PCBID[5:3]         PCBID[6]           PCBID[7]
#define PCB(b,d) (((b) & 7) + (((d) & 7) << 3) + (((b) & 8) << 3) + (((d) & 8) << 4))

MEM_CONFIG_BLOCK_BOARDID_MAP BoardIdMapBlock = {
  {
    .Signature      = MEM_CONFIG_BLOCK_BOARDID_MAP_SIGNAUTE,
    .BlockSize      = sizeof(MEM_CONFIG_BLOCK_HEADER) + sizeof(BOARD_ID_MAP_ENTRY) * ENTRY_COUNT,
    .BoardMask      = BOARD_ID_MASK_DEFAULT
  },
  {
    {PCB(RS600, DRAM_2G_SR_315      ), RS600_8G_ID        },
    {PCB(RS600, DRAM_3G_SR_315      ), RS600_12G_ID       },
    {PCB(RS600, DRAM_4G_SR_315      ), RS600_16G_ID       },
    {PCB(RS600, DRAM_4G_DR_315_HS   ), RS600_16G_HS_ID    },
    {PCB(RS600, DRAM_6G_SR_315_HS   ), RS600_24G_HS_ID    },
    {PCB(RS600, DRAM_8G_DR_315      ), RS600_32G_ID       },
    {PCB(RS600, DRAM_8G_DR_315_LO   ), RS600_32G_LO_ID    },
    {PCB(RS600, DRAM_8G_DR_315_x8_HS), RS600_32G_x8_HS_ID },
    {PCB(RS600, DRAM_8G_HYNIX_315   ), RS600_32G_HYNIX_ID },
    {PCB(RS600, DRAM_12G_DR_315_HS  ), RS600_48G_HS_ID    },
    {PCB(RS600, DRAM_16G_DR_x8_315  ), RS600_64G_x8_ID    },
    {PCB(RS600, DRAM_16G_RAYSON_315 ), RS600_64G_RAYSON_ID},
  }
};
