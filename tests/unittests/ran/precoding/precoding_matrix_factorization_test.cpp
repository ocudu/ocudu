// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "precoding_matrix_test_fixture.h"
#include "ocudu/phy/support/precoding_configuration.h"
#include "ocudu/ran/beamforming/beam_weights_codebook.h"
#include "ocudu/ran/beamforming/beam_weights_codebook_generator.h"
#include "ocudu/ran/precoding/precoding_codebooks.h"
#include <gtest/gtest.h>

using namespace ocudu;

/// Tolerance for floating-point arithmetics equality comparisons.
static constexpr float TOLERANCE_FLOATING_POINT = 1e-5;

/// Assert that two float-based complex values are equal.
#define ASSERT_CF_EQ(val1, val2)                                                                                       \
  do {                                                                                                                 \
    ASSERT_NEAR((val1).real(), (val2).real(), TOLERANCE_FLOATING_POINT);                                               \
    ASSERT_NEAR((val1).imag(), (val2).imag(), TOLERANCE_FLOATING_POINT);                                               \
  } while (false)

namespace {

class PrecodingMatrixFactorizationFixture : public precoding_matrix_fixture
{};

} // namespace

TEST_P(PrecodingMatrixFactorizationFixture, PrecodingMatrixFactorization)
{
  // Apply the beam weights to the MIMO precoding matrix.
  precoding_weight_matrix result_weights(nof_layers, nof_antenna_ports);
  for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
    for (unsigned i_port = 0; i_port != nof_antenna_ports; ++i_port) {
      cf_t coefficient = {};
      // Compute the total coefficient from the product of MIMO precoding and beamforming.
      for (unsigned i_beam = 0; i_beam != beam_list.size(); ++i_beam) {
        beam_identifier beam = beam_list[i_beam];
        coefficient += mimo_weights.get_coefficient(i_layer, i_beam) * beam_codebook.get_coefficient(beam, i_port);
      }
      result_weights.set_coefficient(coefficient, i_layer, i_port);
    }
  }

  precoding_configuration result = precoding_configuration::make_wideband(result_weights);
  ASSERT_EQ(result.get_nof_layers(), reference.get_nof_layers());
  ASSERT_EQ(result.get_nof_ports(), reference.get_nof_ports());

  // Ensure that the result matches the reference precoding matrix.
  for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
    for (unsigned i_port = 0; i_port != nof_antenna_ports; ++i_port) {
      ASSERT_CF_EQ(result.get_coefficient(i_layer, i_port, 0), reference.get_coefficient(i_layer, i_port, 0));
    }
  }
}

static constexpr std::array<antenna_topology, 3> topologies = {antenna_topology::single_panel_two_one,
                                                               antenna_topology::single_panel_four_one,
                                                               antenna_topology::single_panel_two_two};

static constexpr std::array<pmi_codebook_typeI_single_panel, 3> panels = {
    pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::two_one, pmi_codebook_typeI_mode::one},
    pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::four_one, pmi_codebook_typeI_mode::one},
    pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::two_two, pmi_codebook_typeI_mode::one}};

static const std::vector<test_case_t> test_cases = generate_precoding_matrix_test_cases(topologies, panels);

INSTANTIATE_TEST_SUITE_P(PrecodingMatrixFactorizationTest,
                         PrecodingMatrixFactorizationFixture,
                         ::testing::ValuesIn(test_cases));
