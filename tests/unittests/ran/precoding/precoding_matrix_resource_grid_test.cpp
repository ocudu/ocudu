// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "precoding_matrix_test_fixture.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/phy/generic_functions/precoding/precoding_factories.h"
#include "ocudu/phy/support/precoding_configuration.h"
#include "ocudu/phy/support/re_buffer.h"
#include "ocudu/phy/support/re_pattern.h"
#include "ocudu/phy/support/resource_grid.h"
#include "ocudu/phy/support/resource_grid_mapper.h"
#include "ocudu/phy/support/resource_grid_reader.h"
#include "ocudu/phy/support/support_factories.h"
#include "ocudu/ran/beamforming/beam_weights_codebook.h"
#include "ocudu/ran/beamforming/beam_weights_codebook_generator.h"
#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/ran/precoding/precoding_codebooks.h"
#include <cmath>
#include <gtest/gtest.h>
#include <numeric>
#include <random>

using namespace ocudu;

namespace {

class PrecodingMatrixResourceGridFixture : public precoding_matrix_fixture
{
protected:
  // For the resource grid test, allocate a few PRB on a single OFDM symbol.
  static constexpr unsigned nof_prb  = 2;
  static constexpr unsigned nof_re   = nof_prb * NOF_SUBCARRIERS_PER_RB;
  static constexpr unsigned i_symbol = 0;

  static std::shared_ptr<channel_precoder_factory>     precoder_factory;
  static std::shared_ptr<resource_grid_factory>        rg_factory;
  static std::shared_ptr<resource_grid_mapper_factory> mapper_factory;
  static std::unique_ptr<resource_grid_mapper>         mapper;

  /// List of antenna ports.
  static_vector<uint8_t, precoding_constants::MAX_NOF_PORTS> antenna_ports;
  /// Reserved RE pattern list.
  re_pattern_list reserved;
  /// Resource grid allocation configuration.
  resource_grid_mapper::allocation_configuration allocation;
  // Buffer for each antenna port to hold the symbols after applying the beam weights.
  static_re_buffer<precoding_constants::MAX_NOF_PORTS, MAX_NOF_SUBCARRIERS> beamformed;
  /// Buffer to hold the error scale
  static_vector<std::vector<float>, precoding_constants::MAX_NOF_PORTS> error_scale;

  void SetUp() override
  {
    // Call the base fixture test setup.
    precoding_matrix_fixture::SetUp();

    // Generate the list of antenna ports.
    antenna_ports.resize(nof_antenna_ports);
    std::iota(antenna_ports.begin(), antenna_ports.end(), 0);

    // Allocate nof_prb contiguous PRB from CRB 0, a single OFDM symbol, shared by both precodings.
    allocation = {.bwp        = crb_interval(0, nof_prb),
                  .freq_alloc = rb_allocation::make_type1(0, nof_prb),
                  .time_alloc = ofdm_symbol_range(i_symbol, i_symbol + 1)};

    // Resize beamformed output and error scale buffer properly.
    beamformed.resize(nof_antenna_ports, nof_re);
    error_scale.resize(nof_antenna_ports);

    // Initialize with zeros the beamformed output and the error scale buffer.
    for (unsigned i_port = 0; i_port != nof_antenna_ports; ++i_port) {
      error_scale[i_port].resize(nof_re);

      ocuduvec::zero(beamformed.get_slice(i_port));
      ocuduvec::zero(error_scale[i_port]);
    }
  }

  static void SetUpTestSuite()
  {
    if (!precoder_factory) {
      precoder_factory = create_channel_precoder_factory("generic");
      ASSERT_NE(precoder_factory, nullptr) << "Cannot create channel precoder factory.";
    }

    if (!rg_factory) {
      rg_factory = create_resource_grid_factory();
      ASSERT_NE(rg_factory, nullptr) << "Cannot create resource grid factory.";
    }

    if (!mapper_factory) {
      mapper_factory = create_resource_grid_mapper_factory(precoder_factory);
      ASSERT_NE(mapper_factory, nullptr) << "Cannot create resource grid mapper factory.";
    }

    if (!mapper) {
      mapper = mapper_factory->create();
      ASSERT_NE(mapper, nullptr) << "Cannot create resource grid mapper.";
    }
  }

  /// Create a resource grid with the given number of ports, mapping the specified data to the resource grid using the
  /// given precoding configuration.
  std::unique_ptr<resource_grid> create_grid_and_map(unsigned                       nof_ports,
                                                     span<ci8_t>                    data,
                                                     span<const uint8_t>            ports,
                                                     const precoding_configuration& precoding)
  {
    // Create resource grid.
    std::unique_ptr<resource_grid> grid = rg_factory->create(nof_ports, MAX_NSYMB_PER_SLOT, nof_re);
    grid->set_all_zero();

    // Map data to the resource grid.
    resource_grid_mapper::symbol_buffer_adapter buffer(data);
    mapper->map(grid->get_writer(), buffer, allocation, reserved, ports, precoding);

    return grid;
  }

  /// Apply the beam weights to the MIMO-precoded resource grid. Accumulate the magnitude of the per-beam contributions,
  /// which will be used to scale the error tolerance, to account for cbf16-quantization errors in the resource grid.
  void apply_beamforming(std::unique_ptr<resource_grid> grid)
  {
    std::vector<cf_t>           beam_re(nof_re);
    const resource_grid_reader& reader = grid->get_reader();
    for (unsigned i_beam = 0; i_beam != reader.get_nof_ports(); ++i_beam) {
      // Only the beams actually written by the MIMO precoding carry data; each occupied beam is combined once.
      if (reader.is_empty(i_beam)) {
        continue;
      }
      reader.get(beam_re, i_beam, i_symbol, 0, 1);
      for (unsigned i_port = 0; i_port != nof_antenna_ports; ++i_port) {
        cf_t       weight = beam_codebook.get_coefficient(to_beam_id(i_beam), i_port);
        span<cf_t> out    = beamformed.get_slice(i_port);
        for (unsigned i_re = 0; i_re != nof_re; ++i_re) {
          out[i_re] += weight * beam_re[i_re];
          error_scale[i_port][i_re] += std::abs(weight) * std::abs(beam_re[i_re]);
        }
      }
    }
  }

  /// Compare two resource grids allowing a tolerance margin for 16-bit brain-float quantization. Assert if the
  /// difference exceed the threshold for any RE.
  void compare_resource_grids(std::unique_ptr<resource_grid> beam_grid, std::unique_ptr<resource_grid> ref_grid)
  {
    // Buffer to hold reference REs.
    std::vector<cf_t> ref_re(nof_re);

    // Compare each reference resource grid port with the beamformed equivalent.
    for (unsigned i_port = 0; i_port != nof_antenna_ports; ++i_port) {
      // Reference REs for the antenna port.
      ref_grid->get_reader().get(ref_re, i_port, i_symbol, 0, 1);
      // Beamformed REs for the antenna port.
      span<const cf_t> got = beamformed.get_slice(i_port);

      for (unsigned i_re = 0; i_re != nof_re; ++i_re) {
        // Tolerance accounts for the brain-float (cbf16) storage of both resource grids, scaled by the accumulated
        // magnitude of the beam contributions. Bf16 keeps 8 significant bits, allow some margin.
        static constexpr float bf16_rel_error = 2.0F / 256.0F;
        float                  tolerance      = error_scale[i_port][i_re] * bf16_rel_error + 1e-2F;
        ASSERT_NEAR(got[i_re].real(), ref_re[i_re].real(), tolerance);
        ASSERT_NEAR(got[i_re].imag(), ref_re[i_re].imag(), tolerance);
      }
    }
  }
};

} // namespace

std::shared_ptr<channel_precoder_factory>     PrecodingMatrixResourceGridFixture::precoder_factory = nullptr;
std::shared_ptr<resource_grid_mapper_factory> PrecodingMatrixResourceGridFixture::mapper_factory   = nullptr;
std::shared_ptr<resource_grid_factory>        PrecodingMatrixResourceGridFixture::rg_factory       = nullptr;
std::unique_ptr<resource_grid_mapper>         PrecodingMatrixResourceGridFixture::mapper           = nullptr;

TEST_P(PrecodingMatrixResourceGridFixture, PrecodingMatrixResourceGrid)
{
  // Extract the MIMO matrix from the test Precoding Matrix Indicator.
  const precoding_configuration mimo = precoding_configuration::make_wideband(mimo_weights);
  // The number of ports of the precoding matrix must equal the number of beams allocated in the PMI.
  ASSERT_EQ(mimo.get_nof_ports(), beam_list.size());

  // Generate random data for each layer.
  std::vector<ci8_t> data = generate_random_data(nof_re * nof_layers);

  // Get the resource grid port list from the beam list.
  static_vector<uint8_t, 2 * max_nof_layers> beam_ports = beam_list_to_ports(beam_list);

  // Generate the MIMO-precoded resource grid and map data onto it.
  unsigned                       total_nof_beams = get_total_nof_beams(topology);
  std::unique_ptr<resource_grid> beam_grid       = create_grid_and_map(total_nof_beams, data, beam_ports, mimo);

  // Apply beamforming to the resource grid.
  apply_beamforming(std::move(beam_grid));

  // Apply layer mapping and precoding with the reference, compact, precoding matrix.
  std::unique_ptr<resource_grid> ref_grid = create_grid_and_map(nof_antenna_ports, data, antenna_ports, reference);

  // Ensure that the result matches the reference precoding matrix.
  compare_resource_grids(std::move(beam_grid), std::move(ref_grid));
}

static constexpr std::array<antenna_topology, 2> topologies = {antenna_topology::single_panel_two_one,
                                                               antenna_topology::single_panel_four_one};

static constexpr std::array<pmi_codebook_typeI_single_panel, 2> panels = {
    pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::two_one, pmi_codebook_typeI_mode::one},
    pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::four_one, pmi_codebook_typeI_mode::one}};

static const std::vector<test_case_t> test_cases = generate_precoding_matrix_test_cases(topologies, panels);

INSTANTIATE_TEST_SUITE_P(PrecodingMatrixResourceGridTest,
                         PrecodingMatrixResourceGridFixture,
                         ::testing::ValuesIn(test_cases));
