// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/pusch/pusch_decoder_buffer.h"

namespace ocudu {

/// Implements a dummy PUSCH decoder buffer.
class pusch_decoder_buffer_dummy : public pusch_decoder_buffer
{
public:
  // See interface for documentation.
  span<log_likelihood_ratio> get_next_block_view(unsigned /* block_size */) override
  {
    return span<log_likelihood_ratio>();
  }

  // See interface for documentation.
  void on_new_softbits(span<const log_likelihood_ratio> /* softbits */) override
  {
    // Ignore.
  }

  // See interface for documentation.
  void on_end_softbits() override
  {
    // Ignore.
  }
};

} // namespace ocudu
