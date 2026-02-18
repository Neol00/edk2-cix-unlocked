# Overview
* **Feature Name:** Beep Debug
* **PI Phase(s) Supported:** PEI, DXE, SMM
* **SMM Required?** Yes

More Information:

## Purpose
Very often it is necessary to debug very close to the reset vector or in production systems that lack serial ports, seven segment displays, or useful LED that are typically used to output useful debug messages.

The BeepDebugFeaturePkg includes some useful beep focused debug libraries.

This isn't intended for production use.

There is not currently seamless integration into the SecCore component that handles the reset vector. In order to debug that early, it will be necessary to use the BeepLib directly in SEC code.

# High-Level Theory of Operation
It provides a library, BeepStatusCodeHandlerLib, used by edk2 StatusCodeHandler.efi, used to do beep if needed.
It also provide a library of BeepMap lib which maps the status code to a beep value.
A library of Beep lib is needed by platform, and this pkg has a Null implementation.

In the library contstructor function, BeepStatusCodeHandlerLib registers the call back function for ReportStatusCode. When called, it calls GetBeepFromStatusCode (); in BeepMapLib to get beep value from status code, and calls Beep () in BeepLib to beep a speaker.

BeepStatusCodeHandlerLib includes three libraries for PEI, RuntimeDxe, and SMM:
* PeiBeepStatusCodeHandlerLib
* RuntimeDxeBeepStatusCodeHandlerLib
* SmmBeepStatusCodeHandlerLib

## Firmware Volumes
These libraries need to be linked into StatusCodeHandler components.
Make sure one puts the StatusCodeHandler.efi after the ReportStatusCodeRouter.efi.

## Modules
* BeepStatusCodeHandlerLib
* BeepMapLib
* BeepLibNull

## BeepStatusCodeHandlerLib
This library registers the callback function for ReportStatusCode, gets beep value from status code, and does the beep.

## BeepMapLib
This library provides a function to get a beep value for a status code.

## BeepLibNull
This library provide a function to perform the beep.

## Key Functions
* In PeiBeepStatusCodeHandlerLib:
```
  EFI_STATUS
  EFIAPI
  BeepStatusCodeReportWorker (
    IN CONST  EFI_PEI_SERVICES        **PeiServices,
    IN EFI_STATUS_CODE_TYPE           CodeType,
    IN EFI_STATUS_CODE_VALUE          Value,
    IN UINT32                         Instance,
    IN CONST EFI_GUID                 *CallerId,
    IN CONST EFI_STATUS_CODE_DATA     *Data OPTIONAL
  )
```

* In RuntimeDxeBeepStatusCodeHandlerLib:
```
  EFI_STATUS
  EFIAPI
  BeepStatusCodeReportWorker (
    IN EFI_STATUS_CODE_TYPE           CodeType,
    IN EFI_STATUS_CODE_VALUE          Value,
    IN UINT32                         Instance,
    IN EFI_GUID                       *CallerId,
    IN EFI_STATUS_CODE_DATA           *Data OPTIONAL
  )
```

* In SmmBeepStatusCodeHandlerLib:
```
  EFI_STATUS
  EFIAPI
  BeepStatusCodeReportWorker (
    IN EFI_STATUS_CODE_TYPE           CodeType,
    IN EFI_STATUS_CODE_VALUE          Value,
    IN UINT32                         Instance,
    IN EFI_GUID                       *CallerId,
    IN EFI_STATUS_CODE_DATA           *Data OPTIONAL
    )
```

* In BeepMapLib:
```
  UINT32
  EFIAPI
  GetBeepValueFromStatusCode (
    IN EFI_STATUS_CODE_TYPE           CodeType,
    IN EFI_STATUS_CODE_VALUE          Value
    )
```

* In BeepLib:
```
  VOID
  EFIAPI
  Beep (
    IN UINT32  Value
    )
```

## Configuration
* Configure PCD gBeepDebugFeaturePkgTokenSpaceGuid.PcdStatusCodeUseBeep.
  In board DSC file, the board developer needs to configure the type of gBeepDebugFeaturePkgTokenSpaceGuid.PcdStatusCodeUseBeep control desired.
  [PcdsFixedAtBuild] is the feature default value as this has lowest size.
  [PcdsDynamicExDefault] is the most common configuration as it provides dynamic control during debugging.
* Implemented board specific BeepMapLib if custom status code to beep code mapping as needed.
* Provide the board specific BeepLib to perform beeps on the board specific hardware.
```The default library does not cause any hardware to beep```

## Data Flows
Status Code (ReportStatusCode) -> Beep Value (GetBeepValueFromStatusCode).

## Control Flows
ReportStatusCode () -> BeepStatusCodeReportWorker () -> GetBeepValueFromStatusCode () -> Beep ()

## Build Flows
Standalone build
* build -a IA32 -a X64 -p Debugging\BeepDebugFeaturePkg\BeepDebugFeaturePkg.dsc

AdvanceFeaturePkg build
* build -a IA32 -a X64 -p AdvancedFeaturePkg/AdvancedFeaturePkg.dsc
## Test Point Results
None

## Functional Exit Criteria
N/A

## Feature Enabling Checklist
* Verify in board DSC file that gBeepDebugFeaturePkgTokenSpaceGuid.PcdBeepDebugFeatureEnable set to TRUE
* Verify board specific BeepLib implemented and included in board DSC file.
* Verify that the board has a PlatformHookLib instance.  There is a null library implementation if needed
```
      PlatformHookLib|MdeModulePkg/Library/BasePlatformHookLibNull/BasePlatformHookLibNull.inf
```
* Verify that your board has the StatusCodeHandler components (PEIM or driver) desired.
```
  Example:
    MdeModulePkg/Universal/StatusCodeHandler/RuntimeDxe/StatusCodeHandlerRuntimeDxe.inf {
    <LibraryClasses>
      NULL|BeepDebugFeaturePkg/Library/BeepStatusCodeHandlerLib/RuntimeDxeBeepStatusCodeHandlerLib.inf
    }
  There are default StatusCodeHandlers for PEI, RT, and SMM in BeepDebugFeaturePkg/Include in PreMemory.fdf and PostMemory.fdf for use.  But most boards will already have these components and you will just want to add the appropriate *StatusCodeHandlerLib.inf to each component.
```
* Build
* Remove all the memory from the system and verify audible beep is heard when attempting to boot.

## Common Optimizations
N/A
