// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "sss_processor_impl.h"
#include "ocudu/ocuduvec/prod.h"
#include "ocudu/ocuduvec/sc_prod.h"
#include "ocudu/phy/support/resource_grid_writer.h"

using namespace ocudu;

const sss_sequence_generator sss_processor_impl::sequence_generator;

void sss_processor_impl::mapping(span<const cf_t> sequence, resource_grid_writer& grid, const config_t& args) const
{
  // Calculate symbol and first subcarrier for SSS.
  unsigned l = args.ssb_first_symbol + ssb_l;
  unsigned k = args.ssb_first_subcarrier + ssb_k_begin;

  // Extract the precoding and beamforming information - only one PRG.
  const precoding_beamforming_composite& precoding = args.precoding_and_beamforming.get_prg(0);

  // Write in the resource grid port that carries each of the beams, after applying the MIMO precoding weight.
  for (unsigned i_beam = 0, nof_beams = precoding.beams.size(); i_beam != nof_beams; ++i_beam) {
    std::array<cf_t, sequence_len> precoded_sequence;
    ocuduvec::sc_prod(precoded_sequence, sequence, precoding.mimo.get_coefficient(0, i_beam));

    grid.put(to_uint(precoding.beams[i_beam]), l, k, precoded_sequence);
  }
}

void sss_processor_impl::map(resource_grid_writer& grid, const config_t& config)
{
  // Generate sequence.
  std::array<cf_t, sequence_len> sequence;
  sequence_generator.generate(sequence, config.phys_cell_id, config.amplitude);

  // Mapping to physical resources.
  mapping(sequence, grid, config);
}
