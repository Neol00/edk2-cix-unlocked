/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 **/

EFI_STATUS
UpdateDtb (
  VOID   *fdt,
  UINTN  *ImageSize
  );

VOID
DumpDtbStatus (
  VOID  *fdt
  );

UINT32
fdt_check_header_ext (
  VOID  *fdt
  );
