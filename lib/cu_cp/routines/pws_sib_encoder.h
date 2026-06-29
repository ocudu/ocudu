// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/expected.h"
#include <cstdint>

namespace ocudu::ocucp {

/// Encode a SIB6 (ETWS primary notification) into a packed ASN.1 buffer (TS 38.331 section 6.3.1).
/// Inputs correspond directly to the NGAP WRITE-REPLACE WARNING REQUEST IEs.
expected<byte_buffer> encode_sib6(uint16_t msg_id, uint16_t serial_num, uint16_t warning_type);

/// Encode a SIB7 (ETWS secondary notification) into a packed ASN.1 buffer (TS 38.331 section 6.3.1).
/// The full warning_msg_contents is carried in a single last segment.
expected<byte_buffer>
encode_sib7(uint16_t msg_id, uint16_t serial_num, uint8_t data_coding_scheme, const byte_buffer& warning_msg_contents);

/// Encode a SIB8 (CMAS notification) into a packed ASN.1 buffer (TS 38.331 section 6.3.1).
/// The full warning_msg_contents is carried in a single last segment.
expected<byte_buffer>
encode_sib8(uint16_t msg_id, uint16_t serial_num, uint8_t data_coding_scheme, const byte_buffer& warning_msg_contents);

} // namespace ocudu::ocucp
