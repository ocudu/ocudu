#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

"""Generate E1AP corpus from the asn1_e1ap_test.cpp unit-test fixtures."""

import argparse
import pathlib
import zipfile

SCRIPT_DIR = pathlib.Path(__file__).parent

E1AP_SEEDS = {
    "bearer_context_setup_request.bin": bytes.fromhex(
        "00080069000007000200020009000d0013000010a6ae39efbe0d424cd85f4a9c3aee"
        "0414000e0005303b9aca00003a400302f8990017000140000f002a400001002a0023"
        "0040010002123005f5e10001f0ac15048600000036000301300a20ef000000800009"
        "7a004d40020000"
    ),
    "bearer_context_setup_response.bin": bytes.fromhex(
        "200800370000030002000200090003000340028000104023400001002e401c000001"
        "01f0ac150609000002830006001fac15060980000283000080"
    ),
}


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--zip", metavar="PATH", type=pathlib.Path,
                        help="also write the OSS-Fuzz seed archive")
    args = parser.parse_args()
    out_dir = SCRIPT_DIR / "corpus"
    out_dir.mkdir(parents=True, exist_ok=True)
    for name, data in E1AP_SEEDS.items():
        (out_dir / name).write_bytes(data)
    if args.zip:
        args.zip.parent.mkdir(parents=True, exist_ok=True)
        with zipfile.ZipFile(args.zip, "w", zipfile.ZIP_DEFLATED) as archive:
            for name, data in E1AP_SEEDS.items():
                archive.writestr(name, data)


if __name__ == "__main__":
    main()
