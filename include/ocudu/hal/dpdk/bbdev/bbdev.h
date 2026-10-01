// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Definition of the common functions used by all bbdeb-based hardware-accelerated channel processor
/// implementations.

#pragma once

#include "ocudu/adt/expected.h"
#include "ocudu/hal/dpdk/bbdev/bbdev_acc.h"
#include <rte_bbdev.h>
#include <rte_bbdev_op.h>
#include <rte_mempool.h>

namespace ocudu {
namespace dpdk {

/// \brief bbdev initialization.
///
/// \param[in]  cfg    Configuration of the bbdev-based hardware-accelerator.
/// \param[in]  logger OCUDU logger.
/// \return BBDEV device information if successful, otherwise returns an empty structure.
expected<::rte_bbdev_info> bbdev_start(const bbdev_acc_configuration& cfg, ocudulog::basic_logger& logger);

/// \brief bbdev shutdown management.
///
/// \param[in]  dev_id ID of the bbdev-based hardware-accelerator.
/// \param[in]  logger OCUDU logger.
bool bbdev_stop(unsigned dev_id, ocudulog::basic_logger& logger);

/// \brief Creates a new op pool for a given hardware-accelerator device.
///
/// \param[in]  pool_name     Unique ID for the new mbuf pool.
/// \param[in]  op_type       Type of bbdev op.
/// \param[in]  nof_elements  Number of elements in the op pool.
/// \param[in]  socket        Socket used by the bbdev accelerator.
/// \param[in]  logger        OCUDU logger.
/// \return Pointer to the created op pool.
rte_mempool* create_op_pool(const char*             pool_name,
                            ::rte_bbdev_op_type     op_type,
                            uint16_t                nof_elements,
                            int                     socket,
                            ocudulog::basic_logger& logger);

} // namespace dpdk
} // namespace ocudu
