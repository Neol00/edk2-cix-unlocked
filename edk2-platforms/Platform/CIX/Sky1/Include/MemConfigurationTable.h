/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved

**/
#ifndef _MEM_CONFIGURATION_TABLE_H_
#define _MEM_CONFIGURATION_TABLE_H_

typedef UINT8   uint8_t;
typedef UINT16  uint16_t;
typedef UINT32  uint32_t;
typedef UINT64  uint64_t;
typedef INT16   int16_t;

#define MEM_CONFIG_MAJOR_VER 0x1
#define MEM_CONFIG_MINOR_VER 0x6

#define CDCB_MEM_QUICK_CONFIG_OFFSET 0
#pragma pack(push, 1)
typedef struct {
#define MULTI_BOARD_DISABLE      0
#define MULTI_BOARD_METHOD_GPIO  1
#define MULTI_BOARD_METHOD_FIXED 2
#define MULTI_BOARD_METHOD_AUTO  0xFF
  uint8_t    BoardIdGettingMethod;
  uint8_t    BoardId;
  uint8_t    BoardIdChanged;
#define ENV_EMU     0
#define ENV_ATE     1
#define ENV_AUTO    0xFF

  uint8_t    BootEnv;
  uint32_t   LogFilter;

#define VOL_Major_VER_V0          0x0     //v0.0
#define VOL_Major_VER_V1          0x1     //v1.0
#define VOL_Major_VER_V2          0x2     //v2.0
#define VOL_Major_VER_V3          0x3     //v3.0
#define VOL_Major_VER_V4          0x4     //v4.0
#define VOL_Major_VER_AUTO        0xFF    //VF-Ver value is hardcode according to board id
                                          //phecda: 4
                                          //others: 1
  uint8_t  vol_major_ver;
#define DFS_DVFS_DIS              0x0     // neither dfs nor dvfs
#define DFS_ENA                   0x1     // dfs enable
#define DVFS_ENA                  0x2     // dvfs enable
#define DFS_DVFS_AUTO             0xFF    // if set AUTO, value is same as 0x0
  uint8_t  dfs_dvfs;

#define VOL_Sub_VER_V0            0x0     //v0.0
#define VOL_Sub_VER_V1            0x1     //v1.0
#define VOL_Sub_VER_V2            0x2     //v2.0
#define VOL_Sub_VER_V3            0x3     //v3.0
#define VOL_Sub_VER_AUTO          0xFF    //VF-Ver value is hardcode according to board id
                                          //phecda: 0
                                          //others: 0
  uint8_t  vol_sub_ver;

  #define CPU_MAX_FREQ_2_5G 0
  #define CPU_MAX_FREQ_2_6G 1
  uint8_t  cpufmax;                      //config cpu max frquency

  uint32_t FeatHash;

  uint8_t  Reserved_1[16];
} MEM_QUICK_CONFIG;

#define MEM_CONFIG_HEADER_SIGNATURE 0x42434443 // "CDCB"
#define CDCB_MEM_CONFIG_HEADER_OFFSET (sizeof(MEM_QUICK_CONFIG))
typedef struct {
  uint32_t    Signature;
  uint32_t    Reserved0;
  uint16_t    MajorVer;
  uint16_t    MinorVer;
  uint16_t    BlockTbOffset;
  uint8_t     BlockCount;
  uint8_t     Reserved1;
  uint8_t     HeaderSize;
  uint8_t     HeaderCheckSum;
  uint16_t    TotalSize;
  uint32_t    Reserved2[10];
  uint32_t    HashValue;
} MEM_CONFIG_HEADER;

#define CDCB_MEM_CONFIG_BLOCK_TABLE_OFFSET (CDCB_MEM_CONFIG_HEADER_OFFSET + (sizeof(MEM_CONFIG_HEADER)))
typedef struct {
  uint16_t Guid;
  uint16_t Offset;
} MEM_CONFIG_BLOCK_TABLE_ENTRY;

typedef struct {
  uint32_t    Signature;
  uint32_t    Reserved0;
  uint16_t    BlockSize;
  uint8_t     BlockChecksum;
  uint8_t     Reserved1;
  uint16_t    BoardMask;
  uint16_t    Reserved2;
} MEM_CONFIG_BLOCK_HEADER;

#define BLOCK_OFFSET(offset) (CDCB_MEM_CONFIG_HEADER_OFFSET + (offset))

#define MEM_CONFIG_BLOCK_BOARDID_GUID      0x1000
#define MEM_CONFIG_BLOCK_BOARDID_SIGNAUTE  0x44494442 // "BDID"
typedef struct {
  MEM_CONFIG_BLOCK_HEADER Header;
  uint8_t                 GpioIndex0;
  uint8_t                 GpioIndex1;
  uint8_t                 GpioIndex2;
  uint8_t                 GpioIndex3;
} MEM_CONFIG_BLOCK_BOARDID;

#define MEM_CONFIG_BLOCK_CONFIG_GUID       0x1001
#define MEM_CONFIG_BLOCK_CONFIG_SIGNAUTE   0x464E4F43 //"CONF"
typedef struct {
  MEM_CONFIG_BLOCK_HEADER Header;
  uint16_t                MaxFreq;
  uint8_t                 ChMask;
#define DDR_TYPE_LPDDR4X 0
#define DDR_TYPE_LPDDR5  1
  uint8_t                 DdrType;
  uint8_t                 DeviceDensity; // Gb
  uint8_t                 DeviceWidth;
  uint8_t                 RankNum;
  uint8_t                 rsvd;
} MEM_CONFIG_BLOCK_CONFIG;

#define MEM_CONFIG_BLOCK_FEATURE_GUID      0x1002
#define MEM_CONFIG_BLOCK_FEATURE_SIGNAUTE  0x54414546 //"FEAT"
typedef struct {
  uint8_t                 WckAlwaysOn;
  uint8_t                 DataMask;
  uint8_t                 AutoPrechargeEn;
  uint8_t                 PbrEn;
  uint8_t                 RfmEn;
  uint8_t                 SelBankInQ;
#define  PORT_PRIORITY_FIXED 0
#define  PORT_PRIORITY_QOS   1
  uint8_t                 PortPriority;
  uint8_t                 BdwP0;
  uint8_t                 BdwP1;
  uint8_t                 AddressMode;
#define  MBIST_DISABLE 0
#define  MBIST_DDRC    1
#define  MBIST_DDRPI   2
  uint8_t                 MbistEn;
  uint8_t                 MbistMode; // 0: MOVI1_3N 1: LIMIT_MOVI 2: SELF_REF_RETENTION 3: IDLE_RETENTION 4: MEM_INITIAL_PROGRAMMING
  uint8_t                 EyeScanEn;
  uint8_t                 BpEn;
  uint8_t                 WrLEcc;
  uint8_t                 RdLEcc;
  uint8_t                 RPriorityP0;
  uint8_t                 WPriorityP0;
  uint8_t                 BdwOvflowP0;
  uint8_t                 RPriorityP1;
  uint8_t                 WPriorityP1;
  uint8_t                 BdwOvflowP1;
#define TRAIN_MODE_PI_INIT_LVL 0
#define TRAIN_MODE_SW_TRAIN    1
  uint8_t                 TrainMode; // 0: PI_INIT_LVL 1: SW Train
  uint8_t                 IEcc;
  uint8_t                 PeriodicTrain; // BIT0: CALVL, BIT1: WRLVL, BIT2: GTLVL, BIT3: RDLVL, BIT4: WDQLVL, BIT7: DQS_OSC
  uint8_t                 rsvd[3];
} MEM_CONFIG_FEATURE;

typedef struct {
  MEM_CONFIG_BLOCK_HEADER Header;
  MEM_CONFIG_FEATURE      Feat;
} MEM_CONFIG_BLOCK_FEATURE;

#define MEM_CONFIG_BLOCK_SPD_GUID      0x1003
#define MEM_CONFIG_BLOCK_SPD_SIGNAUTE  0x4450534D //"MSPD"
typedef struct {
  uint8_t                 byte[512];
} MEM_CONFIG_SPD;

#define MEM_CONFIG_BLOCK_BUSCFG_LP4X_GUID      0x1004
#define MEM_CONFIG_BLOCK_BUSCFG_LP4X_SIGNATURE 0x344C4342 // "BCL4"
typedef struct {
  uint16_t                MaxMemFreq;       ///< Bitmap of DDR rate
  uint8_t                 RankPerCh;        ///< Bitmap of rank type of Dimm0
  uint8_t                 CA_ODT;           ///< CA_ODT
  uint8_t                 CK_ODT;           ///< CK_ODT
  uint8_t                 CS_ODT;           ///< CS_ODT
  uint8_t                 DQ_ODT;           ///< DQ_ODT
  uint8_t                 SOC_ODT;          ///< SOC_ODT
  uint8_t                 NTDQ_ODT;         ///< NTDQ_ODT
  uint8_t                 ODT_PDDS;         ///< PDDS
  uint16_t                DqVref;
  uint16_t                CaVref;
} MEM_CONFIG_BUSCFG_LP4X_ENTRY;

typedef struct {
  MEM_CONFIG_BLOCK_HEADER      Header;
  MEM_CONFIG_BUSCFG_LP4X_ENTRY BusCfg[];
} MEM_CONFIG_BUSCFG_LP4X;

#define MEM_CONFIG_BLOCK_BUSCFG_LP5_GUID       0x1005
#define MEM_CONFIG_BLOCK_BUSCFG_LP5_SIGNATURE  0x354C4342 // "BCL5"
typedef struct {
  uint16_t                MaxMemFreq;       ///< Bitmap of DDR rate
  uint8_t                 RankPerCh;        ///< Bitmap of rank type of Dimm0
  uint8_t                 CA_ODT;           ///< CA_ODT
  uint8_t                 CK_ODT;           ///< CK_ODT
  uint8_t                 CS_ODT;           ///< CS_ODT
  uint8_t                 DQ_ODT;           ///< DQ_ODT
  uint8_t                 WCK_ODT;          ///< WCK_ODT
  uint8_t                 SOC_ODT;          ///< SOC_ODT
  uint8_t                 NTDQ_ODT;         ///< NTDQ_ODT
  uint8_t                 ODT_PDDS;         ///< PDDS
  uint16_t                DqVref;
  uint16_t                CaVref;
} MEM_CONFIG_BUSCFG_LP5_ENTRY;

typedef struct {
  MEM_CONFIG_BLOCK_HEADER     Header;
  MEM_CONFIG_BUSCFG_LP5_ENTRY BusCfg[];
} MEM_CONFIG_BUSCFG_LP5;

#define MEM_CONFIG_BLOCK_PHYPADCFG_GUID       0x1006
#define MEM_CONFIG_BLOCK_PHYPADCFG_SIGNATURE  0x43444150 // "PADC"
typedef struct {
  uint16_t  MaxMemFreq;    ///< Bitmap of DDR rate
  uint8_t   RankPerCh;     ///< Bitmap of rank type of Dimm0
  uint8_t   DqDrv;
  uint8_t   DqOdt;
  uint8_t   DqsDrv;
  uint8_t   DqsOdt;
  uint8_t   CkDrv;
  uint8_t   CaDrv;
  uint8_t   CsDrv;
  uint8_t   CkeDrv;
  uint8_t   FdbkDrv;
  uint8_t   FdbkOdt;
  uint8_t   RstDrv;
  uint8_t   FFE;           ///< Ck Cke Ca Cs Dq Dqs
  uint8_t   DFE;           ///< Dq
  uint8_t   CTLE;
  uint8_t   Rsvd[3];
} MEM_CONFIG_PHYPADCFG_ENTRY;

typedef struct {
  MEM_CONFIG_BLOCK_HEADER     Header;
  MEM_CONFIG_PHYPADCFG_ENTRY  PhyPadCfg[];
} MEM_CONFIG_PHYPADCFG;

#define MEM_CONFIG_BLOCK_BIOS_SETUP_GUID       0x1007
#define MEM_CONFIG_BLOCK_BIOS_SETUP_SIGNATURE  0x54455342 // "BSET"

typedef struct {
  MEM_CONFIG_BLOCK_HEADER Header;
  uint16_t                MemFreq;          // Auto: 0xFFFF
  uint8_t                 WckAlwaysOn;      // 0: Disable 1: Enable 0xFF: Auto
  uint8_t                 DataMask;         // 0: Disable 1: Enable 0xFF: Auto
  uint8_t                 RfmEn;            // 0: Disable 1: Enable 0xFF: Auto
  uint8_t                 AutoPrechargeEn;  // 0: Disable 1: Enable 0xFF: Auto
  uint8_t                 PbrEn;            // 0: Disable 1: Enable 0xFF: Auto
  uint8_t                 PortPriority;     // 0: PORT_PRIORITY_FIXED 1: PORT_PRIORITY_QOS 0xFF: Auto
  uint8_t                 BdwP0;            // Only valid when PortPriority = PORT_PRIORITY_QOS, range: 0 ~ 100
  uint8_t                 BdwP1;            // Only valid when PortPriority = PORT_PRIORITY_QOS, range: 0 ~ 100
  uint8_t                 MbistEn;          // 0: Disable 1: Enable 0xFF: Auto
  uint8_t                 MbistMode;        // 0: Disable 1: Enable 0xFF: Auto
  uint8_t                 EyeScanEn;        // 0: Disable 1: Enable 0xFF: Auto
  uint8_t                 BpEn;             // 0: Disable 1: Enable 0xFF: Auto
  uint8_t                 ChannelMask;      // 0xFF: Auto. 0x1~0xF: BIT0~3 map to channel 0~3, 1 = Enable, 0 = Disable.
  uint8_t                 WrLEcc;
  uint8_t                 RdLEcc;
  uint8_t                 RPriorityP0;
  uint8_t                 WPriorityP0;
  uint8_t                 BdwOvflowP0;
  uint8_t                 RPriorityP1;
  uint8_t                 WPriorityP1;
  uint8_t                 BdwOvflowP1;
  uint8_t                 WrPostamble;
  uint8_t                 RdPostamble;
  uint8_t                 WrDbi;
  uint8_t                 RdDbi;
  uint8_t                 IEcc;
  uint8_t                 DdrIrqPrt;
} MEM_CONFIG_BIOS_SETUP;

#define MEM_CONFIG_DQ_SWAP_GUID       0x1008
#define MEM_CONFIG_DQ_SWAP_SIGNATURE  0x504D5144 // "DQMP"

#define DQ_0   0
#define DQ_1   1
#define DQ_2   2
#define DQ_3   3
#define DQ_4   4
#define DQ_5   5
#define DQ_6   6
#define DQ_7   7
#define DM_0   8
#define BYTE_0 0
#define BYTE_1 1
#define BYTE_2 2
#define BYTE_3 3

typedef struct {
  uint8_t DQ[8];
  uint8_t DM;
} DQM_MAP_ENTRY;

typedef struct {
  MEM_CONFIG_BLOCK_HEADER Header;
  struct {
    uint8_t       Byte0 : 2;
    uint8_t       Byte1 : 2;
    uint8_t       Byte2 : 2;
    uint8_t       Byte3 : 2;
    DQM_MAP_ENTRY DataSlice[4];
  } Channel[4];
} MEM_CONFIG_DQ_SWAP;

#define MEM_CONFIG_BLOCK_BOARDID_MAP_GUID      0x1009
#define MEM_CONFIG_BLOCK_BOARDID_MAP_SIGNAUTE  0x504D4442 // "BDMP"

typedef struct {
  uint8_t PcbId;
  uint8_t BoardId;
} BOARD_ID_MAP_ENTRY;

typedef struct {
  MEM_CONFIG_BLOCK_HEADER Header;
  BOARD_ID_MAP_ENTRY      BoardIdMap[];
} MEM_CONFIG_BLOCK_BOARDID_MAP;

#define MEM_CONFIG_TRACE_LEN_DIFF_GUID      0x100A
#define MEM_CONFIG_TRACE_LEN_DIFF_SIGNAUTE  0x46444C54 // "TLDF"

typedef struct {
  uint16_t AvgFlightTime[4]; // DataSlice 0~3
  int16_t  TraceLenDiff[4];
} TRACE_LEN_DIFF_ENTRY;

typedef struct {
  MEM_CONFIG_BLOCK_HEADER Header;
  TRACE_LEN_DIFF_ENTRY    Channel[4];
} MEM_CONFIG_TRACE_LEN_DIFF;

#define MEM_CONFIG_TRAIN_OPTIMIZE_GUID      0x100B
#define MEM_CONFIG_TRAIN_OPTIMIZE_SIGNAUTE  0x504F4E54 // "TNOP"

typedef struct {
  uint16_t MaxMemFreq;
  uint8_t  RankPerCh;
  uint8_t  ChMsk;
  uint8_t  Center;
  uint8_t  Shift;
  uint8_t  Threshold;
  uint8_t  rsvd;
} MEM_CONFIG_TRAIN_OPTIMIZE_ENTRY;

typedef struct {
  MEM_CONFIG_BLOCK_HEADER         Header;
  MEM_CONFIG_TRAIN_OPTIMIZE_ENTRY TrnOpt[];
} MEM_CONFIG_TRAIN_OPTIMIZE;
#pragma pack(pop)
#endif
