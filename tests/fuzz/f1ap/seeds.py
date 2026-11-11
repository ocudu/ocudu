# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

"""
Seed corpus of the F1AP fuzz harness, from the known-good test vectors in the unit tests.

Each seed is a raw F1AP PDU, with no harness prefix. See tests/fuzz/README.md.

Written to disk by tests/fuzz/gen_corpus.py, see CORPORA at the end of this file.
"""

# ---------------------------------------------------------------------------
# F1AP corpus seeds
# ---------------------------------------------------------------------------
# Taken from the unit-test fixtures cited per seed
# ---------------------------------------------------------------------------
F1AP_SEEDS = {
    # Initial UL RRC Message Transfer (initiatingMessage).
    # Source: rx_msg of when_initial_ul_rrc_message_transfer_correct_then_unpacking_successful
    #         in tests/unittests/asn1/asn1_f1ap_test.cpp
    "initial_ul_rrc_message_transfer": bytes.fromhex(
        "000b4080a40000060029000340a127006f00090002f899000bc614e0005f000300a1"
        "2700320007061dec89d0576600800071705c00b001117aec701061e0007c20408d0"
        "7810020a2090480ca8000f800000000008370842000088165000048200002069a06"
        "aa4988000200204000400d008013b64b1814400e468acf120000096070820f177e06"
        "0870000000e25038000040bde802000400000000028201950300c400004e40020000"
    ),
    # UE Context Setup Request as precanned by the DU procedure test.
    # Source: msg of f1ap_handles_precanned_ue_context_setup_request_correctly
    #         in tests/unittests/f1ap/du/f1ap_du_ue_context_setup_procedure_test.cpp
    "precanned_ue_context_setup_request": bytes.fromhex(
        "0005004400000800280002001600294003400000003f00090000f11000066c001000"
        "6b0001000009000100004a00060000490001080032400a09000320080895755b0c00"
        "b8400100"
    ),
    # UE Context Setup Request (initiatingMessage).
    # Source: rx_msg of when_ue_context_setup_request_correct_then_unpacking_successful
    #         in tests/unittests/asn1/asn1_f1ap_test.cpp
    "ue_context_setup_request": bytes.fromhex(
        "0005008273000006002800020034003f00090000f1100753040470006b0001040009"
        "00822060811c188080ff2301057a3560a613000060401c4d00000007f00000000309"
        "800030200e2680000103f80000000184c0001810071340000101fc00000000c16040"
        "1c4d00000607f00000000305800071340000181fc000000000100200101345a2d140"
        "3100004020001008080100220001004000201010020044000100800080202004008"
        "4000201010020041e3fc01c0011e4b2ef97ffd7c90a10000030102000e7c000dea0"
        "4d00e7adfe1cf5ffefe400007d48086289294c8dfff78dfff5fffbfefe0000001805"
        "c38080c07e0601803c0406039dff010180e4ffcd9ffb0c04080039fe485080000180"
        "8100073ef002ece0000058f0000395340006000000000000f2a820000927630027fa"
        "901f030000000000040000002000365907e10c80ff2301057a3560a613000060401c"
        "4d00000007f00000000309800030200e2680000103f80000000184c0001810071340"
        "000101fc00000000c160401c4d00000607f00000000305800071340000181fc00000"
        "0000100200101345a2d1403100004020001008080100220001004000201010020044"
        "0001008000802020040084000201010020041e3fc01c0011e4b2ef97ffd7c90a1000"
        "0030102000e7c000dea04d00e7adfe1cf5ffefe400007d48086289294c8dfff78dff"
        "f5fffbfefe0000001805c38080c07e0601803c0406039dff010180e4ffcd9ffb0c04"
        "080039fe4850800001808100073ef002ece0000058f0000395340006000000000000"
        "f2a820000023002700002200225140080900fe000000070000000000000000000000"
        "002066001702000000a1400140009e400420203a00"
    ),
}

# ---------------------------------------------------------------------------
# Seeds of each fuzz target, with the name of its corpus sub-directory
# ---------------------------------------------------------------------------
CORPORA = {
    "f1ap_pdu_decoder_fuzzer": ("f1ap", F1AP_SEEDS),
}
