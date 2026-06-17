// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/antenna_topology.h"

namespace ocudu {

/// Beam identifier type.
enum class beam_identifier : uint16_t { n0 = 0 };

/// Convert an unsigned 16-bit integer to a beam identifier type.
inline beam_identifier to_beam_id(uint16_t beam_id)
{
  return static_cast<beam_identifier>(beam_id);
}

/// Convert a beam identifier to a 16-bit unsigned integer.
inline uint16_t to_uint(beam_identifier beam_id)
{
  return static_cast<uint16_t>(beam_id);
}

} // namespace ocudu
