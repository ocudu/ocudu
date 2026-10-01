// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"

namespace ocudu {
namespace odu {

struct du_manager_params;
struct du_manager_context;
class du_cell_manager;
class du_ue_manager;
class du_ran_resource_manager;
class du_manager_metrics_aggregator_impl;

struct du_proc_context_view {
  /// Static parameters of the DU.
  du_manager_params&                  params;
  du_manager_context&                 ctxt;
  du_cell_manager&                    cell_mng;
  du_ue_manager&                      ue_mng;
  du_ran_resource_manager&            res_mng;
  du_manager_metrics_aggregator_impl& metrics;
  ocudulog::basic_logger&             logger;
};

} // namespace odu
} // namespace ocudu
