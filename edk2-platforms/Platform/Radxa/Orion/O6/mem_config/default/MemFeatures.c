#include "MemConfigBinTool.h"
#include "MemConfigurationTable.h"

MEM_CONFIG_BLOCK_FEATURE MemFeatureBlock = {
  {
    .Signature      = MEM_CONFIG_BLOCK_FEATURE_SIGNAUTE,
    .BlockSize      = sizeof(MEM_CONFIG_BLOCK_FEATURE),
    .BoardMask      = BOARD_ID_MASK_DEFAULT
  },
  {
  // Performance feature
    .DataMask         = 1,
    .RfmEn            = 0,
    .WckAlwaysOn      = 1,
    .AutoPrechargeEn  = 1,
    .PbrEn            = 1,
    .SelBankInQ       = 1,
    .PortPriority     = PORT_PRIORITY_QOS,
    .BdwP0            = 100,
    .BdwP1            = 100,
    .BdwOvflowP0      = 1,
    .BdwOvflowP1      = 1,
    .RPriorityP0      = 4,
    .WPriorityP0      = 4,
    .RPriorityP1      = 4,
    .WPriorityP1      = 4,
    .AddressMode      = 0,
    .WrLEcc           = 0,
    .RdLEcc           = 0,
    .IEcc             = 0,
// DBG feature
    .BpEn             = 0,
    .MbistEn          = 0,
    .MbistMode        = 1,
    .EyeScanEn        = 0,
    .TrainMode        = 0,
    .PeriodicTrain    = 0,
  }
};
