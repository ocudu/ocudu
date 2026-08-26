// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdcch_modulator_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/ocuduvec/bit.h"
#include "ocudu/ocuduvec/sc_prod.h"
#include "ocudu/phy/support/re_pattern.h"
#include "ocudu/phy/support/resource_grid_mapper.h"
#include "ocudu/ran/beamforming/beam_identifier_helpers.h"
#include "ocudu/ran/precoding/precoding_codebooks.h"

using namespace ocudu;

void pdcch_modulator_impl::scramble(span<uint8_t> b_hat, span<const uint8_t> b, const pdcch_modulator::config_t& config)
{
  // Calculate initial sequence state.
  unsigned c_init = ((config.n_rnti << 16U) + config.n_id) % (1U << 31U);

  // Initialize the scrambler with the initial state.
  scrambler->init(c_init);

  // Apply sequence as unpacked XOR.
  scrambler->apply_xor(b_hat, b);
}

void pdcch_modulator_impl::modulate(span<cf_t> d_pdcch, span<const uint8_t> b_hat, float scaling)
{
  // Adapt the bits for the modulator.
  static_bit_buffer<MAX_BITS> b_hat_packed(b_hat.size());
  ocuduvec::bit_pack(b_hat_packed, b_hat);

  // Modulate as QPSK.
  modulator->modulate(d_pdcch, b_hat_packed, modulation_scheme::QPSK);

  // Apply scaling to conform power.
  if (std::isnormal(scaling)) {
    ocuduvec::sc_prod(d_pdcch, d_pdcch, scaling);
  }
}

void pdcch_modulator_impl::map(resource_grid_writer& grid, const re_buffer_reader<>& d_pdcch, const config_t& config)
{
  // Resource element allocation within a resource block for PDCCH.
  static constexpr re_prb_mask re_mask = {true, false, true, true, true, false, true, true, true, false, true, true};

  // Create PDCCH mapping pattern.
  re_pattern pattern;
  pattern.crb_mask = config.rb_mask;
  pattern.symbols.fill(config.start_symbol_index, config.start_symbol_index + config.duration);
  pattern.re_mask = re_mask;

  // Actual mapping.
  mapper->map(grid, d_pdcch, pattern, to_precoding_beamforming_configuration(config.precoding));
}

void pdcch_modulator_impl::modulate(resource_grid_writer&            grid,
                                    span<const uint8_t>              data,
                                    const pdcch_modulator::config_t& config)
{
  std::array<uint8_t, MAX_BITS> temp_b_hat;

  // Apply scrambling.
  span<uint8_t> b_hat = span<uint8_t>(temp_b_hat).first(data.size());
  scramble(b_hat, data, config);

  // Apply modulation mapping.
  static_re_buffer<1, MAX_RE> d_pdcch(1, data.size() / 2);
  modulate(d_pdcch.get_slice(0), b_hat, config.scaling);

  // Map to resource elements.
  map(grid, d_pdcch, config);
}
