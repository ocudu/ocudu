// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cu_configurator.h"
#include "ocudu/cu_cp/cu_cp_configuration.h"
#include "ocudu/e2/e2_cu.h"
#include "ocudu/e2/e2ap_configuration.h"
#include "ocudu/e2/gateways/e2_connection_client.h"

namespace ocudu::ocucp {

/// O-RAN CU-CP configuration.
struct o_cu_cp_config {
  /// CU-CP configuration.
  cu_cp_configuration cu_cp_config;
  /// E2AP configuration.
  e2ap_configuration e2ap_config;
};

/// O-RAN CU-CP dependencies.
struct o_cu_cp_dependencies {
  /// E2 connection client.
  e2_connection_client* e2_client = nullptr;
  /// E2 CU metrics interface.
  e2_cu_metrics_interface* e2_cu_metric_iface = nullptr;
  // E2 CU configurator.
  cu_configurator* cu_cfg = nullptr;
};

} // namespace ocudu::ocucp
