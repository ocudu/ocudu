// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocudulog/logger.h"

namespace ocudu {
namespace dpdk {

/// Interfacing to DPDK's EAL.
class dpdk_eal
{
public:
  /// Constructor.
  /// \param[in] logger OCUDU logger.
  /// \param[in] enable_pdump_init When set to true, initializes the DPDK pdump library, which is required to capture
  /// packets on DPDK ports with the dpdk-pdump tool.
  explicit dpdk_eal(ocudulog::basic_logger& logger_, bool enable_pdump_init = false);

  /// Destructor.
  ~dpdk_eal();

  // Returns the internal OCUDU logger.
  /// \return OCUDU logger.
  ocudulog::basic_logger& get_logger() { return logger; }

private:
  /// OCUDU logger.
  ocudulog::basic_logger& logger;
  /// Indicates whether the DPDK pdump library was successfully initialized.
  bool pdump_initialized = false;
};

} // namespace dpdk
} // namespace ocudu
