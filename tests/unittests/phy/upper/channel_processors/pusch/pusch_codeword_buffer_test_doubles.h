// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocuduvec/bit.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/phy/upper/channel_processors/pusch/pusch_codeword_buffer.h"

namespace ocudu {

class pusch_codeword_buffer_spy : public pusch_codeword_buffer
{
public:
  pusch_codeword_buffer_spy() = default;

  pusch_codeword_buffer_spy(unsigned buffer_size) : data(buffer_size), scrambling_seq(buffer_size) {}

  span<log_likelihood_ratio> get_next_block_view(unsigned block_size) override
  {
    ocudu_assert(!completed, "Wrong state.");
    block_size = std::min(block_size, static_cast<unsigned>(data.size()) - count);
    return span<log_likelihood_ratio>(data).subspan(count, block_size);
  }

  void on_new_block(span<const log_likelihood_ratio> new_data, const bit_buffer& new_sequence) override
  {
    ocudu_assert(!completed, "Wrong state.");
    ocudu_assert(new_sequence.size() == new_data.size(), "Sizes must be equal.");
    unsigned block_size = new_data.size();

    // Append soft bits.
    span<log_likelihood_ratio> dst_data = span<log_likelihood_ratio>(data).subspan(count, block_size);
    ocuduvec::copy(dst_data, new_data);

    // Append scrambling sequence.
    ocuduvec::copy_offset(scrambling_seq, count, new_sequence, 0, new_sequence.size());

    count += block_size;
  }

  void on_end_codeword() override
  {
    ocudu_assert(!completed, "Wrong state.");
    completed = true;
  }

  span<const log_likelihood_ratio> get_data() const
  {
    ocudu_assert(completed, "Wrong state.");
    return span<const log_likelihood_ratio>(data).first(count);
  }

  const bit_buffer& get_scrambling_seq() const
  {
    ocudu_assert(completed, "Wrong state.");
    return scrambling_seq;
  }

private:
  bool                              completed = false;
  unsigned                          count     = 0;
  std::vector<log_likelihood_ratio> data;
  dynamic_bit_buffer                scrambling_seq;
};

} // namespace ocudu
