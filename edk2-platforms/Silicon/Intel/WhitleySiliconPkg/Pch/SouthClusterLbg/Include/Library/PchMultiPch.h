/** @file
  Prototype of the MultiPch library.

  @copyright
  Copyright 2019 - 2021 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _PCH_MULTI_PCH_LIB_H_
#define _PCH_MULTI_PCH_LIB_H_

#include <Ppi/PchPolicy.h>
#include <Library/PchMultiPchBase.h>

#define PCH_IP_INFO_REVISION              1

typedef struct _PCH_IP_INFO {
  /**
    Revision 1:   Original version
  **/
  UINT8                        Revision;

  BOOLEAN                      Valid[PCH_MAX];
  UINT8                        SocketId[PCH_MAX];
  UINT8                        Segment[PCH_MAX];
  UINT8                        Bus[PCH_MAX];
  UINT64                       P2sbBar[PCH_MAX];
  UINT64                       TempBar[PCH_MAX];
  UINT64                       PmcBar[PCH_MAX];
  UINT64                       SpiBar[PCH_MAX];
} PCH_IP_INFO;

#endif // _PCH_MULTI_PCH_LIB_H_
