// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/format.h"
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
#include "ocudu/ran/csi_report/csi_report_formatters.h"
#include "ocudu/ran/precoding/precoding_codebook_configuration.h"
#include "ocudu/ran/precoding/precoding_codebook_type1_helpers.h"
#include "ocudu/ran/precoding/precoding_codebooks.h"
#include "ocudu/ran/precoding/precoding_matrix_indicator.h"
#include "ocudu/support/error_handling.h"
#include <gtest/gtest.h>
#include <ostream>
#include <random>
#include <tuple>
#include <vector>

namespace ocudu {

/// Maximum number of layers supported.
static constexpr unsigned max_nof_layers = 4;

/// Parameters of a single test case.
struct test_case_t {
  precoding_matrix_indicator pmi;
  unsigned                   nof_layers;
};

static std::mt19937 rgen;

inline std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  return os << fmt::format("nof_layers={} pmi={}", test_case.nof_layers, test_case.pmi);
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

    // Extract number of layers and PMI.
    nof_layers = param.nof_layers;
    pmi        = param.pmi;

    // The antenna panel configuration determines both the number of antenna ports and the antenna topology.
    pmi_codebook_single_panel_config panel_config;

    if (const auto* type1sp = std::get_if<pmi_typeI_single_panel>(&pmi)) {
      panel_config = type1sp->panel_config.n1_n2;
    } else if (const auto* type2 = std::get_if<pmi_typeII>(&pmi)) {
      panel_config = type2->config.n1_n2;
    } else {
      report_error("The PMI does not describe a single-panel configuration.");
    }

    const pmi_codebook_single_panel_info& panel_info = get_single_panel_info(panel_config);
    nof_antenna_ports                                = 2 * panel_info.n1 * panel_info.n2;
    topology                                         = get_single_panel_topology(panel_config);

    // Composite type containing the MIMO precoding weights and the beam list, both derived from the test's PMI.
    precoding_beamforming_composite composite_precoding = get_mimo_matrix_from_pmi(param.pmi, param.nof_layers);

    // Extract MIMO weights and beam list separately.
    mimo_weights = composite_precoding.mimo;
    beam_list    = composite_precoding.beams;

    // Generate the beam weights codebook from the antenna topology.
    beam_codebook = generate_beam_weights_codebook(topology);

    // Generate the reference precoding matrix.
    precoding_weight_matrix reference_weights = make_precoding(pmi, param.nof_layers);

    reference = precoding_configuration::make_wideband(reference_weights);
    ASSERT_EQ(reference.get_nof_layers(), param.nof_layers);
    ASSERT_EQ(reference.get_nof_ports(), nof_antenna_ports);
  }
};

// Generate a random vector of REs.
inline std::vector<ci8_t> generate_random_data(unsigned nof_re)
{
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
inline static_vector<uint8_t, max_nof_beams_per_pmi> beam_list_to_ports(precoding_beam_list beams)
{
  static_vector<uint8_t, max_nof_beams_per_pmi> beam_ports;
  for (beam_identifier beam : beams) {
    beam_ports.push_back(static_cast<uint8_t>(to_underlying(beam)));
  }
  return beam_ports;
}

inline std::vector<test_case_t> generate_typeI_precoding_matrix_test_cases()
{
  static constexpr std::array<pmi_codebook_typeI_single_panel, 3> panels = {
      pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::two_one, pmi_codebook_typeI_mode::one},
      pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::four_one, pmi_codebook_typeI_mode::one},
      pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::two_two, pmi_codebook_typeI_mode::one}};

  std::vector<test_case_t> test_cases;

  for (unsigned nof_layers = 1; nof_layers != max_nof_layers + 1; ++nof_layers) {
    for (const pmi_codebook_typeI_single_panel& panel : panels) {
      const pmi_typeI_single_panel_param_ranges ranges = get_pmi_ranges_typeI_single_panel(panel, nof_layers);

      for (uint8_t i_1_1 = 0; i_1_1 != ranges.i_1_1; ++i_1_1) {
        for (uint8_t i_1_2 = 0; i_1_2 != ranges.i_1_2; ++i_1_2) {
          for (uint8_t i_1_3 = 0; i_1_3 != ranges.i_1_3; ++i_1_3) {
            for (uint8_t i_2 = 0; i_2 != ranges.i_2; ++i_2) {
              pmi_typeI_single_panel pmi = {
                  .panel_config = panel,
                  .i_1_1        = i_1_1,
                  .i_1_2        = i_1_2,
                  .i_1_3        = i_1_3,
                  .i_2          = i_2,
              };

              test_cases.push_back({.pmi = pmi, .nof_layers = nof_layers});
            }
          }
        }
      }
    }
  }

  return test_cases;
}

/// Draws a random set of Type II combining coefficients for one layer.
inline pmi_typeII::layer_coefficients
generate_random_typeII_coefficients(unsigned nof_beams, unsigned n_psk, bool subband_amplitude)
{
  // Maximum wideband amplitude index, as per TS38.214 Table 5.2.2.2.3-2.
  static constexpr uint8_t max_wideband_amplitude_index = 7;

  std::uniform_int_distribution<uint8_t> amplitude_dist(0, max_wideband_amplitude_index - 1);
  std::uniform_int_distribution<uint8_t> phase_dist(0, n_psk - 1);
  std::uniform_int_distribution<uint8_t> subband_amplitude_dist(0, 1);
  std::uniform_int_distribution<uint8_t> strongest_dist(0, 2 * nof_beams - 1);

  // Number of combining coefficients, one per beam and polarization.
  unsigned nof_coefficients = 2 * nof_beams;

  // Resulting random per layer coefficients.
  pmi_typeII::layer_coefficients coefficients;

  // Generate random wideband amplitude values.
  std::generate_n(
      std::back_inserter(coefficients.i_1_4), nof_coefficients, [&amplitude_dist]() { return amplitude_dist(rgen); });

  // Generate random phase values.
  std::generate_n(
      std::back_inserter(coefficients.i_2_1), nof_coefficients, [&phase_dist]() { return phase_dist(rgen); });

  // Generate random subband amplitude values if enabled.
  if (subband_amplitude) {
    std::generate_n(std::back_inserter(coefficients.i_2_2), nof_coefficients, [&subband_amplitude_dist]() {
      return subband_amplitude_dist(rgen);
    });
  }

  // The strongest coefficient carries the maximum wideband amplitude among the reported coefficients.
  coefficients.i_1_3                     = strongest_dist(rgen);
  coefficients.i_1_4[coefficients.i_1_3] = max_wideband_amplitude_index;

  return coefficients;
}

inline std::vector<test_case_t> generate_typeII_precoding_matrix_test_cases()
{
  static constexpr std::array<pmi_codebook_single_panel_config, 3> panels = {pmi_codebook_single_panel_config::two_one,
                                                                             pmi_codebook_single_panel_config::four_one,
                                                                             pmi_codebook_single_panel_config::two_two};

  std::vector<test_case_t> test_cases;

  for (pmi_codebook_single_panel_config panel : panels) {
    const pmi_codebook_single_panel_info& panel_info = get_single_panel_info(panel);

    // The number of combined beams cannot exceed the number of beam groups the panel provides.
    unsigned max_nof_combined_beams = std::min(max_nof_typeII_beams, panel_info.n1 * panel_info.n2);

    for (unsigned nof_beams = 2; nof_beams <= max_nof_combined_beams; ++nof_beams) {
      for (auto phase_alphabet_size : {pmi_codebook_typeII_phase_size::qpsk, pmi_codebook_typeII_phase_size::psk8}) {
        for (bool subband_amplitude : {false, true}) {
          pmi_codebook_typeII config = {.n1_n2               = panel,
                                        .nof_beams           = nof_beams,
                                        .phase_alphabet_size = phase_alphabet_size,
                                        .subband_amplitude   = subband_amplitude};

          for (unsigned nof_layers = 1; nof_layers != max_nof_typeII_layers + 1; ++nof_layers) {
            const pmi_typeII_param_ranges ranges = get_pmi_ranges_typeII(config, nof_layers);

            for (unsigned i_1_1 = 0; i_1_1 != ranges.i_1_1; ++i_1_1) {
              for (unsigned i_1_2 = 0; i_1_2 != ranges.i_1_2; ++i_1_2) {
                pmi_typeII pmi = {.config = config,
                                  .i_1_1  = static_cast<uint8_t>(i_1_1),
                                  .i_1_2  = static_cast<uint16_t>(i_1_2),
                                  .layers = {}};

                for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
                  pmi.layers.push_back(generate_random_typeII_coefficients(
                      nof_beams, static_cast<unsigned>(phase_alphabet_size), subband_amplitude));
                }

                test_cases.push_back({.pmi = pmi, .nof_layers = nof_layers});
              }
            }
          }
        }
      }
    }
  }

  return test_cases;
}

inline std::vector<test_case_t> generate_precoding_matrix_test_cases()
{
  std::vector<test_case_t> test_cases;

  // Generate Type I Single-Panel test cases.
  auto typeI_tests = generate_typeI_precoding_matrix_test_cases();
  // Generate Type II test cases.
  auto typeII_tests = generate_typeII_precoding_matrix_test_cases();

  test_cases.insert(test_cases.end(), typeI_tests.begin(), typeI_tests.end());
  test_cases.insert(test_cases.end(), typeII_tests.begin(), typeII_tests.end());

  return test_cases;
}

} // namespace ocudu
