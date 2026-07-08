This firmware is based on edk2-cix v0.2.2-1 with its prebuilt closed-source binaries (bootloader1.img containing the SE/PBL and SCP firmware) that support OPP table tuning via pm_config. Newer Radxa releases (v1.1.0+) use updated SCP firmware that controls CPU frequency internally, ignoring pm_config OPP tables entirely — which is why runtime overclocking requires staying on the older bootloader1.img.

### V2.4

#### NVRAM
- **Added boot-time crash dump sweeper** — new `PstoreSweeper` in `CixPlatformBootManagerLib`. On every kernel crash, Linux's efi-pstore dumps the dmesg buffer into `dump-type0-*` UEFI variables under `LINUX_EFI_CRASH_GUID`. If the OS never drains them (systemd-pstore.service disabled or missing), repeated crashes fill the 160KB SPI variable store until efivar writes fail with "No space left on device" and boot slows down. The sweeper runs in `PlatformBootManagerBeforeConsole()`: when free variable-store space drops below 50%, it deletes crash dump variables oldest-first (sorted by the pstore timestamp embedded in the variable name) until the store is back above 50% free. Only variables under the Linux crash vendor GUID are ever touched.

#### Stability
- **Reset power supplies on boot** — `InitGpio()` in `PlatformEnvHookLib` now drives all five PCIe PERST# pins and the onboard power-enable rails (LOM, M.2 SSD, WLAN, VGFX, camera, and all USB VBUS lines) low for 100ms before normal GPIO initialization powers them up. This guarantees every onboard device gets a clean power cycle even after a forced power-off or crash, instead of coming up in a half-alive state from the previous session. Pinmux initialization now runs before GPIO initialization as part of the same change.

#### ACPI
- **Added `reg-io-width` property to SCMI shared memory** — mainline Linux ≥ 6.13 uses this property to perform 32-bit accesses to the SCMI shmem; without it the SCMI transport can fail to probe on newer kernels, taking DVFS, clocks, and the panthor GPU driver down with it. Note: the "Enable ACPI SCMI" BIOS option mentioned in CIX's kernel documentation does not exist in this BIOS because the SCMI DVFS/CLKS devices are permanently enabled here — the option only exists in stock Radxa firmware where they ship disabled.
- **Fixed PCIe bus-range/WordBusNumber mismatch** — the PCIE2 X4 root complex (PRC1) declared `bus-range 0x90–0xbf` in its `_DSD` while its `_CRS` WordBusNumber only allocates buses 0x90–0xaf. Corrected the `_DSD` to match. The other four root complexes were verified consistent.

#### Firmware update
- **Fixed FmpSetImage return value** — `FmpSetImage()` in `SystemFirmwareReportDxe` always returned `EFI_SUCCESS` even when the underlying image dispatch failed, so a failed firmware update could be reported as successful to capsule tooling. It also skipped `ImageIndex` validation (the check was commented out). Now returns the real dispatch status and rejects out-of-range image indexes with `EFI_INVALID_PARAMETER`. This also fixes the ACS "SetImage, conformance checkpoint" test failure.

#### USB-PD
- **Guard against invalid PD alert pin** — `SortEnabledAlertPin()` in `PdDxe` now skips PD devices whose alert pin is the unassigned sentinel `0xFF`. The PD device list comes from a runtime configuration data block, so an unassigned pin could previously be registered as a GPIO alert interrupt on a nonexistent pin 255.

#### SMBIOS
- **Fixed dead State assignment in Type 45** — when a component's firmware version cannot be read, the entry's `State` field is set to `FirmwareInventoryStateUnknown`; previously the assignment ran before the template `CopyMem` overwrote the struct, so it never took effect.

#### Build
- **Fixed ASL compatibility with current iASL** — iASL 20260408 no longer accepts `Printf`/`printf` statements (internal compiler error). Converted all 16 ACPI debug printf statements across `Dsdt-AcpiRam.asl`, `Dsdt-Dpu.asl`, `Dsdt-ScmiMailbox.asl`, and the Merak `EC.asl` to equivalent `Debug =` stores, and rewrote the two invalid `UDBG(Printf(...))` calls in the Merak `CixWmi.asl` to build the string with `Concatenate`/`ToHexString` first. The firmware now builds with a stock system iasl.

#### Overclocking / power management
These came out of a source audit of the pm_config patching path (`PmConfigUpdateDxe`), which applies user OPP/voltage/TDP settings to the SPI `pm_config` and cold-resets so SCP re-reads it.
- **Fixed a soft-brick reset loop from an out-of-range SoC voltage offset** — `EdpCfg[].DeltaMv` is a signed 10-bit field (−512..+511), but `PmSocVoltageOffset` is a 16-bit, OS-writable setup variable. A value above 511 was truncated when stored, so the read-back never matched the requested value, the driver saw a "change" every boot, and it re-wrote pm_config and cold-reset on **every** boot — recoverable only by clearing NVRAM. The magnitude is now clamped to 511 so it always round-trips.
- **Added read-back verification before the cold reset** — `FirmwareRawEntryUpdate` returns a `UINT16` status code that gets zero-extended into `EFI_STATUS`, so the existing `EFI_ERROR()` check can never detect a device-level write failure (error bit 63 is always clear). After writing pm_config the driver now re-reads it and only cold-resets if the flash matches what it intended to write; a silently-failed write no longer reset-loops.
- **Bounded OPP table indexing to the array size** — `PatchOppEntries()` guarded the OPP index against the flash-supplied `DomainOpp->Size` but not the real `OppTable[13]` array bound, so a corrupt/oversized `Size` with a hidden sustained entry could write one `DVFS_OPP_T` past the array into the next domain. Now also bounded by `DOMAIN_MAX_OPP_ENTRIES`.
- **Zero-initialize the setup variable before reading it** — if a `PLATFORM_SETUP_DATA` stored by an older BIOS is shorter than the current struct, `GetVariable` succeeds with a short size and previously left the tail (OPP frequencies/voltages, TDP, voltage offset) as uninitialized stack that could be written into pm_config as real values. The struct is now zeroed first, so any field an older variable didn't have reads as 0 ("stock", no patch).

#### Setup variables
- **Fixed a stack overflow when the stored setup variable is larger than the current build** — `PlatformSetupVariableInit()` and `NetworkStackVariableInit()` called `ZeroMem(&var, VarSize)` using the size `GetVariable` returns. On `EFI_BUFFER_TOO_SMALL` (a stored variable from a *larger* struct, e.g. after a downgrade) that size exceeds the stack buffer and the `ZeroMem` overruns it, corrupting the stack in the DXE entry point on the first boot after a layout change — a boot loop, since the crash precedes the variable being rewritten. Both now zero the struct with `sizeof` before reading, which also stops a shorter stored variable from leaving stack garbage in the tail.

#### PCIe
- **Fixed a bogus I/O window advertised on all five root bridges** — when a root complex has no I/O aperture (all five on the O6), `PciHostBridgeLib` advertised a *valid* `0..0xFFFF` I/O window instead of the "absent" convention (`Base > Limit`) used by every other window type. Because `PcdPciIoTranslation` is 0 and nothing routes port I/O to PCIe, a discrete GPU's legacy I/O BAR could be assigned at CPU physical 0 and have I/O decode enabled there, reading garbage or faulting during boot. The no-I/O branch now uses `Base = MAX_UINT64, Limit = 0` to match the memory windows.

#### ACPI (no embedded controller)
- **Extended the no-EC gating to EC0, HWMN, and the GPI4 alert** — the Orion O6 has no real embedded controller, and `BAT0._STA` was already forced to 0 to avoid million-iteration I2C timeout loops against absent hardware. But `EC0._STA` and `HWMN._STA` still reported present (0x03), and the `GPI4._L06` GPIO event still called `EC0.EVNT()` — so any hwmon/fan access or a spurious pin-6 edge could still enter those `AE_AML_LOOP_TIMEOUT` stalls. `EC0` and `HWMN` are now hidden (`_STA` returns 0), and the `_L06` handler no longer calls into the EC. (HWMN's methods all route to `\_SB.EC0.*`; the SCMI alternatives are commented out, so hiding it loses no working functionality on this board.)

#### Stability
- **Raised the global SoC watchdog timeout from 20s to 90s** — `GlobalWatchdogDxe` arms the hardware watchdog at driver entry and only disarms it at ReadyToBoot, so the single period must cover all of BDS console-connect and device enumeration. This watchdog is active in RELEASE builds (the disable is `#ifndef NDEBUG`), and expiry is an unconditional SoC reset. 20s could false-trip on a slow-but-progressing cold boot (slow USB hubs, multiple NVMe, PXE/HTTP attempts). A genuinely hung boot never reaches ReadyToBoot regardless, so 90s preserves the hang-recovery safety net while removing the false resets.

### V2.3

#### Profile Manager
- **Added BIOS Profile Manager** — new `ProfileManagerDxe` driver that saves and restores complete BIOS settings as named profiles. Supports 5 profile slots stored in a dedicated 24KB region of SPI flash (offset 0x108000), persisting across NVRAM resets. Profiles capture the entire `PLATFORM_SETUP_DATA` structure (OPP tables, PCIe/USB/Memory settings, TDP configuration, etc.) and can be saved, loaded, or deleted from a new "Profile Manager" submenu. Uses the NOR flash DiskIo protocol for direct SPI flash I/O, bypassing the `FirmwareRawEntryUpdate` 4KB/type restriction. Each profile slot is CRC32-verified to detect corruption.

#### Audio
- **Fixed audio DMA memory reservation** — the stock Radxa BIOS reserves three audio memory regions (DSP at 0xCDE08000, Audio DMA at 0xD0000000, HDA DMA at 0xD0700000) via an unknown mechanism in a prebuilt proprietary binary — there is no trace in the open-source EDK2 code of how these regions are marked as reserved. Without reservation, Linux would allocate over them, causing `OF: reserved mem: node audio_dma_mem_region ... unable to alloc` errors on every boot. Solved by adding explicit `BuildMemoryAllocationHob()` calls with `EfiReservedMemoryType` for all three regions in `MemoryInitPeiLib.c`, which creates proper EFI memory map entries that Linux respects.
- **Removed RT5682 I2C codec from Merak SSDT** — the Merak ACPI tables included an RT5682 I2C audio codec device definition from the laptop reference design. The Orion O6 uses a Realtek ALC256NA HDA codec instead. The orphaned RT5682 device caused `rt5682-i2c: probe failed` ASoC errors. Removed the entire RT5682 device definition from `Audio.asl`.

#### ACPI
- **Removed orphaned PCIe power regulators from DSDT** — removed `Dsdt-CdnsPciePwr.asl` include which defined 5 PCIe voltage regulator devices (PVC0–PVC4) with GPIO pin references that don't match the Orion O6 hardware. These caused 5 `reg-fixed-voltage PRP0001:0a-0e: probe failed with error -12` (ENOMEM) errors on every boot.
- **Fixed ramoops record and console sizes** — changed `record-size` and `console-size` in `Dsdt-dst.asl` from 0x20000 to 0x40000 to match the 0x80000 `RAMOOPS_RES_SIZE`. The old values caused `ramoops PRP0001:03: probe failed with error -22` (EINVAL) because the region couldn't fit all sub-buffers.
- **Conditional CIXH2020 PCIe device disable** — the DSDT contains two sets of PCIe devices: standard PNP0A08 host bridges (used by mainline Linux) and CIX vendor CIXH2020 Cadence PCIe controller devices (used by CIX's vendor kernel). Both claim overlapping MMIO resources, causing 5 `CIXH2020:00-04: platform device creation failed: -16` (EBUSY) errors on mainline. Each CIXH2020 `_STA` method now checks if its corresponding PNP0A08 host bridge is active and disables itself if so, eliminating the resource conflict while preserving compatibility with CIX's vendor kernel.
- **Enabled PCIe Hotplug, LTR, and DPC in _OSC negotiation** — changed the PCI_OSC_SUPPORT capability mask in `Dsdt-Pcie.asl` from 0x1C to 0xFD, granting the OS control over PCIe Native Hot Plug, Latency Tolerance Reporting, and Downstream Port Containment in addition to PME, AER, and PCIe Capability Structure control. Previously the platform only advertised PME/AER/PCIeCapability support, causing the kernel to log `_OSC: platform does not support [PCIeHotplug LTR DPC]` on every root port during boot.
- **Disabled EC lid and battery support** — set `EC_LID_SUPPORT=0` and `EC_BATTERY_SUPPORT=0` in `EC.asl`. The Orion O6 is a single-board computer with no lid switch or battery. The enabled code paths caused `\_SB.EC0.WRIT`, `\_SB.EC0.TRAS`, and `\_SB.LID._LID` ACPI method timeouts (`AE_AML_LOOP_TIMEOUT`) on every boot.
- **Fixed eDP panel property on non-eDP DisplayPort ports** — split the `DP_PORT_INIT` macro into `DP_PORT_INIT` (standard DP) and `EDP_PORT_INIT` (eDP with panel reference) in `Dsdt-Dpu.asl`. Previously all 5 DP ports included an `edp-panel` property regardless of type. Stock Radxa kernels with an uninitialized `edp_panel` pointer in the DP driver would crash with a NULL pointer dereference in `drm_panel_prepare()` when probing non-eDP ports. Only DP02 (the actual eDP connector) now uses `EDP_PORT_INIT`.

#### Power management
- **Fixed CPPC SCMI performance domain mapping** — the `PLAT_PSD_INFO` domain field, which `GetCpuPerfData()` and `GetCpcGranularity()` use as the SCMI performance domain index, had incorrect values. CPUs 0-3 (BIG) mapped to domain 2 (LITTLE), CPUs 4-7 (MID) mapped to domain 4 (BIG_G1), and CPUs 8-11 (LITTLE) mapped to domain 8 (NPU). This caused all _CPC objects to report wrong frequency ranges — notably the LITTLE cores showed 400–1000 MHz (the NPU's OPP table) instead of the correct 800–2800 MHz. Fixed to: BIG→domain 3 (BIG_G0), MID→domain 5 (MID_G0), LITTLE→domain 2 (LITTLE).
- **Fixed CPPC patching to search SSDT tables** — `PatchCppcInDsdt` only searched the DSDT for _CPC packages to patch HighestPerf/NominalPerf with user-configured OPP values. However, the _CPC packages are generated dynamically by the `SsdtCpuTopologyGenerator` and live in an SSDT, not the DSDT — so the function never found anything to patch and was effectively a no-op. Renamed to `PatchCppcInAcpi` and changed to search both SSDTs and the DSDT, ensuring user OPP frequency changes from the BIOS settings are correctly applied to the _CPC objects that Linux reads via cpufreq.

#### CPU topology
- **Consolidated to 3 visible CPU clusters** — remapped the 5 SCMI performance domains (BIG_G0, BIG_G1, MID_G0, MID_G1, LITTLE) to 3 ACPI clusters (BIG, MID, LITTLE). All BIG cores share one DesiredPerf register and _CPC entry, all MID cores share another, and all LITTLE cores share a third. The G0/G1 sub-groups within each cluster type share identical frequency ranges so they are presented as a single cluster to the OS. This simplifies cpufreq scaling and matches the logical topology visible to Linux.
- **Fixed PPTT cache topology** — rewrote the Processor Properties Topology Table (PPTT) to eliminate phantom L4/L5/L6 cache levels that appeared in `lscpu`. The Linux kernel counts caches found via both `NextLevelOfCache` chains and processor node private resource arrays — if the same cache appears in both, it gets double-counted as separate levels. The fix removes all private resource references from cluster and package nodes (set `NumberOfPrivateResources` to 0) and relies exclusively on `NextLevelOfCache` chains: L1D/L1I → L2 → L3. Additionally changed A720 L2 from per-cluster (2 instances) to per-core (8 × 512KB instances) matching the actual hardware, and added a zero-size L2 placeholder for A520 cores (which have no physical L2) to maintain a consistent 3-level cache chain across all cores.

### V2.2

#### Security
- **Added RngDxe driver** — provides EFI_RNG_PROTOCOL using CIX's platform RNG driver. Enables kernel KASLR (Kernel Address Space Layout Randomization) on boot, which was previously silently disabled due to the missing protocol.

#### ACPI
- **Enabled LINUX_ACPI_CONFIG_OVERRIDE** — activates O6-specific USB-C, PCIe, and HDA ACPI configuration overrides that were present in the stock Radxa BIOS but missing from our build. Ensures correct device enumeration for USB Type-C alternate mode, PCIe link configuration, and High Definition Audio controller initialization.

#### Memory layout
- **Added FwShareMemory HOB** — added a memory allocation HOB for the firmware shared memory region (PcdReservedFwShareMemoryBase/Size), matching the stock Radxa BIOS memory map. Prevents potential memory corruption from overlapping allocations in the firmware-shared communication region.

#### CPU topology
- **Reordered core enumeration: Big → Mid → Little** — changed the dynamic MADT/SSDT CPU enumeration order so that the fastest cores appear first in Linux. The new order is: Big cluster 1 (boot core), Big cluster 0, Mid cluster 0, Mid cluster 1, Little cluster. Previously cores were enumerated in physical order (Little first), causing the Linux scheduler to preferentially use the slower Little (A520) cores for single-threaded workloads. The BIOS core enable/disable settings now follow the same order, and each core displays a help text identifying its cluster type (Big/Mid/Little, A720/A520) and frequency domain.
- **Fixed PPTT to match MADT enumeration order** — reordered the Processor Properties Topology Table (PPTT) so that its ACPI processor UIDs match the new MADT enumeration order. Previously the PPTT used physical core IDs (Little=0-3, Mid=4-7, Big=8-11) while the dynamic MADT assigned UIDs sequentially in enumeration order, causing Linux to map cores to the wrong clusters. The PPTT now uses: Big1=UID 0-1, Big0=UID 2-3, Mid0=UID 4-5, Mid1=UID 6-7, Little=UID 8-11. This fixes `lscpu -e` showing incorrect CORE and cache topology columns.

#### SMBIOS
- **Populated SMBIOS Type 45 (Firmware Inventory)** — added firmware inventory entries for all system firmware components (SE, PM, PBL, ATF, TEE, EC, STMM, UEFI). Each entry reports firmware name, version, manufacturer, and state via standard SMBIOS Type 45 structures, queryable by tools like `dmidecode -t 45`.

#### SPI flash
- **Fixed SPI_VARIABLE_BASE overflow** — corrected SPI_VARIABLE_BASE from 0x390000 to 0x380000 to prevent variable storage from exceeding the 4MB SPI flash boundary.

### V2.1

#### Power management
- **Added a safe hidden sustained OPP entry for all domains** — each domain now has a hidden OPP entry at index 0 that cannot be edited from the BIOS settings. The system always boots at this safe frequency, guaranteeing a stable environment during early boot initialization. If the user sets unstable or invalid OPP values that prevent the OS from booting, a simple reboot will return to UEFI at the safe sustained frequency where settings can be corrected. Note that the hidden entry acts as a floor: the SCP firmware tests all lower OPP entries before settling at the sustained value, so users cannot underclock below it. Another flaw is that the DSU always runs at the highest allowed frequency and does not have a safeguard.

#### GPU
- **AMD GOP driver updated** — updated from v1.68 to v2.10. Adds support for newer AMD GPUs that were not recognized by the old driver.

#### Filesystem
- **Ext4 driver updated** — enabled the ext4 driver and updated Ext4Pkg from the September 2022 snapshot to upstream tianocore/edk2-platforms mainline (February 2026). Includes 3+ years of bug fixes for block group checksum validation, extent handling, directory parsing, symlink resolution, and inode management.

#### USB
- **USB VBUS power cycling on Linux reboot** — added an ACPI `_PTS` (Prepare To Sleep) method to the DSDT that drives all 6 USB VBUS GPIOs LOW with a 50ms delay before Linux-initiated reboots. Linux calls PSCI `SYSTEM_RESET` directly via SMC, bypassing the UEFI `ResetSystem` runtime service where the existing USB power cycling code runs. Without this, USB devices would not see a VBUS power cycle and occasionally fail to re-enumerate on the next boot.

#### Hardware Information menu
- **Memory Chip identification** — expanded the Memory Chip display from 4 entries to 15, covering all known Orion O6 board revisions and RAM vendors (Samsung, Hynix, Hive Semi, Rayson). Previously showed "Undefined" for any board not in the original 4-entry list.
- **Domain frequency display** — added a new "Domain Frequencies" section showing sustained frequencies (in MHz) for GPU Core, GPU Top, DSU, NPU, VPU, and MMHUB. Frequencies are queried live from the SCP via SCMI performance protocol.
- **CPU Max and Sustained Frequency** — split the single "CPU Speed" entry into "CPU Max Frequency" and "CPU Sustained Frequency" for more detailed CPU clock reporting. Fixed rounding errors: max frequency now truncates correctly (e.g. 2800 MHz displays as 2800, not 2801), sustained frequency rounds correctly (e.g. 800 MHz displays as 800, not 799).
- **Removed duplicate entries** — removed PMIC Version, PD Version, and the entire Firmware Information section (SE, PBL, ATF, PM, TEE, UEFI, EC versions) from Hardware Information as they are already listed under System Information.

#### Platform defaults
- **ACPI as default boot mode** — set `PcdDefaultDtPref` to `FALSE` so that a fresh flash, NVRAM reset, or "Reset all System settings" defaults to ACPI instead of Device Tree.
- **GPU stock OPP defaults corrected** — GPU Core and GPU Top default OPP tables had an off-by-one error causing a duplicate 250 MHz entry and the top-end entries (1100 MHz for GPU Core, 1000 MHz for GPU Top) being missing. Fixed to match the stock `opp_config_custom.h` values exactly.
- **DSU OPP range widened** — the DSU hidden sustained OPP was changed from 500 MHz to 400 MHz and the maximum configurable frequency was increased to 2250 MHz. Note: the SCP firmware always boots the DSU at its highest configured OPP entry regardless of the `sustained_idx` setting. Setting an unstable maximum DSU frequency may prevent the system from booting.

#### Build system
- **Automatic Python command detection** — removed the static `PYTHON3_ENABLE=TRUE` export. The build system now auto-detects the correct Python command: `python3` if available, falling back to `python`. No manual configuration required for systems where only `python` is installed.

### V2.0

#### USB
- **Reliable USB detection** — fixed GPIO VBUS configuration (GPIO 19/20 set to INOUT_HIGH, added GPIO 40 for USB port 6-7 VBUS, corrected USB_DRIVE_VBUS0 pin mux) and ported updated USB stack source code (XhciDxe, UsbBusDxe, UsbMassStorageDxe, etc.) to eliminate the ~50% USB detection failure on boot.
- **USB power cycling on reboot** — all USB VBUS GPIOs are driven LOW with a 50ms delay before PSCI reset, ensuring connected USB devices re-enumerate cleanly after reboot.

#### ACPI
- **Restored iomux pin controller** — a previous modification had removed the `Dsdt-iomux.asl` include from the DSDT and changed the DSDT revision from 5 to 2, breaking all I2C and PCIe pin mux references. Both restored.
- **I2C bus frequency initialization** — restored I2C `_INI` methods with zero-check fallbacks and added 8 I2C frequency writes to the AcpiSocDxe GNVA area so Linux sees correct bus speeds.
- **PCIe root port initialization** — restored `_INI` methods for all 5 root ports (PRC0–PRC4) with zero-check fallbacks, added 25 PCIe config writes to GNVA (bandwidth, speed, payload, ASPM), and updated all `_DSD` properties to use dynamic values.
- **PCIe _OSC capabilities** — changed `_OSC` control mask from `0x10` to `0x1C` to grant Linux native **PME** (Power Management Events) and **AER** (Advanced Error Reporting) control. The hardware defines AER interrupt lines (correctable, fatal, non-fatal) for all 5 root ports. Hotplug/SHPC/LTR remain disabled (no hardware support).
- **PCIe boot reliability** — all 8 PCIe `_STA` methods (PRC0–PRC4, PCP0–PCP2) now unconditionally return 0xF. The original code checked a `PcieLinkUpStatus` snapshot which caused failure when PCIe links hadn't trained by GNVA write time, permanently hiding NVMe and other devices.
- **PCIe power regulators** — moved `CdnsPciePwr.asl` (PVC0–PVC4 devices) from SSDT to DSDT include to fix `AE_NOT_FOUND` errors on PVC4 cross-table Package references.
- **Missing iomux pin groups** — added 9 missing pin groups to MUX1 (5 PCIe PERST + 4 power regulator groups) that were causing `reg-fixed-voltage` probe failures (`-ENOMEM`).
- **Ramoops** — changed `RAMOOPS_RES_SIZE` from 0xA0000 (not power of 2) to 0x80000 and reduced record/console sizes to fit, fixing ramoops probe failure (`-EINVAL`).
- **Removed non-existent hardware** — removed TPM device includes (no TPM on O6), disabled CSI-DMA camera devices (`PcdAcpiCsiDmaEnable=FALSE`), disabled battery/lid support in EC.asl (O6 is a single-board computer).
- **AC power supply** — fixed missing `Return` in EC.asl `_PSR` method that caused ACPI warnings.
- **Thermal zone improvements** — expanded from 5 thermal zones to 16 by exposing all available SoC temperature sensors. Added 11 new zones: VPU, GPU Bottom, GPU Top, SoC Bridge, DDR Bottom, DDR Top, CI700 Interconnect, NPU, SoC Trace, and 2 board NTC thermistors. Added `_TZD` (Thermal Zone Devices) methods to CPU and GPU zones so Linux properly associates thermal sensors with their CPU/GPU devices for cpufreq thermal throttling. Updated `_STR` names with descriptive labels (e.g. "CPU Big Cluster 0 (CPU 8-9)"). Fixed sustainable power values to match Sky1-Linux DTS (B0=5500mW, B1=6000mW, M0=5000mW, M1=4500mW). Removed `_PSV` from monitoring-only zones that lack cooling devices to eliminate `_PSL evaluation failure` warnings.
- **DSU PMU** — added ARM DynamIQ Shared Unit Performance Monitoring Unit device (HID `ARMHD500`, GIC SPI 2) to the DSDT. Enables `perf` to read DSU-level performance counters (L3 cache hits/misses, bus cycles, interconnect traffic) via `/sys/bus/event_source/devices/arm_dsu_0/`. Useful for verifying overclocking success by measuring actual DSU cycle counts.
- **Compilation error cleanup** — added 26 missing `External()` declarations to Ssdt.asl, fixed RX8900 RTC HID to `EPSO0001`, removed iasl error-suppression flags from the build so all warnings are visible.
- **SMMU HTTU override** — corrected the Hardware Translation Table Update flags on all 3 SMMU nodes. The EDK2 header defines `HTTU_OVERRIDE` as `BIT1` but HTTU is actually a 2-bit field at bits [2:1]. Changed from `BIT1` to `(2 << 1)` for HA+HD (Access+Dirty) to match hardware IDR0 capabilities and eliminate the `IDR0.HTTU features overridden by FW configuration (0x0)` kernel warning.
- **PCIe RMR bypass** — added a Reserved Memory Region node with all 10 PCIe root port Stream IDs mapped through the PCIe SMMU. This pre-installs bypass Stream Table Entries during SMMU init, preventing `F_TRANSLATION` faults before IOMMU domains are attached by device drivers.
- **CPU cache topology** — created a complete PPTT table describing the CIX CD8180's 12-core big.LITTLE topology: Cluster L0 (4× A520: 32KB L1I/D), Clusters M0/M1 (2× A720 each: 64KB L1I/D, 512KB L2), Clusters B0/B1 (2× A720 each: 64KB L1I/D, 512KB L2), shared 12MB L3 (Haydn DSU). Linux reports correct cache sizes in `/sys`.

#### SMBIOS
- **Memory reporting** — corrected Type 17 form factor to `Die` (LPDDR5 on-package), added `VolatileSize` and `MemoryOperatingModeCapability` fields.

#### Power management
- **GNVA initialization** — added `UpdateAcpiGpnv()` to the ReadyToBoot hook array so DSDT runtime variables are populated before Linux boots (was only running at ExitBootServices, too late for ACPI).
- **Shutdown** — rewrote PowerButtonDxe with proper event handling and 5-second polling. Green LED (GPIO 15) is turned off on shutdown.

#### Memory configuration
- **Multi-revision board support** — expanded BoardIdMap from 6 to 12 entries to support newer Orion O6 board revisions with different DRAM configurations. Added support for: 16G Hive Semi (HS), 24G HS, 32G x8 HS, 32G Hynix, 48G HS, and 64G Rayson RAM types. Includes per-vendor PHY pad and bus configuration blocks (Hive Semi, Hynix) and vendor-specific training optimizations. All changes use the old v1.6 binary format for compatibility with the existing MemConfigUpdateDxe firmware.
