// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/log_likelihood_ratio.h"
#include "ocudu/phy/upper/rx_buffer_decoder_callback.h"
#include "ocudu/phy/upper/rx_buffer_pool.h"
#include "ocudu/phy/upper/unique_rx_buffer.h"
#include "ocudu/ran/slot_point.h"

namespace ocudu {

class rx_buffer_pool_spy : public rx_buffer_pool, private unique_rx_buffer::buffer_management
{
public:
  unique_rx_buffer reserve(slot_point slot, trx_buffer_identifier id, unsigned nof_codeblocks, bool new_data) override
  {
    if (is_locked()) {
      return unique_rx_buffer();
    }
    is_buffer_locked = true;
    return unique_rx_buffer(*this, 0);
  }
  void run_slot(slot_point slot) override { current_slot = slot; }

  slot_point get_current_slot() const { return current_slot; }

  bool is_locked() const { return is_buffer_locked; }

  unsigned                   get_nof_codeblocks() const override { return 0; }
  void                       reset_codeblocks_crc() override {}
  span<bool>                 get_codeblocks_crc() override { return {}; }
  unsigned                   get_absolute_codeblock_id(unsigned codeblock_id) const override { return 0; }
  span<log_likelihood_ratio> get_codeblock_soft_bits(unsigned codeblock_id, unsigned codeblock_size) override
  {
    return {};
  }
  bit_buffer get_codeblock_data_bits(unsigned codeblock_id, unsigned data_size) override
  {
    static static_bit_buffer<8> buffer = {};
    return buffer;
  }
  void decode_cb_in_sequence(unsigned                    retransmission,
                             unsigned                    codeblock_id,
                             rx_buffer_decoder_callback& decoder_callback) override
  {
    decoder_callback.codeblock_decode(codeblock_id);
  }

  bool try_lock()
  {
    is_buffer_locked = true;
    return true;
  }
  void unlock(unsigned) override { is_buffer_locked = false; }
  void release(unsigned) override { is_buffer_locked = false; }

  void clear()
  {
    current_slot     = {};
    is_buffer_locked = false;
  }

private:
  slot_point current_slot     = {};
  bool       is_buffer_locked = false;
};

} // namespace ocudu
