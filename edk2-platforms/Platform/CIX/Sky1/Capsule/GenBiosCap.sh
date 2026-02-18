#!/bin/bash
#  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
cd ../../../../../
export BOARD_NAME=Capsule
export WORKSPACE=$PWD
export GEN_CAP_PATH=$WORKSPACE/edk2-platforms/Platform/CIX/Sky1/$BOARD_NAME
export KEY_PATH=$WORKSPACE/edk2-platforms/Platform/CIX/Sky1/Bin
export FW_VERSION=$2
export LOWEST_SUPPORTED_VERSION=0
echo WORKSPACE=$WORKSPACE



if [ ! -n "$1" ] || [ "$1" == "?" ] || [ ! -n "$2" ]; then
if [ "$1" != "?" ]; then
  echo "Parameters error."
fi
  echo "Usage:"
  echo "  $0 ?"
  echo "  $0 filename version"
  echo "Examples:"
  echo "  $0 cix_flash_all.bin 5"

else

cd $WORKSPACE/edk2/BaseTools/Source/Python
python3 -m Capsule.GenerateCapsule -e -o $GEN_CAP_PATH/$1.cap --guid 351c9808-4b53-4363-9314-d339386e5685 \
--hardware-instance 0 --monotonic-count 1 --fw-version $FW_VERSION --lsv $LOWEST_SUPPORTED_VERSION \
--signer-private-cert $KEY_PATH/NewCert003.pem --other-public-cert $KEY_PATH/NewSub.pub.pem \
--trusted-public-cert $KEY_PATH/ntroot.pub.pem -v --update-image-index 1 $GEN_CAP_PATH/$1

fi


