#include "BoardConfig.h"
#include "MemConfigBinTool.h"
#include "MemConfigurationTable.h"

MEM_CONFIG_TRACE_LEN_DIFF MemTraceLenDiffCfg = {
  {
    .Signature      = MEM_CONFIG_TRACE_LEN_DIFF_SIGNAUTE,
    .BlockSize      = sizeof(MEM_CONFIG_TRACE_LEN_DIFF),
    .BoardMask      = BOARD_ID_MASK_DEFAULT
  },
  {
    // Avg Flight Time      Trace Length Diff
    {{500, 500, 500, 500}, {28, 85,  111, 62 }}, // CH0
    {{500, 500, 500, 500}, {80, 119, 97,  28 }}, // CH1
    {{500, 500, 500, 500}, {63, 128, 97,  28 }}, // CH2
    {{500, 500, 500, 500}, {64, 27,  61,  100}}, // CH3
  }
};
