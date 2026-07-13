// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/adt/to_array.h"
#include "ocudu/ran/antenna_topology.h"
#include "ocudu/ran/beamforming/beam_identifier_helpers.h"
#include "ocudu/ran/beamforming/beam_weights_codebook_generator.h"
#include "ocudu/support/math/math_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace ocudu {
static std::ostream& operator<<(std::ostream& os, antenna_topology topology)
{
  os << to_string(topology);
  return os;
}

} // namespace ocudu

namespace {

class BeamCodebookGeneratorFixture : public ::testing::TestWithParam<antenna_topology>
{};

} // namespace

TEST_P(BeamCodebookGeneratorFixture, Generate)
{
  antenna_topology topology           = GetParam();
  unsigned         nof_panels         = get_nof_antenna_panels(topology);
  unsigned         nof_elements_dim1  = get_nof_antenna_elements_dim1(topology);
  unsigned         nof_elements_dim2  = get_nof_antenna_elements_dim2(topology);
  unsigned         nof_polarizations  = get_nof_antenna_polarizations(topology);
  unsigned         nof_beams_dim1     = get_nof_beams_dim1(topology);
  unsigned         nof_beams_dim2     = get_nof_beams_dim2(topology);
  unsigned         nof_total_antennas = get_total_nof_ports(topology);

  // Generate beam weights.
  beam_weights_codebook beam_weights = generate_beam_weights_codebook(topology);

  // Iterate over the panel.
  for (unsigned i_panel = 0; i_panel != nof_panels; ++i_panel) {
    // Iterate over polarizations.
    for (unsigned i_pol = 0; i_pol != nof_polarizations; ++i_pol) {
      // Iterate over the first dimension beams.
      for (unsigned i_beam_dim1 = 0; i_beam_dim1 != nof_beams_dim1; ++i_beam_dim1) {
        // Iterate over the second dimension beams.
        for (unsigned i_beam_dim2 = 0; i_beam_dim2 != nof_beams_dim2; ++i_beam_dim2) {
          // Get beam identifier.
          beam_identifier beam_id = get_beam_id(topology, i_panel, i_pol, i_beam_dim1, i_beam_dim2);

          // Calculate starting antenna port index.
          unsigned i_port = nof_elements_dim1 * nof_elements_dim2 * (nof_polarizations * i_panel + i_pol);

          // Calculate the beamforming coefficient normalization by the number of transmit antenna ports.
          float amplitude = std::sqrt(1.0F / static_cast<float>(nof_total_antennas));

          // Generate expected coefficients for the panel, beams, and polarization.
          std::vector<cf_t> expected_coefficients(nof_total_antennas, 0.0);
          for (unsigned j = 0; j != nof_elements_dim1; ++j) {
            for (unsigned k = 0; k != nof_elements_dim2; ++k) {
              cf_t u_m                        = std::polar(1.0F, TWOPI * j * i_beam_dim1 / nof_beams_dim1);
              cf_t v_l_m                      = u_m * std::polar(amplitude, TWOPI * k * i_beam_dim2 / nof_beams_dim2);
              expected_coefficients[i_port++] = v_l_m;
            }
          }

          // Check each of the antenna port coefficients.
          for (i_port = 0; i_port != nof_total_antennas; ++i_port) {
            float err = std::abs(beam_weights.get_coefficient(beam_id, i_port) - expected_coefficients[i_port]);
            ASSERT_LT(err, 1e-6) << fmt::format(
                "panel={} pol={} l={} m={} antenna={}\ncoeff=[{}];\nexpected=[{}];\nerr={:.3f}",
                i_panel,
                i_pol,
                i_beam_dim1,
                i_beam_dim2,
                i_port,
                beam_weights.get_beam_coefficients<32>(beam_id),
                expected_coefficients,
                err);
          }
        }
      }
    }
  }
}

static constexpr auto antenna_topologies = to_array<antenna_topology>({antenna_topology::one_port,
                                                                       antenna_topology::two_port,
                                                                       antenna_topology::four_ports,
                                                                       antenna_topology::eight_ports,
                                                                       antenna_topology::single_panel_two_one,
                                                                       antenna_topology::single_panel_two_two,
                                                                       antenna_topology::single_panel_four_one});

INSTANTIATE_TEST_SUITE_P(BeamCodebookGenerator, BeamCodebookGeneratorFixture, ::testing::ValuesIn(antenna_topologies));
