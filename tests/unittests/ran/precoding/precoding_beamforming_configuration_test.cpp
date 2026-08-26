// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/precoding/precoding_codebooks.h"
#include "ocudu/ran/precoding_beamforming_configuration.h"
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;

namespace {

/// Random generator.
std::mt19937 rgen(1234);

/// Number of PRB that comprise a PRG in the subband tests.
constexpr unsigned prg_size = 4;

class PrecodingBeamformingConfigFixture : public ::testing::Test
{
protected:
  /// Generates a MIMO precoding matrix with random weights.
  precoding_weight_matrix generate_random_mimo(unsigned nof_layers, unsigned nof_beams)
  {
    precoding_weight_matrix mimo(nof_layers, nof_beams);
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      for (unsigned i_beam = 0; i_beam != nof_beams; ++i_beam) {
        mimo.set_coefficient({weight_dist(rgen), weight_dist(rgen)}, i_layer, i_beam);
      }
    }

    return mimo;
  }

  /// Generates a beam list with beamformed beams of the given antenna topology.
  static precoding_beam_list generate_beamformed_list(antenna_topology topology, unsigned nof_beams)
  {
    precoding_beam_list beams;
    for (unsigned i_beam = 0; i_beam != nof_beams; ++i_beam) {
      beams.push_back(get_beam_id(topology, 0, i_beam % get_nof_antenna_polarizations(topology), 1, 0));
    }

    return beams;
  }

  std::uniform_real_distribution<float> weight_dist{-1.0F, +1.0F};
};

TEST_F(PrecodingBeamformingConfigFixture, DefaultConstructor)
{
  precoding_beamforming_configuration config;

  ASSERT_EQ(config.get_nof_layers(), 0);
  ASSERT_EQ(config.get_nof_beams(), 0);
  ASSERT_EQ(config.get_nof_prg(), 0);
  ASSERT_EQ(config.get_prg_size(), MAX_NOF_PRBS);
}

TEST_F(PrecodingBeamformingConfigFixture, NoMimoNoBeamforming)
{
  static constexpr unsigned one_prg = 1;

  for (unsigned nof_layers : {1U, 2U, 4U, 8U}) {
    unsigned nof_beams = nof_layers;

    // No MIMO precoding and no beamforming. Each layer is mapped onto the antenna port with the same index.
    precoding_beamforming_configuration config(nof_layers, nof_beams, one_prg, MAX_NOF_PRBS);

    ASSERT_EQ(config.get_nof_layers(), nof_layers);
    ASSERT_EQ(config.get_nof_beams(), nof_beams);
    ASSERT_EQ(config.get_nof_prg(), one_prg);

    const precoding_beamforming_composite& prg = config.get_prg(0);

    // The MIMO precoding matrix is the identity matrix.
    ASSERT_EQ(prg.mimo, make_identity(nof_layers));

    // The beam list selects the antenna ports.
    ASSERT_EQ(prg.beams, get_default_beam_list(nof_layers));
    for (unsigned i_beam = 0; i_beam != nof_layers; ++i_beam) {
      ASSERT_EQ(prg.beams[i_beam], to_beam_id(i_beam));
    }
  }
}

TEST_F(PrecodingBeamformingConfigFixture, MimoNoBeamforming)
{
  static constexpr unsigned nof_layers = 2;
  static constexpr unsigned nof_ports  = 4;

  // MIMO precoding and no beamforming. The MIMO precoding matrix maps the layers onto the antenna ports.
  precoding_weight_matrix             mimo   = generate_random_mimo(nof_layers, nof_ports);
  precoding_beamforming_configuration config = precoding_beamforming_configuration::make_wideband(mimo);

  ASSERT_EQ(config.get_nof_layers(), nof_layers);
  ASSERT_EQ(config.get_nof_beams(), nof_ports);
  ASSERT_EQ(config.get_nof_prg(), 1);
  ASSERT_EQ(config.get_prg_size(), MAX_NOF_PRBS);

  const precoding_beamforming_composite& composite = config.get_prg(0);
  ASSERT_EQ(composite.mimo, mimo);
  ASSERT_EQ(composite.beams, get_default_beam_list(nof_ports));
}

TEST_F(PrecodingBeamformingConfigFixture, NoMimoBeamforming)
{
  static constexpr antenna_topology topology  = antenna_topology::single_panel_four_one;
  static constexpr unsigned         nof_beams = 4;

  // No MIMO precoding and beamforming. Each layer is mapped onto one beam.
  precoding_beam_list                 beams  = generate_beamformed_list(topology, nof_beams);
  precoding_beamforming_configuration config = precoding_beamforming_configuration::make_wideband(beams);

  ASSERT_EQ(config.get_nof_layers(), nof_beams);
  ASSERT_EQ(config.get_nof_beams(), nof_beams);

  const precoding_beamforming_composite& composite = config.get_prg(0);
  ASSERT_EQ(composite.mimo, make_identity(nof_beams));
  ASSERT_EQ(composite.beams, beams);
}

TEST_F(PrecodingBeamformingConfigFixture, MimoBeamforming)
{
  for (unsigned nof_layers : {1U, 2U, 4U}) {
    // Create a Type I Single-Panel PMI for generating a valid pair of MIMO precoding and beamforming.
    pmi_typeI_single_panel pmi = {
        .panel_config = pmi_codebook_typeI_single_panel{.n1_n2 = pmi_codebook_single_panel_config::four_one,
                                                        .mode  = pmi_codebook_typeI_mode::one},
        .i_1_1        = 1,
        .i_1_2        = std::nullopt,
        .i_1_3        = (nof_layers > 1) ? std::optional<unsigned>(0) : std::nullopt,
        .i_2          = 1};

    // Get the composite precoding configuration from the PMI parameters.
    precoding_beamforming_composite composite = get_mimo_matrix_from_pmi(precoding_matrix_indicator(pmi), nof_layers);

    // Create a wideband precoding and beamforming configuration from the composite precoding.
    precoding_beamforming_configuration config = precoding_beamforming_configuration::make_wideband(composite);

    // Validate the resultant configuration matches the with the composite precoding configuration.
    ASSERT_EQ(config.get_nof_layers(), nof_layers);
    ASSERT_EQ(config.get_nof_beams(), composite.beams.size());

    const precoding_beamforming_composite& prg = config.get_prg(0);
    ASSERT_EQ(prg.mimo, composite.mimo);
    ASSERT_EQ(prg.beams, composite.beams);

    // The beams are beamformed beams, therefore they are not the default ones.
    ASSERT_NE(prg.beams, get_default_beam_list(config.get_nof_beams()));
  }
}

TEST_F(PrecodingBeamformingConfigFixture, SubbandGranularity)
{
  static constexpr unsigned nof_layers = 2;
  static constexpr unsigned nof_beams  = 2;
  static constexpr unsigned nof_prg    = 8;

  precoding_beamforming_configuration config(nof_layers, nof_beams, nof_prg, prg_size);
  ASSERT_EQ(config.get_nof_prg(), nof_prg);
  ASSERT_EQ(config.get_prg_size(), prg_size);

  // Set a different composite per PRG.
  std::vector<precoding_beamforming_composite> composite_list;
  for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
    precoding_beam_list beams;
    beams.push_back(to_beam_id(i_prg % precoding_constants::MAX_NOF_PORTS));
    beams.push_back(to_beam_id((i_prg + 1) % precoding_constants::MAX_NOF_PORTS));

    composite_list.push_back({generate_random_mimo(nof_layers, nof_beams), beams});
    config.set_prg(composite_list.back(), i_prg);
  }

  // Verify that each PRG keeps its own configuration.
  for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
    const precoding_beamforming_composite& composite = config.get_prg(i_prg);

    ASSERT_EQ(composite.mimo, composite_list[i_prg].mimo);
    ASSERT_EQ(composite.beams, composite_list[i_prg].beams);
    ASSERT_EQ(composite.beams[0], to_beam_id(i_prg % precoding_constants::MAX_NOF_PORTS));
    ASSERT_EQ(composite.beams[1], to_beam_id((i_prg + 1) % precoding_constants::MAX_NOF_PORTS));
  }
}

TEST_F(PrecodingBeamformingConfigFixture, SetGetComposite)
{
  static constexpr unsigned nof_layers = 2;
  static constexpr unsigned nof_beams  = 4;

  // No MIMO precoding and no beamforming. Each layer is mapped onto the antenna port with the same index.
  precoding_beamforming_configuration config(nof_layers, nof_beams, 2, prg_size);

  // Modify the composite of the second PRG, taking the current one as a base.
  precoding_beamforming_composite composite = config.get_prg(1);
  cf_t                            coefficient{0.5F, -0.25F};
  beam_identifier                 beam_id = to_beam_id(6);
  composite.mimo.set_coefficient(coefficient, 1, 3);
  composite.beams[3] = beam_id;
  config.set_prg(composite, 1);

  ASSERT_EQ(config.get_prg(1).mimo.get_coefficient(1, 3), coefficient);
  ASSERT_EQ(config.get_prg(1).beams[3], beam_id);

  // The remaining PRG is unmodified. Since no MIMO precoding and no beamforming are applied, the MIMO precoding
  // coefficients out of the main diagonal are zero and the beam list is the default one - port index equals beam id.
  ASSERT_EQ(config.get_prg(0).mimo.get_coefficient(1, 3), cf_t());
  ASSERT_EQ(config.get_prg(0).beams[3], to_beam_id(3));
}

TEST_F(PrecodingBeamformingConfigFixture, CopyAndAssignment)
{
  precoding_beam_list beams = generate_beamformed_list(antenna_topology::single_panel_four_one, 2);

  precoding_beamforming_configuration config(2, 2, 4, prg_size);
  config.set_prg({generate_random_mimo(2, 2), beams}, 0);

  // Copy construction.
  precoding_beamforming_configuration copy(config);
  ASSERT_EQ(copy, config);

  // Copy assignment.
  precoding_beamforming_configuration assigned;
  assigned = config;
  ASSERT_EQ(assigned, config);

  // Modifying the copy does not modify the original.
  precoding_beamforming_composite composite = copy.get_prg(0);
  composite.beams[0]                        = beam_identifier::n0;
  copy.set_prg(composite, 0);
  ASSERT_NE(copy, config);
}

TEST_F(PrecodingBeamformingConfigFixture, EqualityOperator)
{
  precoding_beamforming_configuration config(2, 2, 2, prg_size);
  precoding_beamforming_configuration other(2, 2, 2, prg_size);
  ASSERT_EQ(config, other);

  // A different number of PRG.
  ASSERT_NE(config, precoding_beamforming_configuration(2, 2, 3, prg_size));

  // A different PRG size.
  ASSERT_NE(config, precoding_beamforming_configuration(2, 2, 2, 2 * prg_size));

  // A different number of layers or beams.
  ASSERT_NE(config, precoding_beamforming_configuration(1, 2, 2, prg_size));
  ASSERT_NE(config, precoding_beamforming_configuration(2, 4, 2, prg_size));

  // A different MIMO precoding matrix.
  precoding_beamforming_composite composite = other.get_prg(1);
  composite.mimo.set_coefficient({1.0F, 0.0F}, 0, 1);
  other.set_prg(composite, 1);
  ASSERT_NE(config, other);

  // A different beam list.
  other              = config;
  composite          = other.get_prg(0);
  composite.beams[0] = to_beam_id(7);
  other.set_prg(composite, 0);
  ASSERT_NE(config, other);
}

TEST_F(PrecodingBeamformingConfigFixture, Resize)
{
  static constexpr unsigned nof_layers = 2;
  static constexpr unsigned nof_beams  = 2;

  precoding_beamforming_configuration config(nof_layers, nof_beams, 2, prg_size);
  precoding_weight_matrix             mimo = generate_random_mimo(nof_layers, nof_beams);
  config.set_prg({mimo, generate_beamformed_list(antenna_topology::single_panel_four_one, nof_beams)}, 0);

  // Grow - the surviving PRG keeps its configuration and the appended ones get the default configuration.
  config.resize(4, 2 * prg_size);
  ASSERT_EQ(config.get_nof_prg(), 4);
  ASSERT_EQ(config.get_prg_size(), 2 * prg_size);
  ASSERT_EQ(config.get_prg(0).mimo, mimo);
  for (unsigned i_prg = 2; i_prg != 4; ++i_prg) {
    const precoding_beamforming_composite& composite = config.get_prg(i_prg);
    ASSERT_EQ(composite.mimo, make_identity(nof_layers));
    ASSERT_EQ(composite.beams, get_default_beam_list(nof_beams));
  }

  // Shrink.
  config.resize(1, prg_size);
  ASSERT_EQ(config.get_nof_prg(), 1);
  ASSERT_EQ(config.get_prg_size(), prg_size);
  ASSERT_EQ(config.get_prg(0).mimo, mimo);
}

TEST_F(PrecodingBeamformingConfigFixture, ScalingOperator)
{
  static constexpr unsigned nof_layers = 2;
  static constexpr unsigned nof_beams  = 2;
  static constexpr float    scale      = 0.25F;

  precoding_beamforming_configuration config(nof_layers, nof_beams, 2, prg_size);
  precoding_beam_list                 beams = generate_beamformed_list(antenna_topology::single_panel_four_one, 2);
  config.set_prg({make_identity(nof_layers), beams}, 0);

  precoding_weight_matrix expected = make_identity(nof_layers);
  expected *= scale;

  config *= scale;

  // The MIMO precoding weights are scaled and the beams are not modified.
  ASSERT_EQ(config.get_prg(0).mimo, expected);
  ASSERT_EQ(config.get_prg(1).mimo, expected);
  ASSERT_EQ(config.get_prg(0).beams, beams);
}

} // namespace
