# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

"""
Seed corpus of the NGAP fuzz harnesses, from the known-good
(and known-bad) test vectors in the unit tests.

Written to disk by tests/fuzz/gen_corpus.py, see CORPORA at the end of this file.
"""

# ---------------------------------------------------------------------------
# NGAP corpus seeds
# ---------------------------------------------------------------------------
# Taken from tests/unittests/ngap/ngap_test_messages.h,
# tests/unittests/ngap/ngap_asn1_packer_test.cpp and
# tests/unittests/asn1/asn1_ngap_test.cpp
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

    # DownlinkNASTransport (initiatingMessage / id-DownlinkNASTransport).
    # Source: ngap_msg[] in dl_nas_transport_pack_and_unpack in asn1_ngap_test.cpp
    "downlink_nas_transport": bytes.fromhex(
        "0004403e000003000a000200010055000200010026002b2a7e00560002000021681c"
        "d489650fdcc7c70eca8fa9be44702010c7f0791fa852e282847c9f4ce5c19451"
    ),

    # InitialUEMessage (initiatingMessage / id-InitialUEMessage).
    # Source: ngap_msg[] in init_ue_msg_pack_and_unpack in asn1_ngap_test.cpp
    "initial_ue_message": bytes.fromhex(
        "000f4080a20000040055000200010026007d7c7e00417100760100f110000001014d"
        "436f77425159444b325675417945416e363648396b7a485461465a4b303537414972"
        "37412b6e6c736149587852334e6973364c566f75466942343ddfabf5cd652eb25414"
        "91484d41432d53484100858bbb1f42f1256f9a37531a772a2cf2b78ff160488402e"
        "d489399b6b737420079000f4000f110000000001000f110000075005a400118"
    ),

    # UEContextReleaseCommand (initiatingMessage / id-UEContextRelease).
    # Source: ngap_msg[] in ue_context_release_pack_and_unpack in asn1_ngap_test.cpp
    "ue_context_release_command": bytes.fromhex("002900100000020072000400010001000f400148"),

    # PDUSessionResourceSetupRequest (initiatingMessage / id-PDUSessionResourceSetup).
    # Source: ngap_msg[] in session_res_setup_request_pack_and_unpack in asn1_ngap_test.cpp
    # Has no PDU Session Aggregate Maximum Bit Rate for its Non-GBR QoS flow, so the
    # CU-CP rejects the PDU session (triggers the validation failure path).
    "pdu_session_resource_setup_request_without_ambr": bytes.fromhex(
        "001d006c000004000a000200010055000200010026002e2d7e00680100252e0100c2"
        "110006010003300101060603e80603e8290501c0a80c7b25080764656661756c7412"
        "01004a0027000001000021000003008b000a01f0c0a811d200000001008600011000"
        "88000700010000090000"
    ),

    # UplinkNASTransport (initiatingMessage / id-UplinkNASTransport).
    # Source: ngap_msg[] in ul_ran_status_transfer_pack_and_unpack in asn1_ngap_test.cpp
    "uplink_nas_transport": bytes.fromhex(
        "002e403c000004000a0002000100550002000100260016157e00572d105e86219e7d"
        "da9995e3850384cfbea53b0079400f4000f110000000001000f110000075"
    ),
}

# ---------------------------------------------------------------------------
# ngap_cu_cp corpus seeds
# ---------------------------------------------------------------------------
# The full-stack harness prefixes a control byte selecting the UE state it
# brings up before injecting the message, so it needs its own corpus.
# ---------------------------------------------------------------------------

CU_CP_AWAITING_SETUP_COMPLETE = 0
CU_CP_CONNECTED = 1
CU_CP_SECURED = 2

# The Initial Context Setup Request drives the UE from connected to secured, so
# it is seeded against a connected UE; everything else against a secured one.
NGAP_CU_CP_STATES = {
    "initial_context_setup_request": CU_CP_CONNECTED,
}

def chain(state: int, *pdus: bytes) -> bytes:
    """Control byte followed by length-prefixed messages."""
    out = bytes([state])
    for pdu in pdus:
        assert len(pdu) < 256, "a seed message must fit a single length byte"
        out += bytes([len(pdu)]) + pdu
    return out


NGAP_CU_CP_SEEDS = {
    name: chain(NGAP_CU_CP_STATES.get(name, CU_CP_SECURED), data)
    for name, data in NGAP_SEEDS.items()
}

# A chain, to seed the mutator with the shape the format exists for: the same
# procedure run twice against one UE, which the single-message seeds cannot
# express.
NGAP_CU_CP_SEEDS["initial_context_setup_request_twice"] = chain(
    CU_CP_CONNECTED,
    NGAP_SEEDS["initial_context_setup_request"],
    NGAP_SEEDS["initial_context_setup_request"],
)

# NAS delivery followed by the release answering it: a minimal ordering between
# procedures, which the single-message seeds cannot express.
NGAP_CU_CP_SEEDS["downlink_nas_transport_then_release"] = chain(
    CU_CP_SECURED,
    NGAP_SEEDS["downlink_nas_transport"],
    NGAP_SEEDS["ue_context_release_command"],
)

# ---------------------------------------------------------------------------
# Seeds of each fuzz target, with the name of its corpus sub-directory
# ---------------------------------------------------------------------------
CORPORA = {
    "ngap_pdu_decoder_fuzzer": ("ngap", NGAP_SEEDS),
    "ngap_cu_cp_fuzzer":       ("ngap_cu_cp", NGAP_CU_CP_SEEDS),
}
