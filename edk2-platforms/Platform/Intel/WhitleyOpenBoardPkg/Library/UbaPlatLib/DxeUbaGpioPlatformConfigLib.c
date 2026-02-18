/** @file

  @copyright
  Copyright 2012 - 2017 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/UbaCfgDb.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/DynamicSiLibraryProtocol.h>

//
// UBA and GPIO headers
//

#include <Library/UbaGpioPlatformConfig.h>
#include <Library/GpioLib.h>

STATIC PLATFORM_GPIO_CONFIG_TABLE             mGpioParams;
DYNAMIC_SI_LIBARY_PROTOCOL                    *mDynamicSiLibraryProtocol = NULL;

/**
  The library constructor call. Gets required protocols and stores for later usage
  This also applies for SMM mode usage

  @param[in]  None

  @retval EFI_SUCCESS             The function completed successfully

**/
EFI_STATUS
EFIAPI
InitializeDxeUbaPlatLib (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                            Status;
  UBA_CONFIG_DATABASE_PROTOCOL          *UbaConfigProtocol = NULL;
  UINTN                                 TableSize;

  Status = gBS->LocateProtocol (&gDynamicSiLibraryProtocolGuid, NULL, (VOID **) &mDynamicSiLibraryProtocol);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return EFI_NOT_FOUND;
  }

    Status = gBS->LocateProtocol (
                    &gUbaConfigDatabaseProtocolGuid,
                    NULL,
                    (VOID **) &UbaConfigProtocol
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

  TableSize = sizeof (PLATFORM_GPIO_CONFIG_TABLE);
  Status = UbaConfigProtocol->GetData (
                                UbaConfigProtocol,
                                &gPlatformGpioPlatformConfigDataGuid,
                                &mGpioParams,
                                &TableSize
                                );

  return Status;

}

/**
  Reads GPIO pin to get DFX jumper status

  @param[out] DfxJumper - The pointer to the DFX jumper input

  @retval Status - Success if GPIO's are read properly

**/
EFI_STATUS
GpioGetDfxPadVal (
  OUT UINT32 *DfxJumper
  )
{
  EFI_STATUS           Status;

  if (mGpioParams.ReservedM == UNUSED_GPIO) {
    return EFI_UNSUPPORTED;
  }

  Status = mDynamicSiLibraryProtocol->GpioGetInputValue (mGpioParams.ReservedM, DfxJumper);
  return Status;
}

/**
  Reads GPIO pin to get recovery jumper status

  @param[out] RcvJumper - The pointer to the Recovery jumper input

  @retval Status - Success if GPIO's are read properly

**/
EFI_STATUS
GpioGetRcvPadVal (
  OUT UINT32 *RcvJumper
  )
{
  EFI_STATUS           Status;

  if (mGpioParams.RcvJumper == UNUSED_GPIO) {
    return EFI_UNSUPPORTED;
  }

  Status = mDynamicSiLibraryProtocol->GpioGetInputValue (mGpioParams.RcvJumper, RcvJumper);
  return Status;
}

/**
  Reads GPIO pin to get FM ADR trigger pin

  @param[out] FmAdrTrigger - The pointer to the ADR trigger input

  @retval Status - Success if GPIO's are read properly

**/
EFI_STATUS
GpioGetFmAdrTriggerPadVal (
  OUT UINT32 *FmAdrTrigger
  )
{
  EFI_STATUS           Status;

  if (mGpioParams.FmAdrTrigger == UNUSED_GPIO) {
    return EFI_UNSUPPORTED;
  }

  Status = mDynamicSiLibraryProtocol->GpioGetInputValue (mGpioParams.FmAdrTrigger, FmAdrTrigger);
  return Status;
}

/**
  Sets GPIO pin to enable ADR on the board

  @param Set[in] - If TRUE means the pas should go 'high', otherwise 'low'

  @retval Status - Success if GPIO set properly

**/
EFI_STATUS
GpioSetAdrEnablePadOutVal (
  IN BOOLEAN Set
  )
{
  EFI_STATUS           Status;

  if (mGpioParams.AdrEnable == UNUSED_GPIO) {
    return EFI_UNSUPPORTED;
  }

  if (Set) {
    Status = mDynamicSiLibraryProtocol->GpioSetOutputValue (mGpioParams.AdrEnable, GpioOutHigh);
  } else {
    Status = mDynamicSiLibraryProtocol->GpioSetOutputValue (mGpioParams.AdrEnable, GpioOutLow);
  }
  return Status;
}

/**
  Reads GPIO pin to Force to S1 config mode pad

  @param[out] ForceS1ConfigPad - Input value of the Force S1 Config pad

  @retval Status - Success if GPIO's are read properly

**/
EFI_STATUS
GpioGetForcetoS1ConfigModePadVal (
  OUT UINT32 *ForceS1ConfigPad
  )
{
  EFI_STATUS           Status;

  if (mGpioParams.ForceTo1SConfigModePad == UNUSED_GPIO) {
    return EFI_UNSUPPORTED;
  }

  Status = mDynamicSiLibraryProtocol->GpioGetInputValue (mGpioParams.ForceTo1SConfigModePad, ForceS1ConfigPad);
  return Status;
}

/**
  Reads GPIO pin related to QAT

  @param[out] QATPad - Input value of the QAT pad

  @retval Status - Success if GPIO's are read properly

**/
EFI_STATUS
GpioGetQATPadVal (
  OUT UINT32 *QATPad
  )
{
  EFI_STATUS           Status;

  if (mGpioParams.QATGpio == UNUSED_GPIO) {
    return EFI_UNSUPPORTED;
  }

  Status = mDynamicSiLibraryProtocol->GpioGetInputValue (mGpioParams.QATGpio, QATPad);
  return Status;
}

/**
  Get GPIO pin for SCI detection for WHEA RAS functionality

  @param[out] WheaSciPad - Input value of the Whea SCI pad

  @retval Status - Success if GPIO's pad read properly

**/
EFI_STATUS
GpioGetWheaSciPad (
  OUT UINT32 *WheaSciPad
  )
{
  if (mGpioParams.WheaSciPad == UNUSED_GPIO) {
    return EFI_UNSUPPORTED;
  }

  *WheaSciPad = (UINT32) mGpioParams.WheaSciPad;
  return EFI_SUCCESS;
}

/**
  Get GPIO pin for FPGA error detection RAS functionality

  @param[out] FpgaErrorPad -The input value of the FPGA error 1 pad

  @retval Status - Success if GPIO's pad read properly

**/
EFI_STATUS
GpioGetFpgaErrorPad1 (
  OUT UINT32 *FpgaErrorPad
  )
{
  if (mGpioParams.FpgaErrorSingnalPad1 == UNUSED_GPIO) {
    return EFI_UNSUPPORTED;
  }

  *FpgaErrorPad = (UINT32) mGpioParams.FpgaErrorSingnalPad1;
  return EFI_SUCCESS;
}

/**
  Get GPIO pin for FPGA error detection RAS functionality

  @param[out] FpgaErrorPad -The input value of the FPGA error 2 pad

  @retval Status - Success if GPIO's pad read properly

**/
EFI_STATUS
GpioGetFpgaErrorPad2 (
  OUT UINT32 *FpgaErrorPad
  )
{

  if (mGpioParams.FpgaErrorSingnalPad2 == UNUSED_GPIO) {
    return EFI_UNSUPPORTED;
  }

  *FpgaErrorPad = (UINT32) mGpioParams.FpgaErrorSingnalPad2;
  return EFI_SUCCESS;
}

/**
  Get GPIO pin for CPU HP SMI detection for RAS functionality

  @retval Status - Success if GPIO's pad read properly

**/
EFI_STATUS
GpioGetCpuHpSmiPad (
  OUT UINT32 *CpuHpSmiPad
  )
{

  if (mGpioParams.CpuHpSmiPad == UNUSED_GPIO) {
    return EFI_UNSUPPORTED;
  }

  *CpuHpSmiPad = (UINT32) mGpioParams.CpuHpSmiPad;
  return EFI_SUCCESS;
}

/**
  Reads GPIO pin that is first bit of the Board ID indication word

  @param[out] BoardID0Gpio - Input value of the first Board ID pad

  @retval Status - Success if GPIO's are read properly

**/
EFI_STATUS
GpioGetBoardId0PadVal (
  OUT UINT32 *BoardID0Gpio
  )
{
  EFI_STATUS           Status;

  if (mGpioParams.BoardID0Gpio == UNUSED_GPIO) {
    return EFI_UNSUPPORTED;
  }

  Status = mDynamicSiLibraryProtocol->GpioGetInputValue (mGpioParams.BoardID0Gpio, BoardID0Gpio);
  return Status;
}

/**
  Sets GPIO's used for Boot Mode

  @param None

  @retval Status - Success if GPIO's are configured

**/
EFI_STATUS
GpioConfigForMFGMode (
  VOID
  )
{
  EFI_STATUS                  Status;

  if (mGpioParams.GpioMfgPad.GpioPad == UNUSED_GPIO) {
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "Start ConfigureGpio() for BootMode Detection.\n"));

  Status = mDynamicSiLibraryProtocol->GpioSetPadConfig (mGpioParams.GpioMfgPad.GpioPad,
    &mGpioParams.GpioMfgPad.GpioConfig);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "End ConfigureGpio() for BootMode Detection.\n"));
  return Status;
}

/**
  Checks whether the MDF jumper has been set

  @param None

  @retval ManufacturingMode - TRUE when MFG jumper is on, FALSE otherwise

**/
BOOLEAN
IsManufacturingMode (
  VOID
  )
{
  BOOLEAN ManufacturingMode = TRUE;

  EFI_STATUS Status;
  UINT32 GpiValue;

  if (mGpioParams.GpioMfgPad.GpioPad == UNUSED_GPIO) {
    return FALSE;
  }

  Status = GpioConfigForMFGMode ();
  ASSERT_EFI_ERROR (Status);

  Status = mDynamicSiLibraryProtocol->GpioGetInputValue (mGpioParams.GpioMfgPad.GpioPad, &GpiValue);
  ASSERT_EFI_ERROR (Status);

  if (!GpiValue) {
    ManufacturingMode = FALSE;
  }
  return ManufacturingMode;
}
