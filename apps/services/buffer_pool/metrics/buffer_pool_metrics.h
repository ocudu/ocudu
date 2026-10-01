// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstddef>

namespace ocudu {

/// Buffer pool metrics.
class buffer_pool_metrics_impl final
{
  size_t central_cache_size;

public:
  explicit buffer_pool_metrics_impl(size_t central_cache_size_) : central_cache_size(central_cache_size_) {}

  /// Returns current central cache size of the segment pool.
  size_t get_central_cache_size() const { return central_cache_size; }
};

} // namespace ocudu
