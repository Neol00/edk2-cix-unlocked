/** @file
  This is to publish the SMM Control Ppi instance.

  Copyright (c) 2019 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>

#include <Ppi/MmControl.h>
#include <IndustryStandard/Pci30.h>

#define SMM_CONTROL_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('i', '4', 's', 'c')

typedef struct {
  UINTN                           Signature;
  EFI_HANDLE                      Handle;
  EFI_PEI_MM_CONTROL_PPI          SmmControl;
} SMM_CONTROL_PRIVATE_DATA;

#define SMM_CONTROL_PRIVATE_DATA_FROM_THIS(a) \
        CR (a, \
          SMM_CONTROL_PRIVATE_DATA, \
          SmmControl, \
          SMM_CONTROL_DEV_SIGNATURE \
      )

//
// Common registers:
//
//
// APM Registers
//
#define R_PCH_APM_CNT                             0xB2
//
// ACPI and legacy I/O register offsets from ACPIBASE
//
#define R_PCH_ACPI_PM1_STS                        0x00
#define B_PCH_ACPI_PM1_STS_PRBTNOR                BIT11

#define R_PCH_SMI_EN                              0x30

#define R_PCH_SMI_STS                             0x34
#define B_PCH_SMI_STS_APM                         BIT5
#define B_PCH_SMI_EN_APMC                         BIT5
#define B_PCH_SMI_EN_EOS                          BIT1
#define B_PCH_SMI_EN_GBL_SMI                      BIT0

/**
  Trigger the software SMI

  @param[in] Data                 The value to be set on the software SMI data port

  @retval EFI_SUCCESS             Function completes successfully
**/
EFI_STATUS
EFIAPI
SmmTrigger (
  UINT8   Data
  )
{
  UINT16  ABase;
  UINT32  OutputData;
  UINT32  OutputPort;

  ABase = FixedPcdGet16 (PcdAcpiBaseAddress);

  ///
  /// Enable the APMC SMI
  ///
  OutputPort  = ABase + R_PCH_SMI_EN;
  OutputData  = IoRead32 ((UINTN) OutputPort);
  OutputData |= (B_PCH_SMI_EN_APMC | B_PCH_SMI_EN_GBL_SMI);
  DEBUG (
    (DEBUG_EVENT,
     "The SMI Control Port at address %x will be written to %x.\n",
     OutputPort,
     OutputData)
    );
  IoWrite32 (
    (UINTN) OutputPort,
    (UINT32) (OutputData)
    );

  OutputPort  = R_PCH_APM_CNT;
  OutputData  = Data;

  ///
  /// Generate the APMC SMI
  ///
  IoWrite8 (
    (UINTN) OutputPort,
    (UINT8) (OutputData)
    );

  return EFI_SUCCESS;
}

/**
  Clear the SMI status


  @retval EFI_SUCCESS             The function completes successfully
  @retval EFI_DEVICE_ERROR        Something error occurred
**/
EFI_STATUS
EFIAPI
SmmClear (
  VOID
  )
{
  UINT16  ABase;
  UINT32  OutputData;
  UINT32  OutputPort;

  ABase = FixedPcdGet16 (PcdAcpiBaseAddress);

  ///
  /// Clear the Power Button Override Status Bit, it gates EOS from being set.
  ///
  OutputPort  = ABase + R_PCH_ACPI_PM1_STS;
  OutputData  = B_PCH_ACPI_PM1_STS_PRBTNOR;
  DEBUG (
    (DEBUG_EVENT,
     "The PM1 Status Port at address %x will be written to %x.\n",
     OutputPort,
     OutputData)
    );
  IoWrite16 (
    (UINTN) OutputPort,
    (UINT16) (OutputData)
    );

  ///
  /// Clear the APM SMI Status Bit
  ///
  OutputPort  = ABase + R_PCH_SMI_STS;
  OutputData  = B_PCH_SMI_STS_APM;
  DEBUG (
    (DEBUG_EVENT,
     "The SMI Status Port at address %x will be written to %x.\n",
     OutputPort,
     OutputData)
    );
  IoWrite32 (
    (UINTN) OutputPort,
    (UINT32) (OutputData)
    );

  ///
  /// Set the EOS Bit
  ///
  OutputPort  = ABase + R_PCH_SMI_EN;
  OutputData  = IoRead32 ((UINTN) OutputPort);
  OutputData |= B_PCH_SMI_EN_EOS;
  DEBUG (
    (DEBUG_EVENT,
     "The SMI Control Port at address %x will be written to %x.\n",
     OutputPort,
     OutputData)
    );
  IoWrite32 (
    (UINTN) OutputPort,
    (UINT32) (OutputData)
    );

  ///
  /// There is no need to read EOS back and check if it is set.
  /// This can lead to a reading of zero if an SMI occurs right after the SMI_EN port read
  /// but before the data is returned to the CPU.
  /// SMM Dispatcher should make sure that EOS is set after all SMI sources are processed.
  ///
  return EFI_SUCCESS;
}

/**
  This routine generates an SMI

  @param[in] This                       The EFI SMM Control protocol instance
  @param[in, out] ArgumentBuffer        The buffer of argument
  @param[in, out] ArgumentBufferSize    The size of the argument buffer
  @param[in] Periodic                   Periodic or not
  @param[in] ActivationInterval         Interval of periodic SMI

  @retval EFI Status                    Describing the result of the operation
  @retval EFI_INVALID_PARAMETER         Some parameter value passed is not supported
**/
EFI_STATUS
EFIAPI
Activate (
  IN EFI_PEI_SERVICES        **PeiServices,
  IN EFI_PEI_MM_CONTROL_PPI  * This,
  IN OUT INT8                *ArgumentBuffer OPTIONAL,
  IN OUT UINTN               *ArgumentBufferSize OPTIONAL,
  IN BOOLEAN                 Periodic OPTIONAL,
  IN UINTN                   ActivationInterval OPTIONAL
  )
{
  EFI_STATUS  Status;
  UINT8       Data;

  if (Periodic) {
    DEBUG ((DEBUG_WARN, "Invalid parameter\n"));
    return EFI_INVALID_PARAMETER;
  }

  // NOTE: Copied from Quark. Matches the usage in PiSmmCommunicationPei
  if (ArgumentBuffer == NULL) {
    Data = 0xFF;
  } else {
    if (ArgumentBufferSize == NULL || *ArgumentBufferSize != 1) {
      return EFI_INVALID_PARAMETER;
    }

    Data = *ArgumentBuffer;
  }
  ///
  /// Clear any pending the APM SMI
  ///
  Status = SmmClear ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return SmmTrigger (Data);
}

/**
  This routine clears an SMI

  @param[in] This                 The EFI SMM Control protocol instance
  @param[in] Periodic             Periodic or not

  @retval EFI Status              Describing the result of the operation
  @retval EFI_INVALID_PARAMETER   Some parameter value passed is not supported
**/
EFI_STATUS
EFIAPI
Deactivate (
  IN EFI_PEI_SERVICES        **PeiServices,
  IN EFI_PEI_MM_CONTROL_PPI  * This,
  IN BOOLEAN                 Periodic OPTIONAL
  )
{
  if (Periodic) {
    return EFI_INVALID_PARAMETER;
  }

  return SmmClear ();
}

/**
  This function is to install an SMM Control PPI
  - <b>Introduction</b> \n
    An API to install an instance of EFI_PEI_MM_CONTROL_PPI. This PPI provides a standard
    way for other modules to trigger software SMIs.

    @retval EFI_SUCCESS           - Ppi successfully started and installed.
    @retval EFI_NOT_FOUND         - Ppi can't be found.
    @retval EFI_OUT_OF_RESOURCES  - Ppi does not have enough resources to initialize the driver.
**/
EFI_STATUS
EFIAPI
PeiInstallSmmControlPpi (
  VOID
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_PPI_DESCRIPTOR          *PpiList;
  SMM_CONTROL_PRIVATE_DATA        *SmmControlPrivate;

  //
  // Initialize private data
  //
  SmmControlPrivate  = AllocateZeroPool (sizeof (*SmmControlPrivate));
  ASSERT (SmmControlPrivate != NULL);
  if (SmmControlPrivate == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  PpiList           = AllocateZeroPool (sizeof (*PpiList));
  ASSERT (PpiList != NULL);
  if (PpiList == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SmmControlPrivate->Signature = SMM_CONTROL_PRIVATE_DATA_SIGNATURE;
  SmmControlPrivate->Handle    = NULL;

  SmmControlPrivate->SmmControl.Trigger  = Activate;
  SmmControlPrivate->SmmControl.Clear    = Deactivate;

  //
  // Install PPI
  //
  PpiList->Flags  = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PpiList->Guid   = &gEfiPeiMmControlPpiGuid;
  PpiList->Ppi    = &SmmControlPrivate->SmmControl;

  Status          = PeiServicesInstallPpi (PpiList);
  ASSERT_EFI_ERROR (Status);

  // Unlike driver, do not disable SMIs as S3 resume continues
  return EFI_SUCCESS;
}
