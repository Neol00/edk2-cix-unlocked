/** @FdtRw.h
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *  Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef __FDT_RW_H
#define __FDT_RW_H

typedef struct _FDT_FIRST_LEVEL_NODE {
  CONST CHAR8                     *NodeName;
  INT32                           NodeOffset;
  struct _FDT_FIRST_LEVEL_NODE    *Next;
} FDT_FIRST_LEVEL_NODE;

INT32
FdtPathOffset (
  CONST VOID   *Fdt,
  CONST CHAR8  *Path
  );

INT32
FdtGetPropLen (
  VOID         *Fdt,
  INT32        Offset,
  CONST CHAR8  *Name
  );

VOID
FdtUpdateNodeOffsetInList (
  INT32  NodeOffset,
  INT32  DiffLen
  );

INT32
FdtSetProp (
  VOID         *Fdt,
  INT32        Offset,
  CONST CHAR8  *Name,
  CONST VOID   *Val,
  INT32        Len
  );

#define FDT_ALIGN(x, a)  (((x) + (a)-1) & ~((a)-1))
#define FDT_TAGALIGN(x)  (FDT_ALIGN((x), FDT_TAGSIZE))

#define FdtPropUpdateFunc(Fdt, Offset, Name, Val, FdtUpdateFunc, RetValue) \
  do {                                                                     \
    if (FixedPcdGetBool (EnableNewNodeSearchFuc)) {                        \
      INT32 OldLen = FdtGetPropLen (Fdt, Offset, Name);                    \
      RetValue = FdtUpdateFunc (Fdt, Offset, Name, Val);                   \
      if (RetValue == 0) {                                                 \
        INT32 NewLen = FdtGetPropLen (Fdt, Offset, Name);                  \
        if (OldLen == 0 &&                                                 \
            NewLen) {                                                      \
            NewLen = sizeof (struct fdt_property) + FDT_TAGALIGN (NewLen); \
        }                                                                  \
  /* Update the node's offset in the list */                         \
        FdtUpdateNodeOffsetInList (                                        \
           Offset, FDT_TAGALIGN (NewLen) - FDT_TAGALIGN (OldLen));         \
     }                                                                     \
    } else {                                                               \
      RetValue = FdtUpdateFunc (Fdt, Offset, Name, Val);                   \
    }                                                                      \
  } while (0)
#endif
