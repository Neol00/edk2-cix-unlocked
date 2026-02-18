/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

// This is a virtual device that supports the efi runtime rtc wake-up capability on the cix platform

#define RTC_SLAVE_ADDRESS  0x51

#define HYM8563_CONTROL2_REG_OFFSET         0x1
#define HYM8563_CTL2_AF		                  0x08

External (\_SB.AMTX, MethodObj)
External (\_SB.RMTX, MethodObj)
External (\_SB.I2C3.MXID, IntObj)

Device (ERTC) {
  Name (_HID, "ERTC0000")
  Name (_UID, 0x0)
  Name (_STA, 0x0)
  Mutex(RTMX,0)
  OperationRegion (I2CA, SystemMemory, 0x04040000, 0x100)
  Field (I2CA, DWordAcc, NoLock, Preserve)
  {
      CR,32,    // Control Register
      SR,32,    // Status Register
      AR,32,    // Address Register
      DR,32,    // Data Register
      ISR,32,   // Interrupt Status Register
      TSR,32,   // Transfer Size Register
      SMPR,32,  // Slave Monitor Pause Register
      TOR,32,   // Time Out Register
      IMR,32,   // Interrupt Mask Register
      IER,32,   // Interrupt Enable Register
      IDR,32,   // Interrupt Disable Register
      GFCR,32,  // Glitch Filter Control Register
  }
  Method (_INI, 0, NotSerialized)
  {
    REST()
  }

  Method(REST,0, Serialized) {
    IDR = 0x2FF
    Local0 = CR
    Local0 = Local0 & (~BIT4)
    Local0 = Local0 | BIT6
    CR = Local0

    TSR = 0
    Local0 = ISR
    ISR = Local0

    Local0 = SR
    SR = Local0
  }

  Method(STAT,0, Serialized) {
    //make sure 7 bit mode
    Local0 = CR
    if( !( Local0 & BIT2 ) ){
      CR = Local0 | BIT2
    }
    CLRB()
    //Check if the bus is free
    Local1 = I2C_RETRIES_NUM
    While(Local1){
      Local0 = SR
      if(!(Local0 & BIT8)){
        Break
      }
      Local1 --
    }
    if(Local1 == 0){
      Return(I2C_TIME_OUT)
    }
    //Set Bus Hold
    SETB()
    Return(I2C_SUCCESS)
  }

  Method(STOP,0, Serialized) {
    CLRB()
    CLRF()
  }

//  Read Buffer From I2C Device
//  Arg0=Address
//  Arg1=Bytes
//  Arg2=Buffer
//  Arg3=IsLastOperation
  Method(READ,4, Serialized) {
    If(Arg1 == 0){
      Return(I2C_SUCCESS)
    }

    If(Arg1 > I2C_FIFO_DEPTH){
      TSR = I2C_FIFO_DEPTH +1
    } Else {
      TSR = Arg1
    }

    // Remark 2146 Method Argument is never used ^  (Arg3)
    Local0 = Arg3
    Local0 = CR
    Local0 = Local0 | BIT0
    CR = Local0

    AR = Arg0 & 0x3FF
    Local3 = 0
    While(Arg1 != 0){
      // Clear the interrupts in interrupt status register.
      Local0 = ISR
      ISR = Local0

      If(Local0 & BIT3){
        Return(I2C_TIME_OUT)
      }

      If(Local0 & BIT2){
        Return(I2C_ERROR)
      }

      Local0 = Local0 & (~BIT1)
      Local0 = Local0 & (~BIT0)
      If(Local0 != 0){
        Return(I2C_ERROR)
      }

      If(Arg1 <= I2C_FIFO_DEPTH){
        Local1 = 1
      }Else{
        Local1 = 0
      }

      if(Local1 == 1){
        if(SR & BIT5){
          Store(DR,Index(Arg2, Local3))
          Local3 = Local3 + 1
          Arg1 = Arg1 - 1
        }
        Continue
      }

      Local4 = TSR
      If(Local4 != 1){
        Continue
      }

      Local5 = Arg1 - I2C_FIFO_DEPTH
      If(Local5 > I2C_FIFO_DEPTH){
        TSR = I2C_FIFO_DEPTH + 1
      }Else{
        TSR = Local5
      }
      Local5 = I2C_FIFO_DEPTH
      While(Local5 != 0){
        Store(DR,Index(Arg2, Local3))
        Local3 = Local3 + 1
        Arg1 = Arg1 - 1
        Local5 = Local5 - 1
      }
    }
    Return(I2C_SUCCESS)
  }

// Write Buffer to I2C Device
//  Arg0=Address
//  Arg1=Bytes
//  Arg2=Buffer
//  Arg3=IsLastOperation
  Method(WRIT,4, Serialized) {
    If(Arg1 == 0){
      Return(I2C_SUCCESS)
    }

    // Remark 2146 Method Argument is never used ^  (Arg3)
    Local0 = Arg3
    Local0 = IER
    Local0 = Local0 | BIT0
    IER = Local0
    /* Set the controller in Master transmit mode */
    Local0 = CR
    Local0 = Local0 & (~BIT0)
    CR = Local0
    // Clear the interrupts in interrupt status register.
    Local0 = ISR
    ISR = Local0
    // real TSR calculated in the loop, we just want to start the transfer
    TSR = 0
    AR = Arg0 & 0x3FF
    Local0 = 0
    Local1 = Arg1
    DR = DeRefOf(Index(Arg2, Local0))
    Local0 ++ //SendBuf
    Local1 -- //SendCount
    While (1) {
      If( Local1 <= I2C_FIFO_DEPTH - 1 ){
        Local2 = 1 //FinalTransfer
        Local3 = Local1 //ChunkSize
      }Else{
        Local2 = 0
        Local3 = I2C_FIFO_DEPTH - 1
      }

      Local4 = Local3 //Index
      While(Local4 > 0)
      {
        DR = DeRefOf(Index(Arg2, Local0))
        Local1--
        Local4--
        Local0++
      }
      If( Local2 ){
        TSR = Local3 + 1
      }Else{
        TSR = Local3
      }
      Local5 = I2C_RETRIES_NUM
      While (1) {
        Local6 = ISR //IntStatusBits
        ISR = Local6
        Local6 = Local6 & (~BIT1)
        Local5 --
        If((Local5 ==0) || (Local6 !=0))
        {
          Break
        }
      }
      If(Local5 == 0){
        Return(I2C_TIME_OUT)
      }
      If(Local6 & ~(BIT0)){
        If(Local6 & BIT3){
          Return(I2C_TIME_OUT)
        }
        If(Local6 & BIT6){
          CLRF()
        }
        Return(I2C_ERROR)
      }
      If(Local2){
        Return(I2C_SUCCESS)
      }
    }
    Return(I2C_SUCCESS)
  }

  Method(CKSB,1, Serialized) {
     Store(Sizeof(Arg0), Local0)
     Store(0, Local1)
     Store(0, Local2)
     While (LLess(Local1, Local0)) {
        if( Local1 !=1 ){
          Mid(Arg0,Local1,1,Local3)
          Local2 = Local2 + ToInteger(Local3)
        }
        Increment(Local1)
      }
    Return(0x100-(Local2&0xFF))
  }
  //Clear Bus Hold
  Method(CLRB, 0, Serialized) {
    Local0 = CR
    if(Local0 & BIT4)
    {
      CR = Local0 & (~BIT4)
    }
  }
  //Set Bus Hold
  Method(SETB, 0, Serialized) {
    Local0 = CR
    if(!(Local0 & BIT4))
    {
      CR = Local0 | BIT4
    }
  }
  //Clear Fifo
  Method(CLRF, 0, Serialized) {
     Local0 = CR
     CR = Local0 | BIT6
     While(CR & BIT6){
     }
  }
  //  Transform data with EC.
  //  Arg0 = Request Buffer
  //  Arg1 = Request Bytes
  //  Arg2 = Response Buffer
  //  Arg3 = Response Bytes
  Method(TRAS,4, Serialized) {
    Acquire(RTMX, 0xFFFF)
    if(\_SB.AMTX(\_SB.I2C3.MXID,EC_I2C_DEV_INST_ID)){
      Return(I2C_ERROR)
    }
    Local0 = 0
    While(1){
      if(STAT() != I2C_SUCCESS){
        Break
      }
      CLRF()
      // Clear the interrupts in interrupt status register.
      Local1 = ISR
      ISR = Local1
      if(WRIT(RTC_SLAVE_ADDRESS,Arg1,Arg0,0)!= I2C_SUCCESS){
        Break
      }
      if(READ(RTC_SLAVE_ADDRESS,Arg3,Arg2,1)!= I2C_SUCCESS){
        Break
      }
      Local0 = 1
      Break
    }

    if(Local0 == 0){
      REST()
      STOP()
      \_SB.RMTX(\_SB.I2C3.MXID,EC_I2C_DEV_INST_ID)
      Release(RTMX)
      Return(I2C_ERROR)
    }
    \_SB.RMTX(\_SB.I2C3.MXID,EC_I2C_DEV_INST_ID)
    Release(RTMX)
    Return(I2C_SUCCESS)
  }

  //
  // Name: CAFG [Clear RTC Alarm Flag]
  // Description: Function to clear the alarm flag of RTC
  // Input: None
  // Output: Status(No zero may have some error in it)
  //
  Method(CAFG, 0, Serialized){
    Local0 = Buffer(1){HYM8563_CONTROL2_REG_OFFSET}
    Local1 = Buffer(1){}
    Local2 = Buffer(2){HYM8563_CONTROL2_REG_OFFSET}
    if (TRAS(Local0,Sizeof(Local0),Local1,Sizeof(Local1)) != I2C_SUCCESS){
      Return(One)
    }
    CreateByteField (Local1, 0x00, CTL2)
    CTL2 &= ~HYM8563_CTL2_AF
    CreateByteField (Local2, 0x01, DATA)
    DATA = CTL2
    if (WRIT(RTC_SLAVE_ADDRESS,Sizeof(Local2),Local2,1) != I2C_SUCCESS){
      Return(One)
    }
    Return(0)
  }
}
