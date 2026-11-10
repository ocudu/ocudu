# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

"""
Seed corpus of the E1AP fuzz harness, from the known-good test vectors in the unit tests.

Each seed is a raw E1AP PDU, with no harness prefix. See tests/fuzz/README.md.

Written to disk by tests/fuzz/gen_corpus.py, see CORPORA at the end of this file.
"""

# ---------------------------------------------------------------------------
# E1AP corpus seeds
# ---------------------------------------------------------------------------
# Taken from the rx_msg fixtures in tests/unittests/asn1/asn1_e1ap_test.cpp
# ---------------------------------------------------------------------------
E1AP_SEEDS = {
    # Bearer Context Setup Request (initiatingMessage).
    # Source: when_bearer_context_setup_request_correct_then_unpacking_successful
    #         in asn1_e1ap_test.cpp
    "bearer_context_setup_request": bytes.fromhex(
        "00080069000007000200020009000d0013000010a6ae39efbe0d424cd85f4a9c3aee"
        "0414000e0005303b9aca00003a400302f8990017000140000f002a400001002a0023"
        "0040010002123005f5e10001f0ac15048600000036000301300a20ef000000800009"
        "7a004d40020000"
    ),
    # Bearer Context Setup Response (successfulOutcome).
    # Source: when_bearer_context_setup_response_correct_then_unpacking_successful
    #         in asn1_e1ap_test.cpp
    "bearer_context_setup_response": bytes.fromhex(
        "200800370000030002000200090003000340028000104023400001002e401c000001"
        "01f0ac150609000002830006001fac15060980000283000080"
    ),
}

# ---------------------------------------------------------------------------
# Seeds of each fuzz target, with the name of its corpus sub-directory
# ---------------------------------------------------------------------------
CORPORA = {
    "e1ap_pdu_decoder_fuzzer": ("e1ap", E1AP_SEEDS),
}
