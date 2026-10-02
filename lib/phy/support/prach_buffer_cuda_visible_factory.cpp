// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "prach_buffer_cuda_visible_impl.h"
#include "ocudu/cuda/support/cuda_device.h"
#include "ocudu/phy/support/support_factories.h"
#include "ocudu/ran/prach/prach_constants.h"

using namespace ocudu;

std::unique_ptr<prach_buffer>
ocudu::create_prach_buffer_cuda_visible(unsigned                                       max_nof_antennas,
                                        unsigned                                       max_nof_td_occasions,
                                        unsigned                                       max_nof_fd_occasions,
                                        bool                                           is_long_preamble,
                                        const cuda_visible_prach_buffer_configuration& config)
{
  cuda_visible_prach_buffer_configuration resolved = config;

  // Where the GPU reaches host memory through the host page tables, a prefetch migrates nothing and
  // only adds a stream synchronization per occasion, which shows up as a stall in the uplink
  // pipeline. It is therefore turned on only for the GPUs that do migrate pages.
  if (!resolved.enable_prefetch) {
    resolved.enable_prefetch =
        !cuda::get_cuda_device_properties(resolved.device_id).pageable_memory_uses_host_page_tables;
  }

  // The two preamble lengths differ in both dimensions: a long preamble carries fewer, longer
  // sequences than a short one.
  const unsigned nof_symbols = is_long_preamble ? prach_constants::LONG_SEQUENCE_MAX_NOF_SYMBOLS
                                                : prach_constants::SHORT_SEQUENCE_MAX_NOF_SYMBOLS;
  const unsigned sequence_length =
      is_long_preamble ? prach_constants::LONG_SEQUENCE_LENGTH : prach_constants::SHORT_SEQUENCE_LENGTH;

  auto buffer = std::make_unique<prach_buffer_cuda_visible_impl>(
      max_nof_antennas, max_nof_td_occasions, max_nof_fd_occasions, nof_symbols, sequence_length, resolved);
  if (!buffer->is_device_visible()) {
    return nullptr;
  }

  return buffer;
}
