// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pusch_decoder_empty_impl.h"
#include "ocudu/ocuduvec/zero.h"
#include "ocudu/phy/upper/channel_processors/pusch/pusch_decoder_notifier.h"
#include "ocudu/phy/upper/channel_processors/pusch/pusch_decoder_result.h"

using namespace ocudu;

pusch_decoder_empty_impl::pusch_decoder_empty_impl(unsigned nof_prb, unsigned nof_layers) :
  decoder_buffer(nof_prb, nof_layers)
{
}

pusch_decoder_buffer& pusch_decoder_empty_impl::new_data(span<uint8_t>                       transport_block,
                                                         unique_rx_buffer                    rm_buffer_,
                                                         pusch_decoder_notifier&             notifier_,
                                                         const pusch_decoder::configuration& cfg)
{
  decoder_buffer.new_data(transport_block, std::move(rm_buffer_), notifier_, cfg);
  return decoder_buffer;
}

void pusch_decoder_empty_impl::set_nof_softbits(units::bits /* nof_softbits */)
{
  // Do nothing.
}

void pusch_decoder_empty_impl::decoder_buffer_impl::new_data(span<uint8_t>                       transport_block,
                                                             ocudu::unique_rx_buffer             rm_buffer_,
                                                             ocudu::pusch_decoder_notifier&      notifier_,
                                                             const pusch_decoder::configuration& cfg)
{
  // Erase transport block contents from a previous transmission.
  ocuduvec::zero(transport_block);

  // Save inputs.
  rm_buffer = std::move(rm_buffer_);
  ocudu_assert(rm_buffer, "Invalid buffer.");
  notifier       = &notifier_;
  is_new_data    = cfg.new_data;
  softbits_count = 0;
}

span<log_likelihood_ratio> pusch_decoder_empty_impl::decoder_buffer_impl::get_next_block_view(unsigned block_size)
{
  // Return always the first softbits, the decoder will ignore the soft bits.
  return span<log_likelihood_ratio>(softbits_buffer).first(block_size);
}

void pusch_decoder_empty_impl::decoder_buffer_impl::on_new_softbits(span<const log_likelihood_ratio> softbits)
{
  // Accumulate the count of softbits.
  softbits_count += softbits.size();
}

void pusch_decoder_empty_impl::decoder_buffer_impl::on_end_softbits()
{
  // Make sure the notifier is valid.
  ocudu_assert(notifier != nullptr, "Invalid notifier.");

  // Prepare failed transport block.
  pusch_decoder_result result;
  result.ldpc_decoder_stats.reset();
  result.nof_codeblocks_total = 0;
  result.tb_crc_ok            = false;

  // Reset rate matching data if it a new transmission.
  if (is_new_data) {
    rm_buffer->zero_all_cb_soft_bits();
  }

  // Unlock buffer for a retransmissions.
  rm_buffer.unlock();

  // Notify completion.
  notifier->on_sch_data(result);
}
