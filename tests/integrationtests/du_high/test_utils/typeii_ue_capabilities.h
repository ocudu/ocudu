// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/precoding/precoding_codebook_configuration.h"

namespace ocudu {
namespace test_helpers {

/// \brief Builds a UE Capability RAT Container List that reports the Type II codebook for the given band.
///
/// The DU configures the Type II codebook for a user equipment whose capabilities report it, as per TS38.331
/// \e codebookParameters. The container carries the number of beams and the number of CSI-RS ports that the cell
/// needs, so that the DU does not fall back to the Type I codebook.
///
/// \param[in] band        Band that reports the capability.
/// \param[in] nof_beams   Number of beams \f$L\f$ that the user equipment supports.
/// \param[in] nof_tx_ports Number of CSI-RS ports per resource that the user equipment supports.
/// \return The packed container, ready for the \e CU to DU RRC Information of an F1AP message.
byte_buffer create_typeii_ue_capability_container(nr_band band, unsigned nof_beams = 4, unsigned nof_tx_ports = 8);

} // namespace test_helpers
} // namespace ocudu
