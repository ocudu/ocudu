// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/beamforming/beam_identifier.h"
#include "ocudu/ran/precoding/precoding_matrix_indicator.h"
#include <variant>

namespace ocudu {

/// \brief Precoding and beamforming information of a downlink transmission.
///
/// Holds either:
/// - a \ref precoding_matrix_indicator, that maps the transmission layers onto the precoding matrix ports, applied
///   over the default beams (\c std::monostate selects no precoding, i.e. an identity matrix); or
/// - a \ref beam_identifier, for a transmission that is not precoded and is entirely carried by that beam.
using precoding_and_beamforming_info = std::variant<precoding_matrix_indicator, beam_identifier>;

/// \brief Builds the precoding and beamforming of a transmission that is mapped onto a single beam.
///
/// The transmission is not precoded, and the complete allocation is carried by the given beam.
inline precoding_and_beamforming_info make_single_beam_precoding(beam_identifier beam_id)
{
  return precoding_and_beamforming_info{beam_id};
}

/// \brief Builds the precoding and beamforming of a transmission that uses the default beams.
///
/// The transmission is neither precoded nor beamformed: each layer is mapped onto the antenna port with the same index.
inline precoding_and_beamforming_info make_default_precoding()
{
  return precoding_and_beamforming_info{precoding_matrix_indicator{}};
}

} // namespace ocudu
