// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/sch/sch_segmentation.h"

using namespace ocudu;

constexpr units::bits sch_information::CB_CRC_SIZE;

sch_information ocudu::get_sch_segmentation_info(units::bits tbs, float target_code_rate)
{
  ocudu_assert(target_code_rate > 0.F && target_code_rate < 1.F,
               "Invalid target code rate {}, expected a value between 0 and 1",
               target_code_rate);

  sch_information result;

  // Calculate the transport block CRC size.
  result.tb_crc_size = compute_tb_crc_size(tbs);

  // Select base graph.
  result.base_graph = get_ldpc_base_graph(target_code_rate, tbs);

  // Calculate the number of codeblocks.
  result.nof_cb = compute_nof_codeblocks(tbs, result.base_graph);

  // Calculate the number of bits per codeblock.
  units::bits nof_payload_bits_per_cb((tbs + result.tb_crc_size).value() / result.nof_cb);
  if (result.nof_cb > 1) {
    nof_payload_bits_per_cb += sch_information::CB_CRC_SIZE;
  }

  // Calculate lifting size.
  result.lifting_size = compute_lifting_size(tbs, result.base_graph, result.nof_cb);

  // Calculate the codeblock size.
  result.nof_bits_per_cb = compute_codeblock_size(result.base_graph, result.lifting_size);

  // Calculate the number of filler bits per codeblock.
  result.nof_filler_bits_per_cb = result.nof_bits_per_cb - nof_payload_bits_per_cb;

  return result;
}
