// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/beamforming/beam_weights_codebook_generator.h"
#include "ocudu/ran/beamforming/beam_identifier_helpers.h"
#include "ocudu/support/math/math_utils.h"

using namespace ocudu;

beam_weights_codebook ocudu::generate_beam_weights_codebook(antenna_topology topology)
{
  unsigned nof_panels         = get_nof_antenna_panels(topology);
  unsigned nof_elements_dim1  = get_nof_antenna_elements_dim1(topology);
  unsigned nof_elements_dim2  = get_nof_antenna_elements_dim2(topology);
  unsigned nof_polarizations  = get_nof_antenna_polarizations(topology);
  unsigned nof_beams_dim1     = get_nof_beams_dim1(topology);
  unsigned nof_beams_dim2     = get_nof_beams_dim2(topology);
  unsigned nof_total_antennas = get_total_nof_ports(topology);
  unsigned nof_total_beams    = get_total_nof_beams(topology);

  // Allocate beam weights.
  beam_weights_codebook beam_weights = beam_weights_codebook(nof_total_beams, nof_total_antennas);

  // Iterate over single antennas.
  for (unsigned i_antenna = 0; i_antenna != nof_total_antennas; ++i_antenna) {
    beam_weights.set_coefficient(1.0, single_port_to_beam_id(topology, i_antenna), i_antenna);
  }

  // Calculate the beamforming coefficient normalization by the number of transmit antenna ports.
  float amplitude = std::sqrt(1.0F / static_cast<float>(nof_total_antennas));

  // Iterate over the panel.
  for (unsigned i_panel = 0; i_panel != nof_panels; ++i_panel) {
    // Iterate over polarizations.
    for (unsigned i_pol = 0; i_pol != nof_polarizations; ++i_pol) {
      // Iterate over the first dimension beams.
      for (unsigned i_beam_dim1 = 0; i_beam_dim1 != nof_beams_dim1; ++i_beam_dim1) {
        // Iterate over the second dimension beams.
        for (unsigned i_beam_dim2 = 0; i_beam_dim2 != nof_beams_dim2; ++i_beam_dim2) {
          // Get beam identifier.
          beam_identifier beam_id = pmi_codebook_beam_to_beam_id(topology, i_panel, i_pol, i_beam_dim1, i_beam_dim2);

          // Calculate starting antenna port index:
          unsigned i_port = nof_elements_dim1 * nof_elements_dim2 * (nof_polarizations * i_panel + i_pol);

          // Generate expected coefficients for the panel, beams, and polarization.
          for (unsigned j = 0; j != nof_elements_dim1; ++j) {
            for (unsigned k = 0; k != nof_elements_dim2; ++k) {
              cf_t u_m   = std::polar(1.0F, TWOPI * j * i_beam_dim1 / nof_beams_dim1);
              cf_t v_l_m = u_m * std::polar(amplitude, TWOPI * k * i_beam_dim2 / nof_beams_dim2);
              beam_weights.set_coefficient(v_l_m, beam_id, i_port++);
            }
          }
        }
      }
    }
  }

  return beam_weights;
}
