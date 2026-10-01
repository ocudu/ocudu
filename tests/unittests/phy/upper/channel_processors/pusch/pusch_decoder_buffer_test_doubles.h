// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocuduvec/copy.h"
#include "ocudu/phy/upper/channel_processors/pusch/pusch_decoder_buffer.h"

namespace ocudu {

class pusch_decoder_buffer_spy : public pusch_decoder_buffer
{
public:
  pusch_decoder_buffer_spy() = default;

  pusch_decoder_buffer_spy(unsigned buffer_size) { data.resize(buffer_size); }

  // See interface for documentation.
  span<log_likelihood_ratio> get_next_block_view(unsigned block_size) override
  {
    data.resize(block_size + count);
    return span<log_likelihood_ratio>(data).subspan(count, block_size);
  }

  // See interface for documentation.
  void on_new_softbits(span<const log_likelihood_ratio> softbits) override
  {
    span<log_likelihood_ratio> block = get_next_block_view(softbits.size());

    if (block.data() != softbits.data()) {
      ocuduvec::copy(block, softbits);
    }

    count += softbits.size();
  }

  // See interface for documentation.
  void on_end_softbits() override
  {
    // Ignore.
  }

  span<const log_likelihood_ratio> get_data() const { return span<const log_likelihood_ratio>(data).first(count); }

private:
  unsigned                          count = 0;
  std::vector<log_likelihood_ratio> data;
};

} // namespace ocudu
