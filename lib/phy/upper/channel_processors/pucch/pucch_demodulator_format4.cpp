// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief PUCCH Format 4 demodulator definition.

#include "pucch_demodulator_format4.h"
#include "ocudu/adt/format.h"
#include "ocudu/ocuduvec/sc_prod.h"
#include "ocudu/phy/support/mask_types.h"
#include "ocudu/phy/support/resource_grid_reader.h"
#include "ocudu/phy/upper/pucch_formats3_4_helpers.h"
#include "ocudu/phy/upper/pucch_orthogonal_sequence.h"

using namespace ocudu;

void pucch_demodulator_format4::demodulate(span<log_likelihood_ratio>                      llr,
                                           const resource_grid_reader&                     grid,
                                           const channel_estimate&                         estimates,
                                           const pucch_demodulator::format4_configuration& config)
{
  // PUCCH Format 4 modulation scheme can be QPSK or pi/2-BPSK, as per TS38.211 Section 6.3.2.6.2.
  modulation_scheme mod_scheme = config.pi2_bpsk ? modulation_scheme::PI_2_BPSK : modulation_scheme::QPSK;

  // Get a boolean mask of the OFDM symbols carrying DM-RS.
  symbol_slot_mask dmrs_symb_mask = get_pucch_formats3_4_dmrs_symbol_mask(
      config.nof_symbols, config.second_hop_prb.has_value(), config.additional_dmrs);

  // Number of REs per OFDM symbol. PUCCH Format 4 only gets a PRB.
  unsigned nof_re_symb = NOF_SUBCARRIERS_PER_RB;

  // Number of data Resource Elements in a slot for a single Rx port.
  unsigned nof_re_port = (config.nof_symbols - dmrs_symb_mask.count()) * nof_re_symb;

  // Assert that allocations are valid.
  ocudu_assert(config.first_prb * NOF_SUBCARRIERS_PER_RB <= grid.get_nof_subc(),
               "PUCCH Format 4: PRB allocation outside grid (first hop). Requested {}, grid has {} PRBs.",
               config.first_prb,
               grid.get_nof_subc() / NOF_SUBCARRIERS_PER_RB);

  ocudu_assert(!config.second_hop_prb.has_value() ||
                   (*config.second_hop_prb * NOF_SUBCARRIERS_PER_RB <= grid.get_nof_subc()),
               "PUCCH Format 4: PRB allocation outside grid (second hop). Requested {}, grid has {} PRBs.",
               *config.second_hop_prb,
               grid.get_nof_subc() / NOF_SUBCARRIERS_PER_RB);

  static constexpr interval<unsigned, true> nof_symbols_range(pucch_constants::f4::MIN_NOF_SYMS,
                                                              pucch_constants::f4::MAX_NOF_SYMS);
  ocudu_assert(nof_symbols_range.contains(config.nof_symbols),
               "Invalid Number of OFDM symbols allocated to PUCCH Format 4, i.e., {}. Valid range is {}.",
               config.nof_symbols,
               nof_symbols_range);

  // Resize equalized data and post equalization noise variance buffers.
  eq_re.resize(nof_re_port);
  eq_noise_vars.resize(nof_re_port);

  // Assert that the number of RE returned by the channel equalizer matches the expected number of LLR.
  ocudu_assert(eq_re.size() / config.occ_length == llr.size() / get_bits_per_symbol(mod_scheme),
               "Number of equalized REs (i.e. {}) does not match the expected LLR data length (i.e. {})",
               eq_re.size() / config.occ_length,
               llr.size() / get_bits_per_symbol(mod_scheme));

  pucch_3_4_extract_and_equalize(eq_re,
                                 eq_noise_vars,
                                 *equalizer,
                                 *precoder,
                                 grid,
                                 estimates,
                                 dmrs_symb_mask,
                                 config.start_symbol_index,
                                 config.nof_symbols,
                                 1,
                                 config.first_prb,
                                 config.second_hop_prb,
                                 config.rx_ports);

  // Create vectors to hold the output of the inverse blockwise spreading.
  static_vector<cf_t, pucch_constants::MAX_NOF_RE>  original(eq_re.size() / config.occ_length, cf_t());
  static_vector<float, pucch_constants::MAX_NOF_RE> noise_vars(original.size(), 0.0f);

  // Inverse block-wise spreading, as per TS38.211 Section 6.3.2.6.3.
  inverse_blockwise_spreading(original, noise_vars, eq_re, eq_noise_vars, config);

  // Apply soft symbol demodulation.
  demapper->demodulate_soft(llr, original, noise_vars, mod_scheme);

  // Descramble, as per TS38.211 Section 6.3.2.6.1.
  unsigned c_init = (config.rnti * pow2(15)) + config.n_id;
  descrambler->init(c_init);
  descrambler->apply_xor(llr, llr);
}

void pucch_demodulator_format4::inverse_blockwise_spreading(span<cf_t>        original,
                                                            span<float>       noise_vars,
                                                            span<const cf_t>  eq_re,
                                                            span<const float> eq_noise_vars,
                                                            const pucch_demodulator::format4_configuration& config)
{
  unsigned         mod = 12 / config.occ_length;
  span<const cf_t> wn  = pucch_orthogonal_sequence_format4::get_sequence(config.occ_length, config.occ_index);

  for (unsigned k = 0; k != NOF_SUBCARRIERS_PER_RB; ++k) {
    for (unsigned l = 0, l_end = eq_re.size() / NOF_SUBCARRIERS_PER_RB; l != l_end; ++l) {
      unsigned original_index = (l * mod) + (k % mod);
      unsigned spread_index   = (l * 12) + k;
      original[original_index] += eq_re[spread_index] / wn[k];
      noise_vars[original_index] += eq_noise_vars[spread_index];
    }
  }

  // Scale according to the spreading factor.
  ocuduvec::sc_prod(original, original, 1.0F / config.occ_length);
}
