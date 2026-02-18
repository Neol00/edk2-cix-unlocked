#ifndef _MEM_CONFIG_BIN_LAYOUT_H_
#define _MEM_CONFIG_BIN_LAYOUT_H_

// default
extern MEM_QUICK_CONFIG             QuickConfigBlock;
extern MEM_CONFIG_BLOCK_CONFIG      GlobalConfigBlock;
extern MEM_CONFIG_BLOCK_FEATURE     MemFeatureBlock;
extern MEM_CONFIG_BUSCFG_LP5        MemLpddr5BusCfg;
extern MEM_CONFIG_PHYPADCFG         MemPhyPadCfg;
extern MEM_CONFIG_BIOS_SETUP        MemBiosSetup;
extern MEM_CONFIG_DQ_SWAP           DqDmMapBlock;
extern MEM_CONFIG_TRAIN_OPTIMIZE    MemTrainOptimize;
extern MEM_CONFIG_TRACE_LEN_DIFF    MemTraceLenDiffCfg;
extern MEM_CONFIG_BLOCK_BOARDID_MAP BoardIdMapBlock;

extern MEM_CONFIG_BLOCK_CONFIG      GlobalConfigBlock_8G;
extern MEM_CONFIG_BLOCK_CONFIG      GlobalConfigBlock_12G;
extern MEM_CONFIG_BLOCK_CONFIG      GlobalConfigBlock_16G;
extern MEM_CONFIG_BLOCK_CONFIG      GlobalConfigBlock_32G;
extern MEM_CONFIG_BLOCK_CONFIG      GlobalConfigBlock_64G;
extern MEM_CONFIG_BLOCK_CONFIG      GlobalConfigBlock_32G_Lo;

extern MEM_CONFIG_BUSCFG_LP5        MemLpddr5BusCfg_rs600_x8;
extern MEM_CONFIG_PHYPADCFG         MemPhyPadCfg_rs600_x8;

#define CDCB_BLOCK_LIST \
  /* default */ \
  CDCB_BLOCK(MemBiosSetup), \
  CDCB_BLOCK(BoardIdMapBlock), \
  CDCB_BLOCK(GlobalConfigBlock), \
  CDCB_BLOCK(MemFeatureBlock), \
  CDCB_BLOCK(MemLpddr5BusCfg), \
  CDCB_BLOCK(MemPhyPadCfg), \
  CDCB_BLOCK(DqDmMapBlock), \
  CDCB_BLOCK(MemTrainOptimize), \
  CDCB_BLOCK(MemTraceLenDiffCfg), \
  CDCB_BLOCK(GlobalConfigBlock_8G), \
  CDCB_BLOCK(GlobalConfigBlock_12G), \
  CDCB_BLOCK(GlobalConfigBlock_16G), \
  CDCB_BLOCK(GlobalConfigBlock_32G), \
  CDCB_BLOCK(GlobalConfigBlock_64G), \
  CDCB_BLOCK(GlobalConfigBlock_32G_Lo), \
  CDCB_BLOCK(MemLpddr5BusCfg_rs600_x8), \
  CDCB_BLOCK(MemPhyPadCfg_rs600_x8), \

#endif
