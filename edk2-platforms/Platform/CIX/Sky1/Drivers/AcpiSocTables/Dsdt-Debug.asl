/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Mutex(DBGM,0)
Method(UDBG,1,Serialized)
{
#if ASL_UART_DEBUG_ENABLE
  OperationRegion (COMA, SystemMemory, FixedPcdGet64(PcdSerialRegisterBase), 0x100)
  Field (COMA, ByteAcc, NoLock, Preserve){
    offset(0x00),
    UTXD,8,
    offset(0x18),
    UTS,8,
  }
  ToHexString (Arg0, Local0)
  Store(Sizeof(Local0), Local1)
  Store(0, Local2)
  Acquire(DBGM, 0xFFFF)
  While (LLess(Local2, Local1)) {
    Store(0, Local3)
    While(Local3<10000000)
    {
      if((UTS & 0x20)==0x00){
        Break
      }
      Local3++
    }
    Mid(Local0,Local2,1,UTXD)
    Increment(Local2)
  }
  Store(0x0D,UTXD)
  Store(0x0A,UTXD)
  Release(DBGM)
#endif
}