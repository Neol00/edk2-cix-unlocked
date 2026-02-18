/** @file
  Provides services to send progress/error codes to a POST card.

Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __POST_CODE_LIB_H__
#define __POST_CODE_LIB_H__
// #include "../../Library/CixBasePostCodeLibMmio/rlog.h"

#define POST_CODE_PROPERTY_POST_CODE_ENABLED              0x00000008
#define POST_CODE_PROPERTY_POST_CODE_DESCRIPTION_ENABLED  0x00000010

typedef enum {
  PrePiStart                         = 0xE006,
  PrePiEnd                           = 0xE007,
  DxeMainStart                       = 0xE100,
  DxeCoreDispatcherStart             = 0xE101,
  SerialDxeStart                     = 0xE109,
  SerialDxeEnd                       = 0xE10A,
  ArmGicDxeStart                     = 0xE10B,
  ArmGicDxeEnd                       = 0xE10C,
  ArmCpuDxeStart                     = 0xE10D,
  ArmCpuDxeEnd                       = 0xE10E,
  ArmTimerDxeStart                   = 0xE10F,
  ArmTimerDxeEnd                     = 0xE110,
  BdsDxeStart                        = 0xE111,
  BdsDxeEnd                          = 0xE112,
  ConSplitterDxeStart                = 0xE113,
  ConSplitterDxeEnd                  = 0xE114,
  PlatformDxeStart                   = 0xE115,
  PlatformDxeEnd                     = 0xE116,
  PlatformConfigParamsManageDxeStart = 0xE119,
  PlatformConfigParamsManageDxeEnd   = 0xE11A,
  ConfigParamsManageDxeStart         = 0xE117,
  ConfigParamsManageDxeEnd           = 0xE118,
  PcieInitDxeStart                   = 0xE2C1,
  PcieInitDxeEnd                     = 0xE300,
  GmacInitDxeStart                   = 0xE381,
  GmacInitDxeEnd                     = 0xE3C0,
  I2cInitDxeStart                    = 0xE201,
  I2cInitDxeEnd                      = 0xE240,
  SpiInitDxeStart                    = 0xE241,
  SpiInitDxeEnd                      = 0xE280,
  XspiInitDxeStart                   = 0xE3C1,
  XspiInitDxeEnd                     = 0xE400,
  HdaInitDxeStart                    = 0xE401,
  HdaInitDxeEnd                      = 0xE440,
  PdDxeStart                         = 0xE6C1,
  PdDxeEnd                           = 0xE700,
  GpioInterruptDxeStart              = 0xE281,
  GpioInterruptDxeEnd                = 0xE2C0,
  UsbStart                           = 0xE301,
  CdnspDxeStart                      = 0xE302,
  CdnspDxeEnd                        = 0xE303,
  UsbDeviceControlDxeStart           = 0xE304,
  UsbDeviceControlDxeEnd             = 0xE305,
  UsbEnd                             = 0xE340,
  DxeCoreDispatcherEnd               = 0xE1FE,
  DxeMainEnd                         = 0xE1FF,

  BdsStart       = 0xE541,
  BootLogo       = 0xE543,
  BMAfterConsole = 0xE550,
  DxeReadyToBoot = 0xE580,
} POST_CODE_KEY;

/**
  Sends a 32-bit value to a POST card.

  Sends the 32-bit value specified by Value to a POST card, and returns Value.
  Some implementations of this library function may perform I/O operations
  directly to a POST card device.  Other implementations may send Value to
  ReportStatusCode(), and the status code reporting mechanism will eventually
  display the 32-bit value on the status reporting device.

  PostCode() must actively prevent recursion.  If PostCode() is called while
  processing another Post Code Library function, then
  PostCode() must return Value immediately.

  @param  Value  The 32-bit value to write to the POST card.

  @return The 32-bit value to write to the POST card.

**/
UINT32
EFIAPI
PostCode (
  IN POST_CODE_KEY  Value
  );

/**
  Sends a 32-bit value to a POST and associated ASCII string.

  Sends the 32-bit value specified by Value to a POST card, and returns Value.
  If Description is not NULL, then the ASCII string specified by Description is
  also passed to the handler that displays the POST card value.  Some
  implementations of this library function may perform I/O operations directly
  to a POST card device.  Other implementations may send Value to ReportStatusCode(),
  and the status code reporting mechanism will eventually display the 32-bit
  value on the status reporting device.

  PostCodeWithDescription()must actively prevent recursion.  If
  PostCodeWithDescription() is called while processing another any other Post
  Code Library function, then PostCodeWithDescription() must return Value
  immediately.

  @param  Value        The 32-bit value to write to the POST card.
  @param  Description  Pointer to an ASCII string that is a description of the
                       POST code value.  This is an optional parameter that may
                       be NULL.

  @return The 32-bit value to write to the POST card.

**/
UINT32
EFIAPI
PostCodeWithDescription (
  IN UINT32       Value,
  IN CONST CHAR8  *Description  OPTIONAL
  );

/**
  Returns TRUE if POST Codes are enabled.

  This function returns TRUE if the POST_CODE_PROPERTY_POST_CODE_ENABLED
  bit of PcdPostCodePropertyMask is set.  Otherwise FALSE is returned.

  @retval  TRUE   The POST_CODE_PROPERTY_POST_CODE_ENABLED bit of
                  PcdPostCodeProperyMask is set.
  @retval  FALSE  The POST_CODE_PROPERTY_POST_CODE_ENABLED bit of
                  PcdPostCodeProperyMask is clear.

**/
BOOLEAN
EFIAPI
PostCodeEnabled (
  VOID
  );

/**
  Returns TRUE if POST code descriptions are enabled.

  This function returns TRUE if the POST_CODE_PROPERTY_POST_CODE_DESCRIPTION_ENABLED
  bit of PcdPostCodePropertyMask is set.  Otherwise FALSE is returned.

  @retval  TRUE   The POST_CODE_PROPERTY_POST_CODE_DESCRIPTION_ENABLED bit of
                  PcdPostCodeProperyMask is set.
  @retval  FALSE  The POST_CODE_PROPERTY_POST_CODE_DESCRIPTION_ENABLED bit of
                  PcdPostCodeProperyMask is clear.

**/
BOOLEAN
EFIAPI
PostCodeDescriptionEnabled (
  VOID
  );

/**
  Sends a 32-bit value to a POST card.

  If POST codes are enabled in PcdPostCodeProperyMask, then call PostCode()
  passing in Value.  Value is returned.

  @param  Value  The 32-bit value to write to the POST card.

  @return  Value The 32-bit value to write to the POST card.

**/
#define POST_CODE(Value)  PostCodeEnabled() ? PostCode(Value) : Value

/**
  Sends a 32-bit value to a POST and associated ASCII string.

  If POST codes and POST code descriptions are enabled in
  PcdPostCodeProperyMask, then call PostCodeWithDescription() passing in
  Value and Description.  If only POST codes are enabled, then call PostCode()
  passing in Value.  Value is returned.

  @param  Value        The 32-bit value to write to the POST card.
  @param  Description  Pointer to an ASCII string that is a description of the
                       POST code value.

  @return Value        The 32-bit value to write to the POST card.
**/
#define POST_CODE_WITH_DESCRIPTION(Value, Description)  \
  PostCodeEnabled()                              ?     \
    (PostCodeDescriptionEnabled()                ?     \
      PostCodeWithDescription(Value,Description) :     \
      PostCode(Value))                           :     \
    Value

#endif

VOID
TimeStampPrint (
  );
