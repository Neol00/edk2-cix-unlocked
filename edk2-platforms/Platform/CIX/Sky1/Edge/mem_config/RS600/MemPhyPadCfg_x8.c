#include "BoardConfig.h"
#include "MemConfigBinTool.h"
#include "MemConfigurationTable.h"

#define N_OFF 0
#define N_240 1
#define N_120 2
#define N_80  3
#define N_60  4
#define N_48  5
#define N_40  6
#define N_34  7

#define P_OFF (0 << 4)
#define P_240 (1 << 4)
#define P_120 (2 << 4)
#define P_80  (3 << 4)
#define P_60  (4 << 4)
#define P_48  (5 << 4)
#define P_40  (6 << 4)
#define P_34  (7 << 4)

#define IMP_OFF (P_OFF + N_OFF)
#define IMP_240 (P_240 + N_240)
#define IMP_120 (P_120 + N_120)
#define IMP_80  (P_80  + N_80 )
#define IMP_60  (P_60  + N_60 )
#define IMP_48  (P_48  + N_48 )
#define IMP_40  (P_40  + N_40 )
#define IMP_34  (P_34  + N_34 )

#define ENTRY_COUNT 1

MEM_CONFIG_PHYPADCFG MemPhyPadCfg_rs600_x8 = {
  {
    .Signature      = MEM_CONFIG_BLOCK_PHYPADCFG_SIGNATURE,
    .BlockSize      = sizeof(MEM_CONFIG_PHYPADCFG) + sizeof(MEM_CONFIG_PHYPADCFG_ENTRY) * ENTRY_COUNT,
    .BoardMask      = RS600_64G_x8_MASK
  },
  {
    // MaxMemFreq       RankPerCh DqDrv   DqOdt   DqsDrv  DqsOdt  CkDrv   CaDrv   CsDrv   CkeDrv  FdbkDrv FdbkOdt  RstDrv FFE DFE CTLE
    {DDR6400_FREQUENCY, RANK_ALL, IMP_34, IMP_40, IMP_34, IMP_40, IMP_40, IMP_40, IMP_34, IMP_40, IMP_34, IMP_40,  IMP_34, 0,  0, 0x0},
  }
};
