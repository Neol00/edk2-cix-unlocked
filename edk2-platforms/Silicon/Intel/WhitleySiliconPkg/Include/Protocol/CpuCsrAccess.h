/** @file
  Header file for IOX access APIs.

  @copyright
  Copyright 2007 - 2019 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _CPU_CSR_ACCESS_H_
#define _CPU_CSR_ACCESS_H_

extern EFI_GUID         gEfiCpuCsrAccessGuid;

/**

  Computes address of CPU Uncore & IIO PCI configuration space using the MMIO mechanism

  @param[in] SocId      - CPU Socket Node number
  @param[in] BoxInst    - Box Instance, 0 based
  @param[in] Offset     - Register offset; values come from the auto generated header file
  @param[in, out] Size  - Ptr to register size in bytes (may be updated if pseudo-offset)

  @retval Address

**/

typedef
UINT64
(EFIAPI *GET_CPU_CSR_ADDRESS) (
  IN UINT8    SocId,
  IN UINT8    BoxInst,
  IN UINT32   Offset,
  IN OUT UINT8 *Size
  );

/**

  Reads CPU Uncore & IIO PCI configuration space using the MMIO mechanism

  @param[in] SocId    - CPU Socket Node number
  @param[in] BoxInst  - Box Instance, 0 based
  @param[in] Offset   - Register offset; values come from the auto generated header file

  @retval Register value

**/

typedef
UINT32
(EFIAPI *READ_CPU_CSR) (
  IN UINT8    SocId,
  IN UINT8    BoxInst,
  IN UINT32   Offset
  );

/**

  Writes CPU Uncore & IIO PCI configuration space using the MMIO mechanism

  @param[in] SocId     - CPU Socket Node number
  @param[in] BoxInst   - Box Instance, 0 based
  @param[in] Offset    - Register offset; values come from the auto generated header file
  @param[in] Data      - Register data to be written

  @retval None

**/

typedef
VOID
(EFIAPI *WRITE_CPU_CSR) (
  IN UINT8    SocId,
  IN UINT8    BoxInst,
  IN UINT32   Offset,
  IN UINT32   Data
  );

/**

  Reads CPU Memory Controller configuration space using the MMIO mechanism

  @param[in] SocId        - Socket ID
  @param[in] McId         - Memory controller ID
  @param[in] Offset       - Register offset; values come from the auto generated header file

  @retval Register value

**/

typedef
UINT32
(EFIAPI *READ_MC_CPU_CSR) (
  IN UINT8    SocId,
  IN UINT8    McId,
  IN UINT32   Offset
  );

/**

  Writes CPU Memory Controller configuration space using the MMIO mechanism

  @param[in] SocId        - Socket ID
  @param[in] McId         - Memory controller ID
  @param[in] RegOffset    - Register offset; values come from the auto generated header file
  @param[in] Data         - Register data to be written

  @retval None

**/

typedef
VOID
(EFIAPI *WRITE_MC_CPU_CSR) (
  IN UINT8    SocId,
  IN UINT8    McId,
  IN UINT32   RegOffset,
  IN UINT32   Data
  );

/**

  Get CPU Memory Controller configuration space address used by MMIO mechanism

  @param[in] SocId        - Socket ID
  @param[in] McId         - Memory controller ID
  @param[in] Offset       - Register offset; values come from the auto generated header file

  @retval MC Register MMCFG address

**/

typedef
UINTN
(EFIAPI *GET_MC_CPU_ADDR) (
  IN UINT8    SocId,
  IN UINT8    McId,
  IN UINT32   RegOffset
  );

/**

  Reads PCI configuration space using the MMIO mechanism

  @param[in] Socket - Socket
  @param[in] Reg    - "Reg" uses the format in the Bus_Dev_Func_CFG.H files

  @retval Value in requested reg

**/

typedef
UINT32
(EFIAPI *READ_PCI_CSR) (
  IN UINT8    Socket,
  IN UINT32   Reg
  );

/**

  Writes specified data to PCI configuration space using the MMIO mechanism

  @param[in] Socket - Socket
  @param[in] Reg    - "Reg" uses the format in the Bus_Dev_Func_CFG.H files
  @param[in] Data   - Value to write

  @retval VOID

**/

typedef
VOID
(EFIAPI *WRITE_PCI_CSR) (
  IN UINT8    Socket,
  IN UINT32   Reg,
  IN UINT32   Data
  );

/**

  Get PCI configuration space address used MMIO mechanism

  @param[in] Socket - Socket
  @param[in] Reg    - "Reg" uses the format in the Bus_Dev_Func_CFG.H files

  @retval Address of requested reg

**/

typedef
UINT32
(EFIAPI *GET_PCI_CSR_ADDR) (
  IN UINT8    Socket,
  IN UINT32   Reg
  );

/**

   Writes the given command to BIOS to PCU Mailbox Interface CSR register

  @param[in] Socket   - CPU Socket number
  @param[in] Command  - Pcu mailbox command to write
  @param[in] Data     - Pcu mailbox data

  @retval error code from the Pcu mailbox (0 = NO ERROR)

**/

typedef
UINT64
(EFIAPI *BIOS_2_VCODE_MAILBOX_WRITE) (
  IN UINT8  Socket,
  IN UINT32 Command,
  IN UINT32 Data
  );

/**

  Writes the checkpoint code to the checkpoint CSR and breaks if match with debug breakpoint
  @param[in] Socket    - Socket to write
  @param[in] majorCode - Major Checkpoint code to write
  @param[in] minorCode - Minor Checkpoint code to write
  @param[in] data      - Data specific to the minor checkpoint is written to
                         low word of the checkpoint CSR

  @retval VOID

**/

typedef
VOID
(EFIAPI *BREAK_AT_CHECK_POINT) (
  IN UINT8    Socket,
  IN UINT8    MajorCode,
  IN UINT8    MinorCode,
  IN UINT16   Data
  );

typedef struct _EFI_CPU_CSR_ACCESS_PROTOCOL {
  GET_CPU_CSR_ADDRESS         GetCpuCsrAddress;
  READ_CPU_CSR                ReadCpuCsr;
  WRITE_CPU_CSR               WriteCpuCsr;
  BIOS_2_VCODE_MAILBOX_WRITE  Bios2VcodeMailBoxWrite;
  READ_MC_CPU_CSR             ReadMcCpuCsr;
  WRITE_MC_CPU_CSR            WriteMcCpuCsr;
  GET_MC_CPU_ADDR             GetMcCpuCsrAddress;
  READ_PCI_CSR                ReadPciCsr;
  WRITE_PCI_CSR               WritePciCsr;
  GET_PCI_CSR_ADDR            GetPciCsrAddress;
  BREAK_AT_CHECK_POINT        BreakAtCheckpoint;
} EFI_CPU_CSR_ACCESS_PROTOCOL;

#endif // _CPU_CSR_ACCESS_H_

