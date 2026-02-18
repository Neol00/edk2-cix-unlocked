CIX EVB Platform
=======================================

# Summary

This is a port of ARM64 Tiano Core UEFI firmware for the CIX EVB platform based on the CIX P1 SoC.

CIX P1 edk2 code is base on as follows:
- [edk2](https://github.com/tianocore/edk2): `fb493ac84ebc6860e1690770fb88183effadebfb`
- [edk2-platforms](https://github.com/tianocore/edk2-platforms): `8ea6ec38da8812f0703e8845fe639b8845704f96`

# How to build (X86 Linux Environment)
  1. Install Arm GNU Toolchain on X86 machines.
    Download AArch64 bare-metal target (aarch64-none-elf) for x86_64 Linux hosted cross toolchains from https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads. The verified version is 10.2-2020.11

  2. Config your GCC tool in build_and_package.sh
    For Example:

    $ export ARM_TOOLCHAIN_ELF="gcc-arm-10.2-2020.11-x86_64-aarch64-none-elf"

  3. Install ACPI Tool
    Download ACPICA tool from https://github.com/acpica/acpica.git. The verifed tag is R03_31_22

  4. Config your ACPI tool in build_and_package.sh
    For Example:

    $ export IASL_PREFIX="${WORKSPACE}/tools/acpica/generate/unix/bin/"

  5. Config your edk2 submodule update method in build_and_package.sh

  6. Create symbolic link for build_and_package.sh and run it
    For Example:

    $ cd $YOUR_WORKSPACE
    $ ln -s edk2-non-osi/Platform/CIX/Sky1/PackageTool/build_and_package.sh build_and_package.sh
    $ ./build_and_package.sh

  7. Found "cix_flash_all.bin" and "SKY1_BL33_UEFI.fd" in output folder

# How to Flash Firmware
  1. Use SPI Flash Programmer(like DediProg SF100) by flash file "cix_flash_all.bin"

  2. Run FlashUpdate.efi(edk2-non-osi/Platform/CIX/Sky1/FlashTool/FlashUpdate.efi) under UEFI shell
    For Example:

    FS0:\>FlashUpdate.efi -f cix_flash_all.bin

