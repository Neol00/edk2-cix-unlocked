# Orion O6 Custom UEFI BIOS

Custom UEFI (EDK2) firmware for the Radxa Orion O6 single board computer (CIX CD8180 / Sky1 SoC). This firmware adds a BIOS settings menu for configuring OPP (Operating Performance Point) tables and TDP power caps, allowing per-domain frequency and voltage tuning without recompiling the BIOS image.

Based on the Radxa EDK2 release with pm_config reverted to version 2.1 (the version that supports OPP table configuration). Newer Radxa releases moved frequency control into the SCP firmware and no longer respect pm_config OPP tables, which is why this BIOS stays on the older closed-source bootloader2.img that reads OPP tables from the separate pm_config SPI flash partition.

![CIX Logo](cix.png)

## Features

### Overclocking & power management
- **OPP table configuration via BIOS menu** — adjust frequency and voltage for all 12 DVFS domains (GPU Core, GPU Top, CPU Little, CPU Big×2, CPU Mid×2, DSU, NPU, VPU, CI700, MMHUB) directly from the BIOS settings. Changes are patched into the pm_config binary on SPI flash and applied after an automatic cold reset. A value of 0 keeps the stock default.
- **TDP power cap configuration** — per-rail TDP limits (CPU Little, CPU Big×2, CPU Mid×2, DSU, GPU, SOC) configurable from the BIOS.
- **Memory frequency selection** — DDR frequency selectable from the BIOS menu (DDR5-1600 through DDR5-6400) without recompiling. The build-time `MEM_CFG_MEMFREQ` flag only sets the default.

### Hardware support
- **Reliable USB detection** — fixed the ~50% USB detection failure present in the stock BIOS by correcting GPIO VBUS configuration and porting an updated USB stack.
- **PCIe boot reliability** — eliminated the ~70% boot failure where NVMe and other PCIe devices were randomly hidden due to a link-training race condition.
- **Multi-revision board support** — supports all known Orion O6 board revisions including newer DRAM types (Hive Semi, Hynix, Rayson) across 12 board configurations.
- **Proper shutdown** — green LED turns off and USB ports power down on shutdown. USB VBUS is power-cycled on reboot for clean device re-enumeration.

### ACPI & Linux integration
- **16 thermal zones** — all SoC temperature sensors exposed to the OS: 4 CPU clusters, GPU (average + bottom + top), VPU, NPU, DDR (bottom + top), CI700 interconnect, SoC bridge, SoC trace, and 2 board NTC thermistors. CPU and GPU zones are linked to their devices for automatic thermal throttling.
- **CPU cache topology (PPTT)** — Linux correctly reports cache sizes and cluster topology for the 12-core big.LITTLE design (4× A520, 4× A720 mid, 4× A720 big, shared 12 MB L3).
- **DSU performance counters** — ARM DynamIQ Shared Unit PMU exposed via ACPI, enabling `perf` to read L3 cache, bus cycle, and interconnect counters.
- **Native PCIe AER & PME** — Linux has native control over Advanced Error Reporting and Power Management Events for all 5 root ports.
- **SMMU hardware translation table updates** — corrected HTTU flags so the SMMU can update Access and Dirty bits in hardware, eliminating kernel warnings.
- **GPU non-coherent DMA** — GPU correctly marked as non-coherent to prevent framebuffer corruption caused by cache coherency mismatch with the DPU.

### BIOS utilities
- **Reset All to Defaults** — restores all BIOS settings to factory defaults from within the BIOS menu.
- **Clear NVRAM** — enumerates and deletes all UEFI variables for a full factory reset.
- **Cross-platform build support** — builds natively on aarch64 or cross-compiles from x86_64 using system-installed packages.

## Build

### Prerequisites

Debian/Ubuntu cross-compiling from x86_64:
```
sudo apt-get install gcc-aarch64-linux-gnu acpica-tools
```

Building natively on aarch64:
```
sudo apt-get install gcc acpica-tools
```

### Building the flash image

```
git clone --recurse-submodules https://github.com/Neol00/edk2-cix-unlocked.git
cd edk2-cix-unlocked/
make -j$(nproc)
```

The optional `MEM_CFG_MEMFREQ` build flag sets the default memory frequency (defaults to 2750 if omitted). The value is half the DDR rating in MHz:

| Flag value | DDR rating | Effective speed |
|-----------|-----------|-----------------|
| 2750 | DDR5-5500 | 5500 MT/s |
| 3000 | DDR5-6000 | 6000 MT/s |
| 3200 | DDR5-6400 | 6400 MT/s |

```
make -j$(nproc) MEM_CFG_MEMFREQ=3200
```

This only sets the default — the memory frequency can be changed later in the BIOS settings menu without rebuilding.

The output image is at `Build/O6/RELEASE_GCC5/cix_flash_all.bin`.

## Flashing

1. Build the bios or download the release .zip from the Releases tab:

2. Copy the release files or build files to a FAT32 partition or your EFI partition:
   ```
   unzip edk2-cix-unlocked.zip && cp -r orion-o6 /to/FAT32/part/
   ```

3. Reboot into the BIOS and select "Boot to UEFI Shell" in the Boot menu. Alternatively, from GRUB press `c` for command line:
   ```
   grub> chainloader /orion-o6/Shell.efi
   grub> boot
   ```

4. From the UEFI Shell:
   ```
   Shell> fs0:
   FS0:\> cd orion-o6
   FS0:\orion-o6\> startup.nsh
   ```
   Press ENTER to start flashing. Once done, press `q` to quit, then cold reboot:


5. After reboot, press ESC to enter the BIOS when prompted and configure your settings.

## OPP table configuration

The Power Management menu in BIOS settings lets you tune frequency and voltage for each DVFS domain. All values default to the stock OPP tables. Changes are written to the pm_config binary on SPI flash and take effect after an automatic cold reset.

### Domains

| Domain | Stock entries | Freq range (MHz) | Voltage | Notes |
|--------|--------------|-------------------|---------|-------|
| GPU Core | 7 | 100–1100 | 800 mV | Configurable |
| GPU Top | 6 | 100–1000 | 800 mV | Configurable |
| CPU Little | 2 | 800–1800 | 790 mV | Configurable |
| CPU Big G0 | 7 | 800–2600 | 750–920 mV | Configurable |
| CPU Big G1 | 7 | 800–2600 | 750–920 mV | Configurable |
| CPU Mid G0 | 7 | 800–2400 | 750–920 mV | Configurable |
| CPU Mid G1 | 7 | 800–2400 | 750–920 mV | Configurable |
| DSU | 2 | 500–1300 | 790 mV | Configurable |
| NPU | 4 | 400–1200 | Fixed (SOC rail) | Frequency only |
| VPU | 6 | 150–1200 | Fixed (SOC rail) | Frequency only |
| CI700 | 1 | 1500 | Fixed (SOC rail) | Interconnect bus |
| MMHUB | 3 | 375–750 | Fixed (SOC rail) | Memory management hub |

### TDP power caps

Per-rail TDP limits in milliwatts. Stock defaults:

| Rail | Stock TDP (mW) | Notes |
|------|---------------|
| CPU Little | 2400 | Configurable |
| CPU Big G0 | 6700 | Configurable |
| CPU Big G1 | 6500 | Configurable |
| CPU Mid G0 | 8000 | Configurable |
| CPU Mid G1 | 8200 | Configurable |
| DSU | 5500 | Configurable |
| GPU | 12000 | Configurable |
| SOC | 9000 | Configurable |

## ⚠️ Overclocking warning — READ THIS BEFORE CHANGING VALUES

**Changing OPP table values can permanently brick your board if unstable values are set.** There is no software recovery mechanism. Read and understand this section fully before modifying any values.

### How OPP patching works

The key insight behind this BIOS is that OPP tables live in a separate 4 KB partition on SPI flash (`pm_config.bin`, image type 4 at offset `0x3FC000`), independent of the main UEFI firmware. The SCP (System Control Processor) reads this partition very early during power-on — before the ARM CPU cores are even started — and uses it to configure all DVFS power planes and TDP limits. By patching just this 4 KB region, we can change all frequency, voltage, and power settings without recompiling or reflashing the entire BIOS image.

#### SPI flash layout (relevant partitions)

```
Offset      Size     Contents
0x000000    ...      Bootloader stages, ATF, UEFI firmware
0x288000    0xF8000  bootloader2.img (ARM Trusted Firmware + SCP firmware)
0x3FC000    0x1000   pm_config.bin ← OPP tables live here (4 KB, separate partition)
```

#### pm_config binary structure

The 4 KB binary has a fixed layout:

```
Offset   Field
0x00     VersionMajor (uint32)
0x04     VersionMinor (uint32)
0x08     Timestamp (uint32)
0x0C     Signature (uint32, must be 0x46434D50 = "PMCF")
0x10     PmicConfig:
           └─ EdpCfg[8] — per-rail TDP power caps (mW)
         PvtConfig — thermal/sensor configuration
         OppConfig:
           └─ Opps[12] — one per DVFS domain
              └─ Size (number of active entries)
              └─ OppTable[13]:
                   • Level    (uint32)
                   • Voltage  (uint32, millivolts)
                   • Frequency (uint32, kilohertz)
                   • Power    (uint32, milliwatts)
         FanConfig, LogConfig, Reserved
...
0xFF8    CRC1 (uint32, Fletcher-like checksum A)
0xFFC    CRC2 (uint32, Fletcher-like checksum B)
```

Each DVFS domain (GPU Core, GPU Top, CPU Little, CPU Big×2, CPU Mid×2, DSU, NPU, VPU, CI700, MMHUB) has up to 13 OPP entries. Each entry is 16 bytes (4 × uint32). The CRC covers all data from offset 0 up to the CRC fields.

#### Step-by-step patching flow

**Step 1 — User changes values in the BIOS menu**

The BIOS settings form exposes 156 frequency fields and 156 voltage fields (12 domains × 13 entries each), plus 8 TDP power cap fields. Each field accepts:
- **Frequency:** 0–3000 MHz (0 = keep stock default)
- **Voltage:** 0–1600 mV (0 = keep stock default)
- **TDP:** 0–65535 mW (0 = keep stock default)

When the user saves, UEFI writes the entire settings struct to the `PlatformSetupVar` NVRAM variable on SPI flash. This is a standard UEFI variable — it survives reboots but is separate from the pm_config partition.

**Step 2 — PmConfigUpdateDxe detects changes at early boot**

The `PmConfigUpdateDxe` driver runs at DXE entry — very early in the UEFI boot process, before the boot menu or OS loader. On every boot, it:

1. Reads `PlatformSetupVar` from NVRAM to get the user's desired OPP values
2. Reads the current 4 KB `pm_config.bin` from SPI flash
3. Validates the `0x46434D50` ("PMCF") signature
4. Compares each non-zero NVRAM value against the corresponding SPI flash OPP entry:

```
For each domain (0–11):
  For each OPP entry (0–12):
    If NVRAM frequency ≠ 0:
      Convert MHz → kHz (multiply by 1000)
      If SPI entry frequency ≠ converted value → patch it, mark Modified
    If NVRAM voltage ≠ 0:
      If SPI entry voltage ≠ NVRAM voltage → patch it, mark Modified
```

A value of 0 in NVRAM means "use stock default" — the existing SPI flash value is left untouched. This is how the system preserves stock OPP entries for domains you haven't modified.

**Step 3 — SPI flash write and CRC recalculation**

If any value was modified, the driver:

1. Recalculates the dual Fletcher-like CRC over the entire pm_config buffer:
   ```
   cka = 0, ckb = 0
   for each 4-byte word in buffer[0 .. CRC_OFFSET-1]:
       cka += word
       ckb += cka
   store cka at CRC1_OFFSET, ckb at CRC2_OFFSET
   ```
2. Writes the patched 4 KB buffer back to the pm_config SPI flash partition
3. Triggers an **immediate cold reset** (`gRT->ResetSystem(EfiResetCold, ...)`)

The cold reset happens instantly — no boot menu, no OS load. The user sees the board power cycle.

**Step 4 — SCP firmware applies the patched configuration**

On the next power-on, the SCP (System Control Processor) — a separate microcontroller on the SoC — reads the pm_config partition from SPI flash. This happens during the earliest stage of the boot sequence, before the ARM cores are released from reset. The SCP:

1. Validates the pm_config CRC
2. Programs all DVFS regulators with the frequencies and voltages from the OPP tables
3. Configures TDP power caps on each power rail
4. Releases the ARM CPU cores to start executing the UEFI firmware

At this point, the ARM cores boot with the new power configuration already active.

**Step 5 — No reset loop (steady state)**

When `PmConfigUpdateDxe` runs on this second boot, it again compares NVRAM values against SPI flash. Since the SPI flash was patched in Step 3, all values now match — `Modified` stays `FALSE`, no SPI write occurs, and the system boots normally into the UEFI menu or OS.

**Step 6 — DSDT patching at ReadyToBoot**

Just before handing off to the OS bootloader, a ReadyToBoot callback:

1. Re-reads the (now patched) pm_config from SPI flash
2. Finds the maximum GPU voltage across all GPU Core OPP entries
3. Patches the DSDT `gpu-microvolt` property in-place (mV → µV conversion)

This ensures the OS sees the correct GPU voltage limit matching the user's configuration, without needing a separate DSDT rebuild.

### Why bad values can brick the board

The SCP firmware reads the pm_config **before the ARM CPU cores are started**. If you set values that prevent the CPU cores from running (e.g. voltage too low for the set frequency), the system will never reach UEFI. Since UEFI never loads, the patching driver never runs, so it can never fix the bad values. The board is stuck in a boot loop.

**"Reset to Defaults" cannot help you here** — that is a UEFI feature, and UEFI never loads if the SCP configures unstable power settings.

**Removing the CMOS battery will not help** — the bad pm_config is stored on SPI flash, not in CMOS/NVRAM. The SCP reads it from SPI flash regardless of CMOS state.

### What you need to recover

The **only** way to recover from a bricked board is to use an **external SPI flash programmer** to write a known-good image.

**The SPI flash chip operates at 1.8V. Using a 3.3V programmer will destroy the chip.**

A compatible programmer such as [CH341A 3.3V with a 1.8V adapter] is required, along with an SOP8 clip adapter (if not already included with the programmer).

The flash chip is a [W25Q64JWSSIQ](https://www.winbond.com/hq/product/code-storage-flash-memory/serial-nor-flash/?__locale=en&partNo=W25Q64JW) (1.8V, 64Mbit or 8MiB). Be aware that many counterfeit chips exist that do not support the required 133 MHz operation.

To flash with an external programmer:

```
sudo flashrom -p ch341a_spi -w cix_flash_all.bin
```

When reinstalling the flash chip, the notch (pin 1) is closest to the 40-pin connector.

**Tip:** Make sure to have several SPI flash chips as backups, the pins break pretty easily on them.

### Safe overclocking guidelines

1. **Make small changes** — increase frequency by one OPP step at a time, test stability, then push further.
2. **Start with voltage** — if increasing frequency, raise voltage first to give headroom.
3. **Test before pushing further** — boot into the OS, run a stress test, verify stability before going back to change more values.
4. **Know your chip** — silicon quality varies. The maximum stable frequency differs per chip. The BIOS allows up to 4500 MHz for CPU cores but most chips will not be stable anywhere near that.
5. **Keep a backup flash chip** — this is the only recovery path if you go too far.
6. **Write down your last known stable values** — if you brick the board and reflash, you will want to know what worked.

## Changes from stock Radxa BIOS v0.2.2-1

This firmware is based on edk2-cix v0.2.2-1 with its prebuilt closed-source binaries (bootloader2.img, SCP firmware) that support OPP table tuning via pm_config. Newer Radxa releases (v1.1.0+) use updated SCP firmware that controls CPU frequency internally, ignoring pm_config OPP tables entirely — which is why runtime overclocking requires staying on the older bootloader2.img.

The following changes have been made on top of the stock release:

### USB
- **Reliable USB detection** — fixed GPIO VBUS configuration (GPIO 19/20 set to INOUT_HIGH, added GPIO 40 for USB port 6-7 VBUS, corrected USB_DRIVE_VBUS0 pin mux) and ported updated USB stack source code (XhciDxe, UsbBusDxe, UsbMassStorageDxe, etc.) to eliminate the ~50% USB detection failure on boot.
- **USB power cycling on reboot** — all USB VBUS GPIOs are driven LOW with a 50ms delay before PSCI reset, ensuring connected USB devices re-enumerate cleanly after reboot.

### ACPI
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
- **GPU coherency** — changed GPU device (CIXH5000) `_CCA` from 1 (coherent) to 0 (non-coherent). The SoC was designed for non-coherent GPU operation; the DPU reads framebuffer non-coherently from DRAM, so coherent GPU DMA causes stale framebuffer data and display corruption.
- **Thermal zone improvements** — expanded from 5 thermal zones to 16 by exposing all available SoC temperature sensors. Added 11 new zones: VPU, GPU Bottom, GPU Top, SoC Bridge, DDR Bottom, DDR Top, CI700 Interconnect, NPU, SoC Trace, and 2 board NTC thermistors. Added `_TZD` (Thermal Zone Devices) methods to CPU and GPU zones so Linux properly associates thermal sensors with their CPU/GPU devices for cpufreq thermal throttling. Updated `_STR` names with descriptive labels (e.g. "CPU Big Cluster 0 (CPU 8-9)"). Fixed sustainable power values to match Sky1-Linux DTS (B0=5500mW, B1=6000mW, M0=5000mW, M1=4500mW). Removed `_PSV` from monitoring-only zones that lack cooling devices to eliminate `_PSL evaluation failure` warnings.
- **DSU PMU** — added ARM DynamIQ Shared Unit Performance Monitoring Unit device (HID `ARMHD500`, GIC SPI 2) to the DSDT. Enables `perf` to read DSU-level performance counters (L3 cache hits/misses, bus cycles, interconnect traffic) via `/sys/bus/event_source/devices/arm_dsu_0/`. Useful for verifying overclocking success by measuring actual DSU cycle counts.
- **Compilation error cleanup** — added 26 missing `External()` declarations to Ssdt.asl, fixed RX8900 RTC HID to `EPSO0001`, removed iasl error-suppression flags from the build so all warnings are visible.
- **SMMU HTTU override** — corrected the Hardware Translation Table Update flags on all 3 SMMU nodes. The EDK2 header defines `HTTU_OVERRIDE` as `BIT1` but HTTU is actually a 2-bit field at bits [2:1]. Changed from `BIT1` to `(2 << 1)` for HA+HD (Access+Dirty) to match hardware IDR0 capabilities and eliminate the `IDR0.HTTU features overridden by FW configuration (0x0)` kernel warning.
- **PCIe RMR bypass** — added a Reserved Memory Region node with all 10 PCIe root port Stream IDs mapped through the PCIe SMMU. This pre-installs bypass Stream Table Entries during SMMU init, preventing `F_TRANSLATION` faults before IOMMU domains are attached by device drivers.
- **CPU cache topology** — created a complete PPTT table describing the CIX CD8180's 12-core big.LITTLE topology: Cluster L0 (4× A520: 32KB L1I/D), Clusters M0/M1 (2× A720 each: 64KB L1I/D, 512KB L2), Clusters B0/B1 (2× A720 each: 64KB L1I/D, 512KB L2), shared 12MB L3 (Haydn DSU). Linux reports correct cache sizes in `/sys`.

### SMBIOS
- **Memory reporting** — corrected Type 17 form factor to `Die` (LPDDR5 on-package), added `VolatileSize` and `MemoryOperatingModeCapability` fields.

### Power management
- **GNVA initialization** — added `UpdateAcpiGpnv()` to the ReadyToBoot hook array so DSDT runtime variables are populated before Linux boots (was only running at ExitBootServices, too late for ACPI).
- **Shutdown** — rewrote PowerButtonDxe with proper event handling and 5-second polling. Green LED (GPIO 15) is turned off on shutdown.

### Memory Configuration
- **Multi-revision board support** — expanded BoardIdMap from 6 to 12 entries to support newer Orion O6 board revisions with different DRAM configurations. Added support for: 16G Hive Semi (HS), 24G HS, 32G x8 HS, 32G Hynix, 48G HS, and 64G Rayson RAM types. Includes per-vendor PHY pad and bus configuration blocks (Hive Semi, Hynix) and vendor-specific training optimizations. All changes use the old v1.6 binary format for compatibility with the existing MemConfigUpdateDxe firmware.

## Verifying the BIOS is working correctly

After booting into Linux, use the following commands to verify that the BIOS features are active.

### CPU frequencies and topology
```bash
# Check CPU core frequencies and cluster assignment
lscpu -e

# Check that cpufreq scaling is active
cat /sys/devices/system/cpu/cpufreq/policy*/scaling_max_freq
```

### Thermal sensors
```bash
# List all thermal zones with temperatures
for tz in /sys/class/thermal/thermal_zone*/; do
  echo "$(basename $tz): $(cat $tz/type) — $(cat $tz/temp)m°C"
done

# Or use lm-sensors
sudo apt install lm-sensors
sensors
```

All 16 thermal zones should appear (4 CPU clusters, 3 GPU, VPU, NPU, 2 DDR, CI700, SoC Bridge, SoC Trace, 2 NTC board sensors).

### CPU cache topology (PPTT)
```bash
# Verify L1/L2/L3 cache sizes are reported
lscpu | grep -i cache

# Per-core cache details
ls /sys/devices/system/cpu/cpu0/cache/index*/
```

### DSU performance counters
```bash
# Check if the DSU PMU is detected
ls /sys/bus/event_source/devices/arm_dsu_0/

# Read DSU cycle counts
perf stat -e arm_dsu_0/cycles/ sleep 1
```

### PCIe AER
```bash
# Verify AER is OS-controlled (should show AER in OS controls)
dmesg | grep _OSC
```

### SMMU
```bash
# Should show no HTTU override warnings
dmesg | grep -i httu
```
