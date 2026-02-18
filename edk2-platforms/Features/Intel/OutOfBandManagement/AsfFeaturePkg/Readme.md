# Overview
* **Feature Name:** Alert Standard Format (ASF)
* **PI Phase(s) Supported:** PEI, DXE
* **SMM Required?** No

More Information:
* [ASF Specification v2.0](https://www.dmtf.org/sites/default/files/standards/documents/DSP0136.pdf)

## Purpose
The ASF feature provides firmware functionality that implements behavior described in the ASF specification.
Alerting technologies provide advance warning and system failure indication from managed clients to remote management consoles. Once a system alert provides its warning or error report, the next step in remote system manageability is to allow corrective action to be taken — these actions include the ability to remotely reset or power-on or -off the client system.

# High-Level Theory of Operation
Bases on the system reported status code, the driver will send warning or system failure indication from managed clients to remote management consoles.

## Firmware Volumes
* AsfPei: PreMemory
* AsfDxe: PostMemory

## Modules
* AsfPei
* AsfDxe

## AsfPei
Asf initialize and send standard progress messages to NIC.

## AsfDxe
Install Asf Acpi table and register RSC handle to send progress/error event logs to NIC.
Produce Asf Dxe protocol.

## <Library Name>
N/A now.

## Key Functions
AsfPushEvent: This function is provided in protocol, call this function to send messages to NIC.

## Configuration
For PcdControlDataArrays structure information, please check Asf2.0 spec chapter 4.1.2.5 ASF_CONTROLDATA.

## Data Flows
AsfPushEvent() -> through SmBus -> NIC -> Remote management consoles

## Control Flows
N/A now.

## Build Flows
There is not special build flow.

## Test Point Results
There are not test points implemented.

## Functional Exit Criteria
Check Asf Acpi table
Check the event log in remote management consoles

## Feature Enabling Checklist
PcdAsfFeatureEnable to enable this feature.

## Performance Impact
There is no performance impact.

## Common Optimizations
N/A now.
