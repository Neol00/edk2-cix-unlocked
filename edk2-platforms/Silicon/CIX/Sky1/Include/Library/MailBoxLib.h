/** @file

  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _MAIL_BOX_LIB_H_
#define _MAIL_BOX_LIB_H_

#define MAX_TRNG_SIZE  64

typedef enum _MBOX_CHANNEL {
  MBOX_FAST_CHANNEL_0 =  0,
  MBOX_FAST_CHANNEL_1 =  1,
  MBOX_FAST_CHANNEL_2 =  2,
  MBOX_FAST_CHANNEL_3 =  3,
  MBOX_FAST_CHANNEL_4 =  4,
  MBOX_FAST_CHANNEL_5 =  5,
  MBOX_FAST_CHANNEL_6 =  6,
  MBOX_FAST_CHANNEL_7 =  7,
  MBOX_DB_CHANNEL     =  8,
  MBOX_FIFO_CHANNEL   =  9,
  MBOX_REG_CHANNEL    =  10,
} MBOX_CHANNEL;

typedef enum _MBOX_COMMAND {
  MBOX_ECHO_REQUEST          = 0x82000001,
  MBOX_GET_FW_VERSION        = 0x8200000F,
  MBOX_GET_TRNG              = 0x82000010,
  MBOX_ENABLE_GASKET_FENCING = 0x82000012,
} MBOX_COMMAND;

typedef enum _GASKET_FENCE_RANGE_ID {
  FCH_XSPI_RANGE_ID = 0x01,
  MAX_RANGE_ID      = 0Xff,
} GASKET_FENCE_RANGE_ID;

#pragma pack(1)

typedef struct _MBOX_MESSAGE {
  UINT32    Size     : 7;
  UINT32    Type     : 3;
  UINT32    Reserved : 22;
  UINT32    Command;
  UINT32    Data[30];
} MBOX_MESSAGE;

typedef struct _MBOX_FW_VERSION_RESPONSE {
  UINT32    ErrCode;
  UINT32    VerMaj;
  UINT32    VerMin;
} MBOX_FW_VERSION_RESPONSE;

typedef struct _MBOX_TRNG_PARAMETER {
  UINT32    TrngLen;
} MBOX_TRNG_PARAMETER;

typedef struct _MBOX_TRNG_RESPONSE {
  UINT32    ErrCode;
  UINT32    TrngLen;
  UINT8     Trng[MAX_TRNG_SIZE];
} MBOX_TRNG_RESPONSE;

typedef struct _MBOX_GASKET_FENCING_PARAMETER {
  UINT32    Id       : 8;
  UINT32    Reserved : 24;
} MBOX_GASKET_FENCING_PARAMETER;

typedef struct _MBOX_GASKET_FENCING_RESPONSE {
  UINT32    ErrCode;
  UINT32    Reserved;
} MBOX_GASKET_FENCING_RESPONSE;

#pragma pack()

EFI_STATUS
EFIAPI
MBox2SendData (
  IN MBOX_CHANNEL  Channel,
  IN UINT32        *Data
  );

EFI_STATUS
EFIAPI
MBox2ReceiveData (
  IN  MBOX_CHANNEL  Channel,
  OUT UINT32        *Data
  );

EFI_STATUS
EFIAPI
MBoxEchoRequest (
  VOID
  );

EFI_STATUS
EFIAPI
MboxGetFwVersion (
  VOID  *FwVersion
  );

EFI_STATUS
EFIAPI
MboxGetTrng (
  MBOX_TRNG_PARAMETER  *Params,
  MBOX_TRNG_RESPONSE   *Respon
  );

EFI_STATUS
EFIAPI
MboxEnableGasketFencing (
  MBOX_GASKET_FENCING_PARAMETER  *Params,
  MBOX_GASKET_FENCING_RESPONSE   *Respon
  );

#endif /* _MAIL_BOX_LIB_H_ */
