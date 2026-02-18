## @file
#
#  Copyright 2026 Cix Technology Group Co., Ltd. All Rights Reserved.
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import sys
import subprocess
import logging
from pathlib import Path
import argparse

def setup_logging():
    logging.basicConfig(
        level=logging.INFO,
        format='%(asctime)s [%(levelname)s] %(message)s',
        handlers=[
            logging.StreamHandler(sys.stdout)
        ]
    )
    return logging.getLogger()

def run_m4_process(m4_file: Path, output_file: Path, includes: list) -> bool:
    logger = logging.getLogger()

    cmd = [
        "m4",
        *[f"-I{dir}" for dir in includes],
        str(m4_file)
    ]

    logger.info(f"Executing: {' '.join(cmd)} > {output_file}")
    output_file.parent.mkdir(parents=True, exist_ok=True)

    try:
        with open(output_file, 'w', encoding='utf-8') as f:
            result = subprocess.run(
                cmd,
                stdout=f,
                stderr=subprocess.PIPE,
                text=True,
                check=True
            )
        return True
    except subprocess.CalledProcessError as e:
        logging.error(f"m4 failed with code {e.returncode}:\n{e.stderr}")
        return False

def main():
    workspace = os.getenv("WORKSPACE", "")
    if not workspace:
        print("ERROR: WORKSPACE environment variable not set!", file=sys.stderr)
        return 1

    parser = argparse.ArgumentParser()
    parser.add_argument("input_file", help="input file")
    parser.add_argument("output_file", help="output file")
    parser.add_argument("extra_args", nargs=argparse.REMAINDER, help="extra args")

    args = parser.parse_args()

    base_dir = Path(workspace)
    inc_dir = Path(workspace) / "edk2-platforms/Platform/CIX/Sky1/Drivers/AcpiSocTables/tool/python3"
    m4_file = base_dir / args.input_file
    output_file = base_dir / args.output_file

    logger = setup_logging()
    logger.info("=== Starting ACPI PreBuild ===")

    if not m4_file.exists():
        logger.error(f"Input file not found: {m4_file}")
        return 1

    success = run_m4_process(
        m4_file=m4_file,
        output_file=output_file,
        includes=[str(inc_dir)]
    )

    if not success:
        logger.error("ACPI preprocessing failed!")
        return 1

    logger.info(f"Successfully generated: {output_file}")
    return 0

if __name__ == "__main__":
    sys.exit(main())
