/** @file
  Asf Acpi table

  Install Asf Acpi table

  Copyright (c) 1985 - 2022, AMI. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <IndustryStandard/AlertStandardFormatTable.h>
#include <IndustryStandard/Acpi.h>
#include <Protocol/AcpiTable.h>
#include <Library/BaseMemoryLib.h>

// ASF Table Definitions
// Below array size define should follow mAsfAcpiTable setting
#define ASF_RCTL_DEVICES_ARRAY_LENGTH  4
#define ASF_ADDR_DEVICE_ARRAY_LENGTH   11

#pragma pack(push,1)

//
// Alert Remote Control System Actions.
//
typedef struct {
  EFI_ACPI_ASF_RCTL           AsfRctl;
  EFI_ACPI_ASF_CONTROLDATA    ControlDataArray[ASF_RCTL_DEVICES_ARRAY_LENGTH];
} ACPI_ASF_RCTL_ALL;

//
// SmBus Devices with fixed addresses.
//
typedef struct {
  EFI_ACPI_ASF_ADDR    AsfAddr;
  UINT8                FixedSmBusAddresses[ASF_ADDR_DEVICE_ARRAY_LENGTH];
} ACPI_ASF_ADDR_ALL;

//
// ACPI 1.0 Structure for ASF Descriptor Table.
//
typedef struct {
  EFI_ACPI_SDT_HEADER    Header;
  EFI_ACPI_ASF_INFO      AsfInfo;
  ACPI_ASF_RCTL_ALL      AsfRctlAll;
  EFI_ACPI_ASF_RMCP      AsfRmcp;
  ACPI_ASF_ADDR_ALL      AsfAddrAll;
} ASF_DESCRIPTION_TABLE;

#pragma pack(pop)

#define EFI_ACPI_1_0_ASF_DESCRIPTION_TABLE_REVISION  0x20

ASF_DESCRIPTION_TABLE  mAsfAcpiTable = {
  {
    EFI_ACPI_ASF_DESCRIPTION_TABLE_SIGNATURE,
    sizeof (ASF_DESCRIPTION_TABLE),
    EFI_ACPI_1_0_ASF_DESCRIPTION_TABLE_REVISION,
    0,                                              //  Checksum

    // OEM identification
    { 'O',  'E',  'M',  '_', 'I', 'D' },

    // OEM table identification
    { 'D',  '8',  '6',  '5', 'G', 'C', 'H', ' '},   // OEM table identification

    1,                                              // OEM revision number
    ((((('M' << 8) + 'S') << 8) + 'F') << 8) + 'T', // ASL compiler vendor ID
    1000000                                         // ASL compiler revision number
  },
  {
    //
    // EFI_ACPI_ASF_INFO
    //
    {
      0x00,                      // Type "ASF_INFO"
      0x00,                      // Reserved
      sizeof (EFI_ACPI_ASF_INFO) // Length
    },
    0x05,               // Min Watchdog Reset Value
    0xFF,               // Min ASF Sensor poll Wait Time
    0x0001,             // System ID
    0x57010000,         // IANA Manufacture ID for Intel
    0x00,               // Feature Flag
    {
      0x00,             // Reserved
      0x00,
      0x00
    } // Reserved
  },
  {
    //
    // ACPI_ASF_RCTL_ALL
    //
    {
      //
      // EFI_ACPI_ASF_RCTL
      //
      {
        0x02,                      // Type "ASF_RCTL"
        0x00,                      // Reserved
        sizeof (ACPI_ASF_RCTL_ALL) // Length
      },
      0x04,             // Number of Controls
      0x04,             // Array Element Length
      0x0000            // Reserved
    },
    {
      //
      // EFI_ACPI_ASF_CONTROLDATA
      //
      { 0x00, 0x88, 0x00, 0x03 }, // Control 0 --> Reset system
      { 0x01, 0x88, 0x00, 0x02 }, // Control 1 --> Power Off system
      { 0x02, 0x88, 0x00, 0x01 }, // Control 2 --> Power On system
      { 0x03, 0x88, 0x00, 0x04 }  // Control 3 --> Power Cycle Reset (off then on)
    }
  },
  {
    //
    // EFI_ACPI_ASF_RMCP
    //
    {
      0x03,                      // Type "ASF_RMCP"
      0x00,                      // Reserved
      sizeof (EFI_ACPI_ASF_RMCP) // Length
    },
    {
      // Remote Control Capabilities supported Bit Masks
      0x00,                       // System Firmware Capabilities Bit Mask byte 1
      0x00,                       // System Firmware Capabilities Bit Mask byte 2
      0x00,                       // System Firmware Capabilities Bit Mask byte 3
      0x00,                       // System Firmware Capabilities Bit Mask byte 4
      0x00,                       // Special Commands Bit Mask byte 1
      0x00,                       // Special Commands Bit Mask byte 2
      0xF0                        // System Capabilities Bit Mask (Supports Reset,
                                  // Power-Up, Power-Down, Power-Cycle Reset for
                                  // compat and secure port.
    },
    0x00,                       // Boot Option Complete Code
    0x57010000,                 // IANA ID for Intel Manufacturer
    0x00,                       // Special Command
    { 0x00, 0x00 },             // Special Command Parameter
    { 0x00, 0x00 },             // Boot Options
    { 0x00, 0x00 }              // OEM Parameters
  },
  {
    //
    // ACPI_ASF_ADDR_ALL
    //
    {
      //
      // EFI_ACPI_ASF_ADDR
      //
      {
        0x84,                      // Type "ASF_ADDR", last record
        0x00,                      // Reserved
        sizeof (ACPI_ASF_ADDR_ALL) // Length
      },
      0x00,                        // SEEPROM Address
      ASF_ADDR_DEVICE_ARRAY_LENGTH // Number Of Devices
    },
    //
    // Fixed SMBus Address
    //
    {
      0x5C, 0x68, 0x88, 0xC2, 0xD2,
      0xDC, 0xA0, 0xA2, 0xA4, 0xA6,
      0xC8
    }
  }
};

/**
  This function install the ASF acpi Table.

  @param[in]  Event     A pointer to the Event that triggered the callback.
  @param[in]  Context   A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
InstallAsfAcpiTableEvent  (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS               Status;
  UINTN                    TableHandle = 0;
  EFI_ACPI_TABLE_PROTOCOL  *AcpiTableProtocol;
  UINT8                    *ControlDataArrays;
  UINTN                    ControlDataArraysSize;

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTableProtocol);

  if ( EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Locate Acpi protocol %r Error\n", Status));
    return;
  }

  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  ControlDataArrays = (UINT8 *)PcdGetPtr (PcdControlDataArrays);
  ControlDataArraysSize = PcdGetSize (PcdControlDataArrays);

  if (ControlDataArraysSize == (sizeof(EFI_ACPI_ASF_CONTROLDATA) * ASF_RCTL_DEVICES_ARRAY_LENGTH)) {
    // Currently Asf 2.0 spec only support four type of control function, so We support 4 arrays of
    // EFI_ACPI_ASF_CONTROLDATA that should be defined in the PcdControlDataArrays
    CopyMem((VOID *)(UINTN)mAsfAcpiTable.AsfRctlAll.ControlDataArray, (VOID *)(UINTN)ControlDataArrays, ControlDataArraysSize);
  }

{
  UINTN  Index;

  DEBUG ((DEBUG_ERROR, "crystal ControlDataArraysSize = %x\n", ControlDataArraysSize));
  DEBUG ((DEBUG_ERROR, "ControlDataArrays = \n"));
  for (Index = 0; Index < ControlDataArraysSize; Index++) {
    if ((Index != 0) && (Index % 4) == 0) DEBUG ((DEBUG_ERROR, "\n"));
    DEBUG ((DEBUG_ERROR, "%02x ", ControlDataArrays[Index]));
  }
  DEBUG ((DEBUG_ERROR, "\n"));
}

  AcpiTableProtocol->InstallAcpiTable (
                       AcpiTableProtocol,
                       &mAsfAcpiTable,
                       mAsfAcpiTable.Header.Length,
                       &TableHandle
                       );

  return;
}
