# @ build_board.py
# Extensions for building Aowanda using build_bios.py
#
#
# Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
# Copyright (c) 2022, American Megatrends International LLC. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

"""
This module serves as a sample implementation of the build extension
scripts
"""

import os
import sys

def pre_build_ex(config, functions):
    """Additional Pre BIOS build function

    :param config: The environment variables to be used in the build process
    :type config: Dictionary
    :param functions: A dictionary of function pointers
    :type functions: Dictionary
    :returns: nothing
    """
    print("pre_build_ex")

    config["BUILD_DIR_PATH"] = os.path.join(config["WORKSPACE"],
                                            'Build',
                                            config["PLATFORM_BOARD_PACKAGE"],
                                            "{}_{}".format(
                                                config["TARGET"],
                                                config["TOOL_CHAIN_TAG"]))
    # set BUILD_DIR path
    config["BUILD_DIR"] = os.path.join('Build',
                                       config["PLATFORM_BOARD_PACKAGE"],
                                       "{}_{}".format(
                                           config["TARGET"],
                                           config["TOOL_CHAIN_TAG"]))
    config["BUILD_X64"] = os.path.join(config["BUILD_DIR_PATH"], 'X64')
    config["BUILD_IA32"] = os.path.join(config["BUILD_DIR_PATH"], 'IA32')

    if not os.path.isdir(config["BUILD_DIR_PATH"]):
        try:
            os.makedirs(config["BUILD_DIR_PATH"])
        except OSError:
            print("Error while creating Build folder")
            sys.exit(1)

    #@todo: Replace this with PcdFspModeSelection
    if config.get("API_MODE_FSP_WRAPPER_BUILD", "FALSE") == "TRUE":
        config["EXT_BUILD_FLAGS"] += " -D FSP_MODE=0"
    else:
        config["EXT_BUILD_FLAGS"] += " -D FSP_MODE=1"

    if config.get("API_MODE_FSP_WRAPPER_BUILD", "FALSE") == "TRUE":
        raise ValueError("FSP API Mode is currently unsupported on Ice Lake Xeon Scalable")

    # Build the ACPI AML offset table *.offset.h
    print("Info: re-generating PlatformOffset header files")

    execute_script = functions.get("execute_script")

    # AML offset arch is X64, not sure if it matters.
    command = ["build", "-a", "X64", "-t", config["TOOL_CHAIN_TAG"], "-D", "MAX_SOCKET=" + config["MAX_SOCKET"]]

    if config["EXT_BUILD_FLAGS"] and config["EXT_BUILD_FLAGS"] != "":
        ext_build_flags = config["EXT_BUILD_FLAGS"].split(" ")
        ext_build_flags = [x.strip() for x in ext_build_flags]
        ext_build_flags = [x for x in ext_build_flags if x != ""]
        command.extend(ext_build_flags)

    aml_offsets_split = os.path.split(os.path.normpath(config["AML_OFFSETS_PATH"]))
    command.append("-p")
    command.append(os.path.normpath(config["AML_OFFSETS_PATH"]) + '.dsc')
    command.append("-m")
    command.append(os.path.join(aml_offsets_split[0], aml_offsets_split[1], aml_offsets_split[1] + '.inf'))
    command.append("-y")
    command.append(os.path.join(config["WORKSPACE"], "PreBuildReport.txt"))
    command.append("--log=" + os.path.join(config["WORKSPACE"], "PreBuild.log"))

    shell = True
    if os.name == "posix":  # linux
        shell = False

    _, _, _, code = execute_script(command, config, shell=shell)
    if code != 0:
        print(" ".join(command))
        print("Error re-generating PlatformOffset header files")
        sys.exit(1)

    # Build AmlGenOffset command to consume the *.offset.h and produce AmlOffsetTable.c for StaticSkuDataDxe use.

    # Get destination path and filename from config
    relative_file_path = os.path.normpath(config["STRIPPED_AML_OFFSETS_FILE_PATH"])     # get path relative to Platform/Intel
    out_file_path = os.path.join(config["WORKSPACE_PLATFORM"], relative_file_path)      # full path to output file
    out_file_dir = os.path.dirname(out_file_path)                                       # remove filename

    out_file_root_ext = os.path.splitext(os.path.basename(out_file_path))               # root and extension of output file

    # Get relative path for the generated offset.h file
    relative_dsdt_file_path = os.path.normpath(config["DSDT_TABLE_FILE_PATH"])          # path relative to Platform/Intel
    dsdt_file_root_ext = os.path.splitext(os.path.basename(relative_dsdt_file_path))    # root and extension of generated offset.h file

    # Generate output directory if it doesn't exist
    if not os.path.exists(out_file_dir):
        os.mkdir(out_file_dir)

    command = ["python",
               os.path.join(config["MIN_PACKAGE_TOOLS"], "AmlGenOffset", "AmlGenOffset.py"),
               "-d", "--aml_filter", config["AML_FILTER"],
               "-o", out_file_path,
               os.path.join(config["BUILD_X64"], aml_offsets_split[0], aml_offsets_split[1], aml_offsets_split[1], "OUTPUT", os.path.dirname(relative_dsdt_file_path), dsdt_file_root_ext[0] + ".offset.h")]

    # execute the command
    _, _, _, code = execute_script(command, config, shell=shell)
    if code != 0:
        print(" ".join(command))
        print("Error re-generating PlatformOffset header files")
        sys.exit(1)

    print("GenOffset done")


    return None

def _merge_files(files, ofile):
    with open(ofile, 'wb') as of:
        for x in files:
            if not os.path.exists(x):
                return

            with open(x, 'rb') as f:
                of.write(f.read())

def build_ex(config, functions):
    """Additional BIOS build function

    :param config: The environment variables to be used in the build process
    :type config: Dictionary
    :param functions: A dictionary of function pointers
    :type functions: Dictionary
    :returns: config dictionary
    :rtype: Dictionary
    """
    print("build_ex")
    fv_path = os.path.join(config["BUILD_DIR_PATH"], "FV")
    binary_fd = os.path.join(fv_path, "BINARY.fd")
    main_fd = os.path.join(fv_path, "MAIN.fd")
    secpei_fd = os.path.join(fv_path, "SECPEI.fd")
    board_fd = config["BOARD"].upper()
    final_fd = os.path.join(fv_path, "{}.fd".format(board_fd))
    _merge_files((binary_fd, main_fd, secpei_fd), final_fd)
    return None


def post_build_ex(config, functions):
    """Additional Post BIOS build function

    :param config: The environment variables to be used in the post
        build process
    :type config: Dictionary
    :param functions: A dictionary of function pointers
    :type functions: Dictionary
    :returns: config dictionary
    :rtype: Dictionary
    """
    print("post_build_ex")
    fv_path = os.path.join(config["BUILD_DIR_PATH"], "FV")
    board_fd = config["BOARD"].upper()
    final_fd = os.path.join(fv_path, "{}.fd".format(board_fd))
    final_ifwi = os.path.join(fv_path, "{}.bin".format(board_fd))

    ifwi_ingredients_path = os.path.join(config["WORKSPACE_PLATFORM_BIN"], "Ifwi", config["BOARD"])
    flash_descriptor = os.path.join(ifwi_ingredients_path, "FlashDescriptor.bin")
    intel_me = os.path.join(ifwi_ingredients_path, "Me.bin")
    _merge_files((flash_descriptor, intel_me, final_fd), final_ifwi)
    if os.path.isfile(final_fd):
        print("IFWI image can be found at {}".format(final_ifwi))
    return None


def clean_ex(config, functions):
    """Additional clean function

    :param config: The environment variables to be used in the build process
    :type config: Dictionary
    :param functions: A dictionary of function pointers
    :type functions: Dictionary
    :returns: config dictionary
    :rtype: Dictionary
    """
    print("clean_ex")
    return None
