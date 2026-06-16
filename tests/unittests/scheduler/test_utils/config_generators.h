// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lib/scheduler/config/sched_config_manager.h"
#include "lib/scheduler/logging/cell_metrics_handler.h"
#include "ocudu/scheduler/config/cell_config_builder_params.h"

namespace ocudu {

class sched_metrics_ue_configurator;

namespace test_helpers {

/// Helper class to manage cell and UE configurations of a scheduler test.
class test_sched_config_manager
{
public:
  test_sched_config_manager(const scheduler_expert_config& expert_cfg_ = {});
  test_sched_config_manager(const cell_config_builder_params& builder_params = {},
                            const scheduler_expert_config&    expert_cfg_    = {});
  ~test_sched_config_manager();

  const sched_cell_configuration_request_message& get_default_cell_config_request() const { return default_cell_req; }
  const sched_ue_creation_request_message&        get_default_ue_config_request() const { return default_ue_req; }

  const cell_configuration* add_cell(const sched_cell_configuration_request_message& msg);

  const cell_configuration& get_cell(du_cell_index_t cell_idx) const { return *cfg_mng.common_cell_list()[cell_idx]; }

  const ue_configuration* add_ue(const sched_ue_creation_request_message& cfg_req);
  const ue_configuration* update_ue(const sched_ue_reconfiguration_message& cfg_req);
  bool                    rem_ue(du_ue_index_t ue_index);

  const cell_common_configuration_list& common_cell_list() const { return cfg_mng.common_cell_list(); }

private:
  scheduler_expert_config                        expert_cfg;
  std::unique_ptr<sched_configuration_notifier>  cfg_notifier;
  std::unique_ptr<scheduler_metrics_notifier>    metric_notifier;
  std::unique_ptr<sched_metrics_ue_configurator> ue_metrics_configurator;

  sched_cell_configuration_request_message default_cell_req;
  sched_ue_creation_request_message        default_ue_req;

  sched_config_manager cfg_mng;
};

} // namespace test_helpers
} // namespace ocudu
