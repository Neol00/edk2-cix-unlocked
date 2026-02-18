# QemuOpenBoardPkg

This project brings UEFI support to QEMU x86_64 following the MinPlatform specification.

## Capabilities

- Supports IA-32 and hybrid X64 (IA32 PEI Core and X64 DXE Core)
- Modern QEMU (Tested on 7.0.0)
  - PIIX4 and Q35 machines
- Boot UEFI Linux
- Boot UEFI Windows

## How to build

### Pre-requesites

- EDK2
  - How to setup a local tree: https://github.com/tianocore/tianocore.github.io/wiki/Getting-Started-with-EDK-II

- EDK2 Platforms
  - https://github.com/tianocore/edk2-platforms

- Environnements variables:
  - WORKSPACE set to your current workspace
  - PACKAGES_PATH should contain path to:
    - edk2
    - edk2-platforms
    - edk2-platforms/Platform/Intel
    - edk2-platforms/Platform/Qemu
    - edk2-platforms/Silicon/Intel

Currently QemuOpenBoardPkg's PEI Core is 32 bits only, DXE supports either 32 bits or 64 bits

QemuOpenBoardPkg (IA32 PEI - IA32 DXE)

```build -a IA32 -D PEI_ARCH=IA32 -D DXE_ARCH=IA32```

QemuOpenBoardPkg (IA32 PEI - X64 DXE)

```build -a IA32 -a X64 -D PEI_ARCH=IA32 -D DXE_ARCH=X64```

## How to use

Using qemu-system-x86_64, use

```-bios <path to QemuOpenBoard FV>```

To redirect serial output to the console

```-serial stdio```

## Important notes
- Secure boot is not yet available due to QemuOpenBoardPkg NVRAM storage not being persistent yet.
