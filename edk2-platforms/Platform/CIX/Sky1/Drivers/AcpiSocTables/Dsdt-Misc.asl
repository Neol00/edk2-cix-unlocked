/** @file

  Copyright 2024 Cix Technology (Shanghai) Co., Ltd. All Rights Reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define HW_MUTEX_BASE_ADDRESS       0x06510900
#define HW_MUTEX_BASE_SIZE          0x400
#define HW_MUTEX_I2C_OFFSET         0x120
#define HW_MUTEX_MAX_TIMEOUT_RETRY  500
#define HW_MUTEX_DELAY_COUNT        10000
#define HW_MUTEX_SUCCESS            0
#define HW_MUTEX_FAIL               1
#define UNDEFINED_MUTEX_ID          0xFF

OperationRegion (IMTX, SystemMemory, HW_MUTEX_BASE_ADDRESS, HW_MUTEX_BASE_SIZE)
Field (IMTX, DWordAcc, NoLock, Preserve)
{
  Offset(0x120),
  IMX0, 32,
  IMX1, 32,
  IMX2, 32,
  IMX3, 32,
  IMX4, 32,
  IMX5, 32,
  IMX6, 32,
  IMX7, 32,
}

//
// Name: GMTX
// Description: Get value from specific hardware mutex id
// Input: Arg0: mutex id
// Output: Value of mutex
// Notes: mutex for i2c bus
//
Method(GMTX,1, Serialized) {
  Local0=0
	Switch(Arg0) {
		case (0x48) {
      Local0=IMX0
		}
		case (0x49) {
			Local0=IMX1
		}
		case (0x4A) {
			Local0=IMX2
		}
		case (0x4B) {
			Local0=IMX3
		}
		case (0x4C) {
			Local0=IMX4
		}
		case (0x4D) {
			Local0=IMX5
		}
		case (0x4E) {
			Local0=IMX6
		}
		case (0x4F) {
			Local0=IMX7
		}
	}
  Return(Local0)
}

//
// Name: SMTX
// Description: Set value for specific hardware mutex id
// Input:
//       Arg0: mutex id
//       Arg1: value
// Output:
// Notes: mutex for i2c bus
//
Method(SMTX,2, Serialized) {
	Switch(Arg0) {
		case (0x48) {
      IMX0=Arg1
		}
		case (0x49) {
			IMX1=Arg1
		}
		case (0x4A) {
			IMX2=Arg1
		}
		case (0x4B) {
			IMX3=Arg1
		}
		case (0x4C) {
			IMX4=Arg1
		}
		case (0x4D) {
			IMX5=Arg1
		}
		case (0x4E) {
			IMX6=Arg1
		}
		case (0x4F) {
			IMX7=Arg1
		}
	}
}

//
// Name: AMTX
// Description: Acquire hardware mutex
// Input:
//       Arg0: mutex id
//       Arg1: instance id
// Output:Status(No zero may have some error in it)
// Notes: mutex for i2c bus
//
Method(AMTX,2, Serialized) {
  if (Arg0 == UNDEFINED_MUTEX_ID){
    Return(HW_MUTEX_SUCCESS)
  }
  if (GMTX(Arg0) == Arg1){
    Return(HW_MUTEX_FAIL)
  }
  Local0 = HW_MUTEX_MAX_TIMEOUT_RETRY
  While((GMTX(Arg0) != Arg1) && Local0){
    SMTX(Arg0,Arg1)
    Local1 = HW_MUTEX_DELAY_COUNT
    While(Local1){
      Local1 --
    }
    Local0--
  }
  if (Local0 == 0){
    Return(HW_MUTEX_FAIL)
  }
  Return(HW_MUTEX_SUCCESS)
}

//
// Name: RMTX
// Description: Release hardware mutex
// Input:
//       Arg0: mutex id
//       Arg1: instance id
// Output:Status(No zero may have some error in it)
// Notes: mutex for i2c bus
//
Method(RMTX,2, Serialized) {
  if (Arg0 == UNDEFINED_MUTEX_ID){
    Return(HW_MUTEX_SUCCESS)
  }
  if (GMTX(Arg0) == 0){
    Return(HW_MUTEX_FAIL)
  }
  if (GMTX(Arg0) != Arg1){
    Return(HW_MUTEX_FAIL)
  }
  Local0 = HW_MUTEX_MAX_TIMEOUT_RETRY
  While((GMTX(Arg0) == Arg1) && Local0){
    SMTX(Arg0,Arg1)
    Local2 = HW_MUTEX_DELAY_COUNT
    While(Local2){
      Local2--
    }
    Local0--
  }
  if (Local0 == 0){
    Return(HW_MUTEX_FAIL)
  }
  Return(HW_MUTEX_SUCCESS)
}
