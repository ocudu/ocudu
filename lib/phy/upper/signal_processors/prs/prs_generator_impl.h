// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/antenna_ports.h"
#include "ocudu/phy/generic_functions/precoding/channel_precoder.h"
#include "ocudu/phy/support/re_buffer.h"
#include "ocudu/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "ocudu/phy/upper/signal_processors/prs/prs_generator.h"
#include "ocudu/ran/prs/prs.h"
#include "ocudu/ran/resource_block.h"
#include <memory>

namespace ocudu {

/// Generic PRS generator implementation.
class prs_generator_impl : public prs_generator
{
public:
  /// \brief Creates a PRS generator.
  /// \param[in] prg_      Pseudo-random sequence generator.
  /// \param[in] precoder_ Channel precoder.
  prs_generator_impl(std::unique_ptr<pseudo_random_generator> prg_, std::unique_ptr<channel_precoder> precoder_) :
    prg(std::move(prg_)), precoder(std::move(precoder_))
  {
    ocudu_assert(prg, "Invalid pseudo-random sequence generator.");
    ocudu_assert(precoder, "Invalid channel precoder.");
  }

  // See interface for documentation.
  void generate(resource_grid_writer& grid, const prs_generator_configuration& config) override;

private:
  /// \brief Generates a PRS sequence according to TS38.211 Section 7.4.1.7.2.
  /// \param[in] i_symbol Symbol index within the slot.
  /// \param[in] config   PRS transmission configuration.
  /// \return A view to the generated sequence.
  void generate(unsigned i_symbol, const prs_generator_configuration& config);

  /// \brief Maps a PRS sequence according to TS38.211 Section 4.4.1.7.3.
  /// \param[out] grid     Destination resource grid.
  /// \param[in ] i_symbol OFDM symbol within the slot.
  /// \param[in ] config   PRS transmission configuration.
  void map(resource_grid_writer& grid, unsigned i_symbol, const prs_generator_configuration& config);

  /// \brief Obtains the parameter \f$k'\f$ according to TS38.211 Table 7.4.1.7.3-1.
  /// \param[in] comb_size    Parameter \f$K_{comb}^{PRS}\f$.
  /// \param[in] start_symbol Parameter \f$l_{start}^{PRS}\f$.
  /// \param[in] i_symbol     OFDM symbol within the slot, parameter \f$l\f$.
  /// \return
  static uint8_t get_freq_offset(prs_comb_size comb_size, unsigned start_symbol, unsigned i_symbol);

  /// Maximum number of layers.
  static constexpr unsigned one_layer = 1;
  /// Random sequence generator.
  std::unique_ptr<pseudo_random_generator> prg;
  /// Channel precoder.
  std::unique_ptr<channel_precoder> precoder;
  /// Temporary generated sequence.
  static_re_buffer<one_layer, MAX_NOF_SUBCARRIERS, cf_t> sequence;
  /// Temporary precoded symbols.
  static_re_buffer<MAX_PORTS, MAX_NOF_SUBCARRIERS, cbf16_t> symbols;
};

} // namespace ocudu
