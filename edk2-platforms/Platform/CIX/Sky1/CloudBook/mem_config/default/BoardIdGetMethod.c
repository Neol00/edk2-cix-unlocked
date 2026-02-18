#include "MemConfigBinTool.h"
#include "MemConfigurationTable.h"

MEM_CONFIG_BDID_GET_METHOD  BoardIdGetMethod = {
  {
    .Signature = MEM_CONFIG_BDID_GET_METHOD_SIGNAUTE,
    .BlockSize = sizeof (MEM_CONFIG_BDID_GET_METHOD) + sizeof (BDID_GET_METHOD_GPIO_S),
    .BoardMask = BOARD_ID_MASK_DEFAULT
  },
  BDID_GET_METHOD_GPIO,
  {
    16, 15, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
  }
};
