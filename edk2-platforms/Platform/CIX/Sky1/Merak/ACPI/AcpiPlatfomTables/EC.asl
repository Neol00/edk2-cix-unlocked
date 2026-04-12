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

#define EC_BATT_FLAG_AC_PRESENT      0x01
#define EC_BATT_FLAG_BATT_PRESENT    0x02
#define EC_BATT_FLAG_DISCHARGING     0x04
#define EC_BATT_FLAG_CHARGING        0x08
#define EC_BATT_FLAG_LEVEL_CRITICAL  0x10
#define EC_BATT_FLAG_INVALID_DATA    0x20

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

#define EC_BATTERY_SUPPORT 0
#define EC_THERMAL_SUPPORT 0
#define EC_LID_SUPPORT     0
#define EC_PWRB_SUPPORT    1

External (\_SB.GPI4, DeviceObj)
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
      Offset(0x00), // Control Register
      CR,32,
      Offset(0x04), // Status Register
      SR,32,
      Offset(0x08), // Address Register
      AR,32,
      Offset(0x0C), // Data Register
      DR,32,
      Offset(0x10), // Interrupt Status Register
      ISR,32,
      Offset(0x14), // Transfer Size Register
      TSR,32,
      Offset(0x18), // Slave Monitor Pause Register
      SMPR,32,
      Offset(0x1C), // Time Out Register
      TOR,32,
      Offset(0x20), // Interrupt Mask Register
      IMR,32,
      Offset(0x24), // Interrupt Enable Register
      IER,32,
      Offset(0x28), // Interrupt Disable Register
      IDR,32,
      Offset(0x2C), // Glitch Filter Control Register
      GFCR,32,
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
    Name(BUF0, Buffer(9){0xDA,0x03,0xA8,0x00,0x55,0x00,0x00,0x00,0x00})
    Name(BUF1, Buffer(15){})
    if(TRAS(BUF0,Sizeof(BUF0),BUF1,15) == I2C_SUCCESS){
      CreateByteField  (BUF1, 0x0A, TYPE)
      CreateDWordField  (BUF1, 0x0B, DATA)
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

#if EC_BATTERY_SUPPORT
    if(Arg0 & (EC2OS_ACPI_EVENT_AC_PLUG_IN | EC2OS_ACPI_EVENT_AC_PLUG_OUT)){
      Notify(\_SB.AC, 0x80)
      Notify(\_SB.BAT0, 0x80)
    }

    if(Arg0 & (EC2OS_ACPI_EVENT_BAT_FULL_CHARGE)){
      Notify(\_SB.AC, 0x80)
      Notify(\_SB.BAT0, 0x81)
    }
#endif

#if EC_LID_SUPPORT
    if(Arg0 & EC2OS_ACPI_EVENT_LID_STATE_CHANGE){
      Notify(\_SB.LID, 0x80)
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
    Name(BUF0, Buffer(11){0xDA,0x03,0x00,0x00,0x92,0x00,0x00,0x00,0x02,0x00,0x00})
    CreateByteField (BUF0, 0x02, CSUM)
    CreateByteField (BUF0, 0x09, GNUM)
    CreateByteField (BUF0, 0x0A, GVAL)
    GNUM=Arg0
    GVAL=Arg1
    Mid(BUF0,1,Sizeof(BUF0)-1,Local0)
    CSUM=CKSB(Local0)&0xFF
    Name(BUF1, Buffer(11){})
    TRAS(BUF0,Sizeof(BUF0),BUF1,Sizeof(BUF1))
  }

  //
  // Name: RDGP [EC GPIO Read]
  // Description: Function to write into EC GPIO
  // Input: Arg0 -> GPIO number
  // Return: GPIO Value
  //
  Method(RDGP, 1, Serialized){
    Name(BUF0, Buffer(10){0xDA,0x03,0x00,0x00,0x93,0x00,0x00,0x00,0x01,0x00})
    CreateByteField (BUF0, 0x02, CSUM)
    CreateByteField (BUF0, 0x09, GNUM)
    GNUM=Arg0
    Mid(BUF0,1,Sizeof(BUF0)-1,Local0)
    CSUM=CKSB(Local0)&0xFF
    Name(BUF1, Buffer(11){})
    TRAS(BUF0,Sizeof(BUF0),BUF1,Sizeof(BUF1))
    Sleep(20)   // Wait for EC get gpio state then we query gpio state again. this is current EC limitation, to be fix later.
    TRAS(BUF0,Sizeof(BUF0),BUF1,Sizeof(BUF1))
    CreateByteField (BUF1, 0x0A, GVAL)
    Return(GVAL)
  }

  //
  // Name: GKBL [Get EC Keyboard Backlight]
  // Description: Function to get ec keyboard backlight value
  // Return: Package (KBLP, KBLE)
  // KBLP -> EC Keyboard backlight percent
  // KBLE -> EC Keyboard backlight enable stauts
  //
  Method(GKBL, 0, Serialized){
    Name(BUF0, Buffer(9){0xDA,0x03,0x00,0x00,0x22,0x00,0x00,0x00,0x00})
    CreateByteField (BUF0, 0x02, CSUM)
    Mid(BUF0,1,Sizeof(BUF0)-1,Local0)
    CSUM=CKSB(Local0)&0xFF
    Name(BUF1, Buffer(12){})
    TRAS(BUF0,Sizeof(BUF0),BUF1,Sizeof(BUF1))
    CreateByteField (BUF1, 0x0A, KBLP)
    CreateByteField (BUF1, 0x0B, KBLE)
    Return(Package() {KBLP,KBLE})
  }

  //
  // Name: SKBL [Set EC Keyboard Backlight]
  // Description: Function to set ec keyboard backlight
  // Input: Arg0 -> EC Keyboard backlight percent
  // Return: Nothing
  //
  Method(SKBL, 1, Serialized){
    Name(BUF0, Buffer(10){0xDA,0x03,0x00,0x00,0x23,0x00,0x00,0x00,0x01,0x00})
    CreateByteField (BUF0, 0x02, CSUM)
    CreateByteField (BUF0, 0x09, KBLP)
    KBLP=Arg0
    Mid(BUF0,1,Sizeof(BUF0)-1,Local0)
    CSUM=CKSB(Local0)&0xFF
    Name(BUF1, Buffer(11){})
    TRAS(BUF0,Sizeof(BUF0),BUF1,Sizeof(BUF1))
  }

  //
  // Name: GLSS [Get EC Light Sensor Value]
  // Description: Function to get ec light sensor value
  // Input: Nothing
  // Return: Package (LSST, LSSV)
  // LSST -> Light sensor value type
  // LSSV -> Light sensor value
  //
  Method(GLSS, 0, Serialized){
    Name(BUF0, Buffer(9){0xDA,0x03,0x00,0x00,0x32,0x00,0x00,0x00,0x00})
    CreateByteField (BUF0, 0x02, CSUM)
    Mid(BUF0,1,Sizeof(BUF0)-1,Local0)
    CSUM=CKSB(Local0)&0xFF
    Name(BUF1, Buffer(15){})
    TRAS(BUF0,Sizeof(BUF0),BUF1,Sizeof(BUF1))
    CreateByteField (BUF1, 0x0A, LSST)
    CreateDWordField (BUF1, 0x0B, LSSV)
    Return(Package() {LSST,LSSV})
  }

  //
  // Name: SFAT [Set EC Fan to Auto Mode]
  // Description: Function to set ec fan to auto mode
  // Input: None
  // Output: None
  //
  Method(SFAT, 0, Serialized){
    Name(BUF0, Buffer(10){0xDA,0x03,0xA9,0x00,0x52,0x00,0x00,0x00,0x01,0x01})
    Name(BUF1, Buffer(10){})
    TRAS(BUF0,Sizeof(BUF0),BUF1,Sizeof(BUF1))
  }

  //
  // Name: SFMT [Set EC Fan to Mute Mode]
  // Description: Function to set ec fan to mute mode
  // Input: None
  // Output: None
  //
  Method(SFMT, 0, Serialized){
    Name(BUF0, Buffer(10){0xDA,0x03,0xA8,0x00,0x52,0x00,0x00,0x00,0x01,0x02})
    Name(BUF1, Buffer(10){})
    TRAS(BUF0,Sizeof(BUF0),BUF1,Sizeof(BUF1))
  }

  //
  // Name: SFMT [Set EC Fan to Manual Mode]
  // Description: Function to set ec fan to manual mode
  // Input: None
  // Output: None
  //
  Method(SFMN, 0, Serialized){
    Name(BUF0, Buffer(10){0xDA,0x03,0xAA,0x00,0x52,0x00,0x00,0x00,0x01,0x00})
    Name(BUF1, Buffer(10){})
    TRAS(BUF0,Sizeof(BUF0),BUF1,Sizeof(BUF1))
  }

  //
  // Name: SFPF [Set EC Fan to Performance Mode]
  // Description: Function to set ec fan to performance mode
  // Input: None
  // Output: None
  //
  Method(SFPF, 0, Serialized){
    Name(BUF0, Buffer(10){0xDA,0x03,0xA6,0x00,0x52,0x00,0x00,0x00,0x01,0x04})
    Name(BUF1, Buffer(10){})
    TRAS(BUF0,Sizeof(BUF0),BUF1,Sizeof(BUF1))
  }

  //
  // Name: SFPW [Set the PWM duty of EC Fan]
  // Description: Function to set the PWM duty of EC Fan
  // Input:
  //         Arg0 -> Duty cycle, EC_PWM_MAX_DUTY = 100%
  //         Arg1 -> ec_pwm_type
  //         Arg2 -> Type-specific index, or 0 if unique
  // Output: Status(No zero may have some error in it)
  //
  Method(SFPW, 3, Serialized)
  {
    if(Arg0 > 100){
      Return(1);
    }

    \_SB.EC0.SFMN()

    Name(BUF0, Buffer(13){0xDA,0x03,0x00,0x00,0x25,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00})
    Name(BUF1, Buffer(10){})
    CreateByteField (BUF0, 0x02, CSUM)
    CreateByteField (BUF0, 0x0A, DUTY)
    CreateByteField (BUF0, 0x0B, PWMT)
    CreateByteField (BUF0, 0x0C, INDX)
    DUTY=Arg0
    PWMT=Arg1
    INDX=Arg2
    Mid(BUF0,1,Sizeof(BUF0)-1,Local0)
    CSUM=CKSB(Local0)&0xFF

    if(\_SB.EC0.TRAS(BUF0,Sizeof(BUF0),BUF1,Sizeof(BUF1)) != I2C_SUCCESS){
      Return(2);
    }
    Return(0);
  }

  //
  // Name: GFPW [Get the PWM duty of EC Fan]
  // Description: Function to get the PWM duty of EC Fan
  // Input:
  //         Arg0 -> ec_pwm_type
  //         Arg1 -> Type-specific index, or 0 if unique
  // Output: Duty cycle, EC_PWM_MAX_DUTY = 100% , 0xFFFFFFFF means error
  //
  Method(GFPW, 2, Serialized)
  {
    Name(BUF0, Buffer(11){0xDA,0x03,0xD5,0x00,0x26,0x00,0x00,0x00,0x02,0x00,0x00})
    CreateByteField (BUF0, 0x02, CSUM)
    CreateByteField (BUF0, 0x09, PWMT)
    CreateByteField (BUF0, 0x0A, INDX)
    PWMT=Arg0
    INDX=Arg1
    Mid(BUF0,1,Sizeof(BUF0)-1,Local0)
    CSUM=CKSB(Local0)&0xFF

    Name(BUF1, Buffer(12){})
    CreateByteField (BUF1, 0x0B, DUTY)

    if(\_SB.EC0.TRAS(BUF0,Sizeof(BUF0),BUF1,Sizeof(BUF1)) == I2C_SUCCESS){
        Return(DUTY)
    }
    Return(0xFFFFFFFF);
  }
}

Scope (\_SB.GPI4)
{
  Name (_AEI, ResourceTemplate() {
    GpioInt (Level, ActiveLow, Exclusive, PullUp, 0, "\\_SB.GPI4") {6}
  })

  Method (_L06) {
    \_SB.EC0.EVNT()
  }
}

#if EC_BATTERY_SUPPORT

Device(BAT0) {
  Name(_HID, EISAID("PNP0C0A"))
  Name(_UID, 0)
  Method(_PCL,0) {
      Return (
        Package() { _SB }
      )
  }
  Method (_STA,0) {
    // Orion O6 is a desktop SBC with no battery and no real EC.
    // Returning 0x00 hides BAT0 from Linux and prevents EC I2C transactions
    // that loop 1,000,000 times against non-existent hardware, causing
    // AE_AML_LOOP_TIMEOUT errors in \_SB.EC0.WRIT and \_SB.EC0.TRAS.
    Return (0x00)
  }

  Name(BIXP, Package() {
          0x01, // Integer Revision
    0x00000001, // Power Unit = mWh. 0x0 indicates units are mW/mWh
    0xFFFFFFFF, // Unknown Design Capacity.
    0xFFFFFFFF, // Unknown Last Full Charge.
    0x00000001, // Secondary Battery Technology.
    0xFFFFFFFF, // Unknown Design Voltage.
    0x00000000, // Warning Level.
    0x00000000, // Low Level.
           100, // Cycle Count, MUST not be 0xFFFFFFFF
         95000, // Measurement Accuracy 95%
    0x00000000, // Max sampling time, MSFT No specific requirement.
    0x00000000, // Min sampling time, MSFT No specific requirement.
    0x00000000, // Max averaging interval, MSFT No specific requirement.
    0x00000000, // Min averaging interval, MSFT No specific requirement.
    0x00000100, // Battery capacity granularity 1
    0x00000040, // Battery capacity granularity 2
    "BASE-BAT", // Model number
    "123456789",//Serial Number
    "LiP",      // Battery type
    "Simplo",   // OEM information
     0x00000001 // Battery Swapping Capability = Cold swappable
  })
  Method(_BIX,0, Serialized) {
    Name(BUF0, Buffer(10){ 0xDA,0x03,0xF6,0x06,0x00,0x00,0x00,0x00,0x01,0x00 })
    Name(BUF1, Buffer(50){})
    Name(BUF2, Buffer(10){0xDA,0x03,0xF5,0x06,0x01,0x00,0x00,0x00,0x01,0x00})
    Name(BUF3, Buffer(24){})

    if(\_SB.EC0.TRAS(BUF0,Sizeof(BUF0),BUF1,50) == I2C_SUCCESS){
      CreateWordField  (BUF1, 0x0A, CAPB)
      CreateWordField  (BUF1, 0x0C, DSNV)
      CreateDWordField (BUF1, 0x2E, CYCC)
      Local0 = CAPB
      CAPB = ((Local0 &0xff)<<8)| ((Local0&0xff00)>>8)
      Divide(CAPB, 10, , Local0)
      Store(Local0, Index(BIXP, 6))      // warning capacity = 10% of design capacity
      Divide(CAPB, 20, , Local0)
      Store(Local0, Index(BIXP, 7))      // low capacity = 5% of design capacity
      Local0 = DSNV
      DSNV = ((Local0 &0xff)<<8)| ((Local0&0xff00)>>8)
      Local0 = CYCC
      CYCC = ((Local0 &0x000000ff)<<24)| ((Local0&0x0000ff00)<<8) | ((Local0 &0x00ff0000)>>8) | ((Local0&0xff000000)>>24)
      Store(CAPB, Index(BIXP,2))
      Store(DSNV, Index(BIXP,5))
      Store(CYCC, Index(BIXP,8))
      Mid(BUF1,0x16,8,Index(BIXP,16))
      Mid(BUF1,0x1E,8,Index(BIXP,17))
      Mid(BUF1,0x26,8,Index(BIXP,18))
      Mid(BUF1,0x0E,8,Index(BIXP,19))
    }

    if(\_SB.EC0.TRAS(BUF2,Sizeof(BUF2),BUF3,24) == I2C_SUCCESS){
      CreateWordField (BUF3, 0x10, FCAP)
      Local0 = FCAP
      FCAP = ((Local0 &0xff)<<8)| ((Local0&0xff00)>>8)
      Store(FCAP, Index(BIXP,3))
    }
    Return (BIXP)
  }

  Name(BSTP,Package() {
             0, // Battery State.
    0xFFFFFFFF, // Battery Present Rate. (in mWh)
    0xFFFFFFFF, // Battery Remaining Capacity. (in mWh)
    0xFFFFFFFF  // Battery Present Voltage. (in mV)
  })
  Method(_BST,0, Serialized) {
    Name(BUF0, Buffer(10){0xDA,0x03,0xF5,0x06,0x01,0x00,0x00,0x00,0x01,0x00})
    Name(BUF1, Buffer(24){})

    if(\_SB.EC0.TRAS(BUF0,Sizeof(BUF0),BUF1,24) == I2C_SUCCESS){
      CreateWordField (BUF1, 0x0A, ACUV)
      CreateWordField (BUF1, 0x0C, ACUC)
      CreateWordField (BUF1, 0x0E, REMC)
      CreateWordField (BUF1, 0x12, FLAG)

      Local0 = REMC
      REMC = ((Local0 &0xff)<<8)| ((Local0&0xff00)>>8)
      Local0 = ACUV
      ACUV = ((Local0 &0xff)<<8)| ((Local0&0xff00)>>8)
      Local0 = FLAG
      FLAG = ((Local0 &0xff)<<8)| ((Local0&0xff00)>>8)
      Local0 = ACUC
      ACUC = ((Local0 &0xff)<<8)| ((Local0&0xff00)>>8)

      Store(ACUC, Index(BSTP,1))
      Store(REMC, Index(BSTP,2))
      Store(ACUV, Index(BSTP,3))

      Local1 = DeRefOf(Index(BIXP,3))
      Local2 = REMC*100/Local1
      if(FLAG & EC_BATT_FLAG_CHARGING){
        Store(0x02, Index(BSTP,0))
      }elseif(FLAG & EC_BATT_FLAG_DISCHARGING){
        if (Local2 <= 3){ // When the battery level is less than 3%, a battery critical event is triggered
          Store(0x05, Index(BSTP,0))
        }else{
          Store(0x01, Index(BSTP,0))
        }
      }
    }
    Return(BSTP)
  }
}
#endif

#if EC_THERMAL_SUPPORT
PowerResource(ECFN, 0, 0)
{
  Method(_STA, 0, Serialized)
  {
    Name(BUF0, Buffer(11){0xDA,0x03,0xD5,0x00,0x26,0x00,0x00,0x00,0x02,0x00,0x00})
    Name(BUF1, Buffer(12){})

    if(\_SB.EC0.TRAS(BUF0,Sizeof(BUF0),BUF1,Sizeof(BUF1)) == I2C_SUCCESS){
      CreateWordField (BUF1, 0x0A, DUTY)
      if(DUTY != 0){
        Return(One)
      }
    }
    Return(Zero)
  }

  Method(_ON, 0, Serialized)
  {
    Name(BUF0, Buffer(13){0xDA,0x03,0x70,0x00,0x25,0x00,0x00,0x00,0x04,0x00,0x64,0x00,0x00})
    Name(BUF1, Buffer(10){})

    if(\_SB.EC0.TRAS(BUF0,Sizeof(BUF0),BUF1,Sizeof(BUF1)) != I2C_SUCCESS){
      printf ("CIX Debug: FN00 on fail")
    }
  }
  Method(_OFF, 0, Serialized)
  {
    Name(BUF0, Buffer(13){0xDA,0x03,0xD4,0x00,0x25,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00})
    Name(BUF1, Buffer(10){})

    if(\_SB.EC0.TRAS(BUF0,Sizeof(BUF0),BUF1,Sizeof(BUF1)) != I2C_SUCCESS){
      printf ("CIX Debug: FN00 off fail")
    }
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
    Name(BUF0, Buffer(9){0xDA,0x03,0xB3,0x3E,0x0C,0x00,0x00,0x00,0x00})
    Name(BUF1, Buffer(12){})

    if(\_SB.EC0.TRAS(BUF0,Sizeof(BUF0),BUF1,Sizeof(BUF1)) == I2C_SUCCESS){
      CreateByteField (BUF1, 0x0A, TMPI)  //Integral part of temperature
      CreateByteField (BUF1, 0x0B, TMPF)  //Temperature fractional part, accuracy 0.01
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

#if EC_LID_SUPPORT
Device(LID)
{
  Name(_HID,EISAID("PNP0C0D"))

  Method(_STA)
  {
    Return(0x0F)
  }

  Method(_LID,0)
  {
    Name(BUF0, Buffer(9){0xDA,0x03,0x9F,0x3E,0x20,0x00,0x00,0x00,0x00})
    Name(BUF1, Buffer(11){})
    if(\_SB.EC0.TRAS(BUF0,Sizeof(BUF0),BUF1,Sizeof(BUF1)) == I2C_SUCCESS){
      CreateByteField (BUF1, 0x0A, LIDS)
      if(LEqual(LIDS, One)){
        Return(Zero)
      }else{
        Return(One)
      }
    }
    Return(One)
  }
}
#endif

#if EC_BATTERY_SUPPORT
Device (AC)
{
    Name (_HID, "ACPI0003")
    Name (_UID, 0x00)
    Name (_PCL, Package (1) { \_SB })
    Method (_STA, 0, NotSerialized)
    {
        Return (0x0F)
    }
    Method (_PSR, 0, NotSerialized)
    {
      // Orion O6 is a desktop SBC permanently connected to mains power.
      // The EC I2C path triggers AE_AML_LOOP_TIMEOUT (I2C_RETRIES_NUM loop)
      // on boards without a real EC battery controller.
      // Always return 1 (AC present) to prevent the EC timeout and to
      // correctly reflect that this board is never on battery power.
      Return (0x01)
    }
}
#endif
