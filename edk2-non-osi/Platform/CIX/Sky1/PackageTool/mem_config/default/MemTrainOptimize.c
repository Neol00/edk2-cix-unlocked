#include "BoardConfig.h"
#include "MemConfigBinTool.h"
#include "MemConfigurationTable.h"

#define ENTRY_COUNT 2

MEM_CONFIG_TRAIN_OPTIMIZE MemTrainOptimize = {
  {
    .Signature      = MEM_CONFIG_TRAIN_OPTIMIZE_SIGNAUTE,
    .BlockSize      = sizeof(MEM_CONFIG_TRAIN_OPTIMIZE) + sizeof(MEM_CONFIG_TRAIN_OPTIMIZE_ENTRY) * ENTRY_COUNT,
    .BoardMask      = BOARD_ID_MASK_DEFAULT
  },
  {
    //MaxMemFreq        RankPerCh ChMsk  Center Shift Th
    {DDR4800_FREQUENCY, RANK_ALL,  0xF,    0,    0,   0},
    {DDR6400_FREQUENCY, RANK_ALL,  0xF,    1,    0,   0},
  }
};
