# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

"""
Seed corpus of the GTP-U fuzz harness, from the known-good test vectors in the unit tests.

Each seed is a raw GTP-U PDU, with no harness prefix. See tests/fuzz/README.md.

Written to disk by tests/fuzz/gen_corpus.py, see CORPORA at the end of this file.
"""

# ---------------------------------------------------------------------------
# GTP-U corpus seeds
# ---------------------------------------------------------------------------
# Taken from tests/unittests/gtpu/gtpu_test_shared.h
# ---------------------------------------------------------------------------
GTPU_SEEDS = {
    # G-PDU carrying an IPv4/ICMP echo request payload (TEID 1, no sequence
    # number and no extension headers).
    # Source: gtpu_ping_vec_teid_1[] in gtpu_test_shared.h
    "gtpu_ping_vec_teid_1": bytes.fromhex(
        "30ff00540000000145000054e88340004001fa00ac100003ac10000108002cbeb4a4"
        "0001d3456163000000001a20090000000000101112131415161718191a1b1c1d1e1f"
        "202122232425262728292a2b2c2d2e2f3031323334353637"
    ),
    # G-PDU with sequence number and one PDU Session Container extension
    # header, carrying an IPv4/ICMP echo request payload.
    # Source: gtpu_ping_ext_vec[] in gtpu_test_shared.h
    "gtpu_ping_ext_vec": bytes.fromhex(
        "34ff005c000000010000008501000100450000541f6c00004001469c0a2d00010a2d"
        "0047000086b00004000d0170c16300000000f0970700000000001011121314151617"
        "18191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f3031323334353637"
    ),
    # G-PDU with sequence number and two chained extension headers (PDCP PDU
    # Number followed by PDU Session Container), carrying an IPv4/ICMP echo
    # request payload.
    # Source: gtpu_ping_two_ext_vec[] in gtpu_test_shared.h
    "gtpu_ping_two_ext_vec": bytes.fromhex(
        "34ff006000000001000000c00100018501000100450000541f6c00004001469c0a2d"
        "00010a2d0047000086b00004000d0170c16300000000f09707000000000010111213"
        "1415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f3031323334"
        "353637"
    ),
    # G-PDU with sequence number and a Long PDCP PDU Number extension header
    # (PDCP SN 0), carrying the tpdu_1[] test payload.
    # Source: gpdu_tpdu_1_teid_1_long_pdcp_sn_0[] in gtpu_test_shared.h
    "gpdu_tpdu_1_teid_1_long_pdcp_sn_0": bytes.fromhex("34ff000f00000002000000030200000000000000112233"),
    # Echo Request control message (sequence number 0).
    # Source: gtpu_echo_request_sn_0[] in gtpu_test_shared.h
    "gtpu_echo_request_sn_0": bytes.fromhex("320100040000000000000000"),
    # Echo Response control message (sequence number 0).
    # Source: gtpu_echo_response_sn_0[] in gtpu_test_shared.h
    "gtpu_echo_response_sn_0": bytes.fromhex("3202000600000000000000000e00"),
    # Error Indication control message carrying extension header data.
    # Source: gtpu_error_indication[] in gtpu_test_shared.h
    "gtpu_error_indication": bytes.fromhex("361a00180000000000000085010001400100000010000012678500040a350102"),
}

# ---------------------------------------------------------------------------
# Seeds of each fuzz target, with the name of its corpus sub-directory
# ---------------------------------------------------------------------------
CORPORA = {
    "gtpu_pdu_fuzzer": ("gtpu", GTPU_SEEDS),
}
