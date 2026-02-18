/** @file
 *
 *  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved.
 *  Copyright (c) 2020, Andrei Warkentin <andrey.warkentin@gmail.com>
 *  Copyright (c) 2019-2020, Pete Batard <pete@akeo.ie>
 *  Copyright (c) 2016, Linaro Limited. All rights reserved.
 *  Copyright (c) 2011-2020, ARM Limited. All rights reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#include <AsmMacroIoLibV8.h>
#include <Library/ArmLib.h>    
ASM_FUNC (ArmPlatformPeiBootAction)
    ret

//UINTN
//ArmPlatformGetPrimaryCoreMpId (
//  VOID
//  );
ASM_FUNC (ArmPlatformGetPrimaryCoreMpId)
    MOV32  (w0, FixedPcdGet32 (PcdArmPrimaryCore))
    ret

//UINTN
//ArmPlatformIsPrimaryCore (
//  IN UINTN MpId
//  );
ASM_FUNC (ArmPlatformIsPrimaryCore)
    mov   x0, #1
    ret

//UINTN
//ArmPlatformGetCorePosition (
//  IN UINTN MpId
//  );
// With this function: CorePos = ClusterId >> 8
ASM_FUNC (ArmPlatformGetCorePosition)
	and	x1, x0, #ARM_CLUSTER_MASK
	mov	x0, x1, LSR #8
    ret

ASM_FUNCTION_REMOVE_IF_UNREFERENCED
