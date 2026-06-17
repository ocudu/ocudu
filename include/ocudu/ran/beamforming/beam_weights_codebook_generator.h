// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/antenna_topology.h"
#include "ocudu/ran/beamforming/beam_weights_codebook.h"

namespace ocudu {

/// Generates the beamforming weights codebook for a certain antenna topology.
beam_weights_codebook generate_beam_weights_codebook(antenna_topology antenna_topology);

} // namespace ocudu
