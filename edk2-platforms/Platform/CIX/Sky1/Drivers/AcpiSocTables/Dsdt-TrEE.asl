/** @file
  Description: TrEE Driver
  Copyright (c) 2018, Microsoft Corporation. All rights reserved.
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php
  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/
Device (TREE) {
  Name (_HID, "CIXHA023")
  Name (_UID, 0)
  Method (_STA) {
    Return (0xF)
  }
  Method (_CRS, 0x0, NotSerialized) {
    Name (RBUF, ResourceTemplate () {
    })
    Return (RBUF)
  }
  Name(SVCS, Package()
  {
      // Data Format Version Information
      1,
      // Service 1
      Package()
      {
          // ID FTPM Service Version
          ToUUID("36deaa79-c5dd-447c-95e6-b3859589291a"),
          // Major Version
          1,
          // Minor Version
          0,
          // OS Dependencies
          Package() {},
          // Secure Resource Dependencies
          Package() {}
      },
      // Service 2
      Package()
      {
          // ID helloworld Test Service
          ToUUID("B1CC44AE-B9AF-4AAA-8BC1-54C49B24D5AD"),
          // Major Version
          1,
          // Minor Version
          0,
          // OS Dependencies
          Package() {},
          // Secure Resource Dependencies
          Package() {}
      },
      //Service 3
      // Package()
      // {
      //     // ID  UEFI Variables Service
      //     ToUUID("699AA2F1-A42E-40DF-BABE-3AAAD2BB6A47"),
      //     // Major Version
      //     1,
      //     // Minor Version
      //     0,
      //     // OS Dependencies
      //     Package() {},
      //     // Secure Resource Dependencies
      //     Package() {}
      // }
    })
  Function(_DSM,{IntObj,BuffObj},{BuffObj, IntObj, IntObj, PkgObj})
  {
    If(LEqual(Arg0,ToUUID("418E2DA4-7089-4DDB-AACA-A7E2377DBECE"))){
      switch(ToInteger(Arg2))
      {
          // Function 0: Return supported functions, based on revision
          case(0)
          {
              return (Buffer() {0x03})
          }
          // Function 1: Return list of secure service descriptions
          case(1)
          {
              Return(SVCS)
          }
          Default { return (Buffer() {0x40})}
      }
    }
    Else {
      // If not one of the function identifiers we recognize, then return a buffer
      // with 0x44 indicating no functions supported.
      return (Buffer(){0x0})
    }
  }
}