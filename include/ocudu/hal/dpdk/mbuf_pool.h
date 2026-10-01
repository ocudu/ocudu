// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/ocudu_assert.h"
#include <rte_mempool.h>

namespace ocudu {
namespace dpdk {

/// RAII wrapping for dpdk memory buffer pool structures.
class mbuf_pool
{
public:
  /// Constructor.
  /// \param[in] pool_ Pointer to a dpdk memory pool.
  explicit mbuf_pool(::rte_mempool* pool_) : pool(*pool_) { ocudu_assert(pool_, "Invalid mbuf pool."); }

  /// Destructor.
  ~mbuf_pool()
  {
    // Free the memory buffer pool.
    ::rte_mempool_free(&pool);
  }

  /// Returns a pointer to the actual memory pool object.
  /// \return Pointer to the memory pool.
  ::rte_mempool* get_pool() { return &pool; }

  /// Returns a pointer to a constant memory pool object.
  /// \return Pointer to a constant memory pool.
  const ::rte_mempool* get_pool() const { return &pool; }

private:
  ::rte_mempool& pool;
};

} // namespace dpdk
} // namespace ocudu
