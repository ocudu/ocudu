#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

"""
Generate initial seed corpus for the NGAP fuzz harness from the known-good
(and known-bad) test vectors in the unit tests.

Usage:
    python3 tests/fuzz/ngap/gen_corpus.py

Seeds are written to:
    tests/fuzz/ngap/corpus/ngap/
"""

import argparse
import pathlib
import zipfile

SCRIPT_DIR = pathlib.Path(__file__).parent

# ---------------------------------------------------------------------------
# NGAP corpus seeds
# ---------------------------------------------------------------------------
# Taken from tests/unittests/ngap/ngap_test_messages.h and
# tests/unittests/ngap/ngap_asn1_packer_test.cpp
# ---------------------------------------------------------------------------
NGAP_SEEDS = {
    # NGSetupRequest (initiatingMessage / id-NGSetup).
    # Source: ng_setup_request_packed[] in ngap_test_messages.h
    # Contains: GlobalRANNodeID, RANNodeName, SupportedTAList, DefaultPagingDRX.
    "ng_setup_request": bytes.fromhex(
        "00150033000004001b00080000f1100000066c0052400a03807473"
        "74676e6230310066000d00000000070000f110000000080015400160"
    ),

    # InitialContextSetupRequest (initiatingMessage / id-InitialContextSetup).
    # Source: ngap_init_ctx_req in ngap_asn1_packer_test.cpp
    # Contains: security key, UE security capabilities, NAS PDU.
    "initial_context_setup_request": bytes.fromhex(
        "000e008090000008000a0002000c005500020000001c00070000f11002004000000002000100"
        "77000918000c000000000000005e002050636e38151d62356d9a1a0c9f2391885177307ad4"
        "94be15281dfe5fdac06302002240080123456700ffff010026402f2e7e02cf5b405e017e00"
        "42010177000bf200f110020040dd00b06454072000f11000000715020101210201005e0129"
    ),

    # PDU header only (triggers early-exit / truncated-input path).
    "truncated_header": bytes.fromhex("001500"),

    # Corrupt / non-NGAP payload (triggers unpack failure path).
    "invalid_payload": bytes.fromhex("deadbeef"),
}

# ---------------------------------------------------------------------------
# Write seeds to disk
# ---------------------------------------------------------------------------

def write_seeds(seeds: dict, subdir: str) -> None:
    out_dir = SCRIPT_DIR / "corpus" / subdir
    out_dir.mkdir(parents=True, exist_ok=True)
    for name, data in seeds.items():
        path = out_dir / name
        path.write_bytes(data)
        print(f"  wrote {path} ({len(data)} bytes)")


def write_zip(seeds: dict, zip_path: pathlib.Path) -> None:
    zip_path.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as zf:
        for name, data in seeds.items():
            zf.writestr(name, data)
    print(f"  wrote zip {zip_path} ({zip_path.stat().st_size} bytes)")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--zip", metavar="PATH", type=pathlib.Path,
                        help="also write seeds to a zip file (for OSS-Fuzz)")
    args = parser.parse_args()

    print("Generating NGAP fuzz corpus seeds...")
    write_seeds(NGAP_SEEDS, "ngap")
    if args.zip:
        write_zip(NGAP_SEEDS, args.zip)
    print("Done.")


if __name__ == "__main__":
    main()
