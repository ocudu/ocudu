// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdsch_encoder_impl.h"
#include "ocudu/ocuduvec/bit.h"

using namespace ocudu;

void pdsch_encoder_impl::encode(span<uint8_t>        codeword,
                                span<const uint8_t>  transport_block,
                                const configuration& config)
{
  segmenter_config segmenter_cfg = {.transport_block_size = units::bytes(transport_block.size()),
                                    .base_graph           = config.base_graph,
                                    .rv                   = config.rv,
                                    .mod                  = config.mod,
                                    .Nref                 = config.Nref,
                                    .nof_layers           = config.nof_layers,
                                    .nof_ch_symbols       = config.nof_ch_symbols};

  // Initialize the segmenter.
  const ldpc_segmenter_buffer& segment_buffer = segmenter->new_transmission(segmenter_cfg);

  // Prepare codeblock data.
  units::bits cb_size = segment_buffer.get_segment_length();
  cb_data.resize(cb_size.value());

  unsigned offset = 0;
  for (unsigned i_cb = 0, i_cb_end = segment_buffer.get_nof_codeblocks(); i_cb != i_cb_end; ++i_cb) {
    // Retrieve segment description.
    const codeblock_metadata cb_metadata = segment_buffer.get_cb_metadata(i_cb);

    // Copy codeblock data, including TB and/or CB CRC if applicable, as well as filler and zero padding bits.
    segment_buffer.read_codeblock(cb_data, transport_block, i_cb);

    // Encode the segment into a codeblock.
    ldpc_encoder::configuration ldpc_config = {
        .base_graph   = cb_metadata.tb_common.base_graph,
        .lifting_size = cb_metadata.tb_common.lifting_size,
        .Nref         = cb_metadata.tb_common.Nref,
    };
    const ldpc_encoder_buffer& rm_buffer = encoder->encode(cb_data, ldpc_config);

    // Select the correct chunk of the output codeword.
    unsigned rm_length = segment_buffer.get_rm_length(i_cb);
    ocudu_assert(offset + rm_length <= codeword.size(), "Wrong codeword length.");
    span<uint8_t> codeblock = span<uint8_t>(codeword).subspan(offset, rm_length);

    // Rate match the codeblock.
    codeblock_packed.resize(rm_length);
    rate_matcher->rate_match(codeblock_packed, rm_buffer, cb_metadata);

    // Unpack code block.
    ocuduvec::bit_unpack(codeblock, codeblock_packed);

    // Advance write code block.
    offset += rm_length;
  }
}
