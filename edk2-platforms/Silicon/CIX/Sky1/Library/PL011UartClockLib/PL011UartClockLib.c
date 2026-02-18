#/** @file PL011UartClockLib.c
#
#  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
#
#**/

#include <Base.h>
#include <Library/SocInitLib.h>

/**
  Return clock in for PL011 Uart IP

  @return Pcd PL011UartClkInHz
**/
UINT32
EFIAPI
PL011UartClockGetFreq (
  VOID
  )
{
  UINT32  Data32;

 #ifndef FPGA_BOARD
  return FixedPcdGet32 (PL011UartClkInHz);
 #endif
  Data32 = GetSocConfig (FREQ);

  return Data32*1000;
  // return FixedPcdGet32 (PL011UartClkInHz);
}
