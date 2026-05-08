// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pbch_modulator_impl.h"
#include "ocudu/ocuduvec/bit.h"
#include "ocudu/phy/support/resource_grid_writer.h"
#include "ocudu/ran/ssb/ssb_properties.h"

using namespace ocudu;

void pbch_modulator_impl::scramble(span<const uint8_t> b, span<uint8_t> b_hat, const config_t& config)
{
  // Initialize sequence.
  scrambler->init(config.phys_cell_id);

  // Advance sequence.
  scrambler->advance((config.ssb_idx.value() & 0x7) * M_bit);

  // Apply XOR.
  scrambler->apply_xor(b_hat, b);
}

void pbch_modulator_impl::modulate(span<const uint8_t> b_hat, span<cf_t> d_pbch)
{
  // Adapt the bits for the modulator.
  static_bit_buffer<M_bit> b_hat_packed(b_hat.size());
  ocuduvec::bit_pack(b_hat_packed, b_hat);

  // Modulate as QPSK.
  modulator->modulate(d_pbch, b_hat_packed, modulation_scheme::QPSK);
}

void pbch_modulator_impl::map(span<const cf_t> d_pbch, resource_grid_writer& grid, const config_t& config)
{
  unsigned l0 = config.ssb_first_symbol;
  unsigned k0 = config.ssb_first_subcarrier;

  // Calculate DM-RS index shift.
  uint32_t v = config.phys_cell_id % 4;

  // Resource element mask within each resource block. Remove DM-RS from the mask.
  bounded_bitset<NOF_SUBCARRIERS_PER_RB> re_mask = ~bounded_bitset<NOF_SUBCARRIERS_PER_RB>(NOF_SUBCARRIERS_PER_RB);
  re_mask.reset(v);
  re_mask.reset(v + 4);
  re_mask.reset(v + 8);

  // Full SS/PBCH block bandwidth resource blocks.
  bounded_bitset<MAX_NOF_PRBS>        rb_full_mask = ~bounded_bitset<MAX_NOF_PRBS>(NOF_SSB_PRBS);
  bounded_bitset<MAX_NOF_SUBCARRIERS> full_mask    = rb_full_mask.kronecker_product<NOF_SUBCARRIERS_PER_RB>(re_mask);

  // Edges SS/PBCH block bandwidth resource blocks.
  bounded_bitset<MAX_NOF_PRBS> rb_edge_mask(NOF_SSB_PRBS);
  rb_edge_mask.fill(0, 4);
  rb_edge_mask.fill(16, 20);
  bounded_bitset<MAX_NOF_SUBCARRIERS> edge_mask = rb_edge_mask.kronecker_product<NOF_SUBCARRIERS_PER_RB>(re_mask);

  // Map symbols for each of the ports.
  for (unsigned port : config.ports) {
    span<const cf_t> symbols = d_pbch;

    // Put sequence in symbol 1 (0, 1, ..., 239), skip the subcarriers reserved for PBCH DM-RS.
    symbols = grid.put(port, l0 + 1, k0, full_mask, symbols);

    // Put sequence in symbol 2 (0, 1, ..., 41) and (192, 193, ..., 239), skip the subcarriers reserved for PBCH DM-RS.
    symbols = grid.put(port, l0 + 2, k0, edge_mask, symbols);

    // Put sequence in symbol 3 (0, 1, ..., 239), skip the subcarriers reserved for PBCH DM-RS.
    symbols = grid.put(port, l0 + 3, k0, full_mask, symbols);

    ocudu_assert(symbols.empty(), "Symbols must be empty.");
  }
}

void pbch_modulator_impl::put(span<const uint8_t>             bits,
                              resource_grid_writer&           grid,
                              const pbch_modulator::config_t& args)
{
  ocudu_assert(bits.size() == M_bit, "Input span size must equal M_bit");

  // Scramble.
  std::array<uint8_t, M_bit> b_hat;
  scramble(bits, b_hat, args);

  // Modulate.
  std::array<cf_t, M_symb> d_pbch;
  modulate(b_hat, d_pbch);

  // Map.
  map(d_pbch, grid, args);
}
