// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/hal/dpdk/mbuf_pool_factory.h"
#include "dpdk.h"

using namespace ocudu;
using namespace dpdk;

std::unique_ptr<mbuf_pool> ocudu::dpdk::create_mbuf_pool(const char*             pool_name,
                                                         int                     socket,
                                                         const mempool_config&   cfg,
                                                         ocudulog::basic_logger& logger)
{
  // Create a new memory pool.
  ::rte_mempool* pool =
      create_mem_pool(pool_name, socket, cfg.n_mbuf, cfg.mempool_cache_size, cfg.mbuf_data_size, logger);
  if (pool == nullptr) {
    return nullptr;
  }

  return std::make_unique<mbuf_pool>(pool);
}
