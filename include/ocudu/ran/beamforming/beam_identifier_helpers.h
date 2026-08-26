// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/antenna_topology.h"
#include "ocudu/ran/beamforming/beam_identifier.h"
#include "ocudu/support/ocudu_assert.h"
#include <algorithm>
#include <iterator>

namespace ocudu {

/// \brief Convert a single port index to a beam identifier.
///
/// \param[in] antenna_topology Selected antenna topology.
/// \param[in] i_port           Selected antenna port index.
/// \return A beam identifier associated with the antenna topology.
/// \remark An assertion is triggered if the port index exceeds the maximum number of antennas given in the topology.
beam_identifier get_beam_id(antenna_topology antenna_topology, uint8_t i_port);

/// \brief Convert a combination of antenna topology and a selected beam defined in the 3GPP.
///
/// The 3GPP defines beams in the document TS38.214, Section 5.2.2.2, as in function of \f$\nu_{l,m}\f$ for a
/// combination of \f$(N_1, N_2)\f$ and \f$(O_1, O_2)\f$.
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
beam_identifier get_beam_id(antenna_topology antenna_topology,
                            uint8_t          i_panel,
                            uint8_t          i_pol,
                            uint8_t          i_beam_dim1,
                            uint8_t          i_beam_dim2);

/// \brief Get a default beam identifier list for a given number of ports.
///
/// The default list of beams selects the beams that map directly to ports, starting from port 0 to the total number of
/// ports minus one.
///
/// \param[in] nof_ports Number of ports to be mapped directly onto beams.
/// \return The list of beam identifiers associated to the port identifiers, starting from zero and in increasing order.
/// \remark An assertion is triggered if the number of ports exceeds \c precoding_constants::MAX_NOF_PORTS.
inline precoding_beam_list get_default_beam_list(unsigned nof_ports)
{
  ocudu_assert(nof_ports <= precoding_constants::MAX_NOF_PORTS,
               "The number of ports (i.e., {}) exceeds the maximum (i.e., {}).",
               nof_ports,
               precoding_constants::MAX_NOF_PORTS);

  precoding_beam_list beams;
  std::generate_n(std::back_inserter(beams), nof_ports, [i_port = 0U]() mutable { return to_beam_id(i_port++); });

  return beams;
}

} // namespace ocudu
