// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_modulation/evm_calculator.h"
#include "ocudu/phy/upper/channel_modulation/modulation_mapper.h"

namespace ocudu {

/// Generic software implementation of the EVM calculator interface.
class evm_calculator_generic_impl : public evm_calculator
{
public:
  /// Default constructor.
  evm_calculator_generic_impl(std::unique_ptr<modulation_mapper> modulator_) :
    modulator(std::move(modulator_)), temp_hard_bits(MAX_NOF_BITS), temp_modulated(MAX_NOF_SYMBOLS)
  {
    ocudu_assert(modulator, "Invalid modulator.");
  }

  // See interface for documentation.
  float calculate(span<const log_likelihood_ratio> soft_bits,
                  span<const cf_t>                 symbols,
                  modulation_scheme                modulation) override;

private:
  /// Maximum processing block size in number of symbols.
  static constexpr unsigned MAX_NOF_SYMBOLS = 4096;
  /// Maximum processing block size in number of bits.
  static constexpr unsigned MAX_NOF_BITS = MAX_NOF_SYMBOLS * 8;
  /// Internal modulator.
  std::unique_ptr<modulation_mapper> modulator;
  /// Data after hard-decision.
  dynamic_bit_buffer temp_hard_bits;
  /// Modulated data.
  std::vector<cf_t> temp_modulated;
};

} // namespace ocudu
