#include "MemConfigBinTool.h"
#include "MemConfigurationTable.h"

#define AUTO (~0)

MEM_CONFIG_BIOS_SETUP MemBiosSetup = {
  .Header = {
    .Signature      = MEM_CONFIG_BLOCK_BIOS_SETUP_SIGNATURE,
    .BlockSize      = sizeof(MEM_CONFIG_BIOS_SETUP),
    .BoardMask      = BOARD_ID_MASK_DEFAULT
  },
  .ChannelMask = AUTO,
  .MemFreq = AUTO,
  .WckAlwaysOn = AUTO,
  .DataMask = AUTO,
  .RfmEn = AUTO,
  .AutoPrechargeEn = AUTO,
  .PbrEn = AUTO,
  .PortPriority = AUTO,
  .BdwP0 = AUTO,
  .BdwP1 = AUTO,
  .MbistEn = AUTO,
  .MbistMode = AUTO,
  .EyeScanEn = AUTO,
  .BpEn = AUTO,
  .WrLEcc = AUTO,
  .RdLEcc = AUTO,
  .RPriorityP0 = AUTO,
  .WPriorityP0 = AUTO,
  .BdwOvflowP0 = AUTO,
  .RPriorityP1 = AUTO,
  .WPriorityP1 = AUTO,
  .BdwOvflowP1 = AUTO,
  .WrPostamble = AUTO,
  .RdPostamble = AUTO,
  .WrDbi = AUTO,
  .RdDbi = AUTO,
  .IEcc  = AUTO,
  .DdrIrqPrt = AUTO,
};
