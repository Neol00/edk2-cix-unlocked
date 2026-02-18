#include "MemConfigBinTool.h"
#include "MemConfigurationTable.h"

MEM_CONFIG_DQ_SWAP  DqDmMapBlock = {
  {
    .Signature      = MEM_CONFIG_DQ_SWAP_SIGNATURE,
    .BlockSize      = sizeof(MEM_CONFIG_DQ_SWAP),
    .BoardMask      = BOARD_ID_MASK_DEFAULT
  },
  {
  // CH0
    {
  // BYTE Map
  //  BYTE0   BYTE1   BYTE2   BYTE3
      BYTE_0, BYTE_1, BYTE_2, BYTE_3,
  // DQ Map
  //  DQ0   DQ1   DQ2   DQ3   DQ4   DQ5   DQ6   DQ7   DM
      DQ_4, DQ_1, DQ_2, DQ_0, DQ_5, DQ_6, DQ_3, DQ_7, DM_0, // Data Slice 0 - CHA DQ0~7
      DQ_2, DQ_7, DQ_0, DQ_6, DQ_1, DQ_4, DQ_5, DQ_3, DM_0, // Data Slice 1 - CHA DQ8~15
      DQ_3, DQ_2, DQ_6, DQ_7, DQ_1, DQ_4, DQ_5, DQ_0, DM_0, // Data Slice 2 - CHB DQ0~7
      DQ_3, DQ_0, DQ_2, DQ_1, DQ_5, DQ_4, DQ_7, DQ_6, DM_0, // Data Slice 3 - CHB DQ8~15
    },
  // CH1
    {
  // BYTE Map
  //  BYTE0   BYTE1   BYTE2   BYTE3
      BYTE_0, BYTE_1, BYTE_2, BYTE_3,
  // DQ Map
  //  DQ0   DQ1   DQ2   DQ3   DQ4   DQ5   DQ6   DQ7   DM
      DQ_4, DQ_0, DQ_5, DQ_1, DQ_6, DQ_3, DQ_7, DQ_2, DM_0, // Data Slice 0 - CHA DQ0~7
      DQ_6, DQ_7, DQ_3, DQ_2, DQ_5, DQ_0, DQ_4, DQ_1, DM_0, // Data Slice 1 - CHA DQ8~15
      DQ_3, DQ_2, DQ_0, DQ_7, DQ_6, DQ_4, DQ_1, DQ_5, DM_0, // Data Slice 2 - CHB DQ0~7
      DQ_0, DQ_2, DQ_1, DQ_3, DQ_6, DQ_7, DQ_5, DQ_4, DM_0, // Data Slice 3 - CHB DQ8~15
    },
  // CH2
    {
  // BYTE Map
  //  BYTE0   BYTE1   BYTE2   BYTE3
      BYTE_1, BYTE_0, BYTE_3, BYTE_2,
  // DQ Map
  //  DQ0   DQ1   DQ2   DQ3   DQ4   DQ5   DQ6   DQ7   DM
      DQ_2, DQ_1, DQ_4, DQ_0, DQ_5, DQ_6, DQ_3, DQ_7, DM_0, // Data Slice 0 - CHA DQ0~7
      DQ_7, DQ_6, DQ_2, DQ_3, DQ_5, DQ_0, DQ_4, DQ_1, DM_0, // Data Slice 1 - CHA DQ8~15
      DQ_1, DQ_2, DQ_3, DQ_7, DQ_0, DQ_6, DQ_4, DQ_5, DM_0, // Data Slice 2 - CHB DQ0~7
      DQ_0, DQ_1, DQ_2, DQ_5, DQ_4, DQ_7, DQ_3, DQ_6, DM_0, // Data Slice 3 - CHB DQ8~15
    },
  // CH3
    {
  // BYTE Map
  //  BYTE0   BYTE1   BYTE2   BYTE3
      BYTE_0, BYTE_1, BYTE_2, BYTE_3,
  // DQ Map
  //  DQ0   DQ1   DQ2   DQ3   DQ4   DQ5   DQ6   DQ7   DM
      DQ_3, DQ_5, DQ_4, DQ_6, DQ_1, DQ_2, DQ_0, DQ_7, DM_0, // Data Slice 0 - CHA DQ0~7
      DQ_2, DQ_7, DQ_6, DQ_3, DQ_0, DQ_4, DQ_5, DQ_1, DM_0, // Data Slice 1 - CHA DQ8~15
      DQ_4, DQ_5, DQ_6, DQ_7, DQ_1, DQ_2, DQ_3, DQ_0, DM_0, // Data Slice 2 - CHB DQ0~7
      DQ_5, DQ_6, DQ_4, DQ_7, DQ_2, DQ_3, DQ_0, DQ_1, DM_0, // Data Slice 3 - CHB DQ8~15
    }
  }
};
