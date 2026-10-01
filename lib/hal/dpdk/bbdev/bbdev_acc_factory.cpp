// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/hal/dpdk/bbdev/bbdev_acc_factory.h"
#include "ocudu/hal/dpdk/bbdev/bbdev.h"

using namespace ocudu;
using namespace dpdk;

std::shared_ptr<bbdev_acc> ocudu::dpdk::create_bbdev_acc(const bbdev_acc_configuration& cfg,
                                                         ocudulog::basic_logger&        logger)
{
  // bbdev device start procedure.
  expected<::rte_bbdev_info> info = bbdev_start(cfg, logger);
  if (not info.has_value()) {
    return nullptr;
  }

  return std::make_shared<bbdev_acc>(cfg, info.value(), logger);
}
