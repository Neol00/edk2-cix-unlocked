#include "BoardConfig.h"
#include "MemConfigBinTool.h"
#include "MemConfigurationTable.h"

MEM_CONFIG_BLOCK_CONFIG  GlobalConfigBlock_8G = {
  {
    .Signature      = MEM_CONFIG_BLOCK_CONFIG_SIGNAUTE,
    .BlockSize      = sizeof(MEM_CONFIG_BLOCK_CONFIG),
    .BoardMask      = RS600_8G_MASK
  },
  .MaxFreq          = MEM_CFG_MEMFREQ,
  .ChMask           = 0xF,
  .DdrType          = DDR_TYPE_LPDDR5,
  .DeviceDensity    = 8,
  .DeviceWidth      = 16,
  .RankNum          = 1,
};

MEM_CONFIG_BLOCK_CONFIG  GlobalConfigBlock_12G = {
  {
    .Signature      = MEM_CONFIG_BLOCK_CONFIG_SIGNAUTE,
    .BlockSize      = sizeof(MEM_CONFIG_BLOCK_CONFIG),
    .BoardMask      = RS600_12G_MASK
  },
  .MaxFreq          = MEM_CFG_MEMFREQ,
  .ChMask           = 0xF,
  .DdrType          = DDR_TYPE_LPDDR5,
  .DeviceDensity    = 12,
  .DeviceWidth      = 16,
  .RankNum          = 1,
};

MEM_CONFIG_BLOCK_CONFIG  GlobalConfigBlock_16G = {
  {
    .Signature      = MEM_CONFIG_BLOCK_CONFIG_SIGNAUTE,
    .BlockSize      = sizeof(MEM_CONFIG_BLOCK_CONFIG),
    .BoardMask      = RS600_16G_MASK
  },
  .MaxFreq          = MEM_CFG_MEMFREQ,
  .ChMask           = 0xF,
  .DdrType          = DDR_TYPE_LPDDR5,
  .DeviceDensity    = 16,
  .DeviceWidth      = 16,
  .RankNum          = 1,
};

MEM_CONFIG_BLOCK_CONFIG  GlobalConfigBlock_32G = {
  {
    .Signature      = MEM_CONFIG_BLOCK_CONFIG_SIGNAUTE,
    .BlockSize      = sizeof(MEM_CONFIG_BLOCK_CONFIG),
    .BoardMask      = RS600_32G_MASK
  },
  .MaxFreq          = MEM_CFG_MEMFREQ,
  .ChMask           = 0xF,
  .DdrType          = DDR_TYPE_LPDDR5,
  .DeviceDensity    = 16,
  .DeviceWidth      = 16,
  .RankNum          = 2,
};

MEM_CONFIG_BLOCK_CONFIG  GlobalConfigBlock_32G_Lo = {
  {
    .Signature      = MEM_CONFIG_BLOCK_CONFIG_SIGNAUTE,
    .BlockSize      = sizeof(MEM_CONFIG_BLOCK_CONFIG),
    .BoardMask      = RS600_32G_LO_MASK
  },
  .MaxFreq          = DDR4800_FREQUENCY,
  .ChMask           = 0xF,
  .DdrType          = DDR_TYPE_LPDDR5,
  .DeviceDensity    = 16,
  .DeviceWidth      = 16,
  .RankNum          = 2,
};

MEM_CONFIG_BLOCK_CONFIG  GlobalConfigBlock_64G = {
  {
    .Signature      = MEM_CONFIG_BLOCK_CONFIG_SIGNAUTE,
    .BlockSize      = sizeof(MEM_CONFIG_BLOCK_CONFIG),
    .BoardMask      = RS600_64G_x8_MASK
  },
  .MaxFreq          = MEM_CFG_MEMFREQ,
  .ChMask           = 0xF,
  .DdrType          = DDR_TYPE_LPDDR5,
  .DeviceDensity    = 16,
  .DeviceWidth      = 8,
  .RankNum          = 2,
};

MEM_CONFIG_BLOCK_CONFIG  GlobalConfigBlock_16G_HS = {
  {
    .Signature      = MEM_CONFIG_BLOCK_CONFIG_SIGNAUTE,
    .BlockSize      = sizeof(MEM_CONFIG_BLOCK_CONFIG),
    .BoardMask      = RS600_16G_HS_MASK
  },
  .MaxFreq          = DDR5500_FREQUENCY,
  .ChMask           = 0xF,
  .DdrType          = DDR_TYPE_LPDDR5,
  .DeviceDensity    = 16,
  .DeviceWidth      = 16,
  .RankNum          = 2,
};

MEM_CONFIG_BLOCK_CONFIG  GlobalConfigBlock_24G_HS = {
  {
    .Signature      = MEM_CONFIG_BLOCK_CONFIG_SIGNAUTE,
    .BlockSize      = sizeof(MEM_CONFIG_BLOCK_CONFIG),
    .BoardMask      = RS600_24G_HS_MASK
  },
  .MaxFreq          = DDR5500_FREQUENCY,
  .ChMask           = 0xF,
  .DdrType          = DDR_TYPE_LPDDR5,
  .DeviceDensity    = 12,
  .DeviceWidth      = 16,
  .RankNum          = 2,
};

MEM_CONFIG_BLOCK_CONFIG  GlobalConfigBlock_32G_x8_HS = {
  {
    .Signature      = MEM_CONFIG_BLOCK_CONFIG_SIGNAUTE,
    .BlockSize      = sizeof(MEM_CONFIG_BLOCK_CONFIG),
    .BoardMask      = RS600_32G_x8_HS_MASK
  },
  .MaxFreq          = DDR5500_FREQUENCY,
  .ChMask           = 0xF,
  .DdrType          = DDR_TYPE_LPDDR5,
  .DeviceDensity    = 8,
  .DeviceWidth      = 8,
  .RankNum          = 2,
};

MEM_CONFIG_BLOCK_CONFIG  GlobalConfigBlock_32G_Hynix = {
  {
    .Signature      = MEM_CONFIG_BLOCK_CONFIG_SIGNAUTE,
    .BlockSize      = sizeof(MEM_CONFIG_BLOCK_CONFIG),
    .BoardMask      = RS600_32G_HYNIX_MASK
  },
  .MaxFreq          = DDR6000_FREQUENCY,
  .ChMask           = 0xF,
  .DdrType          = DDR_TYPE_LPDDR5,
  .DeviceDensity    = 16,
  .DeviceWidth      = 16,
  .RankNum          = 2,
};

MEM_CONFIG_BLOCK_CONFIG  GlobalConfigBlock_48G_HS = {
  {
    .Signature      = MEM_CONFIG_BLOCK_CONFIG_SIGNAUTE,
    .BlockSize      = sizeof(MEM_CONFIG_BLOCK_CONFIG),
    .BoardMask      = RS600_48G_HS_MASK
  },
  .MaxFreq          = DDR5500_FREQUENCY,
  .ChMask           = 0xF,
  .DdrType          = DDR_TYPE_LPDDR5,
  .DeviceDensity    = 12,
  .DeviceWidth      = 8,
  .RankNum          = 2,
};

MEM_CONFIG_BLOCK_CONFIG  GlobalConfigBlock_64G_Rayson = {
  {
    .Signature      = MEM_CONFIG_BLOCK_CONFIG_SIGNAUTE,
    .BlockSize      = sizeof(MEM_CONFIG_BLOCK_CONFIG),
    .BoardMask      = RS600_64G_RAYSON_MASK
  },
  .MaxFreq          = MEM_CFG_MEMFREQ,
  .ChMask           = 0xF,
  .DdrType          = DDR_TYPE_LPDDR5,
  .DeviceDensity    = 16,
  .DeviceWidth      = 8,
  .RankNum          = 2,
};
