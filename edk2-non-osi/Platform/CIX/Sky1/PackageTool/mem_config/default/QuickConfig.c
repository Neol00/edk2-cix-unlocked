#include "MemConfigBinTool.h"
#include "MemConfigurationTable.h"

MEM_QUICK_CONFIG   QuickConfigBlock = {
  .BoardIdGettingMethod = MULTI_BOARD_METHOD_AUTO,
  .BoardId              = 0xFF,
  .BoardIdChanged       = 0xFF,
  .BootEnv              = ENV_AUTO,
  .LogFilter            = 0xFFFFFFFF,
  .vol_major_ver        = VOL_Major_VER_V15,
  .dfs_dvfs             = DVFS_ENA,
  .vol_sub_ver          = VOL_Sub_VER_V0
};
