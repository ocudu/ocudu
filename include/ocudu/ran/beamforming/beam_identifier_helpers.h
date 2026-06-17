// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/beamforming/beam_identifier.h"

namespace ocudu {

/// \brief Convert a single port index to a beam identifier.
///
/// \param[in] antenna_topology Selected antenna topology.
/// \param[in] i_port           Selected antenna port index.
/// \return A beam identifier associated with the antenna topology.
/// \remark An assertion is triggered if the port index exceeds the maximum number of antennas given in the topology.
beam_identifier single_port_to_beam_id(antenna_topology antenna_topology, uint8_t i_port);

/// \brief Convert a combination of antenna topology and a selected beam defined in the 3GPP.
///
/// The 3GPP defines beams in the document 38.214, Section 5.2.2.2, as in function of \f$\nu_{l,m}\f$ for a combination
/// of \f$(N_1, N_2)\f$ and \f$(O_1, O_2)\f$.
///
/// In other words, each beam identifier corresponds to a set of coefficients that are mapped onto a group of transmit
/// antennas. Transmit antennas are grouped in antenna panels and polarizations.
///
/// See \ref antenna_topology for more information about the different antenna topologies and their parameters.
///
/// The first beam identifiers are reserved for antenna port selection which are necessary for transmitting signals
/// without beamforming (i.e., NZP-CSI-RS for PMI codebook selection).
///
/// The next beams are organized with the following hierarchy: panel, polarization, first beam dimension and second
/// beam dimension.
///
/// \param[in] antenna_topology Selected antenna topology.
/// \param[in] i_panel          Panel index.
/// \param[in] i_pol            Beam polarization index.
/// \param[in] i_beam_dim1      First dimension beam index, parameter \f$l\f$.
/// \param[in] i_beam_dim2      Second dimension beam index, parameter \f$m\f$.
/// \return A beam identifier associated with the antenna topology.
/// \remark An assertion is triggered if any of the indices exceed their maximums given the antenna topology.
beam_identifier pmi_codebook_beam_to_beam_id(antenna_topology antenna_topology,
                                             uint8_t          i_panel,
                                             uint8_t          i_pol,
                                             uint8_t          i_beam_dim1,
                                             uint8_t          i_beam_dim2);

} // namespace ocudu
