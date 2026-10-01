// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "resource_grid_cuda_visible_impl.h"
#include "ocudu/cuda/support/cuda_device.h"
#include "ocudu/phy/support/support_factories.h"
#include "ocudu/support/error_handling.h"
#include <cuda_runtime.h>

using namespace ocudu;

namespace {

/// \brief Factory of resource grids shared with an NVIDIA GPU.
///
/// A grid that fails to obtain its shared allocation is not an error the caller has to handle: the
/// factory falls back to an ordinary host grid, and the accelerated paths find no device contract
/// on it and leave it alone.
class resource_grid_cuda_visible_factory : public resource_grid_factory
{
public:
  resource_grid_cuda_visible_factory(std::shared_ptr<resource_grid_factory>          fallback_factory_,
                                     const cuda_visible_resource_grid_configuration& config_) :
    fallback_factory(std::move(fallback_factory_)), config(config_)
  {
    ocudu_assert(fallback_factory, "Invalid fallback resource grid factory");
  }

  std::unique_ptr<resource_grid> create(unsigned nof_ports, unsigned nof_symbols, unsigned nof_subc) override
  {
    auto grid = std::make_unique<resource_grid_cuda_visible_impl>(nof_ports, nof_symbols, nof_subc, config);
    if (grid->is_device_visible()) {
      return grid;
    }

    return fallback_factory->create(nof_ports, nof_symbols, nof_subc);
  }

private:
  std::shared_ptr<resource_grid_factory>   fallback_factory;
  cuda_visible_resource_grid_configuration config;
};

} // namespace

std::shared_ptr<resource_grid_factory>
ocudu::create_resource_grid_factory_cuda_visible(std::shared_ptr<resource_grid_factory>          fallback_factory,
                                                 const cuda_visible_resource_grid_configuration& config)
{
  if (!cuda::is_cuda_device_available()) {
    return nullptr;
  }

  cuda_visible_resource_grid_configuration resolved = config;

  // Keeping the pages on the device only pays off where the host may still reach them. Where it may
  // not, every host access would migrate the whole grid back, so the default follows the device.
  if (!resolved.prefer_device_residency) {
    resolved.prefer_device_residency = cuda::get_cuda_device_properties(resolved.device_id).concurrent_managed_access;
  }

  return std::make_shared<resource_grid_cuda_visible_factory>(std::move(fallback_factory), resolved);
}
