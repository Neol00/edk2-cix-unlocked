#include "BoardConfig.h"
#include "MemConfigBinTool.h"
#include "MemConfigurationTable.h"

#define ENTRY_COUNT  2

MEM_CONFIG_BUSCFG_LP5  MemLpddr5BusCfg = {
  {
    .Signature = MEM_CONFIG_BLOCK_BUSCFG_LP5_SIGNATURE,
    .BlockSize = sizeof (MEM_CONFIG_BUSCFG_LP5) + sizeof (MEM_CONFIG_BUSCFG_LP5_ENTRY) * ENTRY_COUNT,
    .BoardMask = BOARD_ID_MASK_DEFAULT
  },
  {
    // MaxMemFreq        RankPerCh CA_ODT   CK_ODT   CS_ODT   DQ_ODT   WCK_ODT  SOC_ODT  NTDQ_ODT ODT_PDDS
    { DDR6400_FREQUENCY, RANK_SR, RTT_120, RTT_120, RTT_80, RTT_40,  RTT_40,  RTT_40, RTT_OFF, RTT_40 },
    { DDR6400_FREQUENCY, RANK_DR, RTT_240, RTT_240, RTT_80, RTT_80,  RTT_80,  RTT_40, RTT_OFF, RTT_40 },
  }
};
