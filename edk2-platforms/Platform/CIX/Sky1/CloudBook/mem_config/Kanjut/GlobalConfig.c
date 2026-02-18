#include "MemConfigBinTool.h"
#include "MemConfigurationTable.h"
#include "BoardConfig.h"

MEM_CONFIG_BLOCK_CONFIG  GlobalConfigBlock_2g = {
  {
    .Signature = MEM_CONFIG_BLOCK_CONFIG_SIGNAUTE,
    .BlockSize = sizeof (MEM_CONFIG_BLOCK_CONFIG),
    .BoardMask = DRAM_2G_HYNIX_MASK | DRAM_2G_SAMSUNG_MASK
  },
  .MaxFreq       = DDR3200_FREQUENCY,
  .ChMask        = 0xC,
  .DdrType       = DDR_TYPE_LPDDR5,
  .DeviceDensity = 8,
  .DeviceWidth   = 16,
  .RankNum       = 1,
};

MEM_CONFIG_BLOCK_CONFIG  GlobalConfigBlock_4g = {
  {
    .Signature = MEM_CONFIG_BLOCK_CONFIG_SIGNAUTE,
    .BlockSize = sizeof (MEM_CONFIG_BLOCK_CONFIG),
    .BoardMask = DRAM_4G_HYNIX_MASK | DRAM_4G_SAMSUNG_MASK
  },
  .MaxFreq       = DDR3200_FREQUENCY,
  .ChMask        = 0xC,
  .DdrType       = DDR_TYPE_LPDDR5,
  .DeviceDensity = 16,
  .DeviceWidth   = 16,
  .RankNum       = 1,
};