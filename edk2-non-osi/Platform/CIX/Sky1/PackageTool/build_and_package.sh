#!/usr/bin/env bash

export  BOLD="\e[1m"
export  NORMAL="\e[0m"
export	RED="\e[31m"
export	GREEN="\e[32m"
export	YELLOW="\e[33m"
export  BLUE="\e[94m"
export  CYAN="\e[36m"

export WORKSPACE=$PWD
export PATH_OUT="${WORKSPACE}/output"

export ARM_TOOLCHAIN_ELF="gcc-arm-10.2-2020.11-x86_64-aarch64-none-elf"
export UEFI_PROJECT="Merak"
export UEFI_PROJECT_PATH="Platform/CIX/Sky1"
export GCC5_AARCH64_PREFIX="${WORKSPACE}/tools/gcc/${ARM_TOOLCHAIN_ELF}/bin/aarch64-none-elf-"
export IASL_PREFIX="${WORKSPACE}/tools/acpica/generate/unix/bin/"
export PACKAGES_PATH=$WORKSPACE/edk2:$WORKSPACE/edk2-platforms:$WORKSPACE/edk2-non-osi

exec_blankfile() {
	for ((i=0;i<$2;i++))
	do
		echo -e -n "\xFF" >> $1
	done
}

exec_build_uefi() {

if [ ! -e $WORKSPACE/edk2/MdeModulePkg/Library/BrotliCustomDecompressLib/brotli/.git ]; then
	echo "Need update edk2 submodule!"
	cd $WORKSPACE/edk2
    if [ "${NETWORK}" == "internal" ];then
	    git submodule init
	    git config --list|grep submodule.*url=| grep 'github.com' | sed -e 's#^\(.*url\)=https://github.com/\(.*\)$#git config \1 ssh://git@gitmirror.cixcomputing.com/github_mirror/\2##g'|while read c;do $c;done
    fi
	git submodule update --init
fi

cd $WORKSPACE/edk2-platforms
local COMMIT_HASH=`git rev-parse --short=12 HEAD`

cd $WORKSPACE

if [ ! -e $WORKSPACE/Source/C/bin ]; then
	echo "Need build edk2 basetool!"
	make -C edk2/BaseTools
fi

if [ ! -e $WORKSPACE/tools/acpica/generate/unix/bin ]; then
	echo "Need build acpi tool!"
	make -C tools/acpica
fi

source $WORKSPACE/edk2/edksetup.sh --reconfig

rm -rf ${WORKSPACE}/Build

local BUILD_DATE=`date +%VM%y%m%d%H%M%SN`
local UEFI_TARGET=RELEASE
local BOARD=evb
local UEFI_DSC_FILE="${UEFI_PROJECT_PATH}/${UEFI_PROJECT}/${UEFI_PROJECT}.dsc"
local VARIABLE_TYPE=SPI
local STMM_SUPPORT=TRUE

build -a AARCH64 -t GCC5 -p $UEFI_DSC_FILE -b $UEFI_TARGET -D BOARD_NAME=$BOARD -D BUILD_DATE=$BUILD_DATE -D COMMIT_HASH=$COMMIT_HASH -D SMP_ENABLE=1 -D ACPI_BOOT_ENABLE=1 -D FASTBOOT_LOAD=$FASTBOOT_LOAD -D VARIABLE_TYPE=$VARIABLE_TYPE -D STANDARD_MM=$STMM_SUPPORT

cp Build/${UEFI_PROJECT}/${UEFI_TARGET}_GCC5/FV/SKY1_BL33_UEFI.fd ${PATH_OUT}

}

build_memcfg(){
    echo -e "BUILD MEMCFG $1 Started."
    local memcfg_dir="${PATH_PROJECT}/mem_config"
    local memcfg_file="memory_config.bin"
    local memcfg_target="$1"

    cd $memcfg_dir

    #compile and generate the memory config with the param: $MEM_CONF_FREQ and $MEM_CONF_CH
    make || exit 1

    cd -

    cp $memcfg_dir/$memcfg_file $memcfg_target

    if [[ -e "${memcfg_target}" ]]; then
        echo -e "${GREEN}BUILD MEMCFG to $memcfg_target Success!!${NORMAL}"
    else
        echo -e "${RED}BUILD MEMCFG to $memcfg_target Failed!!${NORMAL}"
        exit 1
    fi
}

build_pmcfg(){
    echo -e "BUILD PMCFG $1 Started."
    local pmcfg_dir="${PATH_PROJECT}/pm_config"
    local pmcfg_file="csu_pm_config.bin"
    local pmcfg_target="$1"

    cd $pmcfg_dir

    #compile and generate the pm config
    make || exit 1

    cd -

    cp $pmcfg_dir/$pmcfg_file $pmcfg_target

    if [[ -e "${pmcfg_target}" ]]; then
        echo -e "${GREEN}BUILD PMCFG to $pmcfg_target Success!!${NORMAL}"
    else
        echo -e "${RED}BUILD PMCFG to $pmcfg_target Failed!!${NORMAL}"
        exit 1
    fi
}

exec_cix_mkimage() {
    export PATH_PACKAGE_TOOL="${WORKSPACE}/edk2-non-osi/Platform/CIX/Sky1/PackageTool"
    export PATH_FIRMARES="${PATH_OUT}/Firmwares"
    export PATH_PROJECT="${WORKSPACE}/edk2-platforms/${UEFI_PROJECT_PATH}/${UEFI_PROJECT}"

    # copy require files to output
    cp -r "${PATH_PACKAGE_TOOL}/Firmwares/" "${PATH_OUT}"

    cp -r "${PATH_PACKAGE_TOOL}/Keys/" "${PATH_OUT}"

    cp -r "${PATH_PACKAGE_TOOL}/certs/" "${PATH_OUT}"

    exec_blankfile "${PATH_FIRMARES}/dummy.bin" 8192

    # build project specific memory config
    if [[ -e "${PATH_PROJECT}/mem_config" ]]; then
        echo -e "${GREEN}found project specific memory config ${PATH_PROJECT}/mem_config${NORMAL}"
        build_memcfg "${PATH_FIRMARES}/memory_config.bin"
    fi

    # build project specific pm config
    if [[ -e "${PATH_PROJECT}/pm_config" ]]; then
        echo -e "${GREEN}found project specific pm config ${PATH_PROJECT}/pm_config${NORMAL}"
        build_pmcfg "${PATH_FIRMARES}/csu_pm_config.bin"
    fi

    # update project specific low level firmware
    if [[ -e "${PATH_PROJECT}/Firmwares/" ]]; then
        echo -e "${GREEN}found project specific firmware folder ${PATH_PROJECT}/Firmwares/${NORMAL}"
        cp ${PATH_PROJECT}/Firmwares/* ${PATH_FIRMARES}
    fi

    # check bootloader1 image
    if [[ ! -e "${PATH_FIRMARES}/bootloader1.img" ]]; then
        echo "ERROR: no file ${PATH_FIRMARES}/bootloader1.img"
        exit 1
    fi

    # check bootloader2 image
    if [[ ! -e "${PATH_FIRMARES}/bootloader2.img" ]]; then
        echo "ERROR: no file ${PATH_FIRMARES}/bootloader2.img"
        exit 1
    fi

	# Copy tools to output
	cp  "${PATH_PACKAGE_TOOL}/cert_uefi_create_rsa" "${PATH_OUT}"
	cp  "${PATH_PACKAGE_TOOL}/cix_package_tool" "${PATH_OUT}"
	cp  "${PATH_PACKAGE_TOOL}/fiptool" "${PATH_OUT}"
	cp  "${PATH_PACKAGE_TOOL}/spi_flash_config_all.json" "${PATH_OUT}"
	cp  "${PATH_PACKAGE_TOOL}/spi_flash_config_ota.json" "${PATH_OUT}"

    # update project specific spi flash layout
    if [[ -e "${PATH_PROJECT}/spi_flash_config_all.json" ]]; then
        echo -e "${GREEN}found project specific ${PATH_PROJECT}/spi_flash_config_all.json${NORMAL}"
        cp ${PATH_PROJECT}/spi_flash_config_all.json ${PATH_OUT}
    fi

    if [[ -e "${PATH_PROJECT}/spi_flash_config_ota.json" ]]; then
        echo -e "${GREEN}found project specific ${PATH_PROJECT}/spi_flash_config_ota.json${NORMAL}"
        cp ${PATH_PROJECT}/spi_flash_config_ota.json ${PATH_OUT}
    fi

    # Generate bootloader3 image
    cd "${PATH_OUT}"

    ./cert_uefi_create_rsa --key-alg rsa --key-size 3072 --hash-alg sha256 -p --ntfw-nvctr 223 \
        --nt-fw-cert ${PATH_OUT}/certs/nt_fw_cert.crt \
        --nt-fw-key-cert ${PATH_OUT}/certs/nt_fw_key.crt \
        --nt-fw-key ${PATH_OUT}/Keys/oem_privatekey.pem \
        --non-trusted-world-key ${PATH_OUT}/Keys/oem_privatekey.pem \
        --nt-fw ${PATH_OUT}/SKY1_BL33_UEFI.fd

    ./fiptool create \
        --trusted-key-cert ${PATH_OUT}/certs/trusted_key_no.crt \
        --nt-fw-key-cert ${PATH_OUT}/certs/nt_fw_key.crt \
        --nt-fw-cert ${PATH_OUT}/certs/nt_fw_cert.crt \
        --nt-fw ${PATH_OUT}/SKY1_BL33_UEFI.fd \
        ${PATH_FIRMARES}/bootloader3.img
    cd -

    if [[ ! -e "${PATH_FIRMARES}/bootloader3.img" ]]; then
        echo "ERROR: no file ${PATH_FIRMARES}/bootloader3.img"
        exit 1
    fi

    # Generate spi flash image
    cd "${PATH_OUT}"

	 echo "./cix_package_tool -c spi_flash_config_all.json -o cix_flash_all.bin"
    ./cix_package_tool -c spi_flash_config_all.json -o cix_flash_all.bin
    echo "./cix_package_tool -c spi_flash_config_ota.json -O cix_flash_ota.bin"
    ./cix_package_tool -c spi_flash_config_ota.json -O cix_flash_ota.bin

    cd -

    echo -e "${GREEN}Generate ${PATH_OUT}/cix_flash_all.bin successful!${NORMAL}"

}

if [[ ! -e "${WORKSPACE}/edk2-non-osi/Platform/CIX/Sky1/PackageTool/build_and_package.sh" ]]; then
    echo "Your work path ${WORKSPACE} not contain edk2-non-osi for CIX!"
    exit 1
fi

if [[ -e "${PATH_OUT}" ]]; then
    rm -rf "${PATH_OUT}"
fi

if [[ ! -e "${PATH_OUT}" ]]; then
    mkdir -p "${PATH_OUT}"
fi

if [ ! -z "$1" ]; then
    UEFI_PROJECT="$1"
fi

case "$UEFI_PROJECT" in
("Merak")
    UEFI_PROJECT_PATH="Platform/CIX/Sky1"
    ;;
("Edge")
    UEFI_PROJECT_PATH="Platform/CIX/Sky1"
    ;;
("CloudBook")
    UEFI_PROJECT_PATH="Platform/CIX/Sky1"
    ;;
("O6")
    UEFI_PROJECT_PATH="Platform/Radxa/Orion"
    ;;
(*)
    echo -e "${RED}Unsupported Project ${UEFI_PROJECT}!!${NORMAL}"
    exit 1
    ;;
esac

echo "Build UEFI Project $UEFI_PROJECT with PATH ${UEFI_PROJECT_PATH}"

set -e

exec_build_uefi
exec_cix_mkimage
