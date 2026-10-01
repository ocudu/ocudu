// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_up/cu_up_config.h"
#include "ocudu/e2/e2_cu.h"
#include "ocudu/e2/e2ap_config.h"
#include "ocudu/e2/gateways/e2_connection_client.h"

namespace ocudu {
namespace ocuup {

/// O-RAN CU-UP configuration.
struct o_cu_up_config {
  /// CU-UP configuration.
  cu_up_config cu_up_cfg;
  /// E2AP configuration.
  e2ap_config e2ap_cfg;
};

/// O-RAN CU-UP dependencies.
struct o_cu_up_dependencies {
  /// CU-UP dependencies.
  cu_up_dependencies cu_dependencies;
  /// E2 CU metrics interface.
  e2_cu_metrics_interface* e2_cu_metric_iface = nullptr;
  /// E2 connection client.
  e2_connection_client* e2_client = nullptr;
};

} // namespace ocuup
} // namespace ocudu
