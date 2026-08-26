// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/beamforming/beam_identifier.h"
#include "ocudu/ran/precoding/precoding_weight_matrix.h"

namespace ocudu {

/// \brief Composite precoding configuration.
///
/// Pairs a set of precoding weights with their associated beam selection.
///
/// The MIMO precoding matrix determines how the transmission layers are mapped onto the selected beams, and it is
/// applied in the upper physical layer in the layer mapping and precoding phase.
///
/// The selected beam list describes the beam that carries each virtual port signal (the MIMO matrix ports). The beam
/// weights are applied in the lower physical layer.
///
/// The pair is invalid if the number of ports in the MIMO precoding matrix is different from the number of beams.
struct precoding_beamforming_composite {
  /// MIMO precoding matrix, with one port per selected beam.
  precoding_weight_matrix mimo;
  /// Selected beam list, with one beam per MIMO precoding matrix port.
  precoding_beam_list beams;
};

} // namespace ocudu
