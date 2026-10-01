// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/hal/dpdk/bbdev/bbdev_op_pool.h"
#include "ocudu/ocudulog/logger.h"
#include <memory>
#include <rte_bbdev_op.h>

namespace ocudu {
namespace dpdk {

/// Returns a bbdev_op_pool instance on success, otherwise returns nullptr.
std::unique_ptr<bbdev_op_pool> create_bbdev_op_pool(const char*             pool_name,
                                                    ::rte_bbdev_op_type     op_type,
                                                    uint16_t                nof_elements,
                                                    int                     socket,
                                                    ocudulog::basic_logger& logger);

} // namespace dpdk
} // namespace ocudu
