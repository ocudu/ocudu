// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../../generic_functions/dft_processor_test_doubles.h"
#include "../../support/resource_grid_test_doubles.h"
#include "ocudu/adt/format.h"
#include "ocudu/ocuduvec/compare.h"
#include "ocudu/ocuduvec/conversion.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/ocuduvec/sc_prod.h"
#include "ocudu/phy/antenna_ports.h"
#include "ocudu/phy/lower/modulation/modulation_factories.h"
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;

TEST(ofdm_demodulator_unittest, demodulate)
{
  std::mt19937                            rgen(0);
  std::uniform_real_distribution<float>   dist_rg(-1, 1);
  std::uniform_int_distribution<unsigned> dist_port(0, MAX_PORTS - 1);

  // Create DFT factory spy.
  std::shared_ptr<dft_processor_factory_spy> dft_factory = std::make_shared<dft_processor_factory_spy>();
  ASSERT_TRUE(dft_factory);

  // Create OFDM demodulator factory.
  ofdm_factory_generic_configuration ofdm_common_config;
  ofdm_common_config.dft_factory                         = dft_factory;
  std::shared_ptr<ofdm_demodulator_factory> ofdm_factory = create_ofdm_demodulator_factory_generic(ofdm_common_config);
  ASSERT_TRUE(ofdm_factory);

  // Iterate all possible numerologies.
  for (subcarrier_spacing scs : {subcarrier_spacing::kHz15,
                                 subcarrier_spacing::kHz30,
                                 subcarrier_spacing::kHz60,
                                 subcarrier_spacing::kHz120,
                                 subcarrier_spacing::kHz240}) {
    // Iterate all possible FFT sizes.
    for (unsigned dft_size : {256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096, 6144, 8192, 12288}) {
      // Skip combinations of SCS>15kHz with large DFT sizes.
      if (scs > subcarrier_spacing::kHz15 && dft_size > 4096) {
        continue;
      }

      // Deduce sampling rate from SCS and DFT size.
      unsigned sampling_rate_Hz = to_sampling_rate_Hz(scs, dft_size);

      // Iterate all possible cyclic prefix.
      for (cyclic_prefix cp : {cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED}) {
        // Skip invalid CP, numerology and DFT size combinations that invalid.
        if (!cp.is_valid(scs, dft_size)) {
          fmt::print("Unsupported cyclic prefix ({}), SCS ({} KHz) and DFT size ({}) combination. Skipping.\n",
                     cp.to_string().c_str(),
                     scs_to_khz(scs),
                     dft_size);
          continue;
        }

        // Reset spies.
        dft_factory->clear_entries();

        // Create OFDM demodulator configuration. Use minimum number of RB.
        ofdm_demodulator_configuration ofdm_config;
        ofdm_config.numerology                = to_numerology_value(scs);
        ofdm_config.bw_rb                     = 11;
        ofdm_config.dft_size                  = dft_size;
        ofdm_config.cp                        = cp;
        ofdm_config.nof_samples_window_offset = 0;
        ofdm_config.scale                     = dist_rg(rgen);
        ofdm_config.center_freq_Hz            = 0.0;

        unsigned nsubc = ofdm_config.bw_rb * NOF_SUBCARRIERS_PER_RB;

        // Create OFDM demodulator.
        std::unique_ptr<ofdm_slot_demodulator> ofdm = ofdm_factory->create_ofdm_slot_demodulator(ofdm_config);
        ASSERT_TRUE(ofdm != nullptr);

        // Check is a DFT processor is created and not used.
        auto& dft_processor_factory_entry = dft_factory->get_entries();
        ASSERT_TRUE(dft_processor_factory_entry.size() == 1);
        dft_processor_spy& dft = *dft_processor_factory_entry[0].dft;
        ASSERT_TRUE(dft.get_entries().empty());

        // Iterate all slots within a subframe.
        for (unsigned slot_idx = 0, nslot = get_nof_slots_per_subframe(scs); slot_idx != nslot; ++slot_idx) {
          // Select a random port.
          unsigned port_idx = dist_port(rgen);

          // Generate random time domain data.
          unsigned            nsymb = get_nsymb_per_slot(cp);
          std::vector<ci16_t> time_data;
          // Iterate all symbols in the slot.
          for (unsigned symbol_idx = 0; symbol_idx != nsymb; ++symbol_idx) {
            // Get the size of the current time-domain symbol.
            unsigned nsamples =
                cp.get_length(nsymb * slot_idx + symbol_idx, scs).to_samples(sampling_rate_Hz) + dft_size;
            for (unsigned sample_idx = 0; sample_idx != nsamples; ++sample_idx) {
              cf_t random_value = {dist_rg(rgen), dist_rg(rgen)};
              time_data.push_back(to_ci16(random_value * ocuduvec::scaling_factor_ci16_to_cf));
            }
          }

          // Reset DFT spy entries.
          dft.clear_entries();

          // Demodulate signal.
          resource_grid_writer_spy rg(MAX_PORTS, MAX_NSYMB_PER_SLOT, ofdm_config.bw_rb);
          ofdm->demodulate(rg, time_data, port_idx, slot_idx);

          // Check the number of calls to DFT processor match with the number of symbols.
          ASSERT_TRUE(dft.get_entries().size() == nsymb);

          // Iterate all symbols.
          std::vector<resource_grid_writer_spy::expected_entry_t> expected_rg;
          unsigned                                                offset      = 0;
          auto                                                    dft_entries = dft.get_entries();
          for (unsigned symbol_idx = 0; symbol_idx != nsymb; ++symbol_idx) {
            // Get the size of the current time-domain symbol.
            unsigned symb_size =
                cp.get_length(nsymb * slot_idx + symbol_idx, scs).to_samples(sampling_rate_Hz) + dft_size;

            // Get input time data.
            span<const ci16_t> time_data_symbol(&time_data[offset], symb_size);

            // Get DFT input.
            span<const cf_t> dft_input = dft_entries[symbol_idx].input;

            // Verify DFT input.
            std::vector<cf_t> expected_dft_input(dft_size);
            ocuduvec::convert(expected_dft_input, time_data_symbol.last(dft_size), ocuduvec::scaling_factor_ci16_to_cf);
            ASSERT_TRUE(ocuduvec::equal(expected_dft_input, dft_input.first(dft_size)));

            // Generate ideal frequency domain outputs.
            for (unsigned subc_idx = 0; subc_idx != nsubc; ++subc_idx) {
              resource_grid_writer_spy::expected_entry_t entry = {};
              entry.port                                       = port_idx;
              entry.symbol                                     = symbol_idx;
              entry.subcarrier                                 = subc_idx;
              if (subc_idx < nsubc / 2) {
                entry.value = dft_entries[symbol_idx].output[dft_size - (nsubc / 2) + subc_idx] * ofdm_config.scale;
              } else {
                entry.value = dft_entries[symbol_idx].output[subc_idx - (nsubc / 2)] * ofdm_config.scale;
              }
              expected_rg.push_back(entry);
            }

            // Increment OFDM symbol offset.
            offset += symb_size;
          }

          // Assert resource grid entries.
          rg.assert_entries(expected_rg);
        }
      }
    }
  }
}
