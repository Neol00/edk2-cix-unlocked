/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


Device (DTPM) {
  Name (_HID, "MSFT0101")
  Name (_CID, "MSFT0101")
  Name (_UID, 1)
  Name (_STA, 0xF)

  Method (_CRS, 0x0, NotSerialized) {
    Name (RBUF, ResourceTemplate () {
      SpiSerialBus (0x0000, PolarityLow, FourWireMode, 0x08, ControllerInitiated, 0x0007A120, ClockPolarityLow, ClockPhaseFirst, "\\_SB.SPI0", 0x00, ResourceConsumer, , )
    })
    Return (RBUF)
  }


  // PPI request CRB
  OperationRegion (TPMC, SystemMemory, FixedPcdGet32(PcdTPMPPIMemoryBase), 0x0C)
  Field (TPMC, DWordAcc, NoLock, Preserve) {
      PPIO, 32, // current PPI request
      PPIR, 32, // last PPI request
      PPIS, 32, // last PPI request status
  }

  // Create objects to hold return values
  Name (PKG2, Package (2) { Zero, Zero })
  Name (PKG3, Package (3) { Zero, Zero, Zero })

  Function(_DSM,{IntObj,BuffObj},{BuffObj, IntObj, IntObj, PkgObj})
  {
    If(LEqual(Arg0, ToUUID ("3dddfaa6-361b-4eb4-a424-8d10089d1653")))
    {
      Switch (ToInteger (Arg2)) {
        //
        // Standard DSM query
        //
        Case (0) {
          Return (Buffer () { 0xFF, 0x01 })
        }

        //
        // Get Physical Presence Interface Version - support 1.3
        //
        Case (1) {
          Return ("1.3")
        }

        //
        // Submit TPM operation to pre-OS (Deprecated)
        //
        Case (2) {
          Return (One) // Not supported
        }

        //
        // Get pending TPM operation requested by OS
        //
        Case (3) {
          PKG2[Zero] = Zero   // Success
          PKG2[One] = PPIO    // current PPI request
          Return (PKG2)
        }

        //
        // Platform-specific action to transition to Pre-OS env
        //
        Case (4) {
          Return (0x2) // Reboot
        }

        //
        // TPM operation Response to OS
        //
        Case (5) {
          PKG3[Zero] = Zero   // Success
          PKG3[One] = PPIR    // last PPI request
          PKG3[2] = PPIS      // last PPI request status
          Return (PKG3)
        }

        //
        // Preferred language code (Deprecated)
        //
        Case (6) {
          Return (0x3) // Not implemented
        }

        //
        // Submit TPM operation to pre-OS env 2
        //
        Case (7) {
          Local0 = DerefOf (Arg3 [Zero])
          // Write current PPI request and then to the doorbell
          Store (Local0, PPIO)
          //Store (0x6a000000, DBB0) // MsgType: 6, Handler: 0xa (TPM-PPI)
          Return (Zero)
        }

        //
        // Get User confirmation status for op
        //
        Case (8) {
          Return (0x4) // Allowed and physically present user not required
        }
      }
    }
    Else {
      // If not one of the function identifiers we recognize, then return a buffer
      // with 0x44 indicating no functions supported.
      return (Buffer(){0})
    }

  }

}
