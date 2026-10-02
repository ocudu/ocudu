// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

// Definition used when the build has no CUDA support. It always returns nullptr, so callers can
// call create_resource_grid_factory_cuda_visible() unconditionally and check the result, instead of
// guarding the call with #ifdef.

#include "ocudu/phy/support/support_factories.h"

using namespace ocudu;

std::shared_ptr<resource_grid_factory>
ocudu::create_resource_grid_factory_cuda_visible(std::shared_ptr<resource_grid_factory>          fallback_factory,
                                                 const cuda_visible_resource_grid_configuration& config)
{
  (void)fallback_factory;
  (void)config;
  return nullptr;
}
