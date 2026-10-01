// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ldpc_segmenter_rx_impl.h"
#include "ocudu/phy/upper/codeblock_metadata.h"
#include "ocudu/ran/sch/sch_segmentation.h"
#include "ocudu/support/math/math_utils.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;
using namespace ocudu::ldpc;

/// Length of the CRC checksum added to the segments.
static constexpr units::bits SEG_CRC_LENGTH{24};

static void check_inputs_rx(span<const log_likelihood_ratio> codeword_llrs, const segmenter_config& cfg)
{
  ocudu_assert(!codeword_llrs.empty(), "Argument transport_block should not be empty.");
  ocudu_assert(codeword_llrs.size() == cfg.nof_ch_symbols * get_bits_per_symbol(cfg.mod),
               "Wrong number of LLRs {} (expected {}).",
               codeword_llrs.size(),
               cfg.nof_ch_symbols * get_bits_per_symbol(cfg.mod));

  ocudu_assert((cfg.rv >= 0) && (cfg.rv <= 3), "Invalid redundancy version.");

  ocudu_assert((cfg.nof_layers >= 1) && (cfg.nof_layers <= 4), "Invalid number of layers.");

  ocudu_assert(cfg.nof_ch_symbols % (cfg.nof_layers) == 0,
               "The number of channel symbols should be a multiple of the product between the number of layers.");
}

void ldpc_segmenter_rx_impl::segment(static_vector<described_rx_codeblock, MAX_NOF_SEGMENTS>& described_codeblocks,
                                     span<const log_likelihood_ratio>                         codeword_llrs,
                                     const segmenter_config&                                  cfg)
{
  check_inputs_rx(codeword_llrs, cfg);

  using namespace units::literals;

  params.base_graph = cfg.base_graph;

  units::bits tbs_bits(cfg.transport_block_size.to_bits());
  params.nof_tb_crc_bits = compute_tb_crc_size(tbs_bits);

  params.nof_tb_bits_in = tbs_bits + params.nof_tb_crc_bits;

  params.nof_segments    = compute_nof_codeblocks(tbs_bits, params.base_graph);
  params.nof_tb_bits_out = params.nof_tb_bits_in;
  if (params.nof_segments > 1) {
    params.nof_tb_bits_out += units::bits(params.nof_segments * SEG_CRC_LENGTH.value());
  }
  params.lifting_size   = compute_lifting_size(tbs_bits, params.base_graph, params.nof_segments);
  params.segment_length = compute_codeblock_size(params.base_graph, params.lifting_size);

  params.nof_crc_bits = (params.nof_segments > 1) ? SEG_CRC_LENGTH : 0_bits;

  // Compute the maximum number of information bits that can be assigned to a segment.
  units::bits max_info_bits =
      units::bits(divide_ceil(params.nof_tb_bits_out.value(), params.nof_segments)) - params.nof_crc_bits;

  // Number of channel symbols assigned to a transmission layer.
  params.nof_symbols_per_layer = cfg.nof_ch_symbols / cfg.nof_layers;
  // Number of segments that will have a short rate-matched length. In TS38.212 Section 5.4.2.1, these correspond to
  // codeblocks whose length E_r is computed by rounding down - floor. For the remaining codewords, the length is
  // rounded up.
  params.nof_short_segments = params.nof_segments - (params.nof_symbols_per_layer % params.nof_segments);

  // Codeword length (after concatenation of codeblocks).
  params.cw_length = static_cast<units::bits>(codeword_llrs.size());

  unsigned cw_offset = 0;
  for (unsigned i_segment = 0; i_segment != params.nof_segments; ++i_segment) {
    params.cw_offset[i_segment] = cw_offset;
    params.nof_filler_bits      = params.segment_length - (max_info_bits + params.nof_crc_bits);

    codeblock_metadata tmp_description = generate_cb_metadata(params, cfg, i_segment);

    unsigned rm_length = tmp_description.cb_specific.rm_length;
    described_codeblocks.push_back({codeword_llrs.subspan(cw_offset, rm_length), tmp_description});
    cw_offset += rm_length;
  }
  // After accumulating all codeblock rate-matched lengths, cw_offset should be the same as cw_length.
  ocudu_assert(params.cw_length.value() == cw_offset, "Cw offset must be equal to the cw length");
}
