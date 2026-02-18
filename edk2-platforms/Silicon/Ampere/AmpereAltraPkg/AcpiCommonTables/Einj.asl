/** @file

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
[0004]                          Signature : "EINJ"    [Error Injection table]
[0004]                       Table Length : 00000150
[0001]                           Revision : 01
[0001]                           Checksum : 09
[0006]                             Oem ID : "Ampere"
[0008]                       Oem Table ID : "Altra   "
[0004]                       Oem Revision : 00000001
[0004]                    Asl Compiler ID : "INTL"
[0004]              Asl Compiler Revision : 20100528

[0004]            Injection Header Length : 00000030
[0001]                              Flags : 00
[0003]                           Reserved : 000000
[0004]              Injection Entry Count : 00000009

[0001]                             Action : 00 [Begin Operation]
[0001]                        Instruction : 03 [Write Register Value]
[0001]              Flags (decoded below) : 00
                   Preserve Register Bits : 0
[0001]                           Reserved : 00

[0012]                    Register Region : [Generic Address Structure]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000000088220140

[0008]                              Value : 0000000000000001
[0008]                               Mask : FFFFFFFFFFFFFFFF

[0001]                             Action : 01 [Get Trigger Table]
[0001]                        Instruction : 00 [Read Register]
[0001]              Flags (decoded below) : 00
                   Preserve Register Bits : 0
[0001]                           Reserved : 00

[0012]                    Register Region : [Generic Address Structure]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000000088220040

[0008]                              Value : 0000000000000000
[0008]                               Mask : FFFFFFFFFFFFFFFF

[0001]                             Action : 08 [Set Error Type With Address]
[0001]                        Instruction : 02 [Write Register]
[0001]              Flags (decoded below) : 01
                   Preserve Register Bits : 1
[0001]                           Reserved : 00

[0012]                    Register Region : [Generic Address Structure]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 20
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 03 [DWord Access:32]
[0008]                            Address : 0000000088221000

[0008]                              Value : 00000000
[0008]                               Mask : FFFFFFFF

[0001]                             Action : 02 [Set Error Type]
[0001]                        Instruction : 02 [Write Register]
[0001]              Flags (decoded below) : 01
                   Preserve Register Bits : 1
[0001]                           Reserved : 00

[0012]                    Register Region : [Generic Address Structure]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 20
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [DWord Access:64]
[0008]                            Address : 0000000088220080

[0008]                              Value : 0000000000000000
[0008]                               Mask : FFFFFFFFFFFFFFFF

[0001]                             Action : 03 [Get Error Type]
[0001]                        Instruction : 00 [Read Register]
[0001]              Flags (decoded below) : 00
                   Preserve Register Bits : 0
[0001]                           Reserved : 00

[0012]                    Register Region : [Generic Address Structure]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 00000000882200c0

[0008]                              Value : 0000000000000000
[0008]                               Mask : FFFFFFFFFFFFFFFF

[0001]                             Action : 04 [End Operation]
[0001]                        Instruction : 03 [Write Register Value]
[0001]              Flags (decoded below) : 01
                   Preserve Register Bits : 1
[0001]                           Reserved : 00

[0012]                    Register Region : [Generic Address Structure]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000000088220100

[0008]                              Value : 0000000000000000
[0008]                               Mask : FFFFFFFFFFFFFFFF

[0001]                             Action : 05 [Execute Operation]
[0001]                        Instruction : 03 [Write Register Value]
[0001]              Flags (decoded below) : 01
                   Preserve Register Bits : 1
[0001]                           Reserved : 00

[0012]                    Register Region : [Generic Address Structure]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 20
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 03 [DWord Access:32]
[0008]                            Address : 0000100000543010

[0008]                              Value : B1700000
[0008]                               Mask : FFFFFFFF

[0001]                             Action : 06 [Check Busy Status]
[0001]                        Instruction : 01 [Read Register Value]
[0001]              Flags (decoded below) : 00
                   Preserve Register Bits : 0
[0001]                           Reserved : 00

[0012]                    Register Region : [Generic Address Structure]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000000088220140

[0008]                              Value : 0000000000000001
[0008]                               Mask : FFFFFFFFFFFFFFFF

[0001]                             Action : 07 [Get Command Status]
[0001]                        Instruction : 00 [Read Register]
[0001]              Flags (decoded below) : 01
                   Preserve Register Bits : 1
[0001]                           Reserved : 00

[0012]                    Register Region : [Generic Address Structure]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000000088220180

[0008]                              Value : 0000000000000000
[0008]                               Mask : FFFFFFFFFFFFFFFF
