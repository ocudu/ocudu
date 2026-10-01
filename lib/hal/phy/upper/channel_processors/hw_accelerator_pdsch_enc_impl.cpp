// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "hw_accelerator_pdsch_enc_impl.h"

using namespace ocudu;
using namespace hal;

void hw_accelerator_pdsch_enc_impl::reserve_queue()
{
  hw_reserve_queue();
}

void hw_accelerator_pdsch_enc_impl::free_queue()
{
  hw_free_queue();
}

bool hw_accelerator_pdsch_enc_impl::enqueue_operation(span<const uint8_t> data,
                                                      span<const uint8_t> aux_data,
                                                      unsigned            cb_index)
{
  return hw_enqueue(data, cb_index);
}

bool hw_accelerator_pdsch_enc_impl::dequeue_operation(span<uint8_t> data,
                                                      span<uint8_t> packed_data,
                                                      unsigned      segment_index)
{
  return hw_dequeue(data, packed_data, segment_index);
}

void hw_accelerator_pdsch_enc_impl::configure_operation(const hw_pdsch_encoder_configuration& config, unsigned cb_index)
{
  hw_config(config, cb_index);
}

bool hw_accelerator_pdsch_enc_impl::is_cb_mode_supported() const
{
  return is_hw_cb_mode_supported();
}

unsigned hw_accelerator_pdsch_enc_impl::get_max_supported_buff_size() const
{
  return get_hw_max_supported_buff_size();
}
