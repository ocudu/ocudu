// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/hal/dpdk/dpdk_eal.h"

#if defined(DPDK_PDUMP_AVAILABLE)
#include <rte_pdump.h>
#endif
#include <rte_eal.h>

using namespace ocudu;
using namespace dpdk;

dpdk_eal::dpdk_eal(ocudulog::basic_logger& logger_, bool enable_pdump_init) : logger(logger_)
{
#if defined(DPDK_PDUMP_AVAILABLE)
  if (enable_pdump_init && ::rte_pdump_init() < 0) {
    logger.warning("dpdk: failed to initialize the pdump library");
  } else {
    pdump_initialized = enable_pdump_init;
  }
#endif
}

dpdk_eal::~dpdk_eal()
{
#if defined(DPDK_PDUMP_AVAILABLE)
  if (pdump_initialized && ::rte_pdump_uninit() < 0) {
    logger.warning("dpdk: failed to uninitialize the pdump library");
  }
#endif
  // Clean up the EAL.
  ::rte_eal_cleanup();
}
