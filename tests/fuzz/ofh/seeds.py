# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

"""
Seed corpus of the OFH fuzz harnesses, from the known-good
(and known-bad) test vectors in the unit tests.

Written to disk by tests/fuzz/gen_corpus.py, see CORPORA at the end of this file.
"""

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
# OFH receiver corpus seeds
#
# Each seed is a configuration header followed by operations, see the input format of the OFH
# receiver fuzzer in tests/fuzz/README.md.
# ---------------------------------------------------------------------------
OP_UPLINK_SLOT = 0
OP_PRACH_OCCASION = 1
OP_UPLANE_FRAME = 2
OP_RAW_FRAME = 3
OP_ADVANCE = 4

# Flags of a U-Plane frame operation.
SLOT_OF_LAST_UPLINK = 0x01
SLOT_OF_LAST_PRACH = 0x02
HARNESS_SEQ_ID = 0x04
ECPRI_SIZE_FROM_INPUT = 0x08

# PRACH formats, as indexes into the format table of the harness.
FORMAT_0 = 0
FORMAT_A1 = 4

# One slot of symbols. Contexts stored by a request become visible to the receiver at the next OTA symbol, and the
# RU sends the data of a slot once that slot has started over the air.
SLOT_SYMBOLS = 14

# Filter index values, O-RAN.WG4.CUS, clause 7.5.2.4.
FILTER_STANDARD = 0
FILTER_PRACH_1P25KHZ = 1
FILTER_PRACH_SHORT = 3


def rx_config(flags=0x00, ul_compr=0x0E, prach_compr=0x0E, misc=0x04):
    """Configuration header. The defaults are one antenna, 10 MHz at 15 kHz and 16-bit IQ without compression."""
    return bytes([flags, ul_compr, prach_compr, misc])


def uplane_message(filter_index, symbol, start_prb, nof_prbs, prb_bytes, comp_hdr=None):
    """U-Plane message with one section, O-RAN.WG4.CUS, clause 7.5.2. The harness writes the slot."""
    header = bytes([0x10 | filter_index, 0x00, 0x00, symbol & 0x3F])
    section = bytes([0x00, 0x10 | ((start_prb >> 8) & 0x03), start_prb & 0xFF, nof_prbs])
    if comp_hdr is not None:
        section += bytes([comp_hdr, 0x00])
    return header + section + bytes(nof_prbs * prb_bytes)


def op_uplink_slot(delta):
    return bytes([OP_UPLINK_SLOT, delta])


def op_prach(delta, fmt, start=0, rb_offset=0, nof_td=0):
    return bytes([OP_PRACH_OCCASION, delta, fmt, start, rb_offset, nof_td])


def op_uplane(eaxc_selector, flags, message, seq_id=None, ecpri_size=None):
    if ecpri_size is not None:
        flags |= ECPRI_SIZE_FROM_INPUT
    op = bytes([OP_UPLANE_FRAME, eaxc_selector, flags])
    if not flags & HARNESS_SEQ_ID:
        op += bytes([seq_id or 0])
    if ecpri_size is not None:
        op += ecpri_size.to_bytes(2, "big")
    return op + len(message).to_bytes(2, "big") + message


def op_raw(frame):
    return bytes([OP_RAW_FRAME]) + len(frame).to_bytes(2, "big") + frame


def op_advance(nof_symbols):
    return bytes([OP_ADVANCE, nof_symbols - 1])


# Number of bytes of a PRB of 16-bit IQ without compression, and of BFP 9-bit with its exponent.
PRB_NONE_16 = 48
PRB_BFP_9 = 28

# With one antenna, eAxC selector 0 is the uplink data eAxC and 1 the PRACH eAxC.
UL_EAXC = 0
PRACH_EAXC = 1

RX_SEEDS = {
    # Uplink slot followed by two symbols of uplink data.
    "pusch_none16": rx_config()
    + op_uplink_slot(1)
    + op_advance(SLOT_SYMBOLS)
    + op_uplane(UL_EAXC, SLOT_OF_LAST_UPLINK | HARNESS_SEQ_ID, uplane_message(FILTER_STANDARD, 0, 0, 4, PRB_NONE_16))
    + op_uplane(UL_EAXC, SLOT_OF_LAST_UPLINK | HARNESS_SEQ_ID, uplane_message(FILTER_STANDARD, 1, 4, 4, PRB_NONE_16)),
    # Uplink data with the dynamic compression header, BFP 9-bit.
    "pusch_bfp9_dynamic_header": rx_config(flags=0x20, ul_compr=0x81)
    + op_uplink_slot(1)
    + op_advance(SLOT_SYMBOLS)
    + op_uplane(
        UL_EAXC,
        SLOT_OF_LAST_UPLINK | HARNESS_SEQ_ID,
        uplane_message(FILTER_STANDARD, 0, 0, 4, PRB_BFP_9, comp_hdr=0x91),
    ),
    # Uplink data received before any uplink request.
    "pusch_without_request": rx_config()
    + op_uplane(UL_EAXC, HARNESS_SEQ_ID, uplane_message(FILTER_STANDARD, 0, 0, 4, PRB_NONE_16)),
    # Uplink request for the current OTA slot, dropped as late.
    "late_uplink_request": rx_config() + op_advance(10) + op_uplink_slot(0),
    # Long preamble PRACH without C-Plane, all 71 PRBs of the occasion in symbol 0.
    "prach_long_format0": rx_config()
    + op_prach(1, FORMAT_0)
    + op_advance(SLOT_SYMBOLS)
    + op_uplane(
        PRACH_EAXC, SLOT_OF_LAST_PRACH | HARNESS_SEQ_ID, uplane_message(FILTER_PRACH_1P25KHZ, 0, 0, 71, PRB_NONE_16)
    ),
    # Long preamble PRACH data for a symbol beyond the occasion.
    "prach_long_symbol_beyond_occasion": rx_config()
    + op_prach(1, FORMAT_0)
    + op_advance(SLOT_SYMBOLS)
    + op_uplane(
        PRACH_EAXC, SLOT_OF_LAST_PRACH | HARNESS_SEQ_ID, uplane_message(FILTER_PRACH_1P25KHZ, 5, 0, 71, PRB_NONE_16)
    ),
    # Long preamble PRACH with C-Plane and the start symbol of the U-Plane ignored.
    "prach_long_cplane_ignore_start_symbol": rx_config(flags=0x03)
    + op_prach(1, FORMAT_0)
    + op_advance(SLOT_SYMBOLS)
    + op_uplane(
        PRACH_EAXC, SLOT_OF_LAST_PRACH | HARNESS_SEQ_ID, uplane_message(FILTER_PRACH_1P25KHZ, 0, 0, 71, PRB_NONE_16)
    ),
    # Short preamble PRACH with C-Plane, 12 PRBs in the first symbol of the occasion.
    "prach_short_a1_cplane": rx_config(flags=0x01)
    + op_prach(1, FORMAT_A1, start=2)
    + op_advance(SLOT_SYMBOLS)
    + op_uplane(
        PRACH_EAXC, SLOT_OF_LAST_PRACH | HARNESS_SEQ_ID, uplane_message(FILTER_PRACH_SHORT, 2, 0, 12, PRB_NONE_16)
    ),
    # Sequence identifier jump between two messages of the same eAxC.
    "seq_id_jump": rx_config(flags=0x10)
    + op_uplink_slot(1)
    + op_advance(SLOT_SYMBOLS)
    + op_uplane(UL_EAXC, SLOT_OF_LAST_UPLINK, uplane_message(FILTER_STANDARD, 0, 0, 1, PRB_NONE_16), seq_id=0)
    + op_uplane(UL_EAXC, SLOT_OF_LAST_UPLINK, uplane_message(FILTER_STANDARD, 1, 0, 1, PRB_NONE_16), seq_id=5),
    # Message for an eAxC that is not configured.
    "unconfigured_eaxc": rx_config()
    + op_uplink_slot(1)
    + op_advance(SLOT_SYMBOLS)
    + op_uplane(0x80 | 20, SLOT_OF_LAST_UPLINK | HARNESS_SEQ_ID, uplane_message(FILTER_STANDARD, 0, 0, 1, PRB_NONE_16)),
    # eCPRI payload size larger than the frame.
    "ecpri_payload_size_too_large": rx_config()
    + op_uplane(
        UL_EAXC, HARNESS_SEQ_ID, uplane_message(FILTER_STANDARD, 0, 0, 1, PRB_NONE_16), ecpri_size=0xFFFF
    ),
    # Frame from another source MAC address.
    "raw_frame_other_source": rx_config()
    + op_raw(bytes.fromhex("020000000001020000000099aefe1000000c00000000") + bytes(42)),
    # Four antennas, 40 MHz at 30 kHz, with OTA symbols advanced across the reception window.
    "four_antennas_30khz": rx_config(flags=0x40, misc=0x0F)
    + op_uplink_slot(2)
    + op_advance(28)
    + op_uplane(3, SLOT_OF_LAST_UPLINK | HARNESS_SEQ_ID, uplane_message(FILTER_STANDARD, 0, 0, 2, PRB_NONE_16))
    + op_advance(40),
}

# ---------------------------------------------------------------------------
# Seeds of each fuzz target, with the name of its corpus sub-directory
# ---------------------------------------------------------------------------
CORPORA = {
    "ofh_uplane_decoder_fuzzer":     ("ofh_uplane", UPLANE_SEEDS),
    "ofh_ecpri_decoder_fuzzer":      ("ofh_ecpri", ECPRI_SEEDS),
    "ofh_vlan_frame_decoder_fuzzer": ("ofh_vlan", VLAN_SEEDS),
    "ofh_receiver_fuzzer":           ("ofh_rx", RX_SEEDS),
}
