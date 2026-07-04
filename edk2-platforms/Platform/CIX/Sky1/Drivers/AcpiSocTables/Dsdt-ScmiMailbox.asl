/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#include <Include/AcpiScmi.h>

#define MAILBOX_SCMI_BEGIN                  \
      Acquire(MBXM, 0xFFFF)                 \
      CERR = 0                              \
      If(CFRE==0)                           \
      {                                     \
        Local0 = 400                        \
        While( Local0 >0 ){                 \
          if(CFRE == 1) {                   \
            Break                           \
          }                                 \
          Sleep(1)                          \
          Local0--                          \
        }                                   \
        if(Local0 == 0){                    \
          Release(MBXM)                     \
          Return(Buffer(4){ACPI_SCMI_BUSY}) \
        }                                   \
      }                                     \
      SIGN = 0x50434303                     \
      FLAG = 0x00                           \

#define MAILBOX_SCMI_PROCESS                \
    Name(RESP, Buffer(96) {})               \
    MSGP = BUFF                             \
    CFRE = 0                                \
    BEEL = 1                                \
    Local0 = 400                            \
    While( Local0 >0 ){                     \
        if(CFRE == 1) {                     \
            Break                           \
        }                                   \
        Sleep(1)                            \
        Local0--                            \
    }                                       \
    if(Local0 == 0){                        \
        Debug = "ASL Debug: SCMI Timeout"   \
        Release(MBXM)                       \
        Return(Buffer(4){ACPI_SCMI_TIMEOUT})\
    }                                       \
    RESP = MSGP                             \
    Release(MBXM)                           \

#define CIX_MAILBOX_SCMI_BEGIN              \
      Acquire(MBXM, 0xFFFF)                 \
      CERR = 0                              \
      If(CFRE==0)                           \
      {                                     \
        Local0 = 400                        \
        While( Local0 >0 ){                 \
          if(CFRE == 1) {                   \
            Break                           \
          }                                 \
          Sleep(1)                          \
          Local0--                          \
        }                                   \
        if(Local0 == 0){                    \
          Release(MBXM)                     \
          Return(Buffer(4){ACPI_SCMI_BUSY}) \
        }                                   \
      }                                     \
      Local1 = MSGA                         \
      MSGA = CIX_SCMI_MESSAGE_ATTRIBUTES    \

#define CIX_MAILBOX_SCMI_PROCESS            \
    Name(RESP, Buffer(96) {})               \
    MSGP = BUFF                             \
    CFRE = 0                                \
    BEEL = 1                                \
    Local0 = 400                            \
    While( Local0 >0 ){                     \
        if(CFRE == 1) {                     \
            Break                           \
        }                                   \
        Sleep(1)                            \
        Local0--                            \
    }                                       \
    if(Local0 == 0){                        \
        Debug = "ASL Debug: SCMI Timeout"   \
        Release(MBXM)                       \
        Return(Buffer(4){ACPI_SCMI_TIMEOUT})\
    }                                       \
    RESP = MSGP                             \
    MSGA = Local1                           \
    Release(MBXM)                           \

Mutex(MBXM,0)

Device (SHM0) {
  Name (_HID, "CIXHA004")
  Name (_UID, 0x0)
  Name (_STA, 0xB)
  Name (_CCA, 0)

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x06590000, 0x80)
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
        Package () {"compatible", "arm,scmi-shmem"},
        Package () { "reg-io-width", 4 },
    }
  })
}

Device (SHM1) {
  Name (_HID, "CIXHA005")
  Name (_UID, 0x0)
  Name (_STA, 0xB)
  Name (_CCA, 0)

  Name (_CRS, ResourceTemplate () {
    Memory32Fixed (ReadWrite, 0x065a0000, 0x80)
  })

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
        Package () {"compatible", "arm,scmi-shmem"},
        Package () { "reg-io-width", 4 },
    }
  })
}

Device (SCMI) {
  Name (_HID, "CIXHA006")
  Name (_UID, 0x0)
  Name (_STA, 0xF)

  Name (_DSD, Package () {
    ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
    Package () {
        Package () {"mboxes", Package () {\_SB.MBX6, 8,
                                          \_SB.MBX7, 8,
        }},
        Package () {"shmem", Package () {\_SB.SHM0,
                                         \_SB.SHM1,
        }},
    },
  })
  Device (DVFS) {
    Name (_HID, "CIXHA008")
    Name (_UID, 0x0)
    Name (_STA, 0xB)

    Name (_DSD, Package () {
      ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
          Package () { "reg", 0x13 },
      }
    })
  }
  Device (CLKS) {
    Name (_HID, "CIXHA009")
    Name (_UID, 0x0)
    Name (_STA, 0xB)

    Name (_DSD, Package () {
      ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
          Package () { "reg", 0x14 },
      }
    })
  }
}

Device(PMMX){
  Name(_HID,"CIXHA000")

  OperationRegion(MBXO,SystemMemory,0x06590000,0xA0)
  Field (MBXO, DWordAcc, NoLock, Preserve) {
    Offset (0x04),
    CFRE, 1,
    CERR, 1,
    Offset (0x0C),
    SIGN, 32,
    FLAG, 32,
    LENG, 32,
    MSID, 8,
    MSTP, 2,
    PRID, 8,
    TOKN, 10,
    ,4,
    Offset (0x1C),
    MSGP, 768,
    Offset (0x80),
    BEEL, 1,
  }
  Field (MBXO, DWordAcc, NoLock, Preserve) {
    MSGA, 32,
    Offset (0x18),
    MHED, 32,
  }

  Method(_STA)
  {
    Return(0x03)
  }

  Name(BUFF, Buffer(96) {})
  CreateDWordField(BUFF, 0x00, DAT0)
  CreateDWordField(BUFF, 0x04, DAT1)
  CreateDWordField(BUFF, 0x08, DAT2)
  CreateDWordField(BUFF, 0x0c, DAT3)

  //Set power state,Arg0=power domain id, Arg1=Flag
  Method(PRSS,2,Serialized){
    MAILBOX_SCMI_BEGIN
    //Message Payload
    DAT0 = 0x0
    DAT1 = Arg0
    DAT2 = Arg1
    //Length
    LENG = 0x10
    //Message Header
    MSID = SCMI_PD_POWER_STATE_SET
    PRID = SCMI_PROTOCOL_ID_POWER_DOMAIN
    //Process Request
    MAILBOX_SCMI_PROCESS
    //Process response
    Return(RESP)
  }

  //Get power domain state,Arg0=power domain id
  Method(PRSG,1,Serialized){
    MAILBOX_SCMI_BEGIN
    //Message Payload
    LENG = 0x08
    DAT0 = Arg0
    MSID = SCMI_PD_POWER_STATE_GET
    PRID = SCMI_PROTOCOL_ID_POWER_DOMAIN
    //Process Requess
    MAILBOX_SCMI_PROCESS
    //Process response
    Return(RESP)
  }

  //Get performance level,Arg0=domain id, Arg1=level index
  Method(PEFG,2,Serialized){
    MAILBOX_SCMI_BEGIN
    //Message Payload
    LENG = 0x0c
    DAT0 = Arg0
    DAT1 = Arg1
    MSID = SCMI_PERF_DESCRIBE_LEVELS
    PRID = SCMI_PROTOCOL_ID_PERF
    //Process Requess
    MAILBOX_SCMI_PROCESS
    //Process response
    Return(RESP)
  }

  //Get Clock Rate, Arg0=ClockId
  Method(CLKG,1,Serialized){
    MAILBOX_SCMI_BEGIN
    //Message Payload
    DAT0 = Arg0
    //Length
    LENG = 0x8
    //Message Header
    MSID = SCMI_CLOCK_RATE_GET
    PRID = SCMI_PROTOCOL_ID_CLOCK
    //Process Requess
    MAILBOX_SCMI_PROCESS
    //Process response
    Return(RESP)
  }

  //Set Clock Rate, Arg0=ClockId, Arg1=Clock Low Word, Arg2=Clock High Word.
  Method(CLKS,3,Serialized){
    MAILBOX_SCMI_BEGIN
    //Message Payload
    DAT0 = 0
    DAT1 = Arg0
    DAT2 = Arg1
    DAT3 = Arg2
    //Length
    LENG = 0x14
    //Message Header
    MSID = SCMI_CLOCK_RATE_SET
    PRID = SCMI_PROTOCOL_ID_CLOCK
    //Process Request
    MAILBOX_SCMI_PROCESS
    //Process response
    Return(RESP)
  }

  //Clock Describe Rates, Arg0=ClockId, Arg1=Rate Index
  Method(CLKD,2,Serialized){
    MAILBOX_SCMI_BEGIN
    //Message Payload
    DAT0 = Arg0
    DAT1 = Arg1
    //Length
    LENG = 0x0c
    //Message Header
    MSID = SCMI_CLOCK_DESCRIBE_RATES
    PRID = SCMI_PROTOCOL_ID_CLOCK
    //Process Request
    MAILBOX_SCMI_PROCESS
    //Process response
    Return(RESP)
  }


  //Clock Device configure, Arg0=ClockId, Arg1=attributes
  Method(CLKC,2,Serialized){
    MAILBOX_SCMI_BEGIN
    //Message Payload
    Store(Arg0, DAT0)
    Store(Arg1, DAT1)
    //Length
    Store(0x0C, LENG)
    //Message Header
    Store(SCMI_CLOCK_CONFIG_SET, MSID)
    Store(SCMI_PROTOCOL_ID_CLOCK, PRID)
    //Process Request
    MAILBOX_SCMI_PROCESS
    //Process response
    Return(RESP)
  }

  /* System Control and Management Interface (SCMI) Sensor Protocol*/

  /**
   * SENG - Read scalar sensor value
   * @Arg0: sensor ID
   * @Arg1: Asynchronous sensor read support
   *
   * Return: the buffer received from PM
   */
  Method(SENG,2,Serialized){
    MAILBOX_SCMI_BEGIN
    //Message Payload
    Store(Arg0, DAT0)
    Store(Arg1, DAT1)
    //Length
    Store(0x0C, LENG)
    //Message Header
    Store(SCMI_SENSOR_READING_GET, MSID)
    Store(SCMI_PROTOCOL_ID_SENSOR, PRID)
    //Process Request
    MAILBOX_SCMI_PROCESS
    //Process response
    Return(RESP)
  }

  //Set PM fan mode, Arg0=fan mode
  Method(SFMD,1,Serialized){
    CIX_MAILBOX_SCMI_BEGIN
    //Message Payload
    DAT0 = Arg0
    //Length
    LENG = 0x8
    //Message Header
    MHED = SCMI_MESSAGE_HEADER_FAN_MODE_SET
    //Process Requess
    CIX_MAILBOX_SCMI_PROCESS
    //Process response
    Return(RESP)
  }
}

/* Shared mutex between OS and ASL env via _DLM obj to get/release mutex. */
Device (MTXD) {
  Name (_HID, "CIXHA007")
  Name (_UID, 0x4)
  Name (_STA, 0xB)

  Name (_DLM, Package (1)
  {
    Package (1)
    {
      \_SB.MBXM,
    }
  })
}
