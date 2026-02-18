#include "BoardConfig.h"
#include "MemConfigBinTool.h"
#include "MemConfigurationTable.h"

MEM_CONFIG_TRACE_LEN_DIFF  MemTraceLenDiffCfg = {
  {
    .Signature = MEM_CONFIG_TRACE_LEN_DIFF_SIGNAUTE,
    .BlockSize = sizeof (MEM_CONFIG_TRACE_LEN_DIFF),
    .BoardMask = BOARD_ID_MASK_DEFAULT
  },
  {
    // Avg Flight Time      Trace Length Diff
    {
      { 500, 500, 500, 500 }, { 0,   0,  0,  0, }
    },                                     // CH0
    {
      { 500, 500, 500, 500 }, { 0,   0,  0,  0, }
    },                                     // CH1
    {
      { 500, 500, 500, 500 }, { 84,  26, 63, 78 }
    },                                         // CH2
    {
      { 500, 500, 500, 500 }, { 125, 53, 28, 29 }
    },                                         // CH3
  }
};
