/** @file

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
[0004]                          Signature : "HEST"    [Hardware Error Source Table]
[0004]                       Table Length : 00000308
[0001]                           Revision : 01
[0001]                           Checksum : 20
[0006]                             Oem ID : "Ampere"
[0008]                       Oem Table ID : "Altra   "
[0004]                       Oem Revision : 00000001
[0004]                    Asl Compiler ID : "INTL"
[0004]              Asl Compiler Revision : 20100528

[0004]                 Error Source Count : 00000008

[0002]                      Subtable Type : 000A [Generic Hardware Error Source v2]
[0002]                          Source Id : 0000
[0002]                  Related Source Id : FFFF
[0001]                           Reserved : 00
[0001]                            Enabled : 01
[0004]             Records To Preallocate : 00000001
[0004]            Max Sections Per Record : 00000001
[0004]                Max Raw Data Length : 00001000

[0012]               Error Status Address : [Generic Address Structure]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000000088200000

[0028]                             Notify : [Hardware Error Notification Structure]
[0001]                        Notify Type : 03 [SCI]
[0001]                      Notify Length : 1C
[0002]         Configuration Write Enable : 0000
[0004]                       PollInterval : 00000BB8
[0004]                             Vector : 00000000
[0004]            Polling Threshold Value : 00000000
[0004]           Polling Threshold Window : 00000000
[0004]              Error Threshold Value : 00000000
[0004]             Error Threshold Window : 00000000

[0004]          Error Status Block Length : 00001000

[0012]                  Read Ack Register : [Generic Address Structure v2]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000100000543010

[0008]                  Read Ack Preserve : 00000000
[0008]                     Read Ack Write : B1D00000

[0002]                      Subtable Type : 000A [Generic Hardware Error Source v2]
[0002]                          Source Id : 0001
[0002]                  Related Source Id : FFFF
[0001]                           Reserved : 00
[0001]                            Enabled : 01
[0004]             Records To Preallocate : 00000001
[0004]            Max Sections Per Record : 00000001
[0004]                Max Raw Data Length : 00001000

[0012]               Error Status Address : [Generic Address Structure]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000000088200008

[0028]                             Notify : [Hardware Error Notification Structure]
[0001]                        Notify Type : 00 [Polled]
[0001]                      Notify Length : 1C
[0002]         Configuration Write Enable : 0000
[0004]                       PollInterval : 00000BB8
[0004]                             Vector : 00000000
[0004]            Polling Threshold Value : 00000000
[0004]           Polling Threshold Window : 00000000
[0004]              Error Threshold Value : 00000000
[0004]             Error Threshold Window : 00000000

[0004]          Error Status Block Length : 00001000

[0012]                  Read Ack Register : [Generic Address Structure v2]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000100000543010

[0008]                  Read Ack Preserve : 00000000
[0008]                     Read Ack Write : B1C00000

[0002]                      Subtable Type : 000A [Generic Hardware Error Source v2]
[0002]                          Source Id : 0002
[0002]                  Related Source Id : FFFF
[0001]                           Reserved : 00
[0001]                            Enabled : 01
[0004]             Records To Preallocate : 00000001
[0004]            Max Sections Per Record : 00000001
[0004]                Max Raw Data Length : 00001000

[0012]               Error Status Address : [Generic Address Structure]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000000088200010

[0028]                             Notify : [Hardware Error Notification Structure]
[0001]                        Notify Type : 03 [SCI]
[0001]                      Notify Length : 1C
[0002]         Configuration Write Enable : 0000
[0004]                       PollInterval : 00000BB8
[0004]                             Vector : 00000000
[0004]            Polling Threshold Value : 00000000
[0004]           Polling Threshold Window : 00000000
[0004]              Error Threshold Value : 00000000
[0004]             Error Threshold Window : 00000000

[0004]          Error Status Block Length : 00001000

[0012]                  Read Ack Register : [Generic Address Structure v2]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000100000543010

[0008]                  Read Ack Preserve : 00000000
[0008]                     Read Ack Write : B1F00000

[0002]                      Subtable Type : 000A [Generic Hardware Error Source v2]
[0002]                          Source Id : 0006
[0002]                  Related Source Id : FFFF
[0001]                           Reserved : 00
[0001]                            Enabled : 01
[0004]             Records To Preallocate : 00000001
[0004]            Max Sections Per Record : 00000001
[0004]                Max Raw Data Length : 00001000

[0012]               Error Status Address : [Generic Address Structure]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000000088200030

[0028]                             Notify : [Hardware Error Notification Structure]
[0001]                        Notify Type : 03 [SCI]
[0001]                      Notify Length : 1C
[0002]         Configuration Write Enable : 0000
[0004]                       PollInterval : 00000BB8
[0004]                             Vector : 00000000
[0004]            Polling Threshold Value : 00000000
[0004]           Polling Threshold Window : 00000000
[0004]              Error Threshold Value : 00000000
[0004]             Error Threshold Window : 00000000

[0004]          Error Status Block Length : 00001000

[0012]                  Read Ack Register : [Generic Address Structure v2]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000100000543010

[0008]                  Read Ack Preserve : 00000000
[0008]                     Read Ack Write : B1900000

[0002]                      Subtable Type : 000A [Generic Hardware Error Source v2]
[0002]                          Source Id : 0007
[0002]                  Related Source Id : FFFF
[0001]                           Reserved : 00
[0001]                            Enabled : 01
[0004]             Records To Preallocate : 00000001
[0004]            Max Sections Per Record : 00000001
[0004]                Max Raw Data Length : 00001000

[0012]               Error Status Address : [Generic Address Structure]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000000088200038

[0028]                             Notify : [Hardware Error Notification Structure]
[0001]                        Notify Type : 03 [SCI]
[0001]                      Notify Length : 1C
[0002]         Configuration Write Enable : 0000
[0004]                       PollInterval : 00000BB8
[0004]                             Vector : 00000000
[0004]            Polling Threshold Value : 00000000
[0004]           Polling Threshold Window : 00000000
[0004]              Error Threshold Value : 00000000
[0004]             Error Threshold Window : 00000000

[0004]          Error Status Block Length : 00001000

[0012]                  Read Ack Register : [Generic Address Structure v2]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000100000543010

[0008]                  Read Ack Preserve : 00000000
[0008]                     Read Ack Write : B1900001

[0002]                      Subtable Type : 000A [Generic Hardware Error Source v2]
[0002]                          Source Id : 0003
[0002]                  Related Source Id : FFFF
[0001]                           Reserved : 00
[0001]                            Enabled : 01
[0004]             Records To Preallocate : 00000001
[0004]            Max Sections Per Record : 00000001
[0004]                Max Raw Data Length : 00001000

[0012]               Error Status Address : [Generic Address Structure]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000000088200018

[0028]                             Notify : [Hardware Error Notification Structure]
[0001]                        Notify Type : 03 [SCI]
[0001]                      Notify Length : 1C
[0002]         Configuration Write Enable : 0000
[0004]                       PollInterval : 00000BB8
[0004]                             Vector : 00000000
[0004]            Polling Threshold Value : 00000000
[0004]           Polling Threshold Window : 00000000
[0004]              Error Threshold Value : 00000000
[0004]             Error Threshold Window : 00000000

[0004]          Error Status Block Length : 00001000

[0012]                  Read Ack Register : [Generic Address Structure v2]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000500000543010

[0008]                  Read Ack Preserve : 00000000
[0008]                     Read Ack Write : B1D00000

[0002]                      Subtable Type : 000A [Generic Hardware Error Source v2]
[0002]                          Source Id : 0004
[0002]                  Related Source Id : FFFF
[0001]                           Reserved : 00
[0001]                            Enabled : 01
[0004]             Records To Preallocate : 00000001
[0004]            Max Sections Per Record : 00000001
[0004]                Max Raw Data Length : 00001000

[0012]               Error Status Address : [Generic Address Structure]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000000088200020

[0028]                             Notify : [Hardware Error Notification Structure]
[0001]                        Notify Type : 00 [Polled]
[0001]                      Notify Length : 1C
[0002]         Configuration Write Enable : 0000
[0004]                       PollInterval : 00000BB8
[0004]                             Vector : 00000000
[0004]            Polling Threshold Value : 00000000
[0004]           Polling Threshold Window : 00000000
[0004]              Error Threshold Value : 00000000
[0004]             Error Threshold Window : 00000000

[0004]          Error Status Block Length : 00001000

[0012]                  Read Ack Register : [Generic Address Structure v2]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000500000543010

[0008]                  Read Ack Preserve : 00000000
[0008]                     Read Ack Write : B1C00000

[0002]                      Subtable Type : 000A [Generic Hardware Error Source v2]
[0002]                          Source Id : 0005
[0002]                  Related Source Id : FFFF
[0001]                           Reserved : 00
[0001]                            Enabled : 01
[0004]             Records To Preallocate : 00000001
[0004]            Max Sections Per Record : 00000001
[0004]                Max Raw Data Length : 00001000

[0012]               Error Status Address : [Generic Address Structure]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000000088200028

[0028]                             Notify : [Hardware Error Notification Structure]
[0001]                        Notify Type : 03 [SCI]
[0001]                      Notify Length : 1C
[0002]         Configuration Write Enable : 0000
[0004]                       PollInterval : 00000BB8
[0004]                             Vector : 00000000
[0004]            Polling Threshold Value : 00000000
[0004]           Polling Threshold Window : 00000000
[0004]              Error Threshold Value : 00000000
[0004]             Error Threshold Window : 00000000

[0004]          Error Status Block Length : 00001000

[0012]                  Read Ack Register : [Generic Address Structure v2]
[0001]                           Space ID : 00 [SystemMemory]
[0001]                          Bit Width : 40
[0001]                         Bit Offset : 00
[0001]               Encoded Access Width : 04 [QWord Access:64]
[0008]                            Address : 0000500000543010

[0008]                  Read Ack Preserve : 00000000
[0008]                     Read Ack Write : B1F00000
