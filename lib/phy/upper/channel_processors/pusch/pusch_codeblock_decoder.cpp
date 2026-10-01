// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pusch_codeblock_decoder.h"

using namespace ocudu;

void pusch_codeblock_decoder::rate_match(span<log_likelihood_ratio>       rm_buffer,
                                         span<const log_likelihood_ratio> cb_llrs,
                                         bool                             new_data,
                                         const codeblock_metadata&        metadata)
{
  dematcher->rate_dematch(rm_buffer, cb_llrs, new_data, metadata);
}

std::optional<unsigned> pusch_codeblock_decoder::decode(bit_buffer                       cb_data,
                                                        span<log_likelihood_ratio>       rm_buffer,
                                                        span<const log_likelihood_ratio> cb_llrs,
                                                        bool                             new_data,
                                                        ocudu::crc_generator_poly        crc_poly,
                                                        bool                             use_early_stop,
                                                        unsigned                         nof_ldpc_iterations,
                                                        const codeblock_metadata&        metadata)
{
  rate_match(rm_buffer, cb_llrs, new_data, metadata);

  // Prepare LDPC decoder configuration.
  ldpc_decoder::configuration decoder_config;
  decoder_config.base_graph      = metadata.tb_common.base_graph;
  decoder_config.lifting_size    = metadata.tb_common.lifting_size;
  decoder_config.nof_filler_bits = metadata.cb_specific.nof_filler_bits;
  decoder_config.nof_crc_bits    = metadata.cb_specific.nof_crc_bits;
  decoder_config.max_iterations  = nof_ldpc_iterations;

  // Select CRC calculator.
  crc_calculator* crc = select_crc(crc_poly);
  ocudu_assert(crc != nullptr, "Invalid CRC calculator.");

  // Decode with early stop.
  if (use_early_stop) {
    return decoder->decode(cb_data, rm_buffer, crc, decoder_config);
  }

  // Without CRC early stop, first decode and skip the CRC if the syndrome check fails.
  std::optional<unsigned> decoder_result = decoder->decode(cb_data, rm_buffer, nullptr, decoder_config);
  if (!decoder_result.has_value()) {
    return std::nullopt;
  }

  // Discard filler bits for the CRC.
  unsigned nof_significant_bits = cb_data.size() - metadata.cb_specific.nof_filler_bits;
  if (crc->calculate(cb_data.first(nof_significant_bits)) == 0) {
    return nof_ldpc_iterations;
  }

  return std::nullopt;
}
