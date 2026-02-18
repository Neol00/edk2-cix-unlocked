/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define EC_SLAVE_ADDRESS  FixedPcdGet32(PcdEcI2cSlaveAddress)
#define I2C_SUCCESS  0
#define I2C_ERROR    1
#define I2C_TIME_OUT 2

#define I2C_RETRIES_NUM 1000000
#define I2C_FIFO_DEPTH  16

#define EC_I2C_DEV_INST_ID          FixedPcdGet32(PcdEcAcpiI2cDeviceInstanceId)

#define EC2OS_ACPI_EVENT_PWRBTN_STATE_CHANGE    BIT1
#define EC2OS_ACPI_EVENT_AC_PLUG_IN             BIT2
#define EC2OS_ACPI_EVENT_AC_PLUG_OUT            BIT3
#define EC2OS_ACPI_EVENT_BAT_STATE_CHANGE       BIT4
#define EC2OS_ACPI_EVENT_LID_STATE_CHANGE       BIT8
#define EC2OS_ACPI_EVENT_WIFI_STATE_CHANGE      BIT9
#define EC2OS_ACPI_EVENT_DOCK_STATE_CHANGE      BIT10
#define EC2OS_ACPI_EVENT_WWAN_STATE_CHANGE      BIT11
#define EC2OS_ACPI_EVENT_ALS_STATE_CHANGE       BIT12
#define EC2OS_ACPI_EVENT_IC_ERROR_CHANGE        BIT13
#define EC2OS_ACPI_EVENT_BAT_FULL_CHARGE        BIT14

#define EC_THERMAL_SUPPORT 1
#define EC_PWRB_SUPPORT    1

#define EC_FAN_MODE        FixedPcdGet8(PcdEcDefaultFanMode)
// This is the PCD value, not EC value
#define PCD_FAN_MODE_AUTO  0x00
#define PCD_FAN_MODE_PERF  0x01
#define PCD_FAN_MODE_MUTE  0x02

External (\_SB.AMTX, MethodObj)
External (\_SB.RMTX, MethodObj)
External (\_SB.I2C6.MXID, IntObj)

Device(EC0){
  Name(_UID, 0)
  Name(_HID, "CIXHA015")
  Mutex(ECMX,0)
  Method(_STA)
  {
    Return(0x03)
  }
  OperationRegion (I2CA, SystemMemory, FixedPcdGet32 (PcdEcI2cBaseAddress), 0x100)
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

//  Write Buffer From I2C Device
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
    Acquire(ECMX, 0xFFFF)
    if(\_SB.AMTX(\_SB.I2C6.MXID,EC_I2C_DEV_INST_ID)){
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
      if(WRIT(EC_SLAVE_ADDRESS,Arg1,Arg0,0)!= I2C_SUCCESS){
        Break
      }
      if(READ(EC_SLAVE_ADDRESS,Arg3,Arg2,1)!= I2C_SUCCESS){
        Break
      }
      Local0 = 1
      Break
    }

    if(Local0 == 0){
      REST()
      STOP()
      \_SB.RMTX(\_SB.I2C6.MXID,EC_I2C_DEV_INST_ID)
      Release(ECMX)
      Return(I2C_ERROR)
    }
    \_SB.RMTX(\_SB.I2C6.MXID,EC_I2C_DEV_INST_ID)
    Release(ECMX)
    //
    CreateByteField(Arg2,1,LENG)
    if(LENG != Arg3){
      Return(I2C_ERROR)
    }
    CreateByteField(Arg2,3,CSUM)
    Mid(Arg2,2,LENG-2,Local1)
    if(CSUM != (CKSB(Local1)&0xFF))
    {
      Return(I2C_ERROR)
    }
    Return(I2C_SUCCESS)
  }

  Method(EVNT, 0, Serialized) {
    Local1 = Buffer(9){0xDA,0x03,0xA8,0x00,0x55,0x00,0x00,0x00,0x00}
    Local2 = Buffer(15){}
    if(TRAS(Local1,Sizeof(Local1),Local2,15) == I2C_SUCCESS){
      CreateByteField  (Local2, 0x0A, TYPE)
      CreateDWordField  (Local2, 0x0B, DATA)
      // Remark   2089 - Object is not referenced ^  (Name [TYPE] is within a method [EVNT])
      Local0 = TYPE
      Local0 = DATA
      Local0 = ((Local0 &0x000000ff)<<24)| ((Local0&0x0000ff00)<<8) | ((Local0 &0x00ff0000)>>8) | ((Local0&0xff000000)>>24)
      NTII(Local0)
    }
  }

  Method(NTII, 1, Serialized) {

#if EC_PWRB_SUPPORT
    if(Arg0 & EC2OS_ACPI_EVENT_PWRBTN_STATE_CHANGE){
       Notify(\_SB.PWRB, 0x80)
    }
#endif

    if(Arg0 & EC2OS_ACPI_EVENT_WIFI_STATE_CHANGE){
    }

    if(Arg0 & EC2OS_ACPI_EVENT_DOCK_STATE_CHANGE){
    }

    if(Arg0 & EC2OS_ACPI_EVENT_WWAN_STATE_CHANGE){
    }
  }

  //
  // Name: WRGP [EC GPIO Write]
  // Description: Function to write into EC GPIO
  // Input: Arg0 -> GPIO number
  //        Arg1 -> GPIO value
  // Return: Nothing
  //
  Method(WRGP, 2, Serialized){
    Local1 = Buffer(11){0xDA,0x03,0x00,0x00,0x92,0x00,0x00,0x00,0x02,0x00,0x00}
    CreateByteField (Local1, 0x02, CSUM)
    CreateByteField (Local1, 0x09, GNUM)
    CreateByteField (Local1, 0x0A, GVAL)
    GNUM=Arg0
    GVAL=Arg1
    Mid(Local1,1,Sizeof(Local1)-1,Local0)
    CSUM=CKSB(Local0)&0xFF
    Local2 = Buffer(11){}
    TRAS(Local1,Sizeof(Local1),Local2,Sizeof(Local2))
  }

  //
  // Name: RDGP [EC GPIO Read]
  // Description: Function to write into EC GPIO
  // Input: Arg0 -> GPIO number
  // Return: GPIO Value
  //
  Method(RDGP, 1, Serialized){
    Local1 = Buffer(10){0xDA,0x03,0x00,0x00,0x93,0x00,0x00,0x00,0x01,0x00}
    CreateByteField (Local1, 0x02, CSUM)
    CreateByteField (Local1, 0x09, GNUM)
    GNUM=Arg0
    Mid(Local1,1,Sizeof(Local1)-1,Local0)
    CSUM=CKSB(Local0)&0xFF
    Local2 = Buffer(11){}
    TRAS(Local1,Sizeof(Local1),Local2,Sizeof(Local2))
    Sleep(20)   // Wait for EC get gpio state then we query gpio state again. this is current EC limitation, to be fix later.
    TRAS(Local1,Sizeof(Local1),Local2,Sizeof(Local2))
    CreateByteField (Local2, 0x0A, GVAL)
    Return(GVAL)
  }

  //
  // Name: SFAT [Set EC Fan to Auto Mode]
  // Description: Function to set ec fan to auto mode
  // Input: None
  // Output: None
  //
  Method(SFAT, 0, Serialized){
    Local0 = Buffer(10){0xDA,0x03,0xA9,0x00,0x52,0x00,0x00,0x00,0x01,0x01}
    Local1 = Buffer(10){}
    TRAS(Local0,Sizeof(Local0),Local1,Sizeof(Local1))
  }

  //
  // Name: SFMT [Set EC Fan to Mute Mode]
  // Description: Function to set ec fan to mute mode
  // Input: None
  // Output: None
  //
  Method(SFMT, 0, Serialized){
    Local0 = Buffer(10){0xDA,0x03,0xA8,0x00,0x52,0x00,0x00,0x00,0x01,0x02}
    Local1 = Buffer(10){}
    TRAS(Local0,Sizeof(Local0),Local1,Sizeof(Local1))
  }

  //
  // Name: SFPF [Set EC Fan to Performance Mode]
  // Description: Function to set ec fan to performance mode
  // Input: None
  // Output: None
  //
  Method(SFPF, 0, Serialized){
    Local0 = Buffer(10){0xDA,0x03,0xA6,0x00,0x52,0x00,0x00,0x00,0x01,0x04}
    Local1 = Buffer(10){}
    TRAS(Local0,Sizeof(Local0),Local1,Sizeof(Local1))
  }

  //
  // Name: SFFD [Set EC Fan PWM to Full Duty]
  // Description: Function to set ec fan pwm to full duty
  // Input: None
  // Output: None
  //
  Method(SFFD, 0, Serialized){
    SFPW(100)
  }

  //
  // Name: SFZD [Set EC Fan PWM to Zero Duty]
  // Description: Function to set ec fan pwm to zero duty
  // Input: None
  // Output: None
  //
  Method(SFZD, 0, Serialized){
    SFPW(0)
  }

  //
  // Name: GFPW [Get Fan PWM]
  // Description: Function to get fan pwm
  // Input: None
  // Output: Fan PWM
  //
  Method(GFPW, 0, Serialized){
    Local0 = Buffer(11){0xDA,0x03,0xD5,0x00,0x26,0x00,0x00,0x00,0x02,0x00,0x00}
    Local1 = Buffer(12){}
    if(TRAS(Local0,Sizeof(Local0),Local1,Sizeof(Local1)) == I2C_SUCCESS) {
      CreateByteField (Local1, 11, DUTY)
      Return(DUTY)
    }
    Return(0xFF)
  }

  //
  // Name: SFPW [Set Fan PWM]
  // Description: Function to set fan pwm
  // Input: Arg0 -> Fan PWM
  // Output: None
  //
  Method(SFPW, 1, Serialized){
    if (Arg0 > 100) {
      Return(One)
    }
    SFAT()
    Local0 = Buffer(13){0xDA,0x03,0xFF,0x00,0x25,0x00,0x00,0x00,0x04,0x00,0xFF,0x00,0x00}
    Local1 = Buffer(10){}
    CreateByteField(Local0,  2, CSUM)
    CreateByteField(Local0, 10, DUTY)
    CSUM = 0xD4 - Arg0
    DUTY = Arg0
    TRAS(Local0,Sizeof(Local0),Local1,Sizeof(Local1))
  }
}

#if EC_THERMAL_SUPPORT
Name (ECFM, EC_FAN_MODE)  // Default EC fan mode

PowerResource(ECFN, 0, 0)
{
  Method(_STA, 0, Serialized)
  {
    Local0 = \_SB.EC0.GFPW()
    if(Local0 != 0xFF && Local0 != 0){
      Return(One)
    }
    Return(Zero)
  }

  Method(_ON, 0, Serialized)
  {
    switch (\_SB.ECFM)
    {
      case (PCD_FAN_MODE_AUTO) {
        \_SB.EC0.SFAT()
      }
      case (PCD_FAN_MODE_PERF) {
        \_SB.EC0.SFPF()
      }
      case (PCD_FAN_MODE_MUTE) {
        \_SB.EC0.SFMT()
      }
      Default {
        \_SB.EC0.SFAT()
      }
    }
  }
  Method(_OFF, 0, Serialized)
  {
    \_SB.EC0.SFMT()
    \_SB.EC0.SFZD()
  }
}

Device(ECFP)
{
  Name(_HID, EISAID("PNP0C0B"))
  Name(_UID,0)
  Name(_PR0, Package(1){ECFN})
}

ThermalZone(ECTZ) {
  Name (_TZD, Package () { \_SB} ) //Thermal Zone Devices

  Method(_TMP, 0, Serialized) {
    Local2 = Buffer(9){0xDA,0x03,0xB3,0x3E,0x0C,0x00,0x00,0x00,0x00}
    Local3 = Buffer(12){}

    if(\_SB.EC0.TRAS(Local2,Sizeof(Local2),Local3,Sizeof(Local3)) == I2C_SUCCESS){
      CreateByteField (Local3, 0x0A, TMPI)  //Integral part of temperature
      CreateByteField (Local3, 0x0B, TMPF)  //Temperature fractional part, accuracy 0.01
      TMPI = ToInteger(TMPI)
      TMPF = ToInteger(TMPF)
      //To degrees Kelvin
      Multiply(TMPI,10,Local0)
      Divide(TMPF, 10, , Local1)
      Add(Local0,Local1,Local0)
      Add(Local0,2732,Local0)
      Return(Local0)
    }
    Return(Zero)
  }
  Method(_SCP, 1, Serialized) {} //Set Cooling Policy

  Method(_TZP) { Return(300) } //Thermal Zone Polling, 30S
}
#endif

#if EC_PWRB_SUPPORT
Device(PWRB)
{
  Name(_HID,EISAID("PNP0C0C"))
  Method(_STA)
  {
    Return(0x0F)
  }
}
#endif
