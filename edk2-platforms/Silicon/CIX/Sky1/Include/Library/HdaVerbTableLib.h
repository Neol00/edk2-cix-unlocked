#/** @file
#
#  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
#
#**/

#ifndef HDA_VERB_TABLE_LIB_H__
#define HDA_VERB_TABLE_LIB_H__

  #include <Base.h>
  #include <Uefi.h>
  #include "IoConfig.h"
  #include "MemoryMap.h"

  #pragma pack(push,1)

/**
  Azalia verb table header
  Every verb table should contain this defined header and followed by verb commands.
**/
typedef struct {
  UINT16    VenId;    ///< Codec Vendor ID
  UINT16    DevId;    ///< Codec Device ID
  UINT8     RevId;    ///< Revision ID of the codec. 0xFF matches any revision.
  UINT8     SdiNum;   ///< SDI number, 0xFF matches any SDI.
  UINT16    CmdsNum;  ///< Number of commands DWORDs following the header.
} HDA_VERB_TABLE_HEADER;

typedef struct {
  HDA_VERB_TABLE_HEADER    Header;
  UINT32                   *VerbPtr;
} HDA_VERB_TABLE;

  #pragma pack(pop)

extern HDA_VERB_TABLE  mPlatformHdaVerbTable[];
extern UINT32          mPlatformHdaVerbTableLength;

#endif
