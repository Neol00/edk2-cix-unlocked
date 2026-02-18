# Overview
* **Feature Name:** Platform Payload
* **PI Phase(s) Supported:** SMM DXE

## Purpose
This package provides a common platform payload which is expected to work on most
Intel platforms with a bootloader.

# High-Level Theory of Operation
Bootloader initialize memory/silicon/platform and payload focus on boot media initializaiton
and OS boot. Payload is expected to be re-used on different Platforms.

In the reality, some payload modules have platform dependency (e.g. only for Intel PCH)
or bootloader dependency (only for coreboot). These modules would be located in
edk2-platform repo.

The generic UEFI payload could be built from EDK2 UefiPayloadPkg in ELF format following
universal payload specification. And a Platform Payload could be built from PlatformPayloadFeaturePkg
to provide Intel platform specific features (e.g. SPI module, PCH SMM) in FV/FD format.
This platform payload could be inserted into universal UEFI payload as an ELF section
to generate a full-feature payload.

## Firmware Volumes
* PlatformPayload.fv

## Build Flows
Use Windows host as example to generate a full-feature payload:

* Setup the build env
<pre>
set WORKSPACE=c:\payload
set PACKAGES_PATH=%WORKSPACE%\edk2;%WORKSPACE%\edk2-platforms\Features\Intel;%WORKSPACE%\edk2-platforms\Platform\Intel
edk2\edksetup.bat
</pre>
* Build universal UEFI payload with platform payload
<pre>
python edk2-platforms\Features\Intel\PlatformPayloadFeaturePkg\PlatformPayloadFeaturePkg.py -t VS2019 -D SMM_SUPPORT=TRUE -DVARIABLE_SUPPORT=NONE -D SMM_VARIABLE=TRUE
</pre>
* Build universal UEFI payload then build the platform payload and patch it to the universal UEFI payload
<pre>
python edk2\UefiPayloadPkg\UniversalPayloadBuild.py -t VS2019 -D SMM_SUPPORT=TRUE -DVARIABLE_SUPPORT=NONE
python edk2-platforms\Features\Intel\PlatformPayloadFeaturePkg\PlatformPayloadFeaturePkg.py -t VS2019 -D SMM_VARIABLE=TRUE -s
</pre>

If build succeeds, the final UEFI payload is at <B>Build\UefiPayloadPkgX64\UniversalPayload.elf</B>

Note that standalone feature package build works with "-a X64" or "-a IA32 -a X64" but "-a IA32" is not supported.
Note that this does not patch the universal UEFI payload, it only creates a PlatformPayload.fv.
<pre>
build -p PlatformPayloadFeaturePkg/PlatformPayloadFeaturePkg.dsc -a X64 -a IA32
</pre>
## Features

1. SMM variable feature configuration
  * PcdPlatformPayloadFeatureEnable - Enables this feature.
  * SMM_VARIABLE - TRUE enables the SPI SMM variable feature implementation.

2. Modules: Currently only SMM variable feature is available.
Several build macros are used as below for SMM variable feature modules.
<pre>
!if $(SMM_VARIABLE) == TRUE
  ## PchSmiDispatchSmm
  ## FvbSmm
  ## FaultTolerantWriteSmm
  ## VariableSmm
  ## VariableSmmRuntimeDxe
!endif
</pre>

3. Data Flows

SMM variable:

The interface with bootloader are defined in PlatformPayloadFeaturePkg\Include\Guid

SpiFlashInfoGuid.h    -- SPI related information for SPI flash operation.

NvVariableInfoGuid.h  -- Variable related information for SPI NV variables.

## Control Flows
EDK2 DXE/SMM core from universal UEFI payload would dispatch all the modules
from this platform payload.

## Test Point Results
None implemented

## Functional Exit Criteria
Boot to UEFI shell and verify variable functionality over resets

## Feature Enabling Checklist
Verify configuration of PcdPlatformPayloadFeatureEnable and SMM_VARIABLE
Boot to UEFI Shell
Update a variable
Reset the system and verify variable remains updated

## Performance Impact
Minimal expected

## Common Optimizations
None
