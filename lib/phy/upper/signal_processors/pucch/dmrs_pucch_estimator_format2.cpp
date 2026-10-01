// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "dmrs_pucch_estimator_format2.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/pucch/pucch_constants.h"

using namespace ocudu;

/// \brief DM-RS RE allocation pattern for PUCCH Format 2.
///
/// Indicates the Resource Elements containing DM-RS symbols within a PRB, as per TS38.211 Section 6.4.1.3.2.2.
static constexpr bounded_bitset<NOF_SUBCARRIERS_PER_RB> format2_prb_re_mask =
    {false, true, false, false, true, false, false, true, false, false, true, false};

unsigned dmrs_pucch_estimator_format2::c_init(unsigned                                           symbol,
                                              const dmrs_pucch_estimator::format2_configuration& config)
{
  uint64_t n_slot = config.slot.slot_index();
  uint64_t n_id   = config.n_id_0;

  return ((get_nsymb_per_slot(config.cp) * n_slot + symbol + 1) * (2 * n_id + 1) * pow2(17) + 2 * n_id) % pow2(31);
}

void dmrs_pucch_estimator_format2::generate_sequence(span<cf_t>                                         sequence,
                                                     unsigned                                           symbol,
                                                     unsigned                                           start_prb,
                                                     const dmrs_pucch_estimator::format2_configuration& config)
{
  // Initialize pseudo-random generator.
  prg->init(c_init(symbol, config));

  // Discard unused pilots.
  prg->advance(start_prb * NOF_DMRS_PER_RB * 2);

  // Generate sequence.
  prg->generate(sequence, M_SQRT1_2);
}

dmrs_pucch_estimator_format2::layer_dmrs_pattern
dmrs_pucch_estimator_format2::generate_dmrs_pattern(const dmrs_pucch_estimator::format2_configuration& config,
                                                    unsigned                                           nof_prb_grid)
{
  layer_dmrs_pattern mask;

  // RE allocation pattern is always the same.
  mask.re_pattern = format2_prb_re_mask;

  // Set used PRB.
  mask.rb_mask.resize(nof_prb_grid);
  mask.rb_mask.fill(config.starting_prb, config.starting_prb + config.nof_prb, true);

  if (config.second_hop_prb.has_value()) {
    // Set second hop PRB allocation.
    mask.rb_mask2.resize(nof_prb_grid);
    mask.rb_mask2.fill(*config.second_hop_prb, *config.second_hop_prb + config.nof_prb, true);

    // Set the hopping symbol index, indicating the start of the second hop. Recall that PUCCH Format 2 occupies a
    // maximum of 2 OFDM symbols.
    mask.hopping_symbol_index = config.start_symbol_index + 1;
  }

  // Set used symbols.
  mask.symbols.resize(config.start_symbol_index + config.nof_symbols);
  mask.symbols.fill(config.start_symbol_index, config.start_symbol_index + config.nof_symbols, true);

  return mask;
}

void dmrs_pucch_estimator_format2::estimate(channel_estimate&                                  estimate,
                                            const resource_grid_reader&                        grid,
                                            const dmrs_pucch_estimator::format2_configuration& config)
{
  ocudu_assert((!config.second_hop_prb.has_value() || (config.nof_symbols == pucch_constants::f2::MAX_NOF_SYMS)),
               "Frequency hopping requires {} OFDM symbols.",
               pucch_constants::f2::MAX_NOF_SYMS);

  unsigned nof_rx_ports = config.ports.size();

  // Prepare DM-RS symbol buffer dimensions.
  re_measurement_dimensions dims;
  dims.nof_subc    = config.nof_prb * NOF_DMRS_PER_RB;
  dims.nof_symbols = config.nof_symbols;
  dims.nof_slices  = pucch_constants::NOF_LAYERS;

  // Resize DM-RS symbol buffer.
  temp_symbols.resize(dims);

  unsigned i_symb_start = config.start_symbol_index;
  unsigned i_symb_end   = config.start_symbol_index + dims.nof_symbols;
  // For each symbol carrying DM-RS (up to 2 symbols maximum).
  for (unsigned i_symb = i_symb_start, i_dmrs_symb = 0; i_symb != i_symb_end; ++i_symb, ++i_dmrs_symb) {
    // Generate sequence.
    unsigned prb_start = ((i_symb != i_symb_start) && (config.second_hop_prb.has_value())) ? *config.second_hop_prb
                                                                                           : config.starting_prb;
    generate_sequence(temp_symbols.get_symbol(i_dmrs_symb, 0), i_symb, prb_start, config);
  }

  // Prepare channel estimator configuration.
  port_channel_estimator::configuration est_cfg;
  est_cfg.scs          = to_subcarrier_spacing(config.slot.numerology());
  est_cfg.cp           = config.cp;
  est_cfg.first_symbol = config.start_symbol_index;
  est_cfg.nof_symbols  = config.nof_symbols;
  est_cfg.dmrs_pattern.assign(1, generate_dmrs_pattern(config, estimate.size().nof_prb));
  est_cfg.rx_ports = config.ports;
  est_cfg.scaling  = 1.0F;

  // Perform estimation for each receive port.
  for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
    const port_channel_estimator_results& ch_est_results = ch_estimator->compute(grid, i_port, temp_symbols, est_cfg);

    for (unsigned i_symbol = est_cfg.first_symbol, last_symbol = est_cfg.first_symbol + est_cfg.nof_symbols;
         i_symbol != last_symbol;
         ++i_symbol) {
      const layer_dmrs_pattern& dmrs_pattern = est_cfg.dmrs_pattern[0];
      const crb_bitmap&         rb_mask =
          ((dmrs_pattern.hopping_symbol_index.has_value()) && (i_symbol >= *dmrs_pattern.hopping_symbol_index))
                      ? dmrs_pattern.rb_mask2
                      : dmrs_pattern.rb_mask;
      unsigned      first_re        = rb_mask.find_lowest() * NOF_SUBCARRIERS_PER_RB;
      unsigned      nof_re          = rb_mask.count() * NOF_SUBCARRIERS_PER_RB;
      span<cbf16_t> estimate_values = estimate.get_symbol_ch_estimate(i_symbol, i_port).subspan(first_re, nof_re);
      ch_est_results.get_symbol_ch_estimate(estimate_values, i_symbol, /*tx_layer=*/0);
    }
    estimate.set_rsrp(ch_est_results.get_rsrp(/*tx_layer=*/0), i_port);
    estimate.set_time_alignment(ch_est_results.get_time_alignment(), i_port);
    estimate.set_cfo_Hz(ch_est_results.get_cfo_Hz(), i_port);
    estimate.set_epre(ch_est_results.get_epre(), i_port);
    estimate.set_noise_variance(ch_est_results.get_noise_variance(), i_port);
    estimate.set_snr(ch_est_results.get_snr(), i_port);
  }
}
