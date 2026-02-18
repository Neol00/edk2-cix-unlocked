#include "BoardConfig.h"
#include "MemConfigBinTool.h"
#include "MemConfigurationTable.h"

#define ENTRY_COUNT 2

MEM_CONFIG_BUSCFG_LP5 MemLpddr5BusCfg_rs600_x8 = {
  {
    .Signature      = MEM_CONFIG_BLOCK_BUSCFG_LP5_SIGNATURE,
    .BlockSize      = sizeof(MEM_CONFIG_BUSCFG_LP5) + sizeof(MEM_CONFIG_BUSCFG_LP5_ENTRY) * ENTRY_COUNT,
    .BoardMask      = RS600_64G_x8_MASK
  },
  {
    //                                                                                                     Dq   Ca
    //MaxMemFreq        RankPerCh CA_ODT   CK_ODT   CS_ODT   DQ_ODT   WCK_ODT  SOC_ODT  NTDQ_ODT ODT_PDDS Vref Vref(% of VDDQ)
    {DDR6400_FREQUENCY, RANK_SR,  RTT_240, RTT_120, RTT_240, RTT_80,  RTT_80,  RTT_40,  RTT_OFF,  RTT_40,  340, 280},
    {DDR6400_FREQUENCY, RANK_DR,  0x10,    RTT_240, RTT_240, RTT_120, RTT_120, RTT_40,  RTT_OFF,  RTT_40,  340, 280},
  }
};
