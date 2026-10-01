// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ptrs_pdsch_generator_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/phy/support/resource_grid_mapper.h"
#include "ocudu/ran/ptrs/ptrs_pattern.h"

using namespace ocudu;

void ptrs_pdsch_generator_generic_impl::generate(resource_grid_writer& grid, const configuration& config)
{
  // Get the number of ports used for PT-RS: it is equal to the number of layers used for the PDSCH transmission.
  unsigned nof_ports = config.precoding_and_beamforming.get_nof_layers();

  // Get the number of DM-RS per RB.
  unsigned nof_dmrs_prb = get_nof_re_per_prb(config.dmrs_type);

  // PT-RS transmission is fixed to one layer.
  static constexpr unsigned nof_layers = 1;

  // The PT-RS antenna port is associated with the lowest indexed DM-RS antenna port among the DM-RS antenna ports
  // assigned for the PDSCH (TS38.214 Section 5.1.6.3).
  static constexpr unsigned i_layer = 0;

  // Prepare PT-RS pattern configuration.
  ptrs_pattern_configuration ptrs_pattern_config = {
      .rnti             = config.rnti,
      .dmrs_type        = config.dmrs_type,
      .dmrs_symbol_mask = config.dmrs_symbols_mask,
      .rb_mask          = config.rb_mask,
      .time_allocation  = config.time_allocation,
      .freq_density     = config.freq_density,
      .time_density     = config.time_density,
      .re_offset        = config.re_offset,
      .nof_ports        = nof_ports,
  };

  // Generate pattern configuration.
  ptrs_pattern pattern = get_ptrs_pattern(ptrs_pattern_config);

  // Count number of RB containing PT-RS.
  unsigned nof_prb_ptrs = divide_ceil(pattern.rb_end - pattern.rb_begin, pattern.rb_stride);

  // Prepare temporary sequence.
  dmrs_sequence.resize(config.rb_mask.count() * nof_dmrs_prb);
  sequence.resize(1, nof_prb_ptrs);

  // Extract parameters to calculate the PRG initial state.
  unsigned nslot    = config.slot.slot_index();
  unsigned nidnscid = config.scrambling_id;
  unsigned nscid    = config.n_scid ? 1 : 0;
  unsigned nsymb    = get_nsymb_per_slot(cyclic_prefix::NORMAL);
  unsigned l_0      = config.dmrs_symbols_mask.find_lowest();

  // Calculate initial sequence state.
  unsigned c_init = ((nsymb * nslot + l_0 + 1) * (2 * nidnscid + 1) * pow2(17) + (2 * nidnscid + nscid)) % pow2(31);

  // Generate sequence for all the symbols.
  pseudo_random_gen->init(c_init);
  pseudo_random_gen->advance(2 * (pattern.rb_begin - config.reference_point_k_rb) * nof_dmrs_prb);
  pseudo_random_gen->generate(dmrs_sequence, M_SQRT1_2 * config.amplitude);

  // Precoding and beamforming of the transmission.
  const precoding_beamforming_configuration& precoding = config.precoding_and_beamforming;

  // Prepare the precoding and beamforming of the layer that carries the PT-RS.
  unsigned nof_beams = precoding.get_nof_beams();
  unsigned nof_prg   = precoding.get_nof_prg();

  precoding_beamforming_configuration layer_precoding(nof_layers, nof_beams, nof_prg, precoding.get_prg_size());
  for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
    const precoding_beamforming_composite& prg_composite = precoding.get_prg(i_prg);

    // The MIMO precoding matrix contains the row of the layer that carries the PT-RS.
    precoding_weight_matrix layer_mimo(nof_layers, nof_beams);
    for (unsigned i_beam = 0; i_beam != nof_beams; ++i_beam) {
      layer_mimo.set_coefficient(prg_composite.mimo.get_coefficient(i_layer, i_beam), 0, i_beam);
    }

    // The PT-RS is carried by the beams of the transmission.
    layer_precoding.set_prg({layer_mimo, prg_composite.beams}, i_prg);
  }

  // Select samples from the sequence.
  span<cf_t> sequence_slice = sequence.get_slice(0);
  for (unsigned i_re = 0; i_re != nof_prb_ptrs; ++i_re) {
    sequence_slice[i_re] = dmrs_sequence[i_re * nof_dmrs_prb * pattern.rb_stride + pattern.re_offset[0] / 2];
  }

  // Iterate all the OFDM symbols of the transmission.
  for (unsigned i_symbol = config.time_allocation.start(), i_symbol_end = config.time_allocation.stop();
       i_symbol != i_symbol_end;
       ++i_symbol) {
    // Skip if the OFDM symbol does not contain PT-RS.
    if (!pattern.symbol_mask.test(i_symbol)) {
      continue;
    }

    // Prepare RE mapping pattern for the symbol.
    re_pattern map_pattern;
    map_pattern.crb_mask.resize(config.rb_mask.size());
    map_pattern.symbols.resize(MAX_NSYMB_PER_SLOT);
    map_pattern.symbols.set(i_symbol);
    for (unsigned i_prb = pattern.rb_begin; i_prb < pattern.rb_end; i_prb += pattern.rb_stride) {
      map_pattern.crb_mask.set(i_prb);
    }
    map_pattern.re_mask.resize(NOF_SUBCARRIERS_PER_RB);
    map_pattern.re_mask.set(pattern.re_offset[0]);

    // Map sequence in the resource grid.
    mapper->map(grid, sequence, map_pattern, layer_precoding);
  }
}
