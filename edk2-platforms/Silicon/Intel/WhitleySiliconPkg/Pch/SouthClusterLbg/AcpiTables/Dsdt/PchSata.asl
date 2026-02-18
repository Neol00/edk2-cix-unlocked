/** @file

  @copyright
  Copyright 2013 - 2020 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#define ROOTPORT_READ           0
#define ROOTPORT_WRITE          1
#define ENDPOINT_READ           2
#define ENDPOINT_WRITE          3

//
// SDSM is Device Specific Method supporting AHCI DEVSLP
// It is not guaranteed to be available on every boot
//
// move one level up to Pch.asl

    Method(_DSM,4,serialized){
      if(PCIC(Arg0)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) };
      if(CondRefOf(\_SB.PC00.SAT0.SDSM)) { return (\_SB.PC00.SAT0.SDSM(Arg0,Arg1,Arg2,Arg3)) };
      Return(Buffer() {0})
    }

    Device(PRT0)
    {
      Name(_ADR,0x0000FFFF)  // Port 0
    }
    Device(PRT1)
    {
      Name(_ADR,0x0001FFFF)  // Port 1
    }
    Device(PRT2)
    {
      Name(_ADR,0x0002FFFF)  // Port 2
    }
    Device(PRT3)
    {
      Name(_ADR,0x0003FFFF)  // Port 3
    }
    Device(PRT4)
    {
      Name(_ADR,0x0004FFFF)  // Port 4
    }
    Device(PRT5)
    {
      Name(_ADR,0x0005FFFF)  // Port 5
    }

