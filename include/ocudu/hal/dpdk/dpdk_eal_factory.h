// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/hal/dpdk/dpdk_eal.h"
#include <memory>

namespace ocudu {
namespace dpdk {

/// \brief Returns a dpdk_eal instance on success, otherwise returns nullptr.
///
/// When \c enable_pdump_init is set to true, the DPDK pdump library, which is required to capture packets on DPDK
/// ports with the dpdk-pdump tool, is initialized right after the EAL.
std::unique_ptr<dpdk_eal>
create_dpdk_eal(const std::string& args, ocudulog::basic_logger& logger, bool enable_pdump_init = false);

} // namespace dpdk
} // namespace ocudu
