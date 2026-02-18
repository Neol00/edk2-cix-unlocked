/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 **/

EFI_STATUS
UpdateDtbSi (
  VOID   *fdt,
  UINTN  *ImageSize
  );

VOID
DumpDtbStatusSi (
  VOID  *fdt
  );

UINT32
fdt_check_header_ext_si (
  VOID  *fdt
  );
