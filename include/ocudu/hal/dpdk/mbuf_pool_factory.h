// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/hal/dpdk/mbuf_pool.h"
#include "ocudu/ocudulog/logger.h"
#include <memory>

namespace ocudu {
namespace dpdk {

/// Configuration structure of a dpdk memory buffer pool.
struct mempool_config {
  /// Number of elements in the mbuf pool.
  unsigned n_mbuf = 2048;
  /// Size of the per-core object cache.
  unsigned mempool_cache_size = 128;
  /// Size of data buffer in each mbuf.
  unsigned mbuf_data_size = 2048;
};

/// Returns a mbuf_pool instance on success, otherwise returns nullptr.
std::unique_ptr<mbuf_pool>
create_mbuf_pool(const char* pool_name, int socket, const mempool_config& cfg, ocudulog::basic_logger& logger);

} // namespace dpdk
} // namespace ocudu
