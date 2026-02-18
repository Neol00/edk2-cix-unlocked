/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device(WMIP){
    // PNP0C14 is PNP ID assigned to WMI mapper
    Name(_HID, EISAID("PNP0C14"))
    Name(_UID, "CIX")

    //
    // Name: UTOA
    // Description: Convert unicode string to ascii string
    // Input:
    //       Arg0: Length of unicode string
    //       Arg1: buffer which contain unicode string
    // Return: Ascii string
    //
    Method(UTOA, 2, Serialized){
        Local0=Arg0/2
        Local0++
        Name(BUF0, Buffer(Local0){})
        Local1=0
        While(Local1 < Arg0){
            Local2 = DeRefOf(Index(Arg1, Local1))
            Store(Local2,Index(BUF0, Local1/2))
            Local1=Local1+2
        }
        Store(0,Index(BUF0, Local1/2))
        Return(BUF0)
    }

    Method(CI01,1) {
        UDBG(Printf("CI01 %o", Arg0))
        return (Zero)
    }

    Method(CI02,1) {
        UDBG(Printf("CI02 %o", Arg0))
        return (Zero)
    }

    Method(CIXL,1) {
        Local0 = DeRefOf(Index(Arg0, 0))
        Mid(Arg0,2,Local0,Local1)
        Local2=UTOA(Local0,Local1)
        UDBG(ToString(Local2))
        return (Zero)
    }
    //
    // Description of data, events and methods supported by this ASL device.
    //
    Name(_WDG, Buffer(){
        // Cix Method
        // guid {0x842b3d2a, 0x1a1f, 0x4d71, 0x98, 0x06, 0x74, 0x84, 0xf9, 0x09, 0x9b, 0xc0}
        0x2a, 0x3d, 0x2b, 0x84, 0x1f, 0x1a, 0x71, 0x4d, 0x98, 0x06, 0x74, 0x84, 0xf9, 0x09, 0x9b, 0xc0,
        67, 73,             // Object Id (CI)
        1,                  // Instance Count
        0x02,               // Flags (WMIACPI_REGFLAG_METHOD)

        // This GUID for returning the MOF data
        0x21, 0x12, 0x90, 0x05, 0x66, 0xd5, 0xd1, 0x11, 0xb2, 0xf0, 0x00, 0xa0, 0xc9, 0x06, 0x29, 0x10,
        0x4D, 0x4F,    // Object Id (MO)
        1,             // Instance Count
        0x00,          // Flags
    })


    //---------------------------------------------------------------------------
    //
    // Name: WMCI
    //
    // Description: WMI ACPI registed method
    //
    // Input:
    //        Arg0 has the instance being queried
    //        Arg1 has the method id (Uppercase)
    //        Arg2 has the data passed
    // Output:
    //
    // Notes:
    //
    //---------------------------------------------------------------------------
    Method(WMCI, 3, NotSerialized)
    {
        //Input Definition
        CreateDWordField (Arg2, 0x00, IIA0) // Input: IntegerArgument
        if(Lequal(Arg0,0))
        {
            if (Lequal(Arg1,1))
            { //Method ID 1
                Return(CI01(IIA0))
            } elseif (Lequal(Arg1,2))
            {
                Return(CI02(IIA0))
            } elseif (Lequal(Arg1,0x8000))
            {
                Return(CIXL(Arg2))
            } else {
                return(One)
            }
        }
        Return(One)
    }


    //---------------------------------------------------------------------------
    //
    // Name: WQMO
    //
    // Description: MOF data
    //
    // Input:
    //
    // Output:
    //
    // Notes:
    //
    //---------------------------------------------------------------------------
    Name(WQMO, buffer() {
        Include("CixWmi.inc")
    })
}