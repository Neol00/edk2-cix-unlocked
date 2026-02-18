#include "BoardConfig.h"
#include "MemConfigBinTool.h"
#include "MemConfigurationTable.h"

#define ENTRY_COUNT 2

MEM_CONFIG_TRAIN_OPTIMIZE MemTrainOptimize_Hynix = {
  {
    .Signature      = MEM_CONFIG_TRAIN_OPTIMIZE_SIGNAUTE,
    .BlockSize      = sizeof(MEM_CONFIG_TRAIN_OPTIMIZE) + sizeof(MEM_CONFIG_TRAIN_OPTIMIZE_ENTRY) * ENTRY_COUNT,
    .BoardMask      = RS600_32G_HYNIX_MASK
  },
  {
    //MaxMemFreq        RankPerCh ChMsk  Center Shift Th
    {DDR4800_FREQUENCY, RANK_ALL,  0xF,    0,    0,   0},
    {DDR6400_FREQUENCY, RANK_ALL,  0xF,    1,    104, 0},
  }
};
