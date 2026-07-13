// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/phy/support/precoding_configuration.h"
#include "ocudu/phy/support/re_pattern.h"
#include "ocudu/phy/support/resource_grid.h"
#include "ocudu/phy/support/resource_grid_mapper.h"
#include "ocudu/ran/antenna_topology.h"
#include "ocudu/ran/beamforming/beam_identifier.h"
#include "ocudu/ran/beamforming/beam_identifier_helpers.h"
#include "ocudu/ran/beamforming/beam_weights_codebook.h"
#include "ocudu/ran/beamforming/beam_weights_codebook_generator.h"
#include "ocudu/ran/precoding/precoding_codebook_configuration.h"
#include "ocudu/ran/precoding/precoding_codebook_helpers.h"
#include "ocudu/ran/precoding/precoding_codebooks.h"
#include "ocudu/ran/precoding/precoding_matrix_indicator.h"
#include <gtest/gtest.h>
#include <ostream>
#include <random>
#include <tuple>
#include <vector>

namespace ocudu {

/// Maximum number of layers supported.
static constexpr unsigned max_nof_layers = 4;

/// Parameters of a single test case: an antenna topology, a Type I single-panel PMI and the number of layers.
struct test_case_t {
  antenna_topology       topology;
  pmi_typeI_single_panel pmi;
  unsigned               nof_layers;
};

inline std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  return os << "nof_layers=" << test_case.nof_layers << " i_1_1=" << test_case.pmi.i_1_1
            << " i_1_3=" << test_case.pmi.i_1_3.value_or(0) << " i_2=" << test_case.pmi.i_2;
}

class precoding_matrix_fixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  /// Number of transmission layers.
  unsigned nof_layers;
  /// Number of antenna ports.
  unsigned nof_antenna_ports;
  /// Antenna topology.
  antenna_topology topology;
  /// Precoding Matrix Indicator (PMI).
  precoding_matrix_indicator pmi;
  /// Beam weights codebook for the antenna topology.
  beam_weights_codebook beam_codebook;
  /// MIMO precoding weights from the composite precoding.
  precoding_weight_matrix mimo_weights;
  /// Beam identifier list from the composite precoding.
  precoding_beam_list beam_list;
  /// Reference, compact, full-form, precoding matrix.
  precoding_configuration reference;

  void SetUp() override
  {
    // Extract test parameters.
    const test_case_t& param = GetParam();

    // Extract number of layers and number of antenna ports.
    nof_layers        = param.nof_layers;
    nof_antenna_ports = get_precoding_codebook_antenna_ports(param.pmi.panel_config);

    // Extract antenna topology configuration.
    topology = param.topology;

    // Extract PMI from the test case.
    pmi = param.pmi;

    // Composite type containing the MIMO precoding weights and the beam list, both derived from the test's PMI.
    precoding_mimo_beam_composite composite_precoding = get_mimo_matrix_from_pmi(param.pmi, param.nof_layers);

    // Extract MIMO weights and beam list separately.
    mimo_weights = std::get<0>(composite_precoding);
    beam_list    = std::get<1>(composite_precoding);

    // Generate the beam weights codebook from the antenna topology.
    beam_codebook = generate_beam_weights_codebook(param.topology);

    // Generate the reference precoding matrix.
    reference = precoding_configuration::make_wideband(make_type1_sp_mode1(param.pmi, param.nof_layers));
    ASSERT_EQ(reference.get_nof_layers(), param.nof_layers);
    ASSERT_EQ(reference.get_nof_ports(), nof_antenna_ports);
  }
};

// Generate a random vector of REs.
inline std::vector<ci8_t> generate_random_data(unsigned nof_re)
{
  static std::mt19937                       rgen(1234);
  static std::uniform_int_distribution<int> dist(-120, 120);

  std::vector<ci8_t> re_data;
  re_data.reserve(nof_re);
  std::generate_n(std::back_inserter(re_data), nof_re, []() {
    return ci8_t(static_cast<int8_t>(dist(rgen)), static_cast<int8_t>(dist(rgen)));
  });
  return re_data;
}

/// Return a list of resource grid port identifiers from the beam list - which is just each beam identifier converted to
/// integer.
inline static_vector<uint8_t, 2 * max_nof_layers> beam_list_to_ports(precoding_beam_list beams)
{
  static_vector<uint8_t, 2 * max_nof_layers> beam_ports;
  for (beam_identifier beam : beams) {
    beam_ports.push_back(static_cast<uint8_t>(to_uint(beam)));
  }
  return beam_ports;
}

inline std::vector<test_case_t> generate_precoding_matrix_test_cases(span<const antenna_topology> topologies,
                                                                     span<const pmi_codebook_typeI_single_panel> panels)
{
  std::vector<test_case_t> test_cases;

  for (unsigned nof_layers = 1; nof_layers != max_nof_layers + 1; ++nof_layers) {
    for (unsigned i_panel = 0; i_panel != panels.size(); ++i_panel) {
      const pmi_codebook_typeI_single_panel&    panel  = panels[i_panel];
      const pmi_typeI_single_panel_param_ranges ranges = get_pmi_ranges_typeI_single_panel(panel, nof_layers);

      for (unsigned i_1_1 = 0; i_1_1 != ranges.i_1_1; ++i_1_1) {
        for (unsigned i_1_2 = 0; i_1_2 != ranges.i_1_2; ++i_1_2) {
          for (unsigned i_1_3 = 0; i_1_3 != ranges.i_1_3; ++i_1_3) {
            for (unsigned i_2 = 0; i_2 != ranges.i_2; ++i_2) {
              pmi_typeI_single_panel pmi = {
                  .panel_config = panel,
                  .i_1_1        = i_1_1,
                  .i_1_2        = i_1_2,
                  .i_1_3        = i_1_3,
                  .i_2          = i_2,
              };

              test_cases.push_back({.topology = topologies[i_panel], .pmi = pmi, .nof_layers = nof_layers});
            }
          }
        }
      }
    }
  }

  return test_cases;
}

} // namespace ocudu
