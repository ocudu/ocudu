// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/antenna_topology.h"
#include "ocudu/ran/precoding/precoding_constants.h"
#include <type_traits>

namespace ocudu {

/// Maximum number of beams that an antenna topology defines.
static constexpr unsigned max_nof_beams = get_max_nof_beams();

/// \brief Beam identifier type.
enum class beam_identifier : unsigned {
  n0 = 0,

  /// Reserved beam identifier that flags an unset beam.
  invalid = max_nof_beams
};

/// \brief List of beams that a transmission is mapped onto.
///
/// The list contains one beam per port of the precoding matrix that maps the transmission layers onto the beams:
/// - a precoded transmission maps the layers onto the beams described by a Precoding Matrix Indicator (PMI), at most
///   \ref max_nof_beams_per_pmi of them; and
/// - a transmission without precoding, i.e., with an identity precoding matrix, maps each layer onto the beam that
///   selects the antenna port with the same index, at most \ref precoding_constants::MAX_NOF_PORTS.
using precoding_beam_list = static_vector<beam_identifier, precoding_constants::MAX_NOF_PORTS>;

/// Convert an unsigned integer to a beam identifier type.
inline beam_identifier to_beam_id(unsigned beam_id)
{
  return static_cast<beam_identifier>(beam_id);
}

/// Convert a beam identifier to its underlying value.
template <typename Integer = unsigned>
Integer to_uint(beam_identifier beam_id)
{
  static_assert(std::is_same_v<std::underlying_type_t<beam_identifier>, Integer>,
                "Integer type must match the underlying type of the beam identifier.");
  return static_cast<Integer>(beam_id);
}

/// Determines whether a beam identifier is within the range of beams that an antenna topology can define.
inline bool is_beam_id_valid(beam_identifier beam_id)
{
  return to_uint(beam_id) < max_nof_beams;
}

} // namespace ocudu
