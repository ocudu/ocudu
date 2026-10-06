// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../../generic_functions/dft_processor_test_doubles.h"
#include "../../support/resource_grid_test_doubles.h"
#include "support/compare_sequences.h"
#include "ocudu/adt/format.h"
#include "ocudu/adt/to_array.h"
#include "ocudu/ocuduvec/conversion.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/ocuduvec/sc_prod.h"
#include "ocudu/phy/lower/modulation/modulation_factories.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/ocudu_assert.h"
#include "fmt/ostream.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <limits>
#include <random>

using namespace ocudu;

/// Testing number of ports.
static constexpr unsigned nof_ports = 4;
/// Alias for port weights.
using port_weight_list = std::array<cf_t, nof_ports>;
/// Alias for port allocation list.
using port_allocation_list = static_vector<unsigned, nof_ports>;

/// Test parameters describing an SCS / DFT size / cyclic prefix combination.
struct ofdm_mod_params {
  /// Subcarrier spacing.
  subcarrier_spacing scs;
  /// DFT size.
  unsigned dft_size;
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Modulation port weights.
  port_weight_list port_weights;
  /// Indicates the ports that shall be filled with data.
  port_allocation_list allocated_ports;
  /// Center frequency in hertz.
  double center_freq_Hz;
};

/// Print test parameters in test output.
static std::ostream& operator<<(std::ostream& os, const ofdm_mod_params& p)
{
  fmt::print(os,
             "scs={} dft_size={} cp={} weights=[{}] ports=[{}] center_freq={:.3f}GHz",
             to_string(p.scs),
             p.dft_size,
             p.cp.to_string(),
             span<const cf_t>(p.port_weights),
             p.allocated_ports,
             p.center_freq_Hz / 1e9);
  return os;
}

class OfdmModulatorFixture : public ::testing::TestWithParam<ofdm_mod_params>
{
protected:
  /// Fix odd number of resource blocks.
  static constexpr unsigned nsubc = 11 * NOF_SUBCARRIERS_PER_RB;
  /// Fix scaling factor different than one.
  static constexpr float scale = M_SQRT1_2;
  /// Maximum allowed error at the DFT input.
  static constexpr float ASSERT_MAX_ERROR = 1e-6f;
  /// Maximum allowed error at the OFDM modulator output, in units of the least significant bit of the 16-bit integer.
  static constexpr float ASSERT_MAX_ERROR_LSB = 1.0f;

  static void SetUpTestCase()
  {
    // Create DFT factory and OFDM modulator factory once for all tests.
    dft_factory = std::make_shared<dft_processor_factory_spy>();
    report_fatal_error_if_not(dft_factory != nullptr, "DFT factory is null");

    ofdm_factory_generic_configuration ofdm_common_config;
    ofdm_common_config.dft_factory = dft_factory;

    ofdm_mod_factory = create_ofdm_modulator_factory_generic(ofdm_common_config);
    report_fatal_error_if_not(ofdm_mod_factory != nullptr, "OFDM modulator factory is null");
  }

  void SetUp() override
  {
    const ofdm_mod_params& params = GetParam();
    scs                           = params.scs;
    dft_size                      = params.dft_size;
    cp                            = params.cp;
    center_freq_Hz                = params.center_freq_Hz;
    sampling_rate_Hz              = to_sampling_rate_Hz(scs, dft_size);

    // Reset spies.
    dft_factory->clear_entries();

    nof_symbols_per_slot = get_nsymb_per_slot(cp);
    nof_slots_per_sf     = get_nof_slots_per_subframe(scs);

    ofdm_modulator_configuration ofdm_config = {.numerology     = to_numerology_value(scs),
                                                .bw_rb          = static_cast<unsigned>(nsubc / NOF_SUBCARRIERS_PER_RB),
                                                .dft_size       = dft_size,
                                                .cp             = cp,
                                                .scale          = scale,
                                                .center_freq_Hz = 0.0};

    // Create OFDM modulator.
    ofdm = ofdm_mod_factory->create_ofdm_slot_modulator(ofdm_config);
    ASSERT_NE(ofdm, nullptr);

    // Change center frequency.
    ofdm->set_center_frequency(center_freq_Hz);

    // Check that a DFT processor is created and not used yet.
    const auto& dft_factory_entries = dft_factory->get_entries();
    ASSERT_EQ(dft_factory_entries.size(), 1U);
    dft = dft_factory_entries[0].dft;
    ASSERT_TRUE(dft->get_entries().empty());
  }

  /// Generate resource grid for the given allocated ports.
  resource_grid_reader_spy generate_resource_grid(const port_allocation_list& port_allocations)
  {
    resource_grid_reader_spy rg(nof_ports, nof_symbols_per_slot, nsubc / NOF_SUBCARRIERS_PER_RB);
    for (unsigned i_port : port_allocations) {
      for (unsigned symbol_idx = 0; symbol_idx != nof_symbols_per_slot; ++symbol_idx) {
        for (unsigned subc_idx = 0; subc_idx != nsubc; ++subc_idx) {
          resource_grid_reader_spy::expected_entry_t entry = {};
          entry.port                                       = static_cast<uint8_t>(i_port);
          entry.symbol                                     = static_cast<uint8_t>(symbol_idx);
          entry.subcarrier                                 = static_cast<uint16_t>(subc_idx);
          entry.value                                      = {dist_rg(gen), dist_rg(gen)};
          rg.write(entry);
        }
      }
    }

    return rg;
  }

  /// Get the expected DFT input for a given resource grid, port weights, and OFDM symbol .
  std::vector<cf_t> get_expected_dft_input(const resource_grid_reader& rg,
                                           span<const cf_t>            port_weights,
                                           span<const unsigned>        port_allocations,
                                           unsigned                    i_symbol)
  {
    // Get data from RG and combine it using the port weights.
    std::vector<cf_t> rg_data_symbol(nsubc);
    std::vector<cf_t> combined_rg_data_symbol(nsubc);
    for (unsigned i_port : port_allocations) {
      // Get the resource elements for the OFDM symbol.
      rg.get(rg_data_symbol, i_port, i_symbol, 0);

      // Apply port weight and accumulate.
      std::transform(rg_data_symbol.begin(),
                     rg_data_symbol.end(),
                     combined_rg_data_symbol.begin(),
                     combined_rg_data_symbol.begin(),
                     [port_weight = port_weights[i_port]](cf_t rg_data, cf_t combined_rg_data) {
                       return rg_data * port_weight + combined_rg_data;
                     });
    }

    // Generate expected DFT input.
    std::vector<cf_t> expected_dft_input(dft_size);
    ocuduvec::zero(expected_dft_input);
    ocuduvec::copy(span<cf_t>(expected_dft_input).first(nsubc / 2),
                   span<cf_t>(combined_rg_data_symbol).last(nsubc / 2));
    ocuduvec::copy(span<cf_t>(expected_dft_input).last(nsubc / 2),
                   span<cf_t>(combined_rg_data_symbol).first(nsubc / 2));

    return expected_dft_input;
  }

  subcarrier_spacing scs;
  unsigned           dft_size;
  unsigned           sampling_rate_Hz;
  cyclic_prefix      cp;
  unsigned           nof_symbols_per_slot;
  unsigned           nof_slots_per_sf;
  double             center_freq_Hz;

  std::unique_ptr<ofdm_slot_modulator> ofdm;
  dft_processor_spy*                   dft = nullptr;

  static std::shared_ptr<dft_processor_factory_spy> dft_factory;
  static std::shared_ptr<ofdm_modulator_factory>    ofdm_mod_factory;

  // Random number generator and distributions for this test fixture.
  static std::mt19937                          gen;
  static std::uniform_real_distribution<float> dist_rg;
};

std::mt19937                               OfdmModulatorFixture::gen(0);
std::uniform_real_distribution<float>      OfdmModulatorFixture::dist_rg(-1.0f, 1.0f);
std::shared_ptr<dft_processor_factory_spy> OfdmModulatorFixture::dft_factory;
std::shared_ptr<ofdm_modulator_factory>    OfdmModulatorFixture::ofdm_mod_factory;

TEST_P(OfdmModulatorFixture, ModulatesCorrectly)
{
  const ofdm_mod_params& params = GetParam();

  for (unsigned i_slot = 0, offset_sf = 0; i_slot != nof_slots_per_sf; ++i_slot) {
    // Reset DFT spy entries before modulating this slot.
    dft->clear_entries();

    // Generate random data in the resource grid for the allocated ports.
    resource_grid_reader_spy rg = generate_resource_grid(params.allocated_ports);

    // Modulate signal.
    std::vector<ci16_t> output(ofdm->get_slot_size(i_slot));
    ofdm->modulate(output, rg, params.port_weights, i_slot);

    // No DFT is called if no allocated ports have ports weights.
    if (std::all_of(params.allocated_ports.begin(),
                    params.allocated_ports.end(),
                    [port_weights = params.port_weights](unsigned i_port) { return port_weights[i_port] == cf_t(); })) {
      // The output must be zero.
      ASSERT_TRUE(std::all_of(output.begin(), output.end(), [](ci16_t modulated) { return modulated == ci16_t(); }));
      continue;
    }

    // Check the number of calls to DFT processor match with the number of symbols.
    ASSERT_EQ(dft->get_entries().size(), static_cast<size_t>(nof_symbols_per_slot));

    // Copy DFT entries before they can be invalidated by clear_entries().
    std::vector dft_entries(dft->get_entries().begin(), dft->get_entries().end());

    // Iterate all symbols.
    unsigned offset_slot = 0;
    for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_slot; ++i_symbol) {
      // Calculate symbol index within the subframe.
      unsigned i_symbol_sf = nof_symbols_per_slot * i_slot + i_symbol;

      // Extract cyclic prefix length for this OFDM symbol.
      unsigned cp_len = cp.get_length(i_symbol_sf, scs).to_samples(sampling_rate_Hz);

      // Total OFDM symbol length.
      unsigned symbol_len = dft_size + cp_len;

      // Calculate initial OFDM symbol phase and scaling.
      cf_t amplitude_phase_compensation =
          cf_t(std::polar(static_cast<double>(scale),
                          -2.0 * M_PI * center_freq_Hz * (offset_sf + cp_len) / static_cast<double>(sampling_rate_Hz)));

      // Get the expected DFT input for this OFDM symbol.
      std::vector expected_dft_input =
          get_expected_dft_input(rg, params.port_weights, params.allocated_ports, i_symbol);

      // Verify DFT input matches with the expected.
      {
        error_type<std::string> compare_result = compare_sequences(
            span<const cf_t>(dft_entries[i_symbol].input),
            span<const cf_t>(expected_dft_input),
            [length = static_cast<float>(dft_size)](const cf_t& actual, const cf_t& expected) {
              return std::abs(actual - expected) / std::sqrt(length);
            },
            ASSERT_MAX_ERROR);
        ASSERT_TRUE(compare_result.has_value()) << compare_result.error();
      }

      // Generate expected time domain output, scaled to the 16-bit integer full scale.
      std::vector<cf_t> expected_output_data(dft_size + cp_len);
      span<cf_t>        expected_output = expected_output_data;
      ocuduvec::sc_prod(expected_output.last(dft_size),
                        dft_entries[i_symbol].output,
                        amplitude_phase_compensation * ocuduvec::scaling_factor_cf_to_ci16);
      ocuduvec::copy(expected_output.first(cp_len), expected_output.last(cp_len));

      // Select a view of the OFDM modulator output.
      span<const ci16_t> output_symbol(output);
      output_symbol = output_symbol.subspan(offset_slot, dft_size + cp_len);

      // Assert generated symbol matches ideal, allowing the rounding and saturation of the 16-bit integer conversion.
      {
        error_type<std::string> compare_result = compare_sequences(
            output_symbol,
            span<const cf_t>(expected_output),
            [](const ci16_t& actual, const cf_t& expected) {
              auto saturate = [](float value) {
                return std::clamp(value,
                                  static_cast<float>(std::numeric_limits<int16_t>::min()),
                                  static_cast<float>(std::numeric_limits<int16_t>::max()));
              };
              return std::max(std::abs(static_cast<float>(actual.real()) - saturate(expected.real())),
                              std::abs(static_cast<float>(actual.imag()) - saturate(expected.imag())));
            },
            ASSERT_MAX_ERROR_LSB);
        ASSERT_TRUE(compare_result.has_value()) << compare_result.error();
      }

      // Increment OFDM symbol offset.
      offset_slot += symbol_len;
      offset_sf += symbol_len;
    }
  }
}

/// Meaningful port weight combinations for a complete OFDM modulator line coverage.
static constexpr auto port_weight_combinations      = to_array<port_weight_list>({{0, 1, 0, 0}, {0.5, 0.1j, 0, 0}});
static const auto     port_allocations_combinations = to_array<static_vector<unsigned, nof_ports>>({{0}, {0, 1}});

/// Pre-compute all valid parameter combinations.
static std::vector<ofdm_mod_params> generate_params()
{
  std::vector<ofdm_mod_params> params;

  auto add_if_valid = [&](subcarrier_spacing          scs,
                          unsigned                    dft_size,
                          cyclic_prefix               cp,
                          const port_weight_list&     port_weights,
                          const port_allocation_list& port_allocations,
                          double                      center_freq_Hz) {
    // Extended cyclic prefix is only supported with 60kHz.
    if ((cp == cyclic_prefix::EXTENDED) && (scs != subcarrier_spacing::kHz60)) {
      return;
    }

    if (cp.is_valid(scs, dft_size)) {
      params.push_back({scs, dft_size, cp, port_weights, port_allocations, center_freq_Hz});
    }
  };

  for (subcarrier_spacing scs : {subcarrier_spacing::kHz15,
                                 subcarrier_spacing::kHz30,
                                 subcarrier_spacing::kHz60,
                                 subcarrier_spacing::kHz120,
                                 subcarrier_spacing::kHz240}) {
    for (unsigned dft_size : {256, 512}) {
      for (cyclic_prefix cp : {cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED}) {
        for (const auto& port_weights : port_weight_combinations) {
          for (const auto& port_allocations : port_allocations_combinations) {
            for (double center_freq_Hz : {1.0e9, 3.5e9}) {
              add_if_valid(scs, dft_size, cp, port_weights, port_allocations, center_freq_Hz);
            }
          }
        }
      }
    }
  }

  return params;
}

INSTANTIATE_TEST_SUITE_P(OfdmModulator, OfdmModulatorFixture, ::testing::ValuesIn(generate_params()));
