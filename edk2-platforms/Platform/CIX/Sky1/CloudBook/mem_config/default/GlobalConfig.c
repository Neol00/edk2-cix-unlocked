#include "MemConfigBinTool.h"
#include "MemConfigurationTable.h"

MEM_CONFIG_BLOCK_CONFIG  GlobalConfigBlock = {
  {
    .Signature = MEM_CONFIG_BLOCK_CONFIG_SIGNAUTE,
    .BlockSize = sizeof (MEM_CONFIG_BLOCK_CONFIG),
    .BoardMask = BOARD_ID_MASK_DEFAULT
  },
  .MaxFreq       = DDR3200_FREQUENCY,
  .ChMask        = 0xC,
  .DdrType       = DDR_TYPE_LPDDR5,
  .DeviceDensity = 8,
  .DeviceWidth   = 16,
  .RankNum       = 1,
};
