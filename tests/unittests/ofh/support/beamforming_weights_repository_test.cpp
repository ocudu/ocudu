// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "compression/beamforming_weights_compressor.h"
#include "support/beamforming_weights_repository.h"
#include "ocudu/ofh/compression/compression_properties.h"
#include "ocudu/ofh/ofh_constants.h"
#include "ocudu/ran/beamforming/beam_weights_codebook_generator.h"
#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>
#include <vector>

using namespace ocudu;
using namespace ocudu::ofh;

namespace {

class beamforming_weights_repository_fixture
  : public ::testing::TestWithParam<std::tuple<antenna_topology, compression_type, unsigned>>
{};

} // namespace

TEST_P(beamforming_weights_repository_fixture, stored_weights_match_direct_compression)
{
  antenna_topology            topology = std::get<0>(GetParam());
  const ru_compression_params compr_params{std::get<1>(GetParam()), std::get<2>(GetParam())};

  beam_weights_codebook          codebook = generate_beam_weights_codebook(topology);
  beamforming_weights_repository repository(codebook, compr_params);

  ASSERT_EQ(repository.get_compression_params().type, compr_params.type);
  ASSERT_EQ(repository.get_compression_params().data_width, compr_params.data_width);

  std::vector<uint8_t> expected(get_packed_beamforming_weights_size(codebook.get_nof_antennas(), compr_params).value());
  for (unsigned i_beam = 0, nof_beams = codebook.get_nof_beams(); i_beam != nof_beams; ++i_beam) {
    beam_identifier beam_id      = to_beam_id(i_beam);
    auto            coefficients = codebook.get_beam_coefficients<MAX_NOF_BEAMFORMING_WEIGHTS>(beam_id);

    compress_beamforming_weights(expected, coefficients, compr_params);

    ASSERT_EQ(span<const uint8_t>(expected), repository.get_weights(beam_id));
  }
}

INSTANTIATE_TEST_SUITE_P(beamforming_weights_repository_test,
                         beamforming_weights_repository_fixture,
                         ::testing::Combine(::testing::Values(antenna_topology::two_port,
                                                              antenna_topology::single_panel_two_one,
                                                              antenna_topology::single_panel_two_two,
                                                              antenna_topology::single_panel_four_one),
                                            ::testing::Values(compression_type::none, compression_type::BFP),
                                            ::testing::Values(9U, 16U)));

#ifdef ASSERTS_ENABLED
TEST(beamforming_weights_repository_test, death_when_beam_id_out_of_range)
{
  beam_weights_codebook          codebook = generate_beam_weights_codebook(antenna_topology::two_port);
  beamforming_weights_repository repository(codebook, {compression_type::none, 16});

  ASSERT_DEATH(repository.get_weights(to_beam_id(codebook.get_nof_beams())), "");
}
#endif
