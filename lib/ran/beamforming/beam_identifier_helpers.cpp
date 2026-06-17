// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/beamforming/beam_identifier_helpers.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;

beam_identifier ocudu::single_port_to_beam_id(antenna_topology topology, uint8_t i_port)
{
  ocudu_assert(i_port < get_total_nof_ports(topology),
               "The port index (i.e., {}) exceeds the maximum (i.e., {}) for the topology {}",
               i_port,
               get_total_nof_ports(topology) - 1,
               to_string(topology));

  return to_beam_id(i_port);
}

beam_identifier ocudu::pmi_codebook_beam_to_beam_id(antenna_topology topology,
                                                    uint8_t          i_panel,
                                                    uint8_t          i_pol,
                                                    uint8_t          i_beam_dim1,
                                                    uint8_t          i_beam_dim2)
{
  [[maybe_unused]] unsigned nof_panels         = get_nof_antenna_panels(topology);
  unsigned                  nof_polarizations  = get_nof_antenna_polarizations(topology);
  unsigned                  nof_beams_dim1     = get_nof_beams_dim1(topology);
  unsigned                  nof_beams_dim2     = get_nof_beams_dim2(topology);
  unsigned                  total_nof_antennas = get_total_nof_ports(topology);

  ocudu_assert(i_panel < nof_panels,
               "The panel index (i.e., {}) exceeds the maximum (i.e., {}) for the topology {}",
               i_panel,
               nof_panels - 1,
               to_string(topology));
  ocudu_assert(i_pol < nof_polarizations,
               "The polarization index (i.e., {}) exceeds the maximum (i.e., {}) for the topology {}",
               i_pol,
               nof_polarizations - 1,
               to_string(topology));
  ocudu_assert(i_beam_dim1 < nof_beams_dim1,
               "The first dimension beam index (i.e., {}) exceeds the maximum (i.e., {}) for the topology {}",
               i_beam_dim1,
               nof_beams_dim1 - 1,
               to_string(topology));
  ocudu_assert(i_beam_dim2 < nof_beams_dim2,
               "The second dimension beam index (i.e., {}) exceeds the maximum (i.e., {}) for the topology {}",
               i_beam_dim2,
               nof_beams_dim2 - 1,
               to_string(topology));

  unsigned i_beam = i_panel;
  i_beam          = nof_polarizations * i_beam + i_pol;
  i_beam          = nof_beams_dim1 * i_beam + i_beam_dim1;
  i_beam          = nof_beams_dim2 * i_beam + i_beam_dim2;

  return to_beam_id(i_beam + total_nof_antennas);
}
