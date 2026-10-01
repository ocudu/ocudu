// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <string>

namespace ocudu {
namespace ether {

/// Configuration for the Ethernet receiver.
struct receiver_config {
  /// Ethernet interface name or identifier.
  std::string interface;
  /// Promiscuous mode flag.
  bool is_promiscuous_mode_enabled;
  /// If set to true, metrics are enabled in the Ethernet receiver.
  bool are_metrics_enabled = false;
};

} // namespace ether
} // namespace ocudu
