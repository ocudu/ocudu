#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

"""
Generate initial seed corpus for the RRC UE fuzz harness from the known-good
(and known-bad) test vectors in the unit tests.

Each seed is a control byte followed by the RRC PDU. See tests/fuzz/README.md
for the control byte layout.

Usage:
    python3 tests/fuzz/rrc/gen_corpus.py

Seeds are written to:
    tests/fuzz/rrc/corpus/rrc_ue/
    tests/fuzz/rrc/corpus/rrc_cu_cp/
"""

import argparse
import pathlib
import zipfile

SCRIPT_DIR = pathlib.Path(__file__).parent

# ---------------------------------------------------------------------------
# Control byte
# ---------------------------------------------------------------------------

CCCH = 0b0000_0000
DCCH = 0b0000_0001
INTEGRITY_VERIFIED = 0b0000_0010
SRB2 = 0b0000_0100

# UE state reached before the payload is injected.
FRESH = 0 << 3
AWAITING_SETUP_COMPLETE = 1 << 3
CONNECTED = 2 << 3
SECURED = 3 << 3


def seed(control: int, pdu: str) -> bytes:
    return bytes([control]) + bytes.fromhex(pdu)


# ---------------------------------------------------------------------------
# RRC corpus seeds
# ---------------------------------------------------------------------------
# PDUs taken from tests/unittests/rrc/rrc_ue_test_helpers.h. The DCCH vectors
# there are PDCP PDUs; the 2-byte PDCP header and the 4-byte MAC-I are stripped
# because the harness injects above PDCP.
# ---------------------------------------------------------------------------
RRC_SETUP_COMPLETE = (
    "10c01000082727e01c3ff100c047e004139000bf202f8998000410000000"
    "f2e04f070f0707100517e004139000bf202f8998000410000000f1001032"
    "e04f070f0702f1b08010027db00000000080101b66900000000080100000"
    "1000000005202f8990000011707f070c0401980b01801017400009053010"
    "1000000000"
)

RRC_SEEDS = {
    # RRCSetupRequest on UL-CCCH against a fresh UE: the pre-authentication entry point.
    "ccch_setup_request": seed(CCCH | FRESH, "1dec89d05766"),

    # Corrupted UL-CCCH message (triggers the unpack failure path).
    "ccch_invalid": seed(CCCH | FRESH, "9dec89de5766"),

    # RRCSetupComplete on SRB1, unprotected, against a UE awaiting it.
    "dcch_setup_complete": seed(DCCH | AWAITING_SETUP_COMPLETE, RRC_SETUP_COMPLETE),

    # SecurityModeComplete on SRB1 with integrity verified, as the CU-CP expects it.
    "dcch_smc_complete": seed(DCCH | INTEGRITY_VERIFIED | CONNECTED, "2a00"),

    # Same message without integrity protection: TS 38.331 Annex B1 requires it to be rejected.
    "dcch_smc_complete_unprotected": seed(DCCH | CONNECTED, "2a00"),

    # RRCReconfigurationComplete on SRB1 after security activation.
    "dcch_reconfig_complete": seed(DCCH | INTEGRITY_VERIFIED | SECURED, "0a00"),

    # RRCReestablishmentComplete on SRB1 after security activation.
    "dcch_reest_complete": seed(DCCH | INTEGRITY_VERIFIED | SECURED, "1800"),

    # UL-DCCH on SRB2, which only exists after security activation.
    "dcch_srb2": seed(DCCH | INTEGRITY_VERIFIED | SRB2 | SECURED, "0a00"),

    # Truncated payload (triggers the early-exit path).
    "dcch_truncated": seed(DCCH | SECURED, "2a"),

    # Corrupt / non-RRC payload (triggers the unpack failure path).
    "dcch_invalid": seed(DCCH | INTEGRITY_VERIFIED | SECURED, "deadbeef"),
}

# ---------------------------------------------------------------------------
# rrc_cu_cp corpus seeds
# ---------------------------------------------------------------------------
# The full-stack harness uses a control byte of its own: bit 0 selects the
# logical channel, bit 1 the SRB and bits 2-3 the UE state. It has no
# integrity_verified bit, because PDCP derives that from the MAC-I the harness
# computes.
# ---------------------------------------------------------------------------

CU_CP_DCCH = 0b0000_0001
CU_CP_SRB2 = 0b0000_0010
CU_CP_AWAITING_SETUP_COMPLETE = 0 << 2
CU_CP_CONNECTED = 1 << 2
CU_CP_SECURED = 2 << 2

RRC_CU_CP_SEEDS = {
    # RRCSetupRequest on UL-CCCH: the pre-authentication entry point.
    "ccch_setup_request": seed(CU_CP_AWAITING_SETUP_COMPLETE, "1dec89d05766"),

    # Corrupted UL-CCCH message (triggers the unpack failure path).
    "ccch_invalid": seed(CU_CP_AWAITING_SETUP_COMPLETE, "9dec89de5766"),

    # RRCResumeRequest on UL-CCCH, which the CU-CP routes by its resume identity.
    "ccch_resume_request": seed(CU_CP_AWAITING_SETUP_COMPLETE, "20202066f020"),

    # RRCSetupComplete on SRB1 against a UE awaiting it.
    "dcch_setup_complete": seed(CU_CP_DCCH | CU_CP_AWAITING_SETUP_COMPLETE, RRC_SETUP_COMPLETE),

    # UL-DCCH against a connected UE, before security activation.
    "dcch_ul_info_transfer": seed(CU_CP_DCCH | CU_CP_CONNECTED, "0000"),

    # RRCReconfigurationComplete on SRB1 after security activation.
    "dcch_reconfig_complete": seed(CU_CP_DCCH | CU_CP_SECURED, "0a00"),

    # MeasurementReport on SRB1 after security activation.
    "dcch_meas_report": seed(CU_CP_DCCH | CU_CP_SECURED, "0800"),

    # UL-DCCH on SRB2, which only exists after security activation.
    "dcch_srb2": seed(CU_CP_DCCH | CU_CP_SRB2 | CU_CP_SECURED, "0000"),

    # Truncated payload (triggers the early-exit path).
    "dcch_truncated": seed(CU_CP_DCCH | CU_CP_SECURED, "2a"),

    # Corrupt / non-RRC payload (triggers the unpack failure path).
    "dcch_invalid": seed(CU_CP_DCCH | CU_CP_SECURED, "deadbeef"),
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
                        help="also write rrc_ue seeds to a zip file (for OSS-Fuzz)")
    parser.add_argument("--zip-cu-cp", metavar="PATH", type=pathlib.Path,
                        help="also write rrc_cu_cp seeds to a zip file (for OSS-Fuzz)")
    args = parser.parse_args()

    print("Generating RRC fuzz corpus seeds...")
    write_seeds(RRC_SEEDS, "rrc_ue")
    write_seeds(RRC_CU_CP_SEEDS, "rrc_cu_cp")
    if args.zip:
        write_zip(RRC_SEEDS, args.zip)
    if args.zip_cu_cp:
        write_zip(RRC_CU_CP_SEEDS, args.zip_cu_cp)
    print("Done.")


if __name__ == "__main__":
    main()
