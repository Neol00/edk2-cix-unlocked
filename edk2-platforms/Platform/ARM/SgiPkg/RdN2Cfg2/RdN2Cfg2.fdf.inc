#
#  Copyright (c) 2022, Arm Limited. All rights reserved.
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#

# Per-platform additional content of the DXE phase firmware volume

  # ACPI support
  INF RuleOverride=ACPITABLE Platform/ARM/SgiPkg/AcpiTables/RdN2Cfg2AcpiTables.inf
