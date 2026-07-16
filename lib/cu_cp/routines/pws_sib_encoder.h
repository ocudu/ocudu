// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/expected.h"
#include <cstdint>
#include <vector>

namespace ocudu::ocucp {

/// Encode a SIB6 (ETWS primary notification) into a packed ASN.1 buffer (TS 38.331 section 6.3.1).
/// Inputs correspond directly to the NGAP WRITE-REPLACE WARNING REQUEST IEs.
expected<byte_buffer> encode_sib6(uint16_t msg_id, uint16_t serial_num, uint16_t warning_type);

/// Encode a SIB7 (ETWS secondary notification) into one or more packed ASN.1 buffers (TS 38.331 section 6.3.1).
/// warning_msg_contents is split into segments of at most max_segment_size bytes; each segment is encoded as a
/// separate SIB7 with the correct warningMessageSegmentNumber and warningMessageSegmentType.
/// dataCodingScheme is present only in the first segment (TS 38.331 section 6.3.1, condition Segment1).
/// The caller places segment 0 in SIB message IE and segments 1..N-1 in Additional SIB Message List IE
/// (TS 38.473 section 9.3.1.58).
expected<std::vector<byte_buffer>> encode_sib7(uint16_t           msg_id,
                                               uint16_t           serial_num,
                                               uint8_t            data_coding_scheme,
                                               const byte_buffer& warning_msg_contents,
                                               uint32_t           max_segment_size);

/// Encode a SIB8 (CMAS notification) into one or more packed ASN.1 buffers (TS 38.331 section 6.3.1).
/// Segmentation and dataCodingScheme placement follow the same rules as encode_sib7.
expected<std::vector<byte_buffer>> encode_sib8(uint16_t           msg_id,
                                               uint16_t           serial_num,
                                               uint8_t            data_coding_scheme,
                                               const byte_buffer& warning_msg_contents,
                                               uint32_t           max_segment_size);

} // namespace ocudu::ocucp
