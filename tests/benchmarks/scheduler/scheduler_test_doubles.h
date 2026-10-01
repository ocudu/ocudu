// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/scheduler/scheduler_configurator.h"
#include "ocudu/scheduler/scheduler_metrics.h"

namespace ocudu {

class sched_cfg_dummy_notifier : public sched_configuration_notifier
{
public:
  void on_ue_config_complete(du_ue_index_t ue_index, bool ue_creation_result) override {}
  void on_ue_deletion_completed(du_ue_index_t ue_index) override {}
};

class sched_dummy_metric_notifier final : public scheduler_metrics_notifier
{
public:
  void report_metrics(const scheduler_cell_metrics& metrics) override {}
};

} // namespace ocudu
