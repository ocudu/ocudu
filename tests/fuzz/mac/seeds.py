# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

"""
Seed corpus of the MAC fuzz harnesses, from the known-good
(and known-bad) test vectors in the unit tests.

Each seed is a control byte followed by the MAC UL-SCH PDU. See tests/fuzz/README.md
for the control byte layout.

Written to disk by tests/fuzz/gen_corpus.py, see CORPORA at the end of this file.
"""

# ---------------------------------------------------------------------------
# Control byte
# ---------------------------------------------------------------------------

ATTACHED_UE = 0b0000_0000
MSG3 = 0b0000_0001
MSGA = 0b0000_0010
CREATE_UE_AFTER_MSG3 = 0b0000_0100


def seed(control: int, pdu: str) -> bytes:
    return bytes([control]) + bytes.fromhex(pdu)


# ---------------------------------------------------------------------------
# MAC UL processor corpus seeds
# Taken from tests/unittests/mac/mac_ul_processor_test.cpp and tests/unittests/mac/mac_ul_pdu_test.cpp
# ---------------------------------------------------------------------------
MAC_UL_SEEDS = {
    # MAC CEs from the attached UE.
    "short_bsr": seed(ATTACHED_UE, "3d59"),
    "short_trunc_bsr": seed(ATTACHED_UE, "3bae"),
    "long_bsr": seed(ATTACHED_UE, "3e0381d9ab"),
    "long_bsr_4_lcgs": seed(ATTACHED_UE, "3e055a02040507"),
    "long_trunc_bsr": seed(ATTACHED_UE, "3c0345d9ab"),
    "invalid_long_bsr": seed(ATTACHED_UE, "3e0201ff"),
    "single_entry_phr": seed(ATTACHED_UE, "39272f"),
    "timing_advance_report": seed(ATTACHED_UE, "2c0007"),
    "cg_confirmation": seed(ATTACHED_UE, "37"),

    # MAC SDUs from the attached UE: DRB with an 8-bit L field, SRB1 with a 16-bit L field, then padding.
    "drb_sdu_and_padding": seed(ATTACHED_UE, "0403aabbcc3f00"),
    "srb1_sdu_16bit_length": seed(ATTACHED_UE, "4100031122333f"),

    # A PDU filled with zeros, as a corrupted reception (triggers the discard path).
    "zero_filled": seed(ATTACHED_UE, "000000000000000000"),

    # Msg3 with UL-CCCH 48 bits and 64 bits.
    "msg3_ccch_48": seed(MSG3, "341e4fc004a606"),
    "msg3_ccch_64": seed(MSG3, "001e4fc004a6061354"),

    # Msg3 with UL-CCCH and a TA report, whose processing is deferred until the UE is created.
    "msg3_ccch_and_ta_report": seed(MSG3 | CREATE_UE_AFTER_MSG3, "341e4fc004a6062c0007"),

    # Msg3 with UL-CCCH and a SE-PHR, whose processing is deferred until the UE is created.
    "msg3_ccch_and_phr": seed(MSG3 | CREATE_UE_AFTER_MSG3, "341e4fc004a60639272f"),

    # MsgA PUSCH with UL-CCCH (2-step RACH).
    "msga_ccch_48": seed(MSG3 | MSGA, "341e4fc004a606"),

    # Msg3 with the C-RNTI MAC CE of the attached UE and a Short BSR, continued in the context of that UE.
    "msg3_crnti_ce_and_short_bsr": seed(MSG3, "3a46013d59"),

    # Msg3 with a C-RNTI MAC CE of a UE that does not exist (triggers the discard path).
    "msg3_unknown_crnti_ce": seed(MSG3, "3a46053d59"),
}

# ---------------------------------------------------------------------------
# Seeds of each fuzz target, with the name of its corpus sub-directory
# ---------------------------------------------------------------------------
CORPORA = {
    "mac_ul_processor_fuzzer": ("mac_ul", MAC_UL_SEEDS),
}
