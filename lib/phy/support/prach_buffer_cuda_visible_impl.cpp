// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "prach_buffer_cuda_visible_impl.h"
#include "ocudu/support/error_handling.h"
#include <cuda_runtime.h>
#include <sched.h>

using namespace ocudu;

/// Number of completion events the pool starts with, enough for the occasions of one slot.

/// Device-side reading contract of the buffer.

prach_buffer_cuda_visible_impl::prach_buffer_cuda_visible_impl(unsigned max_nof_ports_,
                                                               unsigned max_nof_td_occasions_,
                                                               unsigned max_nof_fd_occasions_,
                                                               unsigned max_nof_symbols_,
                                                               unsigned sequence_length_,
                                                               const cuda_visible_prach_buffer_configuration& config_) :
  max_nof_ports(max_nof_ports_),
  max_nof_td_occasions(max_nof_td_occasions_),
  max_nof_fd_occasions(max_nof_fd_occasions_),
  max_nof_symbols(max_nof_symbols_),
  sequence_length(sequence_length_),
  config(config_),
  device_properties(cuda::get_cuda_device_properties(config_.device_id))
{
  const std::size_t nof_elements = static_cast<std::size_t>(max_nof_ports) * max_nof_td_occasions *
                                   max_nof_fd_occasions * max_nof_symbols * sequence_length;
  if (nof_elements == 0) {
    return;
  }

  buffer_tensor.resize(buffer_tensor_type::dimensions_size_type{
      sequence_length, max_nof_symbols, max_nof_fd_occasions, max_nof_td_occasions, max_nof_ports});
  if (!buffer_tensor.is_valid()) {
    return;
  }

  (void)buffer_tensor.advise_accessed_by(cuda::memory_location::device, config.device_id);
}

prach_buffer_cuda_visible_impl::~prach_buffer_cuda_visible_impl()
{
  // The allocation is released by its own destructor. A consumer that enqueued work on it
  // synchronises before the buffer is dropped, which is its own responsibility.
}

unsigned prach_buffer_cuda_visible_impl::get_symbol_offset(unsigned i_port,
                                                           unsigned i_td_occasion,
                                                           unsigned i_fd_occasion,
                                                           unsigned i_symbol) const
{
  ocudu_assert(i_port < max_nof_ports,
               "The port index (i.e., {}) exceeds the maximum number of ports (i.e., {})",
               i_port,
               max_nof_ports);
  ocudu_assert(i_td_occasion < max_nof_td_occasions,
               "The time-domain occasion (i.e., {}) exceeds the maximum number of time-domain occasions (i.e., {})",
               i_td_occasion,
               max_nof_td_occasions);
  ocudu_assert(
      i_fd_occasion < max_nof_fd_occasions,
      "The frequency-domain occasion (i.e., {}) exceeds the maximum number of frequency-domain occasions (i.e., {})",
      i_fd_occasion,
      max_nof_fd_occasions);
  ocudu_assert(i_symbol < max_nof_symbols,
               "The symbol index (i.e., {}) exceeds the maximum number of symbols (i.e., {})",
               i_symbol,
               max_nof_symbols);

  return sequence_length *
         (i_symbol +
          max_nof_symbols * (i_fd_occasion + max_nof_fd_occasions * (i_td_occasion + max_nof_td_occasions * i_port)));
}

span<cbf16_t> prach_buffer_cuda_visible_impl::get_symbol(unsigned i_port,
                                                         unsigned i_td_occasion,
                                                         unsigned i_fd_occasion,
                                                         unsigned i_symbol)
{
  return buffer_tensor.get_view({i_symbol, i_fd_occasion, i_td_occasion, i_port});
}

span<const cbf16_t> prach_buffer_cuda_visible_impl::get_symbol(unsigned i_port,
                                                               unsigned i_td_occasion,
                                                               unsigned i_fd_occasion,
                                                               unsigned i_symbol) const
{
  return buffer_tensor.get_view({i_symbol, i_fd_occasion, i_td_occasion, i_port});
}

span<const cbf16_t> prach_buffer_cuda_visible_impl::get_buffer() const
{
  return buffer_tensor.get_data();
}
