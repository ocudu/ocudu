// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../test_utils/config_generators.h"
#include "lib/scheduler/config/du_cell_group_config_pool.h"
#include "lib/scheduler/ue_context/ue_repository.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include <gtest/gtest.h>

namespace ocudu {

class dummy_harq_timeout_notifier : public harq_timeout_notifier
{
public:
  void on_harq_timeout(du_ue_index_t ue_idx, bool is_dl, bool ack) override {}
  void on_feedback_disabled_harq_timeout(du_ue_index_t ue_idx, bool is_dl, units::bytes tbs) override {}
};

class sched_basic_custom_test_bench
{
public:
  sched_basic_custom_test_bench(
      const scheduler_expert_config&                                 sched_exp_cfg,
      const cell_config_builder_params&                              builder_params,
      const std::optional<sched_cell_configuration_request_message>& sched_cell_cfg_req = {}) :
    expert_cfg{sched_exp_cfg},
    cfg_mng{builder_params, expert_cfg},
    cell_cfg(*cfg_mng.add_cell(sched_cell_cfg_req.has_value() ? *sched_cell_cfg_req
                                                              : cfg_mng.get_default_cell_config_request())),
    ues(expert_cfg.ue),
    cell_ues(ues.add_cell(cell_cfg, nullptr)),
    current_sl_tx{to_numerology_value(cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params.scs), 0}
  {
    slot_indication(current_sl_tx);
    mac_logger.set_level(ocudulog::basic_levels::debug);

    ocudulog::init();
  }

  // This constructor uses default cell_config_builder_params parameters, which will be overwritten by those in
  // sched_cell_cfg_req.
  sched_basic_custom_test_bench(const scheduler_expert_config&                  sched_exp_cfg,
                                const sched_cell_configuration_request_message& sched_cell_cfg_req) :
    sched_basic_custom_test_bench(sched_exp_cfg, cell_config_builder_params{}, sched_cell_cfg_req)
  {
  }

  // Class members.
  scheduler_expert_config                 expert_cfg;
  test_helpers::test_sched_config_manager cfg_mng;
  const cell_configuration&               cell_cfg;
  ue_repository                           ues;
  ue_cell_repository&                     cell_ues;
  std::vector<const ue_configuration*>    ue_ded_cfgs;
  cell_resource_allocator                 res_grid{cell_cfg};
  slot_point                              current_sl_tx;

  ocudulog::basic_logger& mac_logger  = ocudulog::fetch_basic_logger("SCHED", true);
  ocudulog::basic_logger& test_logger = ocudulog::fetch_basic_logger("TEST");

  // Class methods.
  void add_ue(const sched_ue_creation_request_message& ue_req)
  {
    ue_ded_cfgs.emplace_back(cfg_mng.add_ue(ue_req));
    report_error_if_not(ue_ded_cfgs.back() != nullptr, "Failed to create UE configuration");
    ues.add_ue(*ue_ded_cfgs.back(), ue_req.starts_in_fallback, ue_req.ul_ccch_slot_rx, ue_req.cfra_enabled);
  }

  void slot_indication(slot_point slot_tx)
  {
    mac_logger.set_context(slot_tx.sfn(), slot_tx.slot_index());
    test_logger.set_context(slot_tx.sfn(), slot_tx.slot_index());
    res_grid.slot_indication(slot_tx);
  }
};

} // namespace ocudu
