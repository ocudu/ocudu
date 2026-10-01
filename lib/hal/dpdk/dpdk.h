// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Definition of the common functions used by all ACC100-accelerated channel processor function implementations.

#pragma once

#include "ocudu/hal/dpdk/dpdk_eal.h"
#include "ocudu/ocudulog/logger.h"
#include <rte_eal.h>
#include <rte_mbuf.h>

namespace ocudu {
namespace dpdk {

/// \file
/// \brief Common DPDK support functions.

/// EAL initialization (called at application init).
/// \param[in]  argc, argv Command line arguments.
/// \param[in]  logger     OCUDU logger.
bool eal_init(int argc, char** argv, ocudulog::basic_logger& logger);

/// Creates a new memory pool for a given hardware-accelerator device.
/// \param[in]  pool_name          Unique ID for the new mbuf pool.
/// \param[in]  socket             Socket used by the bbdev accelerator.
/// \param[in]  n_mbuf             Number of elements in the mbuf pool.
/// \param[in]  mempool_cache_size Size of the per-core object cache.
/// \param[in]  mbuf_data_size     Size of data buffer in each mbuf.
/// \param[in]  logger             OCUDU logger.
/// \return Pointer to the created mbuf pool.
rte_mempool* create_mem_pool(const char*             pool_name,
                             int                     socket,
                             unsigned                n_mbuf,
                             unsigned                mempool_cache_size,
                             unsigned                mbuf_data_size,
                             ocudulog::basic_logger& logger);

} // namespace dpdk
} // namespace ocudu
