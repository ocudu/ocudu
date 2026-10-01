// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "evm_calculator_generic_impl.h"
#include "ocudu/ocuduvec/bit.h"
#include "ocudu/ocuduvec/dot_prod.h"
#include "ocudu/ocuduvec/subtract.h"

using namespace ocudu;

float evm_calculator_generic_impl::calculate(span<const log_likelihood_ratio> soft_bits,
                                             span<const cf_t>                 symbols,
                                             modulation_scheme                modulation)
{
  // Get modulation order.
  unsigned bits_per_symbol = get_bits_per_symbol(modulation);

  // Verify that soft bits and symbols dimensions are consistent.
  ocudu_assert(soft_bits.size() == symbols.size() * bits_per_symbol,
               "The number of soft bits (i.e., {}) is not consistent with the number of symbols (i.e., {}) and "
               "modulation (i.e., {}).",
               soft_bits.size(),
               symbols.size(),
               to_string(modulation));

  unsigned nof_symbols = symbols.size();
  float    avg_power   = 0.0;

  while (!soft_bits.empty()) {
    unsigned block_nof_symbols =
        std::min(static_cast<unsigned>(symbols.size()), static_cast<unsigned>(MAX_NOF_SYMBOLS));
    unsigned block_nof_bits = block_nof_symbols * bits_per_symbol;

    // Perform hard-decision.
    bit_buffer hard_bits = temp_hard_bits.first(block_nof_bits);
    hard_decision(hard_bits, soft_bits.first(block_nof_bits));

    // Modulate.
    span<cf_t> modulated = span<cf_t>(temp_modulated).first(block_nof_symbols);
    modulator->modulate(modulated, hard_bits, modulation);

    // Calculate EVM.
    ocuduvec::subtract(modulated, symbols.first(block_nof_symbols), modulated);

    // Accumulate power.
    avg_power += std::real(ocuduvec::dot_prod(modulated, modulated));

    // Pop bits and symbols.
    symbols   = symbols.last(symbols.size() - block_nof_symbols);
    soft_bits = soft_bits.last(soft_bits.size() - block_nof_bits);
  }

  return std::sqrt(avg_power / static_cast<float>(nof_symbols));
}
