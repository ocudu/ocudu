// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Helper functions for PDSCH processor implementations.

#pragma once

#include "ocudu/instrumentation/traces/du_traces.h"
#include "ocudu/phy/upper/channel_processors/pdsch/pdsch_processor.h"
#include "ocudu/phy/upper/dmrs_mapping.h"
#include "ocudu/phy/upper/signal_processors/pdsch/dmrs_pdsch_processor.h"
#include "ocudu/phy/upper/signal_processors/ptrs/ptrs_pdsch_generator.h"
#include "ocudu/ran/ptrs/ptrs_pattern.h"
#include "ocudu/support/tracing/event_tracing.h"

namespace ocudu {

/// \brief Generates and maps DM-RS for the PDSCH transmission as per TS38.211 Section 7.4.1.1.
/// \param[out] grid Resource grid writer interface.
/// \param[out] dmrs DM-RS PDSCH processor interface.
/// \param[in]  pdu  Provides the PDSCH processor PDU.
inline void
pdsch_process_dmrs(resource_grid_writer& grid, dmrs_pdsch_processor& dmrs, const pdsch_processor::pdu_t& pdu)
{
  trace_point process_dmrs_tp = l1_dl_tracer.now();

  crb_bitmap rb_mask_bitset = pdu.freq_alloc.get_crb_mask(pdu.bwp_start_rb, pdu.bwp_size_rb);

  // Select the DM-RS reference point.
  unsigned dmrs_reference_point_k_rb = 0;
  if (pdu.ref_point == pdsch_processor::pdu_t::PRB0) {
    dmrs_reference_point_k_rb = pdu.bwp_start_rb;
  }

  // Prepare DM-RS configuration.
  dmrs_pdsch_processor::config_t dmrs_config = {.slot                 = pdu.slot,
                                                .reference_point_k_rb = dmrs_reference_point_k_rb,
                                                .type                 = pdu.dmrs,
                                                .scrambling_id        = pdu.scrambling_id,
                                                .n_scid               = pdu.n_scid,
                                                .amplitude = convert_dB_to_amplitude(-pdu.ratio_pdsch_dmrs_to_sss_dB),
                                                .symbols_mask              = pdu.dmrs_symbol_mask,
                                                .rb_mask                   = rb_mask_bitset,
                                                .precoding_and_beamforming = pdu.precoding_and_beamforming};

  // Put DM-RS.
  dmrs.map(grid, dmrs_config);

  l1_dl_tracer << trace_event("process_dmrs", process_dmrs_tp);
}

/// \brief Generates and maps PT-RS for the PDSCH transmission as per TS38.211 Section 7.4.1.2.
/// \param[out] grid           Resource grid writer interface.
/// \param[out] ptrs_generator PT-RS PDSCH generator interface.
/// \param[in]  pdu            Provides the PDSCH processor PDU.
inline void
pdsch_process_ptrs(resource_grid_writer& grid, ptrs_pdsch_generator& ptrs_generator, const pdsch_processor::pdu_t& pdu)
{
  trace_point process_ptrs_tp = l1_dl_tracer.now();

  // Extract PT-RS configuration parameters.
  const pdsch_processor::ptrs_configuration& ptrs = *pdu.ptrs;

  crb_bitmap rb_mask_bitset = pdu.freq_alloc.get_crb_mask(pdu.bwp_start_rb, pdu.bwp_size_rb);

  // Select the DM-RS reference point.
  unsigned ptrs_reference_point_k_rb = 0;
  if (pdu.ref_point == pdsch_processor::pdu_t::PRB0) {
    ptrs_reference_point_k_rb = pdu.bwp_start_rb;
  }

  // Calculate the PT-RS sequence amplitude following TS38.214 Section 4.1.
  float amplitude = convert_dB_to_amplitude(ptrs.ratio_ptrs_to_pdsch_data_dB - pdu.ratio_pdsch_data_to_sss_dB);

  // Prepare PT-RS configuration.
  ptrs_pdsch_generator::configuration ptrs_config = {
      .slot                      = pdu.slot,
      .rnti                      = pdu.rnti,
      .dmrs_type                 = pdu.dmrs,
      .reference_point_k_rb      = ptrs_reference_point_k_rb,
      .scrambling_id             = pdu.scrambling_id,
      .n_scid                    = pdu.n_scid,
      .amplitude                 = amplitude,
      .dmrs_symbols_mask         = pdu.dmrs_symbol_mask,
      .rb_mask                   = rb_mask_bitset,
      .time_allocation           = {pdu.start_symbol_index, pdu.start_symbol_index + pdu.nof_symbols},
      .freq_density              = ptrs.freq_density,
      .time_density              = ptrs.time_density,
      .re_offset                 = ptrs.re_offset,
      .reserved                  = pdu.reserved,
      .precoding_and_beamforming = pdu.precoding_and_beamforming};

  // Put PT-RS.
  ptrs_generator.generate(grid, ptrs_config);

  l1_dl_tracer << trace_event("process_ptrs", process_ptrs_tp);
}

/// \brief Computes the number of RE used for mapping PDSCH data.
///
/// The number of RE excludes the elements described by \c pdu as reserved and the RE used for DM-RS.
///
/// \param[in] pdu Describes a PDSCH transmission.
/// \return The number of resource elements.
inline unsigned pdsch_compute_nof_data_re(const pdsch_processor::pdu_t& pdu)
{
  // Get PRB mask.
  crb_bitmap crb_mask = pdu.freq_alloc.get_crb_mask(pdu.bwp_start_rb, pdu.bwp_size_rb);

  // Get number of RB.
  unsigned nof_prb = crb_mask.count();

  // Calculate the number of RE allocated in the grid.
  unsigned nof_grid_re = nof_prb * NOF_SUBCARRIERS_PER_RB * pdu.nof_symbols;

  // Generate DM-RS pattern.
  re_pattern dmrs_pattern = get_dmrs_pattern(
      pdu.dmrs, pdu.bwp_start_rb, pdu.bwp_size_rb, pdu.nof_cdm_groups_without_data, pdu.dmrs_symbol_mask);

  // Calculate the number of RE used by DM-RS. It assumes it does not overlap with reserved elements.
  unsigned nof_grid_dmrs = nof_prb * dmrs_pattern.re_mask.count() * dmrs_pattern.symbols.count();

  // Generate reserved pattern.
  re_pattern_list reserved = pdu.reserved;

  // If the pattern contains PT-RS, append the reserved elements to the list.
  if (pdu.ptrs.has_value()) {
    // Extract specific PT-RS configuration.
    const pdsch_processor::ptrs_configuration& ptrs_config = *pdu.ptrs;

    // Create PT-RS pattern configuration.
    ptrs_pattern_configuration ptrs_pattern_config = {
        .rnti             = pdu.rnti,
        .dmrs_type        = pdu.dmrs,
        .dmrs_symbol_mask = pdu.dmrs_symbol_mask,
        .rb_mask          = crb_mask,
        .time_allocation  = {pdu.start_symbol_index, pdu.start_symbol_index + pdu.nof_symbols},
        .freq_density     = ptrs_config.freq_density,
        .time_density     = ptrs_config.time_density,
        .re_offset        = ptrs_config.re_offset,
        .nof_ports        = 1};

    // Calculate PT-RS pattern and convert it to an RE pattern.
    ptrs_pattern ptrs_reserved_pattern = get_ptrs_pattern(ptrs_pattern_config);

    re_pattern ptrs_reserved_re_pattern;
    for (unsigned i_prb = ptrs_reserved_pattern.rb_begin; i_prb < ptrs_reserved_pattern.rb_end;
         i_prb += ptrs_reserved_pattern.rb_stride) {
      ptrs_reserved_re_pattern.crb_mask.set(i_prb);
    }
    ptrs_reserved_re_pattern.symbols = ptrs_reserved_pattern.symbol_mask;
    ptrs_reserved_re_pattern.re_mask.set(ptrs_reserved_pattern.re_offset.front());

    reserved.merge(ptrs_reserved_re_pattern);
  }

  // Calculate the number of reserved resource elements.
  unsigned nof_reserved_re = reserved.get_inclusion_count(pdu.start_symbol_index, pdu.nof_symbols, crb_mask);

  // Subtract the number of reserved RE from the number of allocated RE.
  ocudu_assert(nof_grid_re > nof_reserved_re,
               "The number of reserved RE ({}) exceeds the number of RE allocated in the transmission ({})",
               nof_grid_re,
               nof_reserved_re);
  return nof_grid_re - nof_reserved_re - nof_grid_dmrs;
}

/// \brief Extracts the precoding and beamforming configuration of a codeword.
///
/// The codewords split the transmission layers as described in TS38.211 Table 7.3.1.3-1, and the beams that carry the
/// transmission are split evenly among them, following the beam list order: the first codeword is carried by the first
/// half of the beams and the second codeword is carried by the last half.
///
/// \param[in] precoding     Precoding and beamforming configuration of the transmission.
/// \param[in] nof_codewords Number of codewords of the transmission.
/// \param[in] i_cw          Codeword index.
/// \return The precoding and beamforming configuration of the given codeword.
/// \remark An assertion is triggered if the number of beams is not divisible by the number of codewords.
inline precoding_beamforming_configuration
pdsch_extract_codeword_precoding(const precoding_beamforming_configuration& precoding,
                                 unsigned                                   nof_codewords,
                                 unsigned                                   i_cw)
{
  unsigned nof_layers = precoding.get_nof_layers();
  unsigned nof_beams  = precoding.get_nof_beams();

  ocudu_assert(nof_beams % nof_codewords == 0,
               "The number of beams (i.e., {}) must be divisible by the number of codewords (i.e., {}).",
               nof_beams,
               nof_codewords);

  // Number of beams that carry each of the codewords.
  unsigned nof_beams_cw = nof_beams / nof_codewords;

  // Number of layers of the first codeword, as per TS38.211 Table 7.3.1.3-1.
  unsigned nof_layers_cw0 = nof_layers / nof_codewords;

  // Number of layers of the given codeword.
  unsigned nof_layers_cw = (i_cw == 0) ? nof_layers_cw0 : (nof_layers - nof_layers_cw0);

  // First layer and first beam of the given codeword.
  unsigned first_layer = i_cw * nof_layers_cw0;
  unsigned first_beam  = i_cw * nof_beams_cw;

  precoding_beamforming_configuration result(
      nof_layers_cw, nof_beams_cw, precoding.get_nof_prg(), precoding.get_prg_size());

  // Iterate each PRG.
  for (unsigned i_prg = 0, i_prg_end = precoding.get_nof_prg(); i_prg != i_prg_end; ++i_prg) {
    const precoding_beamforming_composite& prg_composite = precoding.get_prg(i_prg);

    // Extract the MIMO precoding coefficients of the codeword layers and beams.
    precoding_weight_matrix mimo(nof_layers_cw, nof_beams_cw);
    for (unsigned i_layer = 0; i_layer != nof_layers_cw; ++i_layer) {
      for (unsigned i_beam = 0; i_beam != nof_beams_cw; ++i_beam) {
        mimo.set_coefficient(
            prg_composite.mimo.get_coefficient(first_layer + i_layer, first_beam + i_beam), i_layer, i_beam);
      }
    }

    // Extract the beams that carry the codeword.
    precoding_beam_list beams(prg_composite.beams.begin() + first_beam,
                              prg_composite.beams.begin() + first_beam + nof_beams_cw);

    result.set_prg({mimo, beams}, i_prg);
  }

  return result;
}

} // namespace ocudu
