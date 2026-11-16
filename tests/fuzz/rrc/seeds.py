# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

"""
Seed corpus of the RRC fuzz harnesses, from the known-good
(and known-bad) test vectors in the unit tests.

Each seed is a control byte followed by the RRC PDU. See tests/fuzz/README.md
for the control byte layout.

Written to disk by tests/fuzz/gen_corpus.py, see CORPORA at the end of this file.
"""

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

# Source: authenticate_ue() in tests/unittests/cu_cp/cu_cp_test_environment.cpp, PDCP
# header and MAC-I stripped.
RRC_UL_INFORMATION_TRANSFER = "3a0abf002b96882dac46355c4f34464ddaf7b43fde37ae80"

# Source: send_rrc_measurement_report() in tests/unittests/cu_cp/cu_cp_nrppa_test.cpp,
# PDCP header and MAC-I stripped.
RRC_MEASUREMENT_REPORT = "00410004015f741fe0804bf183fc"

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

    # ULInformationTransfer on a connected UE.
    "dcch_ul_information_transfer": seed(DCCH | CONNECTED, RRC_UL_INFORMATION_TRANSFER),

    # MeasurementReport on SRB1 after security activation.
    "dcch_measurement_report": seed(DCCH | INTEGRITY_VERIFIED | SECURED, RRC_MEASUREMENT_REPORT),
}

# ---------------------------------------------------------------------------
# rrc_cu_cp corpus seeds
# ---------------------------------------------------------------------------
# The full-stack harness takes a control byte selecting the UE state, then a
# sequence of steps: a step type byte, a length byte and that many bytes of
# payload. See tests/fuzz/README.md for the input format.
# ---------------------------------------------------------------------------

CU_CP_AWAITING_SETUP_COMPLETE = 0
CU_CP_CONNECTED = 1
CU_CP_SECURED = 2
CU_CP_AWAITING_RECONFIGURATION_COMPLETE = 3

# Step types, in the order of step_type in rrc_cu_cp_fuzzer.cpp.
UL_CCCH = 0
UL_DCCH_SRB1 = 1
UL_DCCH_SRB2 = 2
ADVANCE_TIME = 3
INITIAL_CONTEXT_SETUP = 4
PDU_SESSION_SETUP = 5
PDU_SESSION_MODIFICATION = 6
PDU_SESSION_RELEASE = 7
DL_NAS_TRANSPORT = 8
UE_CONTEXT_MODIFICATION = 9
AMF_UE_CONTEXT_RELEASE = 10
DU_UE_CONTEXT_RELEASE = 11


def steps(state: int, *items: tuple) -> bytes:
    """Control byte followed by (step type, payload hex) steps."""
    out = bytes([state])
    for step_type, pdu in items:
        payload = bytes.fromhex(pdu)
        assert len(payload) < 256, "a step payload must fit a single length byte"
        out += bytes([step_type, len(payload)]) + payload
    return out


# The CU-CP numbers the RRC transactions of a UE in the order it opens them, so
# every sequence below answers with the transaction ID the CU-CP expects:
# RRCSetup 0, SecurityModeCommand 1, UECapabilityEnquiry 2, then the
# RRCReconfiguration of the first PDU session 3, and the next one 0 again.
# The PDUs come from tests/test_doubles/rrc/rrc_test_messages.cpp, packed with
# pack_ul_dcch_msg(): create_security_mode_complete(1),
# create_ue_capability_info(2) and create_rrc_reconfiguration_complete(3) and (0).
SECURITY_MODE_COMPLETE = "2a00"
SECURITY_MODE_FAILURE = "3200"
UE_CAPABILITY_INFORMATION = (
    "4c82202378680ce811d1968097e360e1a80000b0498b8000c1f86800637f"
    "e002e0013141360000000a800163401801e81c790e7c000090200789a001"
    "c4d00000000713410400800809a5600000068081489004803804200c07c0"
    "1000000000000010000000091c004100000965940401000009659400148e"
    "20d800012003e0f000108080100202004008004010020a40a000"
)
RRC_RECONFIGURATION_COMPLETE_3 = "0e00"
RRC_RECONFIGURATION_COMPLETE_0 = "0800"

RRC_CU_CP_SEEDS = {
    # RRCSetupRequest on UL-CCCH: the pre-authentication entry point.
    "ccch_setup_request": steps(CU_CP_AWAITING_SETUP_COMPLETE, (UL_CCCH, "1dec89d05766")),

    # Corrupted UL-CCCH message (triggers the unpack failure path).
    "ccch_invalid": steps(CU_CP_AWAITING_SETUP_COMPLETE, (UL_CCCH, "9dec89de5766")),

    # RRCResumeRequest on UL-CCCH, which the CU-CP routes by its resume identity.
    "ccch_resume_request": steps(CU_CP_AWAITING_SETUP_COMPLETE, (UL_CCCH, "20202066f020")),

    # RRCSetupComplete on SRB1 against a UE awaiting it.
    "dcch_setup_complete": steps(CU_CP_AWAITING_SETUP_COMPLETE, (UL_DCCH_SRB1, RRC_SETUP_COMPLETE)),

    # UL-DCCH against a connected UE, before security activation.
    "dcch_ul_info_transfer": steps(CU_CP_CONNECTED, (UL_DCCH_SRB1, "0000")),

    # RRCReconfigurationComplete on SRB1 after security activation, with no reconfiguration open.
    "dcch_reconfig_complete": steps(CU_CP_SECURED, (UL_DCCH_SRB1, "0a00")),

    # MeasurementReport on SRB1 after security activation.
    "dcch_meas_report": steps(CU_CP_SECURED, (UL_DCCH_SRB1, "0800")),

    # UL-DCCH on SRB2, which only exists after security activation.
    "dcch_srb2": steps(CU_CP_SECURED, (UL_DCCH_SRB2, "0000")),

    # Truncated payload (triggers the early-exit path).
    "dcch_truncated": steps(CU_CP_SECURED, (UL_DCCH_SRB1, "2a")),

    # Corrupt / non-RRC payload (triggers the unpack failure path).
    "dcch_invalid": steps(CU_CP_SECURED, (UL_DCCH_SRB1, "deadbeef")),

    # ULInformationTransfer with a complete message body, against a connected UE.
    "dcch_ul_information_transfer": steps(CU_CP_CONNECTED, (UL_DCCH_SRB1, RRC_UL_INFORMATION_TRANSFER)),

    # MeasurementReport with a complete message body, on SRB1 after security activation.
    "dcch_measurement_report": steps(CU_CP_SECURED, (UL_DCCH_SRB1, RRC_MEASUREMENT_REPORT)),

    # RRCReconfigurationComplete answering the RRCReconfiguration that adds the DRB of a PDU session,
    # which completes the PDU Session Resource Setup.
    "dcch_reconfig_complete_pdu_session": steps(
        CU_CP_AWAITING_RECONFIGURATION_COMPLETE, (UL_DCCH_SRB1, RRC_RECONFIGURATION_COMPLETE_3)
    ),

    # The security activation and UE capability transfer of the Initial Context Setup, answered by the
    # input rather than by the bring-up, so that both messages are mutated in the procedures that
    # wait for them.
    "initial_context_setup": steps(
        CU_CP_CONNECTED,
        (INITIAL_CONTEXT_SETUP, ""),
        (UL_DCCH_SRB1, SECURITY_MODE_COMPLETE),
        (UL_DCCH_SRB1, UE_CAPABILITY_INFORMATION),
    ),

    # SecurityModeFailure answering the SecurityModeCommand. The harness keys SRB1 as the
    # SecurityModeCommand goes out, so this arrives integrity protected, which the CU-CP rejects: a UE
    # sends it unprotected (TS 38.331 section 5.3.4.3).
    "initial_context_setup_security_mode_failure": steps(
        CU_CP_CONNECTED, (INITIAL_CONTEXT_SETUP, ""), (UL_DCCH_SRB1, SECURITY_MODE_FAILURE)
    ),

    # A PDU session set up, modified and released, each RRCReconfiguration answered in turn.
    "pdu_session_lifecycle": steps(
        CU_CP_AWAITING_RECONFIGURATION_COMPLETE,
        (UL_DCCH_SRB1, RRC_RECONFIGURATION_COMPLETE_3),
        (PDU_SESSION_MODIFICATION, ""),
        (UL_DCCH_SRB1, RRC_RECONFIGURATION_COMPLETE_0),
        (PDU_SESSION_RELEASE, ""),
        (UL_DCCH_SRB1, "0a00"),
    ),

    # The RRCReconfiguration of a PDU session left unanswered until its guard timer expires.
    "pdu_session_setup_timeout": steps(CU_CP_AWAITING_RECONFIGURATION_COMPLETE, (ADVANCE_TIME, "ff")),

    # NAS delivery, a UE context modification and the AMF releasing the UE.
    "amf_procedures": steps(
        CU_CP_AWAITING_RECONFIGURATION_COMPLETE,
        (UL_DCCH_SRB1, RRC_RECONFIGURATION_COMPLETE_3),
        (DL_NAS_TRANSPORT, ""),
        (UE_CONTEXT_MODIFICATION, ""),
        (AMF_UE_CONTEXT_RELEASE, ""),
    ),

    # A radio link failure reported by the DU while a PDU session setup waits for the UE.
    "du_release_during_pdu_session_setup": steps(
        CU_CP_AWAITING_RECONFIGURATION_COMPLETE, (DU_UE_CONTEXT_RELEASE, ""), (UL_DCCH_SRB1, RRC_RECONFIGURATION_COMPLETE_3)
    ),
}

# ---------------------------------------------------------------------------
# Seeds of each fuzz target, with the name of its corpus sub-directory
# ---------------------------------------------------------------------------
CORPORA = {
    "rrc_ue_fuzzer":    ("rrc_ue", RRC_SEEDS),
    "rrc_cu_cp_fuzzer": ("rrc_cu_cp", RRC_CU_CP_SEEDS),
}
