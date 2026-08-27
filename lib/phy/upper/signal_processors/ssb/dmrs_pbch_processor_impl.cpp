// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "dmrs_pbch_processor_impl.h"
#include "ocudu/ocuduvec/sc_prod.h"
#include "ocudu/phy/support/resource_grid_writer.h"

using namespace ocudu;

unsigned dmrs_pbch_processor_impl::c_init(const config_t& config)
{
  // Default values for L_max == 4
  uint64_t i_ssb = (config.ssb_idx.value() & 0b11U) + 4UL * config.hrf; // Least 2 significant bits

  if (config.L_max == 8 || config.L_max == 64) {
    i_ssb = config.ssb_idx.value() & 0b111U; // Least 3 significant bits
  }

  return ((i_ssb + 1UL) * ((config.phys_cell_id / 4UL) + 1UL) << 11UL) + ((i_ssb + 1UL) << 6UL) +
         (config.phys_cell_id % 4);
}

void dmrs_pbch_processor_impl::generation(std::array<cf_t, NOF_RE>& sequence, const config_t& config) const
{
  // Calculate initial state
  prg->init(c_init(config));

  // Generate sequence
  prg->generate(sequence, M_SQRT1_2);
}

void dmrs_pbch_processor_impl::mapping(const std::array<cf_t, NOF_RE>& r,
                                       resource_grid_writer&           grid,
                                       const config_t&                 args) const
{
  // Calculate index shift.
  uint32_t v = args.phys_cell_id % 4;

  const uint8_t  l0 = args.ssb_first_symbol;
  const uint16_t k0 = args.ssb_first_subcarrier;

  // Extract the precoding and beamforming information - only one PRG.
  const precoding_beamforming_composite& precoding = args.precoding_and_beamforming.get_prg(0);

  // For each beam.
  for (unsigned i_beam = 0, nof_beams = precoding.beams.size(); i_beam != nof_beams; ++i_beam) {
    // Apply the MIMO precoding weight of the beam and convert the symbols to complex BF16.
    std::array<cbf16_t, NOF_RE> symbols_cbf16;
    ocuduvec::sc_prod(symbols_cbf16, r, precoding.mimo.get_coefficient(0, i_beam));

    // Resource grid port that carries the beam.
    unsigned port = to_uint(precoding.beams[i_beam]);

    // Create view with the symbols.
    span<const cbf16_t> symbols = symbols_cbf16;

    // Put sequence in symbol 1 (0 + v , 4 + v , 8 + v ,..., 236 + v).
    grid.put(port, l0 + 1, k0 + v, stride, symbols.first(nof_dmrs_full_symbol));
    symbols = symbols.last(symbols.size() - nof_dmrs_full_symbol);

    // Put sequence in symbol 2, lower section (0 + v , 4 + v , 8 + v ,..., 44 + v).
    grid.put(port, l0 + 2, k0 + v, stride, symbols.first(nof_dmrs_edge_symbol));
    symbols = symbols.last(symbols.size() - nof_dmrs_edge_symbol);

    // Put sequence in symbol 2, upper section (192 + v , 196 + v , 200 + v ,..., 236 + v).
    grid.put(port, l0 + 2, k0 + v + 192, stride, symbols.first(nof_dmrs_edge_symbol));
    symbols = symbols.last(symbols.size() - nof_dmrs_edge_symbol);

    // Put sequence in symbol 3 (0 + v , 4 + v , 8 + v ,..., 236 + v).
    grid.put(port, l0 + 3, k0 + v, stride, symbols.first(nof_dmrs_full_symbol));
  }
}

void dmrs_pbch_processor_impl::map(resource_grid_writer& grid, const config_t& config)
{
  // Generate sequence.
  std::array<cf_t, NOF_RE> sequence;
  generation(sequence, config);

  // Mapping to physical resources.
  mapping(sequence, grid, config);
}
