## @file
#
# Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

################################################################################
#
# Rules are used with the [FV] section's module INF type to define
# how an FFS file is created for a given INF file. The following Rule are the default
# rules for the different module type. User can add the customized rules to define the
# content of the FFS file.
#
################################################################################

############################################################################
# Example of a DXE_DRIVER FFS file with a Checksum encapsulation section   #
############################################################################
#
#[Rule.Common.DXE_DRIVER]
#  FILE DRIVER = $(NAMED_GUID) {
#    DXE_DEPEX    DXE_DEPEX               Optional $(INF_OUTPUT)/$(MODULE_NAME).depex
#    COMPRESS PI_STD {
#      GUIDED {
#        PE32     PE32                    $(INF_OUTPUT)/$(MODULE_NAME).efi
#        UI       STRING="$(MODULE_NAME)" Optional
#        VERSION  STRING="$(INF_VERSION)" Optional BUILD_NUM=$(BUILD_NUMBER)
#      }
#    }
#  }
#
############################################################################


[Rule.Common.SEC]
  FILE SEC = $(NAMED_GUID) RELOCS_STRIPPED FIXED {
    TE  TE Align = Auto                 $(INF_OUTPUT)/$(MODULE_NAME).efi
  }

[Rule.Common.PEI_CORE]
  FILE PEI_CORE = $(NAMED_GUID) FIXED {
    TE     TE Align = Auto              $(INF_OUTPUT)/$(MODULE_NAME).efi
    UI     STRING ="$(MODULE_NAME)" Optional
  }

[Rule.Common.PEIM]
  FILE PEIM = $(NAMED_GUID) FIXED {
     PEI_DEPEX PEI_DEPEX Optional       $(INF_OUTPUT)/$(MODULE_NAME).depex
     TE       TE Align = Auto           $(INF_OUTPUT)/$(MODULE_NAME).efi
     UI       STRING="$(MODULE_NAME)" Optional
  }

[Rule.Common.PEIM.Binary]
  FILE PEIM = $(NAMED_GUID) {
     PEI_DEPEX PEI_DEPEX Optional       |.depex
     TE       TE Align = Auto           |.efi
     UI       STRING="$(MODULE_NAME)" Optional
  }

[Rule.Common.PEIM.TIANOCOMPRESSED]
  FILE PEIM = $(NAMED_GUID) DEBUG_MYTOOLS_IA32 {
    PEI_DEPEX PEI_DEPEX Optional        $(INF_OUTPUT)/$(MODULE_NAME).depex
    GUIDED A31280AD-481E-41B6-95E8-127F4C984779 PROCESSING_REQUIRED = TRUE {
      PE32      PE32                    $(INF_OUTPUT)/$(MODULE_NAME).efi
      UI        STRING="$(MODULE_NAME)" Optional
    }
  }

[Rule.Common.DXE_CORE]
  FILE DXE_CORE = $(NAMED_GUID) {
    COMPRESS PI_STD {
      GUIDED {
        PE32     PE32                       $(INF_OUTPUT)/$(MODULE_NAME).efi
        UI       STRING="$(MODULE_NAME)" Optional
      }
    }
  }

[Rule.Common.UEFI_DRIVER]
  FILE DRIVER = $(NAMED_GUID) {
    DXE_DEPEX    DXE_DEPEX              Optional $(INF_OUTPUT)/$(MODULE_NAME).depex
    COMPRESS PI_STD {
      GUIDED {
        PE32         PE32                   $(INF_OUTPUT)/$(MODULE_NAME).efi
        UI           STRING="$(MODULE_NAME)" Optional
      }
    }
  }

[Rule.Common.DXE_DRIVER]
  FILE DRIVER = $(NAMED_GUID) {
    DXE_DEPEX    DXE_DEPEX              Optional $(INF_OUTPUT)/$(MODULE_NAME).depex
    COMPRESS PI_STD {
      GUIDED {
        PE32         PE32                   $(INF_OUTPUT)/$(MODULE_NAME).efi
        UI           STRING="$(MODULE_NAME)" Optional
        RAW          ACPI  Optional               |.acpi
        RAW          ASL   Optional               |.aml
      }
    }
  }

[Rule.Common.DXE_RUNTIME_DRIVER]
  FILE DRIVER = $(NAMED_GUID) {
    DXE_DEPEX    DXE_DEPEX              Optional $(INF_OUTPUT)/$(MODULE_NAME).depex
    COMPRESS PI_STD {
      GUIDED {
        PE32         PE32                   $(INF_OUTPUT)/$(MODULE_NAME).efi
        UI           STRING="$(MODULE_NAME)" Optional
      }
    }
  }

[Rule.Common.DXE_RUNTIME_DRIVER.Binary]
  FILE DRIVER = $(NAMED_GUID) {
    DXE_DEPEX    DXE_DEPEX              Optional |.depex
    COMPRESS PI_STD {
      GUIDED {
        PE32         PE32                   |.efi
        UI           STRING="$(MODULE_NAME)" Optional
      }
    }
  }

[Rule.Common.UEFI_APPLICATION]
  FILE APPLICATION = $(NAMED_GUID) {
    COMPRESS PI_STD {
      GUIDED {
        UI     STRING ="$(MODULE_NAME)"     Optional
        PE32   PE32                         $(INF_OUTPUT)/$(MODULE_NAME).efi
      }
    }
  }

[Rule.Common.UEFI_DRIVER.BINARY]
  FILE DRIVER = $(NAMED_GUID) {
    DXE_DEPEX DXE_DEPEX Optional      |.depex
    COMPRESS PI_STD {
      GUIDED {
        PE32      PE32                    |.efi
        UI        STRING="$(MODULE_NAME)" Optional
        VERSION   STRING="$(INF_VERSION)" Optional BUILD_NUM=$(BUILD_NUMBER)
      }
    }
  }

[Rule.Common.UEFI_DRIVER.Binary]
  FILE DRIVER = $(NAMED_GUID) {
    PE32         PE32                   |.efi
    UI           STRING="$(MODULE_NAME)" Optional
    VERSION      STRING="$(INF_VERSION)" Optional BUILD_NUM=$(BUILD_NUMBER)
  }

[Rule.Common.UEFI_APPLICATION.BINARY]
  FILE APPLICATION = $(NAMED_GUID) {
    PE32      PE32                    |.efi
    UI        STRING="$(MODULE_NAME)" Optional
    VERSION   STRING="$(INF_VERSION)" Optional BUILD_NUM=$(BUILD_NUMBER)
  }

[Rule.Common.USER_DEFINED.ACPITABLE]
  FILE FREEFORM = $(NAMED_GUID) {
    RAW ACPI Optional           |.acpi
    RAW ASL  Optional           |.aml
  }

[Rule.Common.PEIM.FMP_IMAGE_DESC]
  FILE PEIM = $(NAMED_GUID) {
     RAW BIN                  |.acpi
     PEI_DEPEX PEI_DEPEX Optional        $(INF_OUTPUT)/$(MODULE_NAME).depex
     PE32      PE32    Align=4K          $(INF_OUTPUT)/$(MODULE_NAME).efi
     UI       STRING="$(MODULE_NAME)" Optional
     VERSION  STRING="$(INF_VERSION)" Optional BUILD_NUM=$(BUILD_NUMBER)
  }
