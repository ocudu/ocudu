// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_modulation/modulation_mapper.h"
#include "ocudu/phy/upper/channel_processors/ssb/pbch_modulator.h"
#include "ocudu/phy/upper/sequence_generators/pseudo_random_generator.h"

namespace ocudu {

/// PBCH modulator implementation.
class pbch_modulator_impl : public pbch_modulator
{
  std::unique_ptr<modulation_mapper>       modulator;
  std::unique_ptr<pseudo_random_generator> scrambler;

  /// Implements TS38.211 Section 7.3.3.1 Scrambling
  /// \param [in] b Inputs bits to scramble
  /// \param [out] b_hat Output bits after scrambling
  /// \param [in] config PBCH modulator arguments
  void scramble(span<const uint8_t> b, span<uint8_t> b_hat, const config_t& config);

  /// Implements TS38.211 Section 7.3.3.2 Modulation
  /// \param [in] b_hat Inputs bits to scramble
  /// \param [out] d_pbch Output symbols
  void modulate(span<const uint8_t> b_hat, span<cf_t> d_pbch);

  /// Implements TS38.211 Section 7.3.3.3 Mapping to physical resources
  /// \param [in] d_pbch Provides the symbols to map
  /// \param [in, out] grid is the destination resource grid
  /// \param [in] config PBCH modulator arguments
  void map(span<const cf_t> d_pbch, resource_grid_writer& grid, const config_t& config);

public:
  pbch_modulator_impl(std::unique_ptr<modulation_mapper> modulator_, std::unique_ptr<pseudo_random_generator> prg_) :
    modulator(std::move(modulator_)), scrambler(std::move(prg_))
  {
    ocudu_assert(modulator, "Invalid modulator.");
    ocudu_assert(scrambler, "Invalid scrambler.");
  }

  void put(span<const uint8_t> bits, resource_grid_writer& grid, const config_t& config) override;
};

} // namespace ocudu
