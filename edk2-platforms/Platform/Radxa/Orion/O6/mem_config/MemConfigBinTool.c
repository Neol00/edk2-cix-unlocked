#include "MemConfigBinTool.h"
#include "MemConfigurationTable.h"
#include "MemConfigBinLayout.h"

#define SIZE_16B_ALIGN(size) (((size) % 16) == 0 ? (size) : (((size) >> 4) + 1) << 4)

CDCB_BLOCK_ENTRY CdcbBlockList[] = {
  CDCB_BLOCK_LIST
};

uint8_t
GenCheckSum (
  uint8_t  *buf,
  uint32_t size
)
{
  uint32_t  i;
  uint8_t   CheckSum;

  CheckSum = 0;
  for (i = 0; i < size; i ++) {
    CheckSum += buf[i];
  }
  return ~CheckSum;
}


void
DJBHash(
  uint8_t  *buf,
  uint32_t size,
  uint32_t *hash
) {
    uint32_t i;

    if (*hash == 0) {
      *hash = 5381;
    }

    for (i = 0; i < size; i ++) {
      *hash = ((*hash << 5) + *hash) + buf[i];
    }
}

void
MemConfigBinOutprint (
  uint8_t  *buf,
  uint32_t size
)
{
  FILE* pFile = fopen("memory_config.bin","wb");
  fwrite((void*)buf, sizeof(uint8_t), size, pFile);
  fclose(pFile);
}

void
main (
  void
)
{
  MEM_CONFIG_BLOCK_TABLE_ENTRY  *BlockTableEntry;
  MEM_CONFIG_BLOCK_HEADER       *BlockHeader;
  MEM_CONFIG_HEADER             CdcbHeader;
  uint32_t                      BlockTableSize;
  uint32_t                      index;
  uint16_t                      offset;
  uint32_t                      MemCfgBinSize;
  uint8_t                       *MemCfgBuf;
  uint32_t                      FeatHash = 0;

  // Init Global Header
  memset((void*)&CdcbHeader, 0, sizeof(CdcbHeader));
  CdcbHeader.Signature      = MEM_CONFIG_HEADER_SIGNATURE;
  CdcbHeader.MajorVer       = MEM_CONFIG_MAJOR_VER;
  CdcbHeader.MinorVer       = MEM_CONFIG_MINOR_VER;
  CdcbHeader.BlockTbOffset  = sizeof(MEM_CONFIG_HEADER);
  CdcbHeader.HeaderSize     = sizeof(MEM_CONFIG_HEADER);
  CdcbHeader.BlockCount     = sizeof(CdcbBlockList) / sizeof(CdcbBlockList[0]);
  CdcbHeader.HeaderCheckSum = 0;

  // Block Table
  BlockTableSize  = CdcbHeader.BlockCount * sizeof (MEM_CONFIG_BLOCK_TABLE_ENTRY);
  // Allign with 16 byte
  BlockTableSize  = SIZE_16B_ALIGN(BlockTableSize);
  BlockTableEntry = (MEM_CONFIG_BLOCK_TABLE_ENTRY*)malloc(BlockTableSize);
  memset((void*)BlockTableEntry, 0, BlockTableSize);

  offset = sizeof(MEM_CONFIG_HEADER) + BlockTableSize;

  for (index = 0; index < sizeof(CdcbBlockList) / sizeof(CdcbBlockList[0]); index++) {
    // Initialize BIOS setup table with 0xFF
    if (CdcbBlockList[index].guid == GUID_INVALID) {
      printf("Error: Invalid GUID in entry %d\n", index);
      return;
    }

    // Cal checksum
    BlockHeader = (MEM_CONFIG_BLOCK_HEADER*) (CdcbBlockList[index].ptr);
    BlockHeader->BlockChecksum = 0;
    BlockHeader->BlockChecksum = GenCheckSum((uint8_t*) CdcbBlockList[index].ptr, BlockHeader->BlockSize);

    // Block table
    BlockTableEntry[index].Guid   = CdcbBlockList[index].guid;
    BlockTableEntry[index].Offset = offset;
    offset += SIZE_16B_ALIGN(BlockHeader->BlockSize);
  }

  CdcbHeader.TotalSize = offset;
  CdcbHeader.HeaderCheckSum = GenCheckSum((uint8_t*)&CdcbHeader, CdcbHeader.HeaderSize);

  MemCfgBinSize = sizeof(MEM_QUICK_CONFIG) + offset;
  MemCfgBuf  = malloc(MemCfgBinSize);
  memset((void*)MemCfgBuf, 0, MemCfgBinSize);

  memcpy((void*)MemCfgBuf, (void*)&QuickConfigBlock, sizeof(MEM_QUICK_CONFIG));
  offset = sizeof(MEM_QUICK_CONFIG);
  memcpy((void*)&MemCfgBuf[offset], (void*)&CdcbHeader, sizeof(MEM_CONFIG_HEADER));
  offset += sizeof(MEM_CONFIG_HEADER);
  memcpy((void*)&MemCfgBuf[offset], (void*)BlockTableEntry, BlockTableSize);
  offset += BlockTableSize;
  for (index = 0; index < sizeof(CdcbBlockList) / sizeof(CdcbBlockList[0]); index++) {
    BlockHeader = (MEM_CONFIG_BLOCK_HEADER*) (CdcbBlockList[index].ptr);
    memcpy((void*)&MemCfgBuf[offset], CdcbBlockList[index].ptr, BlockHeader->BlockSize);

    // Cal hash val if Feat table
    if (CdcbBlockList[index].guid == MEM_CONFIG_BLOCK_FEATURE_GUID) {
      DJBHash((uint8_t*)&MemCfgBuf[offset], SIZE_16B_ALIGN(BlockHeader->BlockSize), &FeatHash);
    }

    offset += SIZE_16B_ALIGN(BlockHeader->BlockSize);
  }
  ((MEM_QUICK_CONFIG*)MemCfgBuf)->FeatHash = FeatHash;
  MemConfigBinOutprint(MemCfgBuf, MemCfgBinSize);
}
