#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

"""
Generate initial seed corpus for the OFH fuzz harnesses from the known-good
(and known-bad) test vectors in the unit tests.

Usage:
    python3 tests/fuzz/ofh/gen_corpus.py

Seeds are written to the corpus sub-directories alongside this script:
    corpus/uplane/   - OFH U-Plane message decoder seeds
    corpus/ecpri/    - eCPRI packet decoder seeds
    corpus/vlan/     - VLAN Ethernet frame decoder seeds
"""

import os
import pathlib

SCRIPT_DIR = pathlib.Path(__file__).parent

# ---------------------------------------------------------------------------
# U-Plane corpus seeds
# Taken from tests/unittests/ofh/serdes/ofh_uplane_packet_decoder_static_impl_test.cpp
# ---------------------------------------------------------------------------
UPLANE_SEEDS = {
    # Valid full packet with 3 PRBs, no-compression, 16-bit samples.
    "valid_3prb_none_16bit": bytes.fromhex(
        "1002404200702403017c017c0186018601900190019a019a01a401a401ae01ae01b801b801c201c201cc01cc01d601d601e001e001ea01ea01f401f401fe01fe0208020802120212021c021c"
        "0226022602300230023a023a02440244024e024e0258025802620262026c026c0276027602800280028a028a02940294029e029e02a802a802b202b202bc02bc02c602c602d002d002da02da"
    ),
    # Valid 1-PRB BFP-9 packet (nof_prbs=0 means use RU default of 2).
    "valid_bfp9_ru2prbs": bytes.fromhex(
        "100240420070240000017c017c0186018601900190019a019a01a40186018601"
        "900190019a019a01a40186018601900190019a019a01a4010101010101010101"
    ),
    # Header only (triggers early-exit path).
    "header_only": bytes.fromhex("10024042007024"),
    # PRACH filter index variant.
    "prach_filter_index": bytes.fromhex(
        "110240420070240000017c017c0186018601900190019a019a01a4018601860190019001"
        "9a019a01a40186018601900190019a019a01a4010101010101010101000000000000000000"
    ),
}

# ---------------------------------------------------------------------------
# eCPRI corpus seeds
# Taken from tests/unittests/ofh/ecpri/ecpri_packet_decoder_impl_test.cpp
# ---------------------------------------------------------------------------
ECPRI_SEEDS = {
    # Valid real-time control packet.
    "valid_rt_control": bytes.fromhex("10020014bebecafe900000000101000000000000fffe0000"),
    # Valid IQ data packet.
    "valid_iq_data": bytes.fromhex("10000014cafedeba900000000101000000000000fffe0000"),
    # Truncated payload (triggers error path).
    "truncated_payload": bytes.fromhex("10000003cafede"),
}

# ---------------------------------------------------------------------------
# VLAN frame corpus seeds
# Taken from tests/unittests/ofh/ethernet/vlan_ethernet_frame_decoder_test.cpp
# ---------------------------------------------------------------------------
VLAN_SEEDS = {
    # Valid VLAN-tagged Ethernet frame (14-byte header + payload).
    "valid_vlan_frame": bytes.fromhex(
        "00112233445580615f0ddfaaaabb660000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000"
    ),
    # Truncated frame (only header, no payload).
    "truncated_frame": bytes.fromhex("00112233445580615f0ddfaaaabb"),
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


def main() -> None:
    print("Generating OFH fuzz corpus seeds...")
    write_seeds(UPLANE_SEEDS, "uplane")
    write_seeds(ECPRI_SEEDS,  "ecpri")
    write_seeds(VLAN_SEEDS,   "vlan")
    print("Done.")


if __name__ == "__main__":
    main()
