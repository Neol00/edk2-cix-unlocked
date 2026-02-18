/** @file
  Interface header file for the Enhanced warning log library class.

  @copyright
  Copyright 2018 - 2021 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _ENHANCED_WARNING_LOG_LIB_
#define _ENHANCED_WARNING_LOG_LIB_

#include <Uefi.h>
#include <MrcCommonTypes.h>

#pragma pack(1)

///
/// Enhanced Warning Log Identification GUID
/// This GUID is used for HOB, UEFI variables, or UEFI Configuration Table as needed by platform implementations
/// {D8E05800-005E-4462-AA3D-9C6B4704920B}
///
#define EWL_ID_GUID { 0xd8e05800, 0x5e, 0x4462, { 0xaa, 0x3d, 0x9c, 0x6b, 0x47, 0x4, 0x92, 0xb } };

///
/// Enhanced Warning Log Revision GUID
/// Rev 1:  {75713370-3805-46B0-9FED-60F282486CFC}
///
#define EWL_REVISION1_GUID { 0x75713370, 0x3805, 0x46b0, { 0x9f, 0xed, 0x60, 0xf2, 0x82, 0x48, 0x6c, 0xfc } };

///
/// Enhanced Warning Log Header
///
typedef struct {
  EFI_GUID  EwlGuid;      /// GUID that uniquely identifies the EWL revision
  UINT32    Size;         /// Total size in bytes including the header and buffer
  UINT32    FreeOffset;   /// Offset of the beginning of the free space from byte 0
                          /// of the buffer immediately following this structure
                          /// Can be used to determine if buffer has sufficient space for next entry
  UINT32    Crc;          /// 32-bit CRC generated over the whole size minus this crc field
                          /// Note: UEFI 32-bit CRC implementation (CalculateCrc32) (References [7])
                          /// Consumers can ignore CRC check if not needed.
  UINT32    Reserved;     /// Reserved for future use, must be initialized to 0
} EWL_HEADER;

///
/// List of all entry types supported by this revision of EWL
///
typedef enum {
  EwlType0  = 0,
  EwlType1  = 1,
  EwlType2  = 2,
  EwlType3  = 3,
  EwlType4  = 4,
  EwlType5  = 5,
  EwlType6  = 6,
  EwlType7  = 7,
  EwlType8  = 8,
  EwlType9  = 9,
  EwlType10 = 10,
  EwlType11 = 11,
  EwlType12 = 12,
  EwlType13 = 13,
  EwlType14 = 14,
  EwlType15 = 15,
  EwlType16 = 16,
  EwlType17 = 17,
  EwlType18 = 18,
  EwlType19 = 19,
  EwlType20 = 20,
  EwlType21 = 21,
  EwlType22 = 22,
  EwlType23 = 23,
  EwlType24 = 24,
  EwlType25 = 25,
  EwlType26 = 26,
  EwlType27 = 27,
  EwlType28 = 28,
  EwlType29 = 29,
  EwlTypeMax,
  EwlTypeOem = 0x8000,
  EwlTypeDelim = MAX_INT32
  } EWL_TYPE;

///
/// EWL severities
///
typedef enum {
  EwlSeverityInfo,
  EwlSeverityWarning,
  EwlSeverityFatal,
  EwlSeverityMax,
  EwlSeverityDelim = MAX_INT32
  } EWL_SEVERITY;

///
/// EWL Size\Type Structure for error checking
///
typedef struct {
  EWL_TYPE Type;
  UINT16   Size;
} EWL_SIZE_CHECK;

///
/// Generic entry header for parsing the log
///
typedef struct {
  EWL_TYPE      Type;
  UINT16        Size;     /// Entries will be packed by byte in contiguous space
  EWL_SEVERITY  Severity; /// Warning, error, informational, this may be extended in the future
} EWL_ENTRY_HEADER;

///
/// Legacy content provides context of the warning
///
typedef struct {
  UINT8     MajorCheckpoint;  // EWL Spec - Appendix B
  UINT8     MinorCheckpoint;
  UINT8     MajorWarningCode; // EWL Spec - Appendix A
  UINT8     MinorWarningCode;
} EWL_ENTRY_CONTEXT;

///
/// Legacy content to specify memory location
///
typedef struct {
  UINT8     Socket;     /// 0xFF = n/a
  UINT8     Channel;    /// 0xFF = n/a
  UINT8     Dimm;       /// 0xFF = n/a
  UINT8     Rank;       /// 0xFF = n/a
} EWL_ENTRY_MEMORY_LOCATION;

///
/// Type 1 = Legacy memory warning log content plus checkpoint
///
typedef struct {
  EWL_ENTRY_HEADER           Header;
  EWL_ENTRY_CONTEXT          Context;
  EWL_ENTRY_MEMORY_LOCATION  MemoryLocation;
} EWL_ENTRY_TYPE1;

///
/// Type 2 = Enhanced type for data IO errors per device, per bit.
///   Primarily associated with MRC training failures. Checkpoint information provides additional
///   details to identify associated training step.
///
typedef struct {
  EWL_ENTRY_HEADER           Header;
  EWL_ENTRY_CONTEXT          Context;
  EWL_ENTRY_MEMORY_LOCATION  MemoryLocation;
  UINT8                      Strobe;     /// 0xFF = n/a; include mapping of Dqs to Dq bits
  UINT8                      Bit;        /// 0xFF = n/a; Dq bit# within strobe group
  MRC_LT                     Level;      /// MrcGtDelim = n/a; Check BIOS SSA spec (References [1])
  MRC_GT                     Group;      /// MrcGtDelim = n/a; Check BIOS SSA spec (References [1])
  UINT8                      EyeSize;    /// 0xFF = n/a
} EWL_ENTRY_TYPE2;

///
/// Type 3 = Enhanced type for command, control IO errors
///   Primarily associated with MRC training failures. Checkpoint information provides additional
///   details to identify associated training step.
///
typedef struct {
  EWL_ENTRY_HEADER           Header;
  EWL_ENTRY_CONTEXT          Context;
  EWL_ENTRY_MEMORY_LOCATION  MemoryLocation;
  MRC_LT                     Level;       /// MrcGtDelim = n/a; Check BIOS SSA spec (References [1])
  MRC_GT                     Group;       /// MrcGtDelim = n/a; Check BIOS SSA spec (References [1])
  GSM_CSN                    Signal;      /// GsmCsnDelim = n/a
  UINT8                      EyeSize;     /// 0xFF = n/a
} EWL_ENTRY_TYPE3;

///
/// Requisite definitions for Type 4
///
/// Advanced Memtest Types
///
typedef enum {
  AdvMtMax = 20,
  AdvMtDelim = MAX_INT32
  } ADV_MT_TYPE;

///
/// Advanced Memtest Error log structure based on processor specific CSR definitions
///
typedef struct {
  UINT32 Dat0S;
  UINT32 Dat1S;
  UINT32 Dat2S;
  UINT32 Dat3S;
  UINT32 EccS;
  UINT32 Chunk;
  UINT32 Column;
  UINT32 ColumnExt;
  UINT32 Row;
  UINT32 RowExt;
  UINT32 Bank;
  UINT32 Rank;
  UINT32 Subrank;
} EWL_ADV_MT_STATUS;

///
/// Type 4 = Enhanced type for DRAM Advanced Memtest errors
///
typedef struct {
  EWL_ENTRY_HEADER           Header;
  EWL_ENTRY_CONTEXT          Context;
  EWL_ENTRY_MEMORY_LOCATION  MemoryLocation;
  ADV_MT_TYPE                MemtestType;
  EWL_ADV_MT_STATUS          AdvMemtestErrorInfo;
  UINT32                     Count;
} EWL_ENTRY_TYPE4;

///
/// Type 5 = Legacy Memtest accumulated DQ errors
///
typedef struct {
  EWL_ENTRY_HEADER           Header;
  EWL_ENTRY_CONTEXT          Context;
  EWL_ENTRY_MEMORY_LOCATION  MemoryLocation;
  UINT8                      SubRank;
  UINT8                      BankAddress;
  UINT8                      DqBytes[9];  /// Byte 0 = DQ[7:0], byte 1 = DQ[15:8], etc.
} EWL_ENTRY_TYPE5;

///
/// Type 6 = Legacy UPI/KTIRC warning log content plus checkpoint
///
typedef struct {
  EWL_ENTRY_HEADER        Header;
  EWL_ENTRY_CONTEXT       Context;
  UINT8                   SocketMask; /// Bitmask of CPU Sockets affected; 0xFF = SystemWide
  UINT8                   SocketType; /// 0 = CPU Socket, 1 = FPGA, 0xFF = System Wide Warning
  UINT8                   Port;       /// 0xFF = n/a; bitmask of affected port(s)
} EWL_ENTRY_TYPE6;

///
/// Type 7 = CPU BIST failures
///
typedef struct{
  EWL_ENTRY_HEADER        Header;
  EWL_ENTRY_CONTEXT       Context;
  UINT8                   Socket;  /// Socket number, 0 based
  UINT32                  Core;    /// Core number, 0 based
} EWL_ENTRY_TYPE7;

///
/// IIO Link Error log structure primary based on PCIE Specification 3.0 (References [8])
///
typedef struct {
  UINT8                   Socket;          /// Socket number, 0 based
  UINT8                   Stack;           /// 0-4, 0 = Cstack, 1-3 = Pstack, 4 MCP-stack (Only SKX-F)
  UINT8                   Port;            /// 0-3
  UINT8                   LtssmMainState;  /// Link state
  UINT8                   LtssmSubState;   /// Check Appendix C to review states definitions
  UINT32                  DidVid;          /// [31:16] DeviceID, [15:0] VendorID of the device
                                           ///   attached to the Root Port
} EWL_IIO_LINK_DESCRIPTION;

///
/// Type 8 = IIO Link Degraded width
///
typedef struct {
  EWL_ENTRY_HEADER         Header;
  EWL_ENTRY_CONTEXT        Context;
  EWL_IIO_LINK_DESCRIPTION LinkDescription;
  UINT8                    ExpectedLinkWidth;  /// Check register "Link Capabilities Register" over
  UINT8                    ActualLinkWidth;    /// PCIE Specification 3.0 (References [8])
} EWL_ENTRY_TYPE8;

///
/// Type 9 = IIO Link Degraded speed
///
typedef struct {
  EWL_ENTRY_HEADER         Header;
  EWL_ENTRY_CONTEXT        Context;
  EWL_IIO_LINK_DESCRIPTION LinkDescription;
  UINT8                    ExpectedLinkSpeed;  /// Check register "Link Capabilities Register" over
  UINT8                    ActualLinkSpeed;    /// PCIE Specification 3.0 (References [8])
} EWL_ENTRY_TYPE9;

///
/// Type 10 = Dq Swizzle Discovery errors
///   Error if 0 or greater than 1 bit set in SwizzledDqLanes per strobe
///
typedef struct {
  EWL_ENTRY_HEADER           Header;
  EWL_ENTRY_CONTEXT          Context;
  EWL_ENTRY_MEMORY_LOCATION  MemoryLocation;
  UINT8                      SwizzlePattern;  /// DQ pattern sent from device
  UINT8                      SwizzledDqLanes; /// DQ pattern received at Host
  UINT8                      LanesPerStrobe;  /// 4 or 8
  UINT8                      Strobe;          /// DQS number to identify device
} EWL_ENTRY_TYPE10;

///
/// Type 13 = NVMDIMM Training Failure
///   Reported when a training issue is encountered
///   Includes additional details on the NVMDIMM SPD and FW revisions
///
typedef struct {
  EWL_ENTRY_HEADER           Header;
  EWL_ENTRY_CONTEXT          Context;
  EWL_ENTRY_MEMORY_LOCATION  MemoryLocation;
  UINT16                     RevisionNvmdimmFw;
  UINT8                      RevisionNvmdimmSpd;
} EWL_ENTRY_TYPE13;

///
/// Type 17 = ME communication failures
///   Failure to communicate with Manageability Engine
///
typedef struct {
  EWL_ENTRY_HEADER       Header;
  EWL_ENTRY_CONTEXT      Context;
  UINT32                 Revision;       /// ME API Revision
  UINT32                 Mefs1;          /// ME Firmware Status 1 (HECI-1 HFS)
  UINT32                 Mefs2;          /// ME Firmware Status 2 (HECI-1 GS_SHDW)
  UINT8                  HeciDevice;     /// HECI device (1, 2, or 3)
  UINT8                  MeAddress;      /// HECI address of ME entity
  UINT8                  SendStatus;     /// Status of send operation
  UINT8                  ReceiveStatus;  /// Status of receive operation
  UINT64                 Request;        /// First 8 bytes of request message
  UINT32                 Response;       /// First 4 bytes of response message
} EWL_ENTRY_TYPE17;

///
/// To get more information about Machine-Check Architecture please check Chapter 15 from Vol. 3B
///  of the Intel(R) 64 and IA-32 Architectures Software Developer's Manual (References [6]) for a
///  general review.
///
/// Type 20 = CPU Machine Check Errors
///
typedef struct {
  EWL_ENTRY_HEADER        Header;
  EWL_ENTRY_CONTEXT       Context;
  UINT32                  CpuId;         /// Refer to CPUID(EAX = 1) instruction to get Type, Family,
                                         ///  Model, and Stepping ID from (References [6])
  UINT8                   Socket;        /// Socket number, 0 based
  UINT32                  Core;          /// Core number, 0 based
  UINT32                  McBankNum;     /// Please refer to mcBankTable
  UINT32                  McBankStatus;  /// Check register IA32_MCi_STATUS MSRs (References [6]&[5])
  UINT32                  McBankAddr;    /// Check register IA32_MCi_ADDR MSRs (References [6]&[5])
  UINT32                  McBankMisc;    /// Check register IA32_MCi_MISC MSRs (References [6]&[5])
} EWL_ENTRY_TYPE20;

///
/// Requisite definitions for Type 21
///
/// Reasons for Topology degradation
///
typedef enum {
  Undefined           = 0,
  LinkFail            = 1,
  InvalidTopology     = 2,
  FeatureVsTopology   = 3,
  DegradeReasonMax,
  DegradeReasonDelim  = MAX_INT32
  } TOPOLOGY_DEGRADE_REASON;

///
/// Type 21: Warning for tracking changes to KTI/UPI topology
///
/// Topology will be represented with a UINT64 bit array
/// 0 indicates absent or inactive link
/// 1 indicates active KTI/UPI link
///
/// Link Bit array member variables follow this format
/// Each nibble corresponds to a socket:
/// Each socket has MAX_FW_KTI_PORTS bits
/// [(8*MAX_FW_KTI_PORTS - 1):7*MAX_FW_KTI_PORTS] - link bit mask for socket 7
/// [(7*MAX_FW_KTI_PORTS - 1):6*MAX_FW_KTI_PORTS] - link bit mask for socket 6
/// ....
/// [(2*MAX_FW_KTI_PORTS - 1):  MAX_FW_KTI_PORTS] - link bit mask for socket 1
/// [(MAX_FW_KTI_PORTS - 1)  :                 0] - link bit mask for socket 0
///
///   Bit 0 indicates an active link on port socket 0 port 0
///   Bit 1 indicates an active link on port socket 0 port 1
///   and so on.

typedef struct {
  EWL_ENTRY_HEADER        Header;
  TOPOLOGY_DEGRADE_REASON Reason;
  UINT64                  DegradedFrom;     /// Link Bit Array
  UINT64                  NewTopology;      /// Link Bit Array
} EWL_ENTRY_TYPE21;

///
/// To get more information about Machine-Check Architecture please check Chapter 15 from Vol. 3B
///  of the Intel 64 and IA-32 Architectures Software Developer's Manual (References [6]) for a
///  general review.
///
/// Type 22 = CPU Machine Check Errors. 2nd Version.
///
typedef struct {
  EWL_ENTRY_HEADER        Header;
  EWL_ENTRY_CONTEXT       Context;
  UINT32                  CpuId;         /// Refer to CPUID(EAX=1) instruction to get Type, Family,
                                         ///  Model, and Stepping ID from (References [6])
  UINT8                   Socket;        /// Socket number, 0 based
  UINT32                  Core;          /// Core number, 0 based
  UINT32                  McBankNum;     /// Please refer to mcBankTable
  UINT64                  McBankStatus;  /// Check register IA32_MCi_STATUS MSRs (References [6]&[5])
  UINT64                  McBankAddr;    /// Check register IA32_MCi_ADDR MSRs (References [6]&[5])
  UINT64                  McBankMisc;    /// Check register IA32_MCi_MISC MSRs (References [6]&[5])
} EWL_ENTRY_TYPE22;

//
// Memory Boot Health check Warning log.
//
typedef struct {
  EWL_ENTRY_HEADER           Header;
  EWL_ENTRY_CONTEXT          Context;
  EWL_ENTRY_MEMORY_LOCATION  MemoryLocation;
  MRC_GT                     Group;      /// MrcGtDelim = n/a;
  INT16                      Offset;     /// Signal offset size that caused the error
} EWL_ENTRY_TYPE25;

//
// Memory Power Management Errors
//
typedef struct {
  EWL_ENTRY_HEADER           Header;
  EWL_ENTRY_CONTEXT          Context;
  EWL_ENTRY_MEMORY_LOCATION  MemoryLocation;
} EWL_ENTRY_TYPE26;

///
/// Type 27 = NVMDIMM Media Log
///   Reported NVMDIMM Media log
///
typedef struct {
  EWL_ENTRY_HEADER           Header;
  EWL_ENTRY_CONTEXT          Context;
  EWL_ENTRY_MEMORY_LOCATION  MemoryLocation;
  UINT64                     TimeStamp;
  UINT64                     DPA;
  UINT64                     PDA;
  UINT8                      Range;
  UINT8                      ErrorType;
  UINT8                      ErrorFlag;
  UINT8                      TransacationType;
  UINT16                     SequenceNumber;
  UINT16                     Rsvd;
} EWL_ENTRY_TYPE27;

///
/// Type 28 = NVMDIMM Thermal Log
///   Reported NVMDIMM Thermal log
///
typedef struct {
  EWL_ENTRY_HEADER           Header;
  EWL_ENTRY_CONTEXT          Context;
  EWL_ENTRY_MEMORY_LOCATION  MemoryLocation;
  UINT64                     TimeStamp;
  UINT32                     HostReportedTempData;
  UINT16                     SequenceNumber;
  UINT16                     Rsvd;
} EWL_ENTRY_TYPE28;

//
// RMT minimum margin check warning log.
//
typedef struct {
  EWL_ENTRY_HEADER           Header;
  EWL_ENTRY_CONTEXT          Context;
  EWL_ENTRY_MEMORY_LOCATION  MemoryLocation;
  MRC_GT                     Group;
  INT16                      NegativeMargin;
  INT16                      PositiveMargin;
  INT16                      MinimumMargin;
} EWL_ENTRY_TYPE29;

#pragma pack()

///
/// Enhanced Warning Log Spec defined data log structure
///
typedef struct {
  EWL_HEADER Header;          /// The size will vary by implementation and should not be assumed
  UINT8      Buffer[4 * 1024];  /// The spec requirement is that the buffer follow the header
} EWL_PUBLIC_DATA;

///
/// EWL private data structure.  This is going to be implementation dependent
///   When we separate OEM hooks via a PPI, we can remove this
///
typedef struct {
  UINT32            bufSizeOverflow;  // Number of bytes that could not be added to buffer
  UINT32            numEntries;       // Number of entries currently logged
  EWL_PUBLIC_DATA   status;           // Spec defined EWL
} EWL_PRIVATE_DATA;

#endif // #ifndef _ENHANCED_WARNING_LOG_LIB_
