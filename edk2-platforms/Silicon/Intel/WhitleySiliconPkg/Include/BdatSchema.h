/** @file

  @copyright
  Copyright 2006 - 2020 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _bdat_h
#define _bdat_h

//
// Memory location that don't care was set to 0xFF
//
#define DO_NOT_CARE 0xFF

#pragma pack(1)

typedef struct bdatSchemaHeader {
  EFI_GUID                      SchemaId;
  UINT32                        DataSize;
  UINT16                        Crc16;
} BDAT_SCHEMA_HEADER_STRUCTURE;

//
// SPD data schema related definition
//

//
// Memory SPD Data Header
//
typedef struct {
  EFI_GUID  MemSpdGuid;   // GUID that uniquely identifies the memory SPD data revision
  UINT32    Size;         // Total size in bytes including the header and all SPD data
  UINT32    Crc;          // 32-bit CRC generated over the whole size minus this crc field
                          // Note: UEFI 32-bit CRC implementation (CalculateCrc32)
                          // Consumers can ignore CRC check if not needed.
  UINT32    Reserved;     // Reserved for future use, must be initialized to 0
} MEM_SPD_RAW_DATA_HEADER;

//
// Memory SPD Raw Data
//
typedef struct {
  MEM_SPD_RAW_DATA_HEADER  Header;

  //
  // This is a dynamic region, where SPD data entries are filled out.
  //
} MEM_SPD_DATA_STRUCTURE;

typedef struct {
  BDAT_SCHEMA_HEADER_STRUCTURE  SchemaHeader;
  MEM_SPD_DATA_STRUCTURE        SpdData;
} BDAT_MEM_SPD_STRUCTURE;

//
// List of all entry types supported by this revision of memory SPD data structure
//
typedef enum {
  MemSpdDataType0  = 0,

  MemSpdDataTypeMax,
  MemSpdDataTypeDelim = MAX_INT32
} MEM_SPD_DATA_TYPE;

//
// Generic entry header for all memory SPD raw data entries
//
typedef struct {
  MEM_SPD_DATA_TYPE      Type;
  UINT16                 Size;     // Entries will be packed by byte in contiguous space. Size of the entry includes the header.
} MEM_SPD_DATA_ENTRY_HEADER;

//
// Structure to specify SPD dimm memory location
//
typedef struct {
  UINT8     Socket;
  UINT8     Channel;
  UINT8     Dimm;
} MEM_SPD_DATA_ENTRY_MEMORY_LOCATION;

//
// Type 0: SPD RDIMM/LRDIMM DDR4
// The NumberOfBytes are 512 for DDR4.
//
typedef struct {
  MEM_SPD_DATA_ENTRY_HEADER           Header;
  MEM_SPD_DATA_ENTRY_MEMORY_LOCATION  MemoryLocation;
  UINT16                              NumberOfBytes;
  //
  // This is a dynamic region, where SPD data are filled out.
  // The total number of bytes of the SPD data must match NumberOfBytes
  //
} MEM_SPD_ENTRY_TYPE0;

//
// Memory training data schema related definition
//

//
// Memory training Data Header
//
typedef struct {
  EFI_GUID  MemDataGuid;  // GUID that uniquely identifies the memory training data revision
  UINT32    Size;         // Total size in bytes including the header and all memory training data
  UINT32    Crc;          // 32-bit CRC generated over the whole size minus this crc field
                          // Note: UEFI 32-bit CRC implementation (CalculateCrc32)
                          // Consumers can ignore CRC check if not needed.
  UINT32    Reserved;     // Reserved for future use, must be initialized to 0
} MEM_TRAINING_DATA_HEADER;

//
// Memory SPD Raw Data
//
typedef struct {
  MEM_TRAINING_DATA_HEADER  Header;

  //
  // This is a dynamic region, where memory training data entries are filled out.
  //
} MEM_TRAINING_DATA_STRUCTURE;

typedef struct {
  BDAT_SCHEMA_HEADER_STRUCTURE  SchemaHeader;
  MEM_TRAINING_DATA_STRUCTURE   Data;
} BDAT_MEM_TRAINING_STRUCTURE;

//
// List of all entry types supported by this revision of memory training data structure
//
typedef enum {
  MemTrainingDataCapability  = 0,
  MemTrainingDataIoGroup     = 1,
  MemTrainingDataDram        = 2,
  MemTrainingDataRcd         = 3,
  MemTrainingDataIoSignal    = 4,
  MemTrainingDataIoLatency   = 5,

  MemTrainingDataTypeMax,
  MemTrainingDataTypeDelim = MAX_INT32
} MEM_TRAINING_DATA_TYPE;

//
// Generic entry header for all memory training data entries
//
typedef struct {
  MEM_TRAINING_DATA_TYPE      Type;
  UINT16                      Size;     // Entries will be packed by byte in contiguous space. Size of the entry includes the header.
} MEM_TRAINING_DATA_ENTRY_HEADER;

//
// Structure to specify memory training data location
//
typedef struct {
  UINT8     Socket;
  UINT8     Channel;
  UINT8     SubChannel;
  UINT8     Dimm;      // 0xFF = n/a
  UINT8     Rank;      // 0xFF = n/a
} MEM_TRAINING_DATA_ENTRY_MEMORY_LOCATION;

//
// List of memory training data scope
//
typedef enum {
  PerBitMemTrainData         = 0,
  PerStrobeMemTrainData      = 1,
  PerRankMemTrainData        = 2,
  PerSubChannelMemTrainData  = 3,
  PerChannelMemTrainData     = 4,

  MemTrainDataScopeMax,
  MemTrainDataScopDelim = MAX_INT32
} MEM_TRAINING_DATA_SCOPE;

//
// Type 0: Define the capability. This info can be helpful for
// the code to display the training data.
//

typedef struct {
  MEM_TRAINING_DATA_ENTRY_HEADER           Header;
  UINT8                                    EccEnable;
  UINT8                                    MaxSocket;
  UINT8                                    MaxChannel;
  UINT8                                    MaxSubChannel;           // It is 1 if there is no sub-channel
  UINT8                                    MaxDimm;
  UINT8                                    MaxRank;
  UINT8                                    MaxStrobePerSubChannel;  // It is the MaxStrobe of the chanenl if there is no sub-channel
  UINT8                                    MaxBitsPerSubChannel;    // It is the MaxBits of the chanenl if there is no sub-channel
} MEM_TRAINING_DATA_ENTRY_TYPE0;

//
// Type 1: General training data that commonly accessed by GetSet API via Group
//

typedef struct {
  MEM_TRAINING_DATA_ENTRY_HEADER           Header;
  MEM_TRAINING_DATA_ENTRY_MEMORY_LOCATION  MemoryLocation;
  MRC_LT                                   Level;
  MRC_GT                                   Group;
  MEM_TRAINING_DATA_SCOPE                  Scope;            // If Scope is PerSubChannelMemTrainData or PerChannelMemTrainData, the training
                                                             // is applicable to whole SubChannel or Channel regardless the Dimm or Rank.
                                                             // The MemoryLoaction.Dimm and MemoryLoaction.Rank should be ignored.
  UINT8                                    NumberOfElements;
  UINT8                                    SizeOfElement;    // Number of bytes of each training data element.
                                                             // 1: UINT8
                                                             // 2: UINT16
                                                             // 4: UINT32
  //
  // This is a dynamic region, where training data are filled out.
  // The total number of bytes of the training data must be equal to
  // NumberOfElements * SizeOfElement
  //
} MEM_TRAINING_DATA_ENTRY_TYPE1;

//
// Type 2: DRAM mode register data
//

typedef struct {
  MEM_TRAINING_DATA_ENTRY_HEADER           Header;
  MEM_TRAINING_DATA_ENTRY_MEMORY_LOCATION  MemoryLocation;
  UINT8                                    NumberOfModeRegisters;
  UINT8                                    NumberOfDrams;

  //
  // This is a dynamic region, where DRAM mode register data are filled out.
  // Each mode register data is one byte. The total number of bytes of the data must be equal to
  // NumberOfModeRegisters * NumberOfDrams. The data is indexed as [ModeRegister][Dram]
  //
} MEM_TRAINING_DATA_ENTRY_TYPE2;

//
// Type 3:  RCD data
//

typedef struct {
  MEM_TRAINING_DATA_ENTRY_HEADER           Header;
  MEM_TRAINING_DATA_ENTRY_MEMORY_LOCATION  MemoryLocation;
  UINT8                                    NumberOfRegisters;

  //
  // This is a dynamic region, where RCD RW register data are filled out.
  // Each RW register data is one byte. The total number of bytes of the data must be equal to
  // NumberOfRegisters.
  //
} MEM_TRAINING_DATA_ENTRY_TYPE3;

//
// Type 4: IO Signal training data
//
typedef struct {
  MRC_GT                                   Signal;
  INT16                                    Value;
} SIGNAL_DATA;

typedef struct {
  MEM_TRAINING_DATA_ENTRY_HEADER           Header;
  MEM_TRAINING_DATA_ENTRY_MEMORY_LOCATION  MemoryLocation;
  MRC_LT                                   Level;
  MEM_TRAINING_DATA_SCOPE                  Scope;            // If Scope is PerSubChannelMemTrainData or PerChannelMemTrainData, the training
                                                             // is applicable to whole SubChannel or Channel regardless the Dimm or Rank.
                                                             // The MemoryLoaction.Dimm and MemoryLoaction.Rank should be ignored.
  UINT8                                    NumberOfSignals;  // Number of SIGNAL_DATA struct
  //
  // This is a dynamic region, where signal training data are filled out.
  // Each signal training data element is defined by a SIGNAL_DATA struct.
  // The total number of bytes of the training data must be equal to
  // NumberOfSignals * sizeof (SIGNAL_DATA)
  //
} MEM_TRAINING_DATA_ENTRY_TYPE4;

//
// Type 5: IO latency, Round trip and IO Comp training data
//

typedef struct {
  MEM_TRAINING_DATA_ENTRY_HEADER           Header;
  MEM_TRAINING_DATA_ENTRY_MEMORY_LOCATION  MemoryLocation;
  MEM_TRAINING_DATA_SCOPE                  Scope;            // If Scope is PerSubChannelMemTrainData or PerChannelMemTrainData, the training
                                                             // is applicable to whole SubChannel or Channel regardless the Dimm or Rank.
                                                             // The MemoryLoaction.Dimm and MemoryLoaction.Rank should be ignored.
  UINT8                                    IoLatency;
  UINT8                                    RoundTrip;
  UINT8                                    IoComp;
} MEM_TRAINING_DATA_ENTRY_TYPE5;

//
// Memory training data HOB header
// This header contains the actual size of the training data in the HOB. The HOB data size is
// always mutiples of 8.
//
typedef struct {
  UINT32    Size;
} MEM_TRAINING_DATA_HOB_HEADER;

#pragma pack()
#endif // _bdat_h
