// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu::ocucp {

/// <AMF Identifier> = <AMF Region ID><AMF Set ID><AMF Pointer>
/// with AMF Region ID length is 8 bits, AMF Set ID length is 10 bits and AMF Pointer length is 6 bits
struct amf_identifier_t {
  uint8_t  amf_region_id = 0;
  uint16_t amf_set_id    = 0;
  uint8_t  amf_pointer   = 0;
};

} // namespace ocudu::ocucp
