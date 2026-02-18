/** @file
  PCI Segment Library for N1SDP SoC with multiple RCs

  Having two distinct root complexes is not supported by the standard
  set of PciLib/PciExpressLib/PciSegmentLib, this PciSegmentLib
  reimplements the functionality to support multiple root ports on
  different segment numbers.

  On the NeoverseN1Soc, a slave error is generated when host accesses the
  configuration space of non-available device or unimplemented function on a
  given bus. So this library introduces a workaround using IsBdfValid(),
  to return 0xFFFFFFFF for all such access.

  In addition to this, the hardware has two other limitations which affect
  access to the PCIe root port:
    1. ECAM space is not contiguous, root port ECAM (BDF = 0:0:0) is isolated
       from rest of the downstream hierarchy ECAM space.
    2. Root port ECAM space is not capable of 8bit/16bit writes.
  The description of the workarounds included for these limitations can
  be found in the comments below.

  Copyright (c) 2007 - 2012, Intel Corporation. All rights reserved.<BR>
  Copyright (c) 2022, ARM Limited. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/PciSegmentLib.h>
#include <NeoverseN1Soc.h>

typedef enum {
  PciCfgWidthUint8      = 0,
  PciCfgWidthUint16,
  PciCfgWidthUint32,
  PciCfgWidthMax
} PCI_CFG_WIDTH;

/**
 Assert the validity of a PCI Segment address.
 A valid PCI Segment address should not contain 1's in bits 28..31 and 48..63

  @param A The address to validate.
  @param M Additional bits to assert to be zero.
**/
#define ASSERT_INVALID_PCI_SEGMENT_ADDRESS(A,M) \
  ASSERT (((A) & (0xffff0000f0000000ULL | (M))) == 0)

#define BUS_OFFSET      20
#define DEV_OFFSET      15
#define FUNC_OFFSET     12
#define REG_OFFSET      4096
#define REG_NUM         0xFFF
#define SEG_OFFSET      32

#define EFI_PCIE_ADDRESS(bus, dev, func, reg) \
  (UINT64) ( \
  (((UINTN) bus)   << BUS_OFFSET)  | \
  (((UINTN) dev)   << DEV_OFFSET)  | \
  (((UINTN) func)  << FUNC_OFFSET) | \
  (((UINTN) (reg)) <  REG_OFFSET ?   \
   ((UINTN) (reg)) : (UINT64) (LShiftU64 ((UINT64) (reg), 32))))

#define GET_PCIE_BASE_ADDRESS(Address)  (Address & 0xF8000000)

/* Root port Entry, BDF Entries Count */
#define BDF_TABLE_ENTRY_SIZE    4
#define BDF_TABLE_HEADER_COUNT  2
#define BDF_TABLE_HEADER_SIZE   8

/* BDF table offsets for PCIe */
#define PCIE_BDF_TABLE_OFFSET   0
#define CCIX_BDF_TABLE_OFFSET   (16 * 1024)

#define GET_BUS_NUM(Address)    (((Address) >> BUS_OFFSET) & 0x7F)
#define GET_DEV_NUM(Address)    (((Address) >> DEV_OFFSET) & 0x1F)
#define GET_FUNC_NUM(Address)   (((Address) >> FUNC_OFFSET) & 0x07)
#define GET_REG_NUM(Address)    ((Address) & REG_NUM)
#define GET_SEG_NUM(Address)    (((Address) >> SEG_OFFSET) & 0xFFFF)

CONST STATIC UINTN mDummyConfigData = 0xFFFFFFFF;

/**
  Check if the requested PCI address is a valid BDF address.

  SCP performs the initial bus scan and prepares a table of valid BDF addresses
  and shares them through non-trusted SRAM. This function validates if the PCI
  address from any PCI request falls within the table of valid entries. If not,
  this function will return 0xFFFFFFFF. This is a workaround to avoid bus fault
  that happens when accessing unavailable PCI device due to RTL bug.

  @param  Address The address that encodes the PCI Bus, Device, Function and
                  Register.

  @return The base address of PCI Express or 0xFFFFFFFF for invalid address.

**/
STATIC
UINTN
IsBdfValid (
  IN UINTN                     Address
  )
{
  UINT16  Segment;
  UINTN   BdfCount;
  UINTN   BdfValue;
  UINTN   Count;
  UINTN   TableBase;
  UINTN   PciAddress;

  Segment = GET_SEG_NUM (Address);

  // Keep the Bus, Device, Function bits. Clear the rest.
  PciAddress = Address & 0xFFFF000;

  if (Segment == 0) {
    TableBase = NEOVERSEN1SOC_NON_SECURE_SRAM_BASE + PCIE_BDF_TABLE_OFFSET;
  } else if (Segment == 1) {
    TableBase = NEOVERSEN1SOC_NON_SECURE_SRAM_BASE + CCIX_BDF_TABLE_OFFSET;
  } else {
    ASSERT (0);
    return mDummyConfigData;
  }

  BdfCount = MmioRead32 (TableBase + BDF_TABLE_ENTRY_SIZE);

  /* Start from the second entry */
  for (Count = BDF_TABLE_HEADER_COUNT;
       Count < (BdfCount + BDF_TABLE_HEADER_COUNT);
       Count++) {
    BdfValue = MmioRead32 (TableBase + (Count * BDF_TABLE_ENTRY_SIZE));
    if (BdfValue == PciAddress) {
      break;
    }
  }

  if (Count == (BdfCount + BDF_TABLE_HEADER_COUNT)) {
    return mDummyConfigData;
  } else {
    return PciAddress;
  }
}

/**
  Get the physical address of a configuration space register.

  Implement a  workaround to avoid generation of slave errors from the bus. That
  is, retrieve the PCI Express Base Address via a PCD entry, add the incomming
  address with that base address and check whether this converted address
  points to a accessible BDF. If it is not accessible, return the address
  of a dummy location so that a read from it does not cause a slave error.

  In addition to this, implement a workaround for accessing the root port's
  configuration space. The root port configuration space is not contiguous
  with the rest of the downstream hierarchy configuration space. So determine
  whether the specified address is for the root port and use a different base
  address for it.

  @param  Address The address that encodes the PCI Bus, Device, Function and
                  Register.

  @return Physical address of the configuration register that corresponds to the
          PCI configuration register specified by input parameter 'Address'.

**/
STATIC
VOID*
GetPciExpressAddress (
  IN UINTN                     Address
  )
{
  BOOLEAN CheckRootPort;
  UINT16  Segment;
  UINT8   Bus;
  UINT8   Device;
  UINT8   Function;
  UINT16  Register;
  UINTN   ConfigAddress;

  Segment  = GET_SEG_NUM (Address);
  Bus      = GET_BUS_NUM (Address);
  Device   = GET_DEV_NUM (Address);
  Function = GET_FUNC_NUM (Address);
  Register = GET_REG_NUM (Address);


  CheckRootPort = (Bus == 0) && (Device == 0) && (Function == 0);

  if (CheckRootPort == FALSE) {
    if (IsBdfValid (Address) == mDummyConfigData) {
      return (VOID*) &mDummyConfigData;
    }
  }

  if (Segment == 0) {
    if (CheckRootPort == TRUE) {
      ConfigAddress = (UINTN) PcdGet32 (PcdPcieRootPortConfigBaseAddress);
    } else {
      ConfigAddress = (UINTN) PcdGet64 (PcdPcieExpressBaseAddress);
    }
  } else if (Segment == 1) {
    if (CheckRootPort == TRUE) {
      ConfigAddress = (UINTN) PcdGet32 (PcdCcixRootPortConfigBaseAddress);
    } else {
      ConfigAddress = (UINTN) PcdGet32 (PcdCcixExpressBaseAddress);
    }
  } else {
      ASSERT (0);
      return (VOID*) &mDummyConfigData;
  }

  ConfigAddress += EFI_PCIE_ADDRESS (Bus, Device, Function, Register);
  return (VOID *)ConfigAddress;
}

/**
  Internal worker function to read a PCI configuration register.

  @param Address    The address that encodes the PCI Bus, Device, Function
                    and Register.
  @param Width      The width of data to read

  @return The value read from the PCI configuration register.
**/
STATIC
UINT32
PciSegmentLibReadWorker (
  IN  UINT64                   Address,
  IN  PCI_CFG_WIDTH            Width
  )
{
  UINTN PciAddress;

  PciAddress = (UINTN) GetPciExpressAddress ((UINTN) Address);

  switch (Width) {
  case PciCfgWidthUint8:
    return MmioRead8 (PciAddress);
  case PciCfgWidthUint16:
    return MmioRead16 (PciAddress);
  case PciCfgWidthUint32:
    return MmioRead32 (PciAddress);
  default:
    ASSERT (0);
  }

  return 0;
}

/**
  Internal worker function to write to a PCI configuration register.

  @param Address   The address that encodes the PCI Bus, Device, Function
                   and Register.
  @param Width     The width of data to write
  @param Data      The value to write.

  @return  The value written to the PCI configuration register.
**/
STATIC
UINT32
PciSegmentLibWriteWorker (
  IN  UINT64                   Address,
  IN  PCI_CFG_WIDTH            Width,
  IN  UINT32                   Data
  )
{
  UINT8    Offset;
  UINT32   WData;
  UINT64   AlignedAddress;
  BOOLEAN  CheckRootPort;

  CheckRootPort = (GET_BUS_NUM (Address) == 0) &&
                    (GET_DEV_NUM (Address) == 0) &&
                    (GET_FUNC_NUM (Address) == 0);

  // 8-bit and 16-bit writes to root port config space is not supported due to
  // a hardware limitation. As a workaround, perform a read-update-write
  // sequence on the whole 32-bit word of the root port config register such
  // that only the specified 8-bits of that word are updated.

  switch (Width) {
  case PciCfgWidthUint8:
    if (CheckRootPort == TRUE) {
      Offset = Address & 0x3;
      AlignedAddress = Address & ~(0x3);
      WData = MmioRead32 ((UINTN) GetPciExpressAddress (AlignedAddress));
      WData &= ~(0xFF << (8 * Offset));
      WData |= (Data << (8 * Offset));
      MmioWrite32 ((UINTN) GetPciExpressAddress (AlignedAddress), WData);
      return Data;
    } else {
      MmioWrite8 ((UINTN) GetPciExpressAddress (Address), Data);
    }
    break;
  case PciCfgWidthUint16:
    if (CheckRootPort == TRUE) {
      Offset = Address & 0x3;
      AlignedAddress = Address & ~(0x3);
      WData = MmioRead32 ((UINTN) GetPciExpressAddress (AlignedAddress));
      WData &= ~(0xFFFF << (8 * Offset));
      WData |= (Data << (8 * Offset));
      MmioWrite32 ((UINTN) GetPciExpressAddress (AlignedAddress), WData);
      return Data;
    } else {
      MmioWrite16 ((UINTN) GetPciExpressAddress (Address), Data);
    }
    break;
  case PciCfgWidthUint32:
    MmioWrite32 ((UINTN) GetPciExpressAddress (Address), Data);
    break;
  default:
    ASSERT (0);
  }

  return Data;
}

/**
  Reads an 8-bit PCI configuration register.

  Reads and returns the 8-bit PCI configuration register specified by Address.
  This function must guarantee that all PCI read and write operations are
  serialized.

  If any reserved bits in Address are set, then ASSERT().

  @param Address     The address that encodes the PCI Segment, Bus,
                     Device, Function and Register.

  @return The 8-bit PCI configuration register specified by the Address.
**/
UINT8
EFIAPI
PciSegmentRead8 (
  IN UINT64                    Address
  )
{
  ASSERT_INVALID_PCI_SEGMENT_ADDRESS (Address, 0);

  return (UINT8) PciSegmentLibReadWorker (Address, PciCfgWidthUint8);
}

/**
  Writes an 8-bit PCI configuration register.

  Writes the 8-bit Value in the PCI configuration register specified by the
  Address. This function must guarantee that all PCI read and write operations
  are serialized.

  If any reserved bits in Address are set, then ASSERT().

  @param Address     The address that encodes the PCI Segment, Bus,
                     Device, Function, and Register.
  @param Value       The value to write.

  @return The value written to the PCI configuration register.
**/
UINT8
EFIAPI
PciSegmentWrite8 (
  IN UINT64                    Address,
  IN UINT8                     Value
  )
{
  ASSERT_INVALID_PCI_SEGMENT_ADDRESS (Address, 0);

  return (UINT8) PciSegmentLibWriteWorker (Address, PciCfgWidthUint8, Value);
}

/**
  Performs a bitwise OR of an 8-bit PCI configuration register with
  an 8-bit value.

  Reads the 8-bit PCI configuration register specified by Address,
  performs a bitwise OR between the read result and the value specified by
  OrData, and writes the result to the 8-bit PCI configuration register
  specified by Address.

  The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations
  are serialized.

  If any reserved bits in Address are set, then ASSERT().

  @param  Address   The address that encodes the PCI Segment, Bus, Device,
                    Function, and Register.
  @param  OrData    The value to OR with the PCI configuration register.

  @return The value written to the PCI configuration register.
**/
UINT8
EFIAPI
PciSegmentOr8 (
  IN UINT64                    Address,
  IN UINT8                     OrData
  )
{
  return PciSegmentWrite8 (Address,
                           (UINT8) (PciSegmentRead8 (Address) | OrData));
}

/**
  Performs a bitwise AND of an 8-bit PCI configuration register with
  an 8-bit value.

  Reads the 8-bit PCI configuration register specified by Address,
  performs a bitwise AND between the read result and the value specified by
  AndData, and writes the result to the 8-bit PCI configuration register
  specified by Address.

  The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations
  are serialized. If any reserved bits in Address are set, then ASSERT().

  @param  Address   The address that encodes the PCI Segment, Bus, Device,
                    Function, and Register.
  @param  AndData   The value to AND with the PCI configuration register.

  @return The value written to the PCI configuration register.
**/
UINT8
EFIAPI
PciSegmentAnd8 (
  IN UINT64                    Address,
  IN UINT8                     AndData
  )
{
  return PciSegmentWrite8 (Address,
                           (UINT8) (PciSegmentRead8 (Address) & AndData));
}

/**
  Performs a bitwise AND of an 8-bit PCI configuration register with
  an 8-bit value, followed by a bitwise OR with another 8-bit value.

  Reads the 8-bit PCI configuration register specified by Address,
  performs a bitwise AND between the read result and the value specified
  by AndData, performs a bitwise OR between the result of the AND operation
  and the value specified by OrData, and writes the result to the 8-bit
  PCI configuration register specified by Address.

  The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations
  are serialized.

  If any reserved bits in Address are set, then ASSERT().

  @param  Address   The address that encodes the PCI Segment, Bus, Device,
                    Function, and Register.
  @param  AndData   The value to AND with the PCI configuration register.
  @param  OrData    The value to OR with the PCI configuration register.

  @return The value written to the PCI configuration register.

**/
UINT8
EFIAPI
PciSegmentAndThenOr8 (
  IN UINT64                    Address,
  IN UINT8                     AndData,
  IN UINT8                     OrData
  )
{
  return PciSegmentWrite8 (Address,
                           (UINT8) ((PciSegmentRead8 (Address) & AndData)
                                     | OrData));
}

/**
  Reads a bit field of a PCI configuration register.

  Reads the bit field in an 8-bit PCI configuration register. The bit field is
  specified by the StartBit and the EndBit. The value of the bit field is
  returned.

  If any reserved bits in Address are set, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param  Address   The PCI configuration register to read.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..7.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..7.

  @return The value of the bit field read from the PCI configuration register.

**/
UINT8
EFIAPI
PciSegmentBitFieldRead8 (
  IN UINT64                    Address,
  IN UINTN                     StartBit,
  IN UINTN                     EndBit
  )
{
  return BitFieldRead8 (PciSegmentRead8 (Address), StartBit, EndBit);
}

/**
  Writes a bit field to a PCI configuration register.

  Writes Value to the bit field of the PCI configuration register. The bit
  field is specified by the StartBit and the EndBit. All other bits in the
  destination PCI configuration register are preserved. The new value of the
  8-bit register is returned.

  If any reserved bits in Address are set, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If Value is larger than the bitmask value range specified by StartBit
  and EndBit, then ASSERT().

  @param  Address   The PCI configuration register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..7.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..7.
  @param  Value     The new value of the bit field.

  @return The value written back to the PCI configuration register.

**/
UINT8
EFIAPI
PciSegmentBitFieldWrite8 (
  IN UINT64                    Address,
  IN UINTN                     StartBit,
  IN UINTN                     EndBit,
  IN UINT8                     Value
  )
{
  return PciSegmentWrite8 (Address,
                           BitFieldWrite8 (PciSegmentRead8 (Address),
                                           StartBit,
                                           EndBit,
                                           Value));
}

/**
  Reads a bit field in an 8-bit PCI configuration, performs a bitwise OR, and
  writes the result back to the bit field in the 8-bit port.

  Reads the 8-bit PCI configuration register specified by Address, performs a
  bitwise OR between the read result and the value specified by
  OrData, and writes the result to the 8-bit PCI configuration register
  specified by Address. The value written to the PCI configuration register is
  returned. This function must guarantee that all PCI read and write operations
  are serialized. Extra left bits in OrData are stripped.

  If any reserved bits in Address are set, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit and
  EndBit, then ASSERT().

  @param  Address   The PCI configuration register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..7.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..7.
  @param  OrData    The value to OR with the PCI configuration register.

  @return The value written back to the PCI configuration register.

**/
UINT8
EFIAPI
PciSegmentBitFieldOr8 (
  IN UINT64                    Address,
  IN UINTN                     StartBit,
  IN UINTN                     EndBit,
  IN UINT8                     OrData
  )
{
  return PciSegmentWrite8 (Address,
                           BitFieldOr8 (PciSegmentRead8 (Address),
                                        StartBit,
                                        EndBit,
                                        OrData));
}

/**
  Reads a bit field in an 8-bit PCI configuration register, performs a bitwise
  AND, and writes the result back to the bit field in the 8-bit register.

  Reads the 8-bit PCI configuration register specified by Address, performs a
  bitwise AND between the read result and the value specified by AndData, and
  writes the result to the 8-bit PCI configuration register specified by
  Address. The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations are
  serialized. Extra left bits in AndData are stripped.

  If any reserved bits in Address are set, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit and
  EndBit, then ASSERT().

  @param  Address   The PCI configuration register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..7.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..7.
  @param  AndData   The value to AND with the PCI configuration register.

  @return The value written back to the PCI configuration register.

**/
UINT8
EFIAPI
PciSegmentBitFieldAnd8 (
  IN UINT64                    Address,
  IN UINTN                     StartBit,
  IN UINTN                     EndBit,
  IN UINT8                     AndData
  )
{
  return PciSegmentWrite8 (Address,
                           BitFieldAnd8 (PciSegmentRead8 (Address),
                                         StartBit,
                                         EndBit,
                                         AndData));
}

/**
  Reads a bit field in an 8-bit port, performs a bitwise AND followed by a
  bitwise OR, and writes the result back to the bit field in the
  8-bit port.

  Reads the 8-bit PCI configuration register specified by Address, performs a
  bitwise AND followed by a bitwise OR between the read result and
  the value specified by AndData, and writes the result to the 8-bit PCI
  configuration register specified by Address. The value written to the PCI
  configuration register is returned. This function must guarantee that all PCI
  read and write operations are serialized. Extra left bits in both AndData and
  OrData are stripped.

  If any reserved bits in Address are set, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit
  and EndBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit
  and EndBit, then ASSERT().

  @param  Address   The PCI configuration register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..7.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..7.
  @param  AndData   The value to AND with the PCI configuration register.
  @param  OrData    The value to OR with the result of the AND operation.

  @return The value written back to the PCI configuration register.

**/
UINT8
EFIAPI
PciSegmentBitFieldAndThenOr8 (
  IN UINT64                    Address,
  IN UINTN                     StartBit,
  IN UINTN                     EndBit,
  IN UINT8                     AndData,
  IN UINT8                     OrData
  )
{
  return PciSegmentWrite8 (Address,
                           BitFieldAndThenOr8 (PciSegmentRead8 (Address),
                                               StartBit,
                                               EndBit,
                                               AndData,
                                               OrData));
}

/**
  Reads a 16-bit PCI configuration register.

  Reads and returns the 16-bit PCI configuration register specified by Address.
  This function must guarantee that all PCI read and write operations
  are serialized.

  If any reserved bits in Address are set, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param  Address   The address that encodes the PCI Segment, Bus, Device,
                    Function, and Register.

  @return The 16-bit PCI configuration register specified by Address.

**/
UINT16
EFIAPI
PciSegmentRead16 (
  IN UINT64                    Address
  )
{
  ASSERT_INVALID_PCI_SEGMENT_ADDRESS (Address, 1);

  return (UINT16) PciSegmentLibReadWorker (Address, PciCfgWidthUint16);
}

/**
  Writes a 16-bit PCI configuration register.

  Writes the 16-bit PCI configuration register specified by Address with the
  value specified by Value. Value is returned.
  This function must guarantee that all PCI read and write operations
  are serialized.

  If any reserved bits in Address are set, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param  Address     The address that encodes the PCI Segment, Bus, Device,
                      Function, and Register.
  @param  Value       The value to write.

  @return The Value written is returned.

**/
UINT16
EFIAPI
PciSegmentWrite16 (
  IN UINT64                    Address,
  IN UINT16                    Value
  )
{
  ASSERT_INVALID_PCI_SEGMENT_ADDRESS (Address, 1);

  return (UINT16) PciSegmentLibWriteWorker (Address, PciCfgWidthUint16, Value);
}

/**
  Performs a bitwise OR of a 16-bit PCI configuration register with
  a 16-bit value.

  Reads the 16-bit PCI configuration register specified by Address, performs a
  bitwise OR between the read result and the value specified by
  OrData, and writes the result to the 16-bit PCI configuration register
  specified by Address. The value written to the PCI configuration register is
  returned. This function must guarantee that all PCI read and write operations
  are serialized.

  If any reserved bits in Address are set, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param  Address The address that encodes the PCI Segment, Bus, Device,
                  Function and Register.
  @param  OrData  The value to OR with the PCI configuration register.

  @return The value written back to the PCI configuration register.

**/
UINT16
EFIAPI
PciSegmentOr16 (
  IN UINT64                    Address,
  IN UINT16                    OrData
  )
{
  return PciSegmentWrite16 (Address,
                            (UINT16) (PciSegmentRead16 (Address) | OrData));
}

/**
  Performs a bitwise AND of a 16-bit PCI configuration register with
  a 16-bit value.

  Reads the 16-bit PCI configuration register specified by Address,
  performs a bitwise AND between the read result and the value specified
  by AndData, and writes the result to the 16-bit PCI configuration register
  specified by the Address.

  The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations
  are serialized.

  If any reserved bits in Address are set, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param  Address   The address that encodes the PCI Segment, Bus, Device,
                    Function, and Register.
  @param  AndData   The value to AND with the PCI configuration register.

  @return The value written to the PCI configuration register.

**/
UINT16
EFIAPI
PciSegmentAnd16 (
  IN UINT64                    Address,
  IN UINT16                    AndData
  )
{
  return PciSegmentWrite16 (Address,
                            (UINT16) (PciSegmentRead16 (Address) & AndData));
}

/**
  Performs a bitwise AND of a 16-bit PCI configuration register with a 16-bit
  value, followed a  bitwise OR with another 16-bit value.

  Reads the 16-bit PCI configuration register specified by Address,
  performs a bitwise AND between the read result and the value specified by
  AndData, performs a bitwise OR between the result of the AND operation and
  the value specified by OrData, and writes the result to the 16-bit PCI
  configuration register specified by the Address.

  The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations
  are serialized.

  If any reserved bits in Address are set, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param  Address   The address that encodes the PCI Segment, Bus, Device,
                    Function, and Register.
  @param  AndData   The value to AND with the PCI configuration register.
  @param  OrData    The value to OR with the PCI configuration register.

  @return The value written to the PCI configuration register.

**/
UINT16
EFIAPI
PciSegmentAndThenOr16 (
  IN UINT64                    Address,
  IN UINT16                    AndData,
  IN UINT16                    OrData
  )
{
  return PciSegmentWrite16 (Address,
                            (UINT16) ((PciSegmentRead16 (Address) & AndData)
                                      | OrData));
}

/**
  Reads a bit field of a PCI configuration register.

  Reads the bit field in a 16-bit PCI configuration register. The bit field is
  specified by the StartBit and the EndBit. The value of the bit field is
  returned.

  If any reserved bits in Address are set, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param  Address   The PCI configuration register to read.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..15.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..15.

  @return The value of the bit field read from the PCI configuration register.

**/
UINT16
EFIAPI
PciSegmentBitFieldRead16 (
  IN UINT64                    Address,
  IN UINTN                     StartBit,
  IN UINTN                     EndBit
  )
{
  return BitFieldRead16 (PciSegmentRead16 (Address), StartBit, EndBit);
}

/**
  Writes a bit field to a PCI configuration register.

  Writes Value to the bit field of the PCI configuration register. The bit
  field is specified by the StartBit and the EndBit. All other bits in the
  destination PCI configuration register are preserved. The new value of the
  16-bit register is returned.

  If any reserved bits in Address are set, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If Value is larger than the bitmask value range specified by StartBit and
  EndBit, then ASSERT().

  @param  Address   The PCI configuration register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..15.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..15.
  @param  Value     The new value of the bit field.

  @return The value written back to the PCI configuration register.

**/
UINT16
EFIAPI
PciSegmentBitFieldWrite16 (
  IN UINT64                    Address,
  IN UINTN                     StartBit,
  IN UINTN                     EndBit,
  IN UINT16                    Value
  )
{
  return PciSegmentWrite16 (Address,
                            BitFieldWrite16 (PciSegmentRead16 (Address),
                                             StartBit,
                                             EndBit,
                                             Value));
}

/**
  Reads the 16-bit PCI configuration register specified by Address,
  performs a bitwise OR between the read result and the value specified
  by OrData, and writes the result to the 16-bit PCI configuration register
  specified by the Address.

  If any reserved bits in Address are set, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit and
  EndBit, then ASSERT().

  @param  Address   The PCI configuration register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..15.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..15.
  @param  OrData    The value to OR with the PCI configuration register.

  @return The value written back to the PCI configuration register.

**/
UINT16
EFIAPI
PciSegmentBitFieldOr16 (
  IN UINT64                    Address,
  IN UINTN                     StartBit,
  IN UINTN                     EndBit,
  IN UINT16                    OrData
  )
{
  return PciSegmentWrite16 (Address,
                            BitFieldOr16 (PciSegmentRead16 (Address),
                                          StartBit,
                                          EndBit,
                                          OrData));
}

/**
  Reads a bit field in a 16-bit PCI configuration, performs a bitwise OR,
  and writes the result back to the bit field in the 16-bit port.

  Reads the 16-bit PCI configuration register specified by Address,
  performs a bitwise OR between the read result and the value specified by
  OrData, and writes the result to the 16-bit PCI configuration register
  specified by the Address.

  The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations are
  serialized. Extra left bits in OrData are stripped.

  If any reserved bits in Address are set, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit and
  EndBit, then ASSERT().

  @param  Address   The address that encodes the PCI Segment, Bus, Device,
                    Function, and Register.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    The ordinal of the least significant bit in a byte is
                    bit 0.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    The ordinal of the most significant bit in a byte is bit 7.
  @param  AndData   The value to AND with the read value from the PCI
                    configuration register.

  @return The value written to the PCI configuration register.

**/
UINT16
EFIAPI
PciSegmentBitFieldAnd16 (
  IN UINT64                    Address,
  IN UINTN                     StartBit,
  IN UINTN                     EndBit,
  IN UINT16                    AndData
  )
{
  return PciSegmentWrite16 (Address,
                            BitFieldAnd16 (PciSegmentRead16 (Address),
                                           StartBit,
                                           EndBit,
                                           AndData));
}

/**
  Reads a bit field in a 16-bit port, performs a bitwise AND followed by a
  bitwise OR, and writes the result back to the bit field in the
  16-bit port.

  Reads the 16-bit PCI configuration register specified by Address, performs a
  bitwise AND followed by a bitwise OR between the read result and
  the value specified by AndData, and writes the result to the 16-bit PCI
  configuration register specified by Address. The value written to the PCI
  configuration register is returned. This function must guarantee that all PCI
  read and write operations are serialized. Extra left bits in both AndData and
  OrData are stripped.

  If any reserved bits in Address are set, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit and
  EndBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit and
  EndBit, then ASSERT().

  @param  Address   The PCI configuration register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..15.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..15.
  @param  AndData   The value to AND with the PCI configuration register.
  @param  OrData    The value to OR with the result of the AND operation.

  @return The value written back to the PCI configuration register.

**/
UINT16
EFIAPI
PciSegmentBitFieldAndThenOr16 (
  IN UINT64                    Address,
  IN UINTN                     StartBit,
  IN UINTN                     EndBit,
  IN UINT16                    AndData,
  IN UINT16                    OrData
  )
{
  return PciSegmentWrite16 (Address,
                            BitFieldAndThenOr16 (PciSegmentRead16 (Address),
                                                 StartBit,
                                                 EndBit,
                                                 AndData,
                                                 OrData));
}

/**
  Reads a 32-bit PCI configuration register.

  Reads and returns the 32-bit PCI configuration register specified by Address.
  This function must guarantee that all PCI read and write operations
  are serialized.

  If any reserved bits in Address are set, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param  Address   The address that encodes the PCI Segment, Bus,
                    Device, Function and Register.

  @return The 32-bit PCI configuration register specified by Address.

**/
UINT32
EFIAPI
PciSegmentRead32 (
  IN UINT64                    Address
  )
{
  ASSERT_INVALID_PCI_SEGMENT_ADDRESS (Address, 3);

  return PciSegmentLibReadWorker (Address, PciCfgWidthUint32);
}

/**
  Writes a 32-bit PCI configuration register.

  Writes the 32-bit PCI configuration register specified by Address with the
  value specified by Value.  Value is returned. This function must guarantee
  that all PCI read and write operations are serialized.

  If any reserved bits in Address are set, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param  Address     The address that encodes the PCI Segment, Bus, Device,
                      Function, and Register.
  @param  Value       The value to write.

  @return The parameter of Value.

**/
UINT32
EFIAPI
PciSegmentWrite32 (
  IN UINT64                    Address,
  IN UINT32                    Value
  )
{
  ASSERT_INVALID_PCI_SEGMENT_ADDRESS (Address, 3);

  return PciSegmentLibWriteWorker (Address, PciCfgWidthUint32, Value);
}

/**
  Performs a bitwise OR of a 32-bit PCI configuration register with a
  32-bit value.

  Reads the 32-bit PCI configuration register specified by Address,
  performs a bitwise OR between the read result and the value specified
  by OrData, and writes the result to the 32-bit PCI configuration register
  specified by Address.

  The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations
  are serialized.

  If any reserved bits in Address are set, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param  Address   The address that encodes the PCI Segment, Bus, Device,
                    Function, and Register.
  @param  OrData    The value to OR with the PCI configuration register.

  @return The value written to the PCI configuration register.

**/
UINT32
EFIAPI
PciSegmentOr32 (
  IN UINT64                    Address,
  IN UINT32                    OrData
  )
{
  return PciSegmentWrite32 (Address, PciSegmentRead32 (Address) | OrData);
}

/**
  Performs a bitwise AND of a 32-bit PCI configuration register with
  a 32-bit value.

  Reads the 32-bit PCI configuration register specified by Address,
  performs a bitwise AND between the read result and the value specified
  by AndData, and writes the result to the 32-bit PCI configuration register
  specified by Address.
  The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations
  are serialized.

  If any reserved bits in Address are set, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param  Address   The address that encodes the PCI Segment, Bus,
                    Device, Function and Register.
  @param  AndData   The value to AND with the PCI configuration register.

  @return The value written to the PCI configuration register.

**/
UINT32
EFIAPI
PciSegmentAnd32 (
  IN UINT64                    Address,
  IN UINT32                    AndData
  )
{
  return PciSegmentWrite32 (Address, PciSegmentRead32 (Address) & AndData);
}

/**
  Performs a bitwise AND of a 32-bit PCI configuration register with
  a 32-bit value, followed by a bitwise OR with another 32-bit value.

  Reads the 32-bit PCI configuration register specified by Address,
  performs a bitwise AND between the read result and the value specified
  by AndData, performs a bitwise OR between the result of the AND operation
  and the value specified by OrData, and writes the result to the 32-bit
  PCI configuration register specified by Address.

  The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations
  are serialized.

  If any reserved bits in Address are set, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param  Address   The address that encodes the PCI Segment, Bus, Device,
                    Function and Register.
  @param  AndData   The value to AND with the PCI configuration register.
  @param  OrData    The value to OR with the PCI configuration register.

  @return The value written to the PCI configuration register.

**/
UINT32
EFIAPI
PciSegmentAndThenOr32 (
  IN UINT64                    Address,
  IN UINT32                    AndData,
  IN UINT32                    OrData
  )
{
  return PciSegmentWrite32 (Address,
                            (PciSegmentRead32 (Address) & AndData) | OrData);
}

/**
  Reads a bit field of a PCI configuration register.

  Reads the bit field in a 32-bit PCI configuration register. The bit field is
  specified by the StartBit and the EndBit. The value of the bit field is
  returned.

  If any reserved bits in Address are set, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param  Address   The PCI configuration register to read.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..31.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..31.

  @return The value of the bit field read from the PCI configuration register.

**/
UINT32
EFIAPI
PciSegmentBitFieldRead32 (
  IN UINT64                    Address,
  IN UINTN                     StartBit,
  IN UINTN                     EndBit
  )
{
  return BitFieldRead32 (PciSegmentRead32 (Address), StartBit, EndBit);
}

/**
  Writes a bit field to a PCI configuration register.

  Writes Value to the bit field of the PCI configuration register. The bit
  field is specified by the StartBit and the EndBit. All other bits in the
  destination PCI configuration register are preserved. The new value of the
  32-bit register is returned.

  If any reserved bits in Address are set, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If Value is larger than the bitmask value range specified by StartBit
  and EndBit, then ASSERT().

  @param  Address   The PCI configuration register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..31.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..31.
  @param  Value     The new value of the bit field.

  @return The value written back to the PCI configuration register.

**/
UINT32
EFIAPI
PciSegmentBitFieldWrite32 (
  IN UINT64                    Address,
  IN UINTN                     StartBit,
  IN UINTN                     EndBit,
  IN UINT32                    Value
  )
{
  return PciSegmentWrite32 (Address,
                            BitFieldWrite32 (PciSegmentRead32 (Address),
                                             StartBit,
                                             EndBit,
                                             Value));
}

/**
  Reads a bit field in a 32-bit PCI configuration, performs a bitwise OR, and
  writes the result back to the bit field in the 32-bit port.

  Reads the 32-bit PCI configuration register specified by Address, performs a
  bitwise OR between the read result and the value specified by
  OrData, and writes the result to the 32-bit PCI configuration register
  specified by Address. The value written to the PCI configuration register is
  returned. This function must guarantee that all PCI read and write operations
  are serialized. Extra left bits in OrData are stripped.

  If any reserved bits in Address are set, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit and
  EndBit, then ASSERT().

  @param  Address   The PCI configuration register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..31.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..31.
  @param  OrData    The value to OR with the PCI configuration register.

  @return The value written back to the PCI configuration register.

**/
UINT32
EFIAPI
PciSegmentBitFieldOr32 (
  IN UINT64                    Address,
  IN UINTN                     StartBit,
  IN UINTN                     EndBit,
  IN UINT32                    OrData
  )
{
  return PciSegmentWrite32 (Address,
                            BitFieldOr32 (PciSegmentRead32 (Address),
                                          StartBit,
                                          EndBit,
                                          OrData));
}

/**
  Reads a bit field in a 32-bit PCI configuration register, performs a bitwise
  AND, and writes the result back to the bit field in the 32-bit register.

  Reads the 32-bit PCI configuration register specified by Address,
  performs a bitwise AND between the read result and the value specified
  by AndData, and writes the result to the 32-bit PCI configuration register
  specified by Address. The value written to the PCI configuration register
  is returned.  This function must guarantee that all PCI read and write
  operations are serialized.  Extra left bits in AndData are stripped.

  If any reserved bits in Address are set, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit and
  EndBit, then ASSERT().

  @param  Address   The PCI configuration register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..31.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..31.
  @param  AndData   The value to AND with the PCI configuration register.

  @return The value written back to the PCI configuration register.

**/
UINT32
EFIAPI
PciSegmentBitFieldAnd32 (
  IN UINT64                    Address,
  IN UINTN                     StartBit,
  IN UINTN                     EndBit,
  IN UINT32                    AndData
  )
{
  return PciSegmentWrite32 (Address,
                            BitFieldAnd32 (PciSegmentRead32 (Address),
                                           StartBit,
                                           EndBit,
                                           AndData));
}

/**
  Reads a bit field in a 32-bit port, performs a bitwise AND followed by a
  bitwise OR, and writes the result back to the bit field in the
  32-bit port.

  Reads the 32-bit PCI configuration register specified by Address, performs a
  bitwise AND followed by a bitwise OR between the read result and
  the value specified by AndData, and writes the result to the 32-bit PCI
  configuration register specified by Address. The value written to the PCI
  configuration register is returned. This function must guarantee that all PCI
  read and write operations are serialized. Extra left bits in both AndData and
  OrData are stripped.

  If any reserved bits in Address are set, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit
  and EndBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit
  and EndBit, then ASSERT().

  @param  Address   The PCI configuration register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..31.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..31.
  @param  AndData   The value to AND with the PCI configuration register.
  @param  OrData    The value to OR with the result of the AND operation.

  @return The value written back to the PCI configuration register.

**/
UINT32
EFIAPI
PciSegmentBitFieldAndThenOr32 (
  IN UINT64                    Address,
  IN UINTN                     StartBit,
  IN UINTN                     EndBit,
  IN UINT32                    AndData,
  IN UINT32                    OrData
  )
{
  return PciSegmentWrite32 (
           Address,
           BitFieldAndThenOr32 (PciSegmentRead32 (Address),
                                StartBit,
                                EndBit,
                                AndData,
                                OrData));
}

/**
  Reads a range of PCI configuration registers into a caller supplied buffer.

  Reads the range of PCI configuration registers specified by StartAddress and
  Size into the buffer specified by Buffer. This function only allows the PCI
  configuration registers from a single PCI function to be read. Size is
  returned. When possible 32-bit PCI configuration read cycles are used to read
  from StartAdress to StartAddress + Size. Due to alignment restrictions, 8-bit
  and 16-bit PCI configuration read cycles may be used at the beginning and the
  end of the range.

  If any reserved bits in StartAddress are set, then ASSERT().
  If ((StartAddress & 0xFFF) + Size) > 0x1000, then ASSERT().
  If Size > 0 and Buffer is NULL, then ASSERT().

  @param  StartAddress  The starting address that encodes the PCI Segment, Bus,
                        Device, Function and Register.
  @param  Size          The size in bytes of the transfer.
  @param  Buffer        The pointer to a buffer receiving the data read.

  @return Size

**/
UINTN
EFIAPI
PciSegmentReadBuffer (
  IN  UINT64                   StartAddress,
  IN  UINTN                    Size,
  OUT VOID                     *Buffer
  )
{
  UINTN                             ReturnValue;

  ASSERT_INVALID_PCI_SEGMENT_ADDRESS (StartAddress, 0);

  if (((StartAddress & 0xFFF) + Size) > 0x1000) {
    ASSERT (0);
    return 0;
  }

  if (Size == 0) {
    return Size;
  }

  if (Buffer == NULL) {
    ASSERT (0);
    return 0;
  }

  // Save Size for return
  ReturnValue = Size;

  if ((StartAddress & BIT0) != 0) {
    // Read a byte if StartAddress is byte aligned
    *(volatile UINT8 *)Buffer = PciSegmentRead8 (StartAddress);
    StartAddress += sizeof (UINT8);
    Size -= sizeof (UINT8);
    Buffer = (UINT8*)Buffer + 1;
  }

  if (Size >= sizeof (UINT16) && (StartAddress & BIT1) != 0) {
    // Read a word if StartAddress is word aligned
    WriteUnaligned16 (Buffer, PciSegmentRead16 (StartAddress));
    StartAddress += sizeof (UINT16);
    Size -= sizeof (UINT16);
    Buffer = (UINT16*)Buffer + 1;
  }

  while (Size >= sizeof (UINT32)) {
    // Read as many double words as possible
    WriteUnaligned32 (Buffer, PciSegmentRead32 (StartAddress));
    StartAddress += sizeof (UINT32);
    Size -= sizeof (UINT32);
    Buffer = (UINT32*)Buffer + 1;
  }

  if (Size >= sizeof (UINT16)) {
    // Read the last remaining word if exist
    WriteUnaligned16 (Buffer, PciSegmentRead16 (StartAddress));
    StartAddress += sizeof (UINT16);
    Size -= sizeof (UINT16);
    Buffer = (UINT16*)Buffer + 1;
  }

  if (Size >= sizeof (UINT8)) {
    // Read the last remaining byte if exist
    *(volatile UINT8 *)Buffer = PciSegmentRead8 (StartAddress);
  }

  return ReturnValue;
}

/**
  Copies the data in a caller supplied buffer to a specified range of PCI
  configuration space.

  Writes the range of PCI configuration registers specified by StartAddress and
  Size from the buffer specified by Buffer. This function only allows the PCI
  configuration registers from a single PCI function to be written. Size is
  returned. When possible 32-bit PCI configuration write cycles are used to
  write from StartAdress to StartAddress + Size. Due to alignment restrictions,
  8-bit and 16-bit PCI configuration write cycles may be used at the beginning
  and the end of the range.

  If any reserved bits in StartAddress are set, then ASSERT().
  If ((StartAddress & 0xFFF) + Size) > 0x1000, then ASSERT().
  If Size > 0 and Buffer is NULL, then ASSERT().

  @param  StartAddress  The starting address that encodes the PCI Segment, Bus,
                        Device, Function and Register.
  @param  Size          The size in bytes of the transfer.
  @param  Buffer        The pointer to a buffer containing the data to write.

  @return The parameter of Size.

**/
UINTN
EFIAPI
PciSegmentWriteBuffer (
  IN UINT64                    StartAddress,
  IN UINTN                     Size,
  IN VOID                      *Buffer
  )
{
  UINTN                             ReturnValue;

  ASSERT_INVALID_PCI_SEGMENT_ADDRESS (StartAddress, 0);

  if (((StartAddress & 0xFFF) + Size) > 0x1000) {
    ASSERT (0);
    return 0;
  }

  if (Size == 0) {
    return Size;
  }

  if (Buffer == NULL) {
    ASSERT (0);
    return 0;
  }

  // Save Size for return
  ReturnValue = Size;

  if ((StartAddress & BIT0) != 0) {
    // Write a byte if StartAddress is byte aligned
    PciSegmentWrite8 (StartAddress, *(UINT8*)Buffer);
    StartAddress += sizeof (UINT8);
    Size -= sizeof (UINT8);
    Buffer = (UINT8*)Buffer + 1;
  }

  if (Size >= sizeof (UINT16) && (StartAddress & BIT1) != 0) {
    // Write a word if StartAddress is word aligned
    PciSegmentWrite16 (StartAddress, ReadUnaligned16 (Buffer));
    StartAddress += sizeof (UINT16);
    Size -= sizeof (UINT16);
    Buffer = (UINT16*)Buffer + 1;
  }

  while (Size >= sizeof (UINT32)) {
    // Write as many double words as possible
    PciSegmentWrite32 (StartAddress, ReadUnaligned32 (Buffer));
    StartAddress += sizeof (UINT32);
    Size -= sizeof (UINT32);
    Buffer = (UINT32*)Buffer + 1;
  }

  if (Size >= sizeof (UINT16)) {
    // Write the last remaining word if exist
    PciSegmentWrite16 (StartAddress, ReadUnaligned16 (Buffer));
    StartAddress += sizeof (UINT16);
    Size -= sizeof (UINT16);
    Buffer = (UINT16*)Buffer + 1;
  }

  if (Size >= sizeof (UINT8)) {
    // Write the last remaining byte if exist
    PciSegmentWrite8 (StartAddress, *(UINT8*)Buffer);
  }

  return ReturnValue;
}
