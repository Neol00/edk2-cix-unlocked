#ifndef _CDCB_BIN_TOOL_H_
#define _CDCB_BIN_TOOL_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DDR_BOOT_FREQUENCY 200     ///< BOOT Freq: DDRC 50MHZ
#define DDR533_FREQUENCY   267     ///< DDR 533
#define DDR1066_FREQUENCY  533     ///< DDR 1066
#define DDR1600_FREQUENCY  800     ///< DDR 1600
#define DDR2133_FREQUENCY  1067    ///< DDR 2133
#define DDR2750_FREQUENCY  1375    ///< DDR 2750
#define DDR3200_FREQUENCY  1600    ///< DDR 3200
#define DDR3733_FREQUENCY  1867    ///< DDR 3733
#define DDR4266_FREQUENCY  2133    ///< DDR 4266
#define DDR4800_FREQUENCY  2400    ///< DDR 4800
#define DDR5500_FREQUENCY  2750    ///< DDR 5500
#define DDR6000_FREQUENCY  3000    ///< DDR 6000
#define DDR6400_FREQUENCY  3200    ///< DDR 6400
#define UNSUPPORTED_DDR_FREQUENCY 3201 ///< Highest limit of DDR frequency

#ifndef MEM_CFG_MEMFREQ
#define MEM_CFG_MEMFREQ    DDR5500_FREQUENCY
#endif

#define RANK_SR  1
#define RANK_DR  2
#define RANK_ALL 3

#define RTT_OFF       0x0         ///< Disabled
#define RTT_240       0x11        ///< RZQ/1
#define RTT_120       0x22        ///< RZQ/2
#define RTT_80        0x33        ///< RZQ/3
#define RTT_60        0x44        ///< RZQ/4
#define RTT_48        0x55        ///< RZQ/5
#define RTT_40        0x66        ///< RZQ/6

#define BOARD_ID_MASK_DEFAULT 0xFFFF
#define BOARD_ID_MASK(id)     (1 << (id))

typedef struct {
  uint16_t guid;
  void     *ptr;
} CDCB_BLOCK_ENTRY;

#define GUID_INVALID 0xFFFF

#define GUID(x) _Generic((x), \
    MEM_CONFIG_BLOCK_BOARDID:     MEM_CONFIG_BLOCK_BOARDID_GUID, \
    MEM_CONFIG_BLOCK_CONFIG:      MEM_CONFIG_BLOCK_CONFIG_GUID, \
    MEM_CONFIG_BLOCK_FEATURE:     MEM_CONFIG_BLOCK_FEATURE_GUID, \
    MEM_CONFIG_SPD:               MEM_CONFIG_BLOCK_SPD_GUID, \
    MEM_CONFIG_BUSCFG_LP4X:       MEM_CONFIG_BLOCK_BUSCFG_LP4X_GUID, \
    MEM_CONFIG_BUSCFG_LP5:        MEM_CONFIG_BLOCK_BUSCFG_LP5_GUID, \
    MEM_CONFIG_PHYPADCFG:         MEM_CONFIG_BLOCK_PHYPADCFG_GUID, \
    MEM_CONFIG_BIOS_SETUP:        MEM_CONFIG_BLOCK_BIOS_SETUP_GUID, \
    MEM_CONFIG_DQ_SWAP:           MEM_CONFIG_DQ_SWAP_GUID, \
    MEM_CONFIG_BLOCK_BOARDID_MAP: MEM_CONFIG_BLOCK_BOARDID_MAP_GUID, \
    MEM_CONFIG_TRACE_LEN_DIFF:    MEM_CONFIG_TRACE_LEN_DIFF_GUID, \
    MEM_CONFIG_TRAIN_OPTIMIZE:    MEM_CONFIG_TRAIN_OPTIMIZE_GUID, \
    default: GUID_INVALID)

#define CDCB_BLOCK(b) {GUID(b), &b}

#endif
