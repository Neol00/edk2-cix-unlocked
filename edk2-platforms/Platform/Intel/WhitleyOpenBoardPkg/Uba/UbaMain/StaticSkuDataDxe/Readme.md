# **Generating ACPI AML Offset table**

The StaticSkuDataDxe driver includes AmlOffsetTable.c which contains the AML offset table.  This Readme captures the build process for this file.

edk2-platforms/Platform/Intel/WhitleyOpenBoardPkg/WilsonCityRvp/build_board.py drives the build since ACPI tables are expected to be board specific.

The AmlOffsetTable.c file is generated in two key steps:
1. Generate the *Dsdt*.offsets.h where *Dsdt* is the DSDT ASL file name specific for this build. For WilsonCityRvp:
   * The DSDT is WhitleyOpenBoardPkg/Features/Acpi/AcpiTables/Dsdt/EPRPPlatform10nm.asl
   * The generated file is Build/WhitleyOpenBoardPkg/DEBUG_VS2015x86/X64/WhitleyOpenBoardPkg/WilsonCityRvp/AmlOffsets/AmlOffsets/OUTPUT/WhitleyOpenBoardPkg/Features/Acpi/AcpiTables/Dsdt/EPRPPlatform10nm.offset.h
   * These are customized in the build_config.cfg file
2. Generate the AmlOffsetTable.c using the AmlGenOffset.py script from the MinPlatformPkg. For WilsonCityRvp:
   * Script is at edk2-platforms/Platform/Intel/MinPlatformPkg/Tools/AmlGenOffset/AmlGenOffset.py
   * Input is Build/WhitleyOpenBoardPkg/DEBUG_VS2015x86/X64/WhitleyOpenBoardPkg/WilsonCityRvp/AmlOffsets/AmlOffsets/OUTPUT/WhitleyOpenBoardPkg/Features/Acpi/AcpiTables/Dsdt/EPRPPlatform10nm.offset.h
   * Output is edk2-platforms/Platform/Intel/WhitleyOpenBoardPkg/Uba/UbaMain/StaticSkuDataDxe/AmlOffsetTable.c

Common Issues:
* The same iasl compiler version must be used to build the AML offset table and to build the DSDT.
* The same iasl compiler version must be used to build the AML offset table and to build the DSDT.  With the addition of -so for building the AML offset table.
* The Board/*AmlOffsets*.dsc file name, Board/*AmlOffsets* directory name, Board/AmlOffsets/*AmlOffsets*.inf file name, and the BASE_NAME in *AmlOffsets*.inf must all match exactly.
