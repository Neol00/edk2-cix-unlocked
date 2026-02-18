/** @file

  @copyright
  Copyright 2010 - 2021 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _IIO_REGS_H_
#define _IIO_REGS_H_

#include <IioSetupDefinitions.h>

/**
==================================================================================================
==================================  General Definitions          ==================================
==================================================================================================
**/
//-----------------------------------------------------------------------------------
// PCIE port index for SKX
//------------------------------------------------------------------------------------
#define SOCKET_0_INDEX           0
#define SOCKET_1_INDEX           21
#define SOCKET_2_INDEX           42
#define SOCKET_3_INDEX           63
#define SOCKET_4_INDEX           84
#define SOCKET_5_INDEX           105
#define SOCKET_6_INDEX           126
#define SOCKET_7_INDEX           147

//-----------------------------------------------------------------------------------
// Number's ports per stack definitions for 10nm
//------------------------------------------------------------------------------------

// STACK0 for: ICX-SP
#define NUMBER_PORTS_PER_STACK0_10NM       1

// NON-STACK0 for: ICX-SP
#define NUMBER_PORTS_PER_NON_STACK0_10NM   4

#define MAX_UNCORE_STACK                   2  // MAX_LOGIC_IIO_STACK - MAX_IIO_STACK

#define MaxIIO                        MAX_SOCKET

#define TOTAL_CB3_DEVICES             64   // IOAT_TOTAL_FUNCS * MAX_SOCKET. Note: this covers up to 8S.
#define MAX_TOTAL_PORTS               (MAX_SOCKET * NUMBER_PORTS_PER_SOCKET)   //NUMBER_PORTS_PER_SOCKET * MaxIIO. As now, treats setup S0-S3 = S4_S7 as optimal

  #define NUM_IAX                       1    //number of IAX per Socket
  #define NUM_DSA                       1    //number of DSA per Socket
  #define NUM_CPM                       1    //number of CPM per Socket
  #define NUM_HQM                       1    //number of HQM per Socket

#define TOTAL_IIO_STACKS              48   // MAX_SOCKET * MAX_IIO_STACK. Not reflect architecture but only sysHost structure!

#define NUMBER_NTB_PORTS_PER_SOCKET       5

#ifndef MAX_STACKS_PER_SOCKET
    #define MAX_STACKS_PER_SOCKET     6
    #define MAX_IIO_PORTS_PER_STACK   NUMBER_PORTS_PER_NON_STACK0_10NM
#endif

#define MAX_IOU_PER_SOCKET            5     // Max IOU number per socket for all silicon generation, SKX, ICX

#define MAX_VMD_ROOTPORTS_PER_PCH     20    // Max number of rootports in PCH
#define MAX_VMD_STACKS_PER_SOCKET     6     // Max number of stacks per socket supported by VMD

#define MAX_RETIMERS_PER_STACK        2            // Max number of retimers per pcie controller (ICX-SP)

#ifndef NELEMENTS
#define NELEMENTS(Array) (sizeof(Array)/sizeof((Array)[0]))
#endif


/**
==================================================================================================
==================================  IIO Root Port Definitions              ====================
==================================================================================================
**/
// Max BDFs definitions
#define MAX_FUNC_NUM            8
#define MAX_DEV_NUM             32
#define MAX_BUS_NUM             256

#define PORT_0_INDEX             0
#define PORT_A_INDEX             1
#define PORT_B_INDEX             2
#define PORT_C_INDEX             3
#define PORT_D_INDEX             4
#define PORT_E_INDEX             5
#define PORT_F_INDEX             6
#define PORT_G_INDEX             7
#define PORT_H_INDEX             8

//-----------------------------------------------------------------------------------
// Port Index definition for SKX
//------------------------------------------------------------------------------------
#define PCIE_PORT_2_DEV          0x02
// IOU0
#define PORT_1A_INDEX            1
#define PORT_1B_INDEX            2
#define PORT_1C_INDEX            3
#define PORT_1D_INDEX            4
// IOU1
#define PORT_2A_INDEX            5
#define PORT_2B_INDEX            6
#define PORT_2C_INDEX            7
#define PORT_2D_INDEX            8
// IOU2
#define PORT_3A_INDEX            9
#define PORT_3B_INDEX            10
#define PORT_3C_INDEX            11
#define PORT_3D_INDEX            12
//MCP0
#define PORT_4A_INDEX            13
#define PORT_4B_INDEX            14
#define PORT_4C_INDEX            15
#define PORT_4D_INDEX            16
//MCP1
#define PORT_5A_INDEX            17
#define PORT_5B_INDEX            18
#define PORT_5C_INDEX            19
#define PORT_5D_INDEX            20

//-----------------------------------------------------------------------------------
// Port Index definition for ICX-SP
//------------------------------------------------------------------------------------
#define PCIE_PORT_0_DEV_0     0x03
#define PCIE_PORT_0_FUNC_0    0x00

#define PCIE_PORT_1A_DEV_1    0x02
#define PCIE_PORT_1A_FUNC_1   0x00
#define PCIE_PORT_1B_DEV_1    0x03
#define PCIE_PORT_1C_DEV_1    0x04
#define PCIE_PORT_1D_DEV_1    0x05
#define PCIE_PORT_1A_FUNC_1   0x00
#define PCIE_PORT_1B_FUNC_1   0x00
#define PCIE_PORT_1C_FUNC_1   0x00
#define PCIE_PORT_1D_FUNC_1   0x00

#define PCIE_PORT_2A_DEV_2    0x02
#define PCIE_PORT_2B_DEV_2    0x03
#define PCIE_PORT_2C_DEV_2    0x04
#define PCIE_PORT_2D_DEV_2    0x05
#define PCIE_PORT_2A_FUNC_2   0x00
#define PCIE_PORT_2B_FUNC_2   0x00
#define PCIE_PORT_2C_FUNC_2   0x00
#define PCIE_PORT_2D_FUNC_2   0x00

#define PCIE_PORT_3A_DEV_3    0x02
#define PCIE_PORT_3B_DEV_3    0x03
#define PCIE_PORT_3C_DEV_3    0x04
#define PCIE_PORT_3D_DEV_3    0x05
#define PCIE_PORT_3A_FUNC_3   0x00
#define PCIE_PORT_3B_FUNC_3   0x00
#define PCIE_PORT_3C_FUNC_3   0x00
#define PCIE_PORT_3D_FUNC_3   0x00

#define PCIE_PORT_4A_DEV_4    0x02
#define PCIE_PORT_4B_DEV_4    0x03
#define PCIE_PORT_4C_DEV_4    0x04
#define PCIE_PORT_4D_DEV_4    0x05
#define PCIE_PORT_4A_FUNC_4   0x00
#define PCIE_PORT_4B_FUNC_4   0x00
#define PCIE_PORT_4C_FUNC_4   0x00
#define PCIE_PORT_4D_FUNC_4   0x00

#define PCIE_PORT_5A_DEV_5    0x02
#define PCIE_PORT_5B_DEV_5    0x03
#define PCIE_PORT_5C_DEV_5    0x04
#define PCIE_PORT_5D_DEV_5    0x05
#define PCIE_PORT_5A_FUNC_5   0x00
#define PCIE_PORT_5B_FUNC_5   0x00
#define PCIE_PORT_5C_FUNC_5   0x00
#define PCIE_PORT_5D_FUNC_5   0x00

//
// Port Config Mode
//
#define REGULAR_PCIE_OWNERSHIP        0
#define VMD_OWNERSHIP                 3
#define PCIEAIC_OCL_OWNERSHIP         4

#define DMI_BUS_NUM                   0

#define NUMBER_TRACE_HUB_PER_SOCKET                           1

//
// 8 stacks per each socket:
//   - 6 IIO stacks (used only on 14nm systems - 10nm doesn't hide per-IP)
//   - 2 uncore stacks (used only for 10nm systems - 14nm doesn't have such stacks)
//
#define NUM_DEVHIDE_REGS_PER_STACK                  8  // devHide 32-bit register for each function on stack
#define NUM_DEVHIDE_UNCORE_STACKS                   2  // number of uncore stacks in setup structure
#define NUM_DEVHIDE_IIO_STACKS                      6  // number of IIO stacks ins etup structure

#if MaxIIO > 4
#define MAX_DEVHIDE_REGS_PER_SYSTEM                  512 // MAX_DEVHIDE_REGS_PER_SOCKET * MaxIIO
#else
#define MAX_DEVHIDE_REGS_PER_SYSTEM                  256 // MAX_DEVHIDE_REGS_PER_SOCKET * MaxIIO
#endif

#endif //_IIO_REGS_H_

