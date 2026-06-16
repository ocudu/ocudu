// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "sub_scheduler_test_environment.h"
#include "lib/scheduler/logging/cell_metrics_handler.h"
#include "lib/scheduler/logging/scheduler_result_logger.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include <gtest/gtest.h>

using namespace ocudu;

sub_scheduler_test_environment::sub_scheduler_test_environment(const sched_cell_configuration_request_message& cell_req,
                                                               unsigned delay_tx_rx_slots_) :
  sub_scheduler_test_environment(config_helpers::make_default_scheduler_expert_config(),
                                 cell_req,
                                 nullptr,
                                 create_pdcch_slot_ind_task<pdcch_resource_allocator_impl>(),
                                 delay_tx_rx_slots_)
{
}

sub_scheduler_test_environment::sub_scheduler_test_environment(scheduler_expert_config sched_cfg_,
                                                               const sched_cell_configuration_request_message& cell_req,
                                                               unsigned delay_tx_rx_slots_) :
  sub_scheduler_test_environment(std::move(sched_cfg_),
                                 cell_req,
                                 nullptr,
                                 create_pdcch_slot_ind_task<pdcch_resource_allocator_impl>(),
                                 delay_tx_rx_slots_)
{
}

static uint8_t derive_max_k_value(const cell_configuration& cell_cfg)
{
  uint8_t     max_k_value = 0;
  const auto& dl_lst      = cell_cfg.params.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
  for (const auto& pdsch : dl_lst) {
    max_k_value = std::max<uint8_t>(pdsch.k0, max_k_value);
  }
  const auto&        ul_lst         = cell_cfg.params.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
  constexpr unsigned max_msg3_delta = 6;
  for (const auto& pusch : ul_lst) {
    max_k_value = std::max<uint8_t>(max_k_value, pusch.k2 + max_msg3_delta);
  }
  return max_k_value;
}

sub_scheduler_test_environment::sub_scheduler_test_environment(
    scheduler_expert_config                         sched_cfg_,
    const sched_cell_configuration_request_message& cell_req,
    std::unique_ptr<pdcch_resource_allocator>       custom_pdcch_alloc,
    std::function<void(slot_point)>                 pdcch_alloc_sl_ind_task,
    unsigned                                        delay_tx_rx_slots_) :
  sched_cfg(std::move(sched_cfg_)),
  cfg_mng(sched_cfg),
  cell_cfg(*cfg_mng.add_cell(cell_req)),
  pdcch_alloc(custom_pdcch_alloc == nullptr ? std::make_unique<pdcch_resource_allocator_impl>(cell_cfg)
                                            : std::move(custom_pdcch_alloc)),
  delay_tx_rx_slots(delay_tx_rx_slots_),
  max_k_value(derive_max_k_value(cell_cfg)),
  pdcch_alloc_slot_ind_fn(std::move(pdcch_alloc_sl_ind_task))
{
  ocudulog::init();
}

void sub_scheduler_test_environment::run_slot()
{
  mac_logger.set_context(next_slot.sfn(), next_slot.slot_index());
  test_logger.set_context(next_slot.sfn(), next_slot.slot_index());

  res_grid.slot_indication(next_slot);

  // pdcch allocator slot indication.
  pdcch_alloc_slot_ind_fn(next_slot);

  // Run slot for the derived class.
  do_run_slot();

  result_logger.on_scheduler_result(res_grid[0].result);

  // Check sched result consistency.
  ASSERT_NO_FATAL_FAILURE(test_scheduler_result_consistency(cell_cfg, res_grid));
  ++next_slot;
}

void sub_scheduler_test_environment::flush_events()
{
  // Log pending allocations before finishing test.
  for (unsigned i = 0; i != max_k_value; ++i) {
    run_slot();
  }
  ocudulog::flush();
}
