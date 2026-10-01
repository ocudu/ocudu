// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/hal/dpdk/bbdev/bbdev_op_pool_factory.h"
#include "ocudu/hal/dpdk/bbdev/bbdev.h"

using namespace ocudu;
using namespace dpdk;

std::unique_ptr<bbdev_op_pool> ocudu::dpdk::create_bbdev_op_pool(const char*             pool_name,
                                                                 ::rte_bbdev_op_type     op_type,
                                                                 uint16_t                nof_elements,
                                                                 int                     socket,
                                                                 ocudulog::basic_logger& logger)
{
  // Create a new bbdev operation pool.
  ::rte_mempool* pool = ::create_op_pool(pool_name, op_type, nof_elements, socket, logger);
  if (pool == nullptr) {
    return nullptr;
  }

  return std::make_unique<bbdev_op_pool>(pool);
}
