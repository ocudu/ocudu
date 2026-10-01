// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "resource_grid_cuda_visible_impl.h"
#include "ocudu/ocuduvec/zero.h"
#include "ocudu/ran/resource_block.h"
#include "ocudu/support/math/math_utils.h"
#include <cuda_runtime.h>
#include <sched.h>

using namespace ocudu;

resource_grid_cuda_visible_impl::resource_grid_cuda_visible_impl(
    unsigned                                        nof_ports_,
    unsigned                                        nof_symbols_,
    unsigned                                        nof_subc_,
    const cuda_visible_resource_grid_configuration& config_) :
  alloc_mask(nof_ports_, nof_symbols_, divide_ceil(nof_subc_, NOF_SUBCARRIERS_PER_RB)),
  config(config_),
  device_properties(cuda::get_cuda_device_properties(config_.device_id)),
  nof_ports(nof_ports_),
  nof_symbols(nof_symbols_),
  nof_subc(nof_subc_)
{
  // Writing from both sides at once is only sound where the GPU lets them hold the allocation
  // together, and only where the pages are not being migrated underneath the writers.
  config.enable_concurrent_downlink_writes = config.enable_concurrent_downlink_writes && !config.enable_prefetch &&
                                             device_properties.concurrent_managed_access;

  grid_tensor.resize(grid_tensor_type::dimensions_size_type{nof_subc, nof_symbols, nof_ports});
  if (!grid_tensor.is_valid()) {
    return;
  }

  // Tell the driver where the elements are expected to live, so that the first touch does not
  // decide it.
  (void)grid_tensor.advise_accessed_by(cuda::memory_location::device, config.device_id);

  reader = std::make_unique<resource_grid_reader_impl>(grid_tensor, alloc_mask);
  writer = std::make_unique<resource_grid_writer_impl>(grid_tensor, alloc_mask);

  // cudaMallocManaged() does not zero the allocation, and the clear below is what the grid contract
  // promises, so it happens here rather than on the first transmission.
  for (unsigned port = 0; port != nof_ports; ++port) {
    ocuduvec::zero(grid_tensor.template get_view<static_cast<unsigned>(resource_grid_dimensions::port)>({port}));
  }
}

resource_grid_cuda_visible_impl::~resource_grid_cuda_visible_impl()
{
  // The allocation is released by its own destructor, but only once no device work still reads or
  // writes it, which is what this wait is for.
  if (maintenance_stream.is_valid()) {
    (void)maintenance_stream.synchronize();
  }
}

bool resource_grid_cuda_visible_impl::clear_on_device()
{
  if (grid_tensor.get_data().empty() || !maintenance_stream.is_valid()) {
    return false;
  }

  std::lock_guard<std::mutex> lock(clear_mutex);

  if (::cudaMemsetAsync(grid_tensor.get_data().data(),
                        0,
                        grid_tensor.get_data().size() * sizeof(cbf16_t),
                        static_cast<::cudaStream_t>(maintenance_stream.native())) != cudaSuccess) {
    return false;
  }

  // The next producer of this grid is not told when the clear finishes, so the clear finishes here.
  // A grid is cleared between transmissions rather than during one, so the wait is off the path
  // that carries data.
  if (!maintenance_stream.synchronize().has_value()) {
    return false;
  }

  return true;
}

void resource_grid_cuda_visible_impl::set_all_zero()
{
  if (alloc_mask.is_empty()) {
    return;
  }

  if (clear_on_device()) {
    alloc_mask.reset_all();
    return;
  }

  for (unsigned port = 0; port != nof_ports; ++port) {
    if (!alloc_mask.is_port_empty(port)) {
      ocuduvec::zero(grid_tensor.template get_view<static_cast<unsigned>(resource_grid_dimensions::port)>({port}));
    }
  }
  alloc_mask.reset_all();
}

resource_grid_writer& resource_grid_cuda_visible_impl::get_writer()
{
  return *writer;
}

const resource_grid_reader& resource_grid_cuda_visible_impl::get_reader() const
{
  return *reader;
}
