/** @file
  The SPD Access Library API provides the necessary functions to initiate SPD
  read/write transactions.

  This API is designed to function as an interface between an agent that needs
  to read/write to a DIMM SPD and a lower level library (such as an SMBus library)
  which handles the actual transactions.  The read/write functions accept DIMM
  location information as well as the SPD byte offset and should then handle
  the steps necessary to initiate (for example) a SMBus transaction to do the
  reading/writing.  Functions are also provided to initialize any data/setup
  steps needed before attempting a read/write transaction and to communicate to
  the library that DIMM detection is complete providing a way for the library
  to know that it can check for a DIMM's presence bofore initiating a transaction.

  @copyright
  Copyright 2018 - 2020 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _SPD_ACCESS_LIB_H_
#define _SPD_ACCESS_LIB_H_

//
// DDR Technology supported
//
typedef enum {
  Ddr4Type = 0,         // DDR4 Technology support
  DdrMaxType            // Enum limit to check valid value
} DDR_TECHNOLOGY_TYPE;

#endif // #ifndef _SPD_ACCESS_LIB_H_
