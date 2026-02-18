SolidRun MacchiatoBin
=====================

# Summary

This is a port of 64-bit TianoCore EDK II firmware for the [SolidRun MacchiatoBin Double Shot](https://solidrun.atlassian.net/wiki/spaces/developer/pages/286655749/MACCHIATObin+Single+Double+Shot+Quick+Start+Guide)
platform based on the Marvell ARMADA 8040 SoC.

# Supported features

Features supported in EDK2:

* 1x PCIE x4
* Networking:
  * 2x 10 GbE via SFP+ / RJ45
  * 1x 2500 Base-X via SFP+
  * 1x 1 GbE SGMII via RJ45
* 1x USB 3.0
* 2x USB 2.0
* 3x SATA
* 1x uSD
* 1x eMMC
* RTC
* SPI flash & memory-mapped variable storage access
* GPIO

Hardware description:

* ACPI (default)
* Device Tree

Others:

* Signed capsule update
* X64 option ROM emulator

# Building the firmware

## Prepare EDKII environment:

Please follow instructions from [Obtaining source code](https://github.com/tianocore/edk2-platforms#obtaining-source-code)
and [Manual building](https://github.com/tianocore/edk2-platforms#manual-building) from the
top level edk2-platforms [Readme.md](https://github.com/tianocore/edk2-platforms#readme).

## Build EDKII:

Use below build command:

  ```
  $ build -a AARCH64 -t GCC5 -b RELEASE -D X64EMU_ENABLE -p Platform/SolidRun/Armada80x0McBin/Armada80x0McBin.dsc
  ```

---
**NOTE**

'-D INCLUDE_TFTP_COMMAND' is optional and can be added in order to enable `tftp` command in UEFI Shell.

---

## Build the final firmware image:

In addition to EDKII binary, the complete firmware image comprises the TF-A and other components.
A complete build instruction can be found at [wiki page](https://github.com/Semihalf/edk2-platforms/wiki/Build_firmware).

## Burning the firmware

Please follow instruction at [wiki page](https://github.com/Semihalf/edk2-platforms/wiki/Burning_firmware)
to burn image to desired boot device.

# ARM System Ready certification.

MacchiatoBin is [System Ready ES](https://developer.arm.com/architectures/system-architectures/arm-systemready/es) certified. Release binary and the firmware components' baselines list are available in a dedicated [wiki page](https://github.com/semihalf/edk2-platforms/wiki).
