// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lib/scheduler/cell/resource_grid.h"
#include "lib/scheduler/config/cell_configuration.h"
#include "lib/scheduler/logging/cell_metrics_handler.h"
#include "lib/scheduler/logging/scheduler_event_logger.h"
#include "lib/scheduler/logging/scheduler_result_logger.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator.h"
#include "lib/scheduler/pucch_scheduling/pucch_allocator_impl.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/sched_random_utils.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/slot_point_extended.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"

namespace ocudu {

/// Common base class used by different types of sub-scheduler tests.
class sub_scheduler_test_environment
{
  /// Helper method to type-erase pdcch_allocator::slot_indication method (it is not part of the interface).
  template <typename PdcchAllocType>
  std::function<void(slot_point)> create_pdcch_slot_ind_task()
  {
    return [this](slot_point sl_tx) { static_cast<PdcchAllocType&>(*pdcch_alloc).slot_indication(sl_tx); };
  }

  sub_scheduler_test_environment(scheduler_expert_config                         sched_cfg_,
                                 const sched_cell_configuration_request_message& cell_req,
                                 std::unique_ptr<pdcch_resource_allocator>       pdcch_alloc,
                                 std::function<void(slot_point)>                 pdcch_alloc_sl_ind_task,
                                 unsigned                                        delay_tx_rx_slots_);

public:
  sub_scheduler_test_environment(const sched_cell_configuration_request_message& cell_req,
                                 unsigned                                        delay_tx_rx_slots_ = 2);
  sub_scheduler_test_environment(scheduler_expert_config                         sched_cfg_,
                                 const sched_cell_configuration_request_message& cell_req,
                                 unsigned                                        delay_tx_rx_slots_ = 2);
  template <typename PdcchAllocType>
  sub_scheduler_test_environment(const sched_cell_configuration_request_message& cell_req,
                                 std::unique_ptr<PdcchAllocType>                 custom_pdcch_alloc,
                                 unsigned                                        delay_tx_rx_slots_ = 2) :
    sub_scheduler_test_environment(config_helpers::make_default_scheduler_expert_config(),
                                   cell_req,
                                   std::move(custom_pdcch_alloc),
                                   create_pdcch_slot_ind_task<PdcchAllocType>(),
                                   delay_tx_rx_slots_)
  {
  }
  template <typename PdcchAllocType>
  sub_scheduler_test_environment(scheduler_expert_config                         sched_cfg_,
                                 const sched_cell_configuration_request_message& cell_req,
                                 std::unique_ptr<PdcchAllocType>                 custom_pdcch_alloc,
                                 unsigned                                        delay_tx_rx_slots_ = 2) :
    sub_scheduler_test_environment(std::move(sched_cfg_),
                                   cell_req,
                                   std::move(custom_pdcch_alloc),
                                   create_pdcch_slot_ind_task<PdcchAllocType>(),
                                   delay_tx_rx_slots_)
  {
  }
  virtual ~sub_scheduler_test_environment() = default;

  slot_point          last_slot_tx() const { return next_slot.without_hyper_sfn() - 1; }
  slot_point          next_slot_rx() const { return next_slot.without_hyper_sfn() - delay_tx_rx_slots; }
  const sched_result& last_sched_result() const { return res_grid[0].result; }

  void run_slot();

  template <typename Func>
  bool run_slot_until(const Func& func = {}, unsigned max_slots = 1000)
  {
    if (func()) {
      return true;
    }
    for (unsigned i = 0; i != max_slots; ++i) {
      run_slot();
      if (func()) {
        return true;
      }
    }
    return false;
  }
  /// It calls the subscheduler run_slot method.
  /// Implemented in the derived class.
  virtual void do_run_slot() = 0;

  /// Helper function to flush pending events and logs.
  void flush_events();

  ocudulog::basic_logger& mac_logger  = ocudulog::fetch_basic_logger("SCHED", true);
  ocudulog::basic_logger& test_logger = ocudulog::fetch_basic_logger("TEST", true);

  scheduler_expert_config                   sched_cfg{config_helpers::make_default_scheduler_expert_config()};
  test_helpers::test_sched_config_manager   cfg_mng;
  const cell_configuration&                 cell_cfg;
  scheduler_event_logger                    ev_logger{cell_cfg.cell_index, cell_cfg.params.pci};
  cell_metrics_handler                      metrics_hdlr{cell_cfg, std::nullopt};
  cell_resource_allocator                   res_grid{cell_cfg};
  scheduler_result_logger                   result_logger{true, cell_cfg.params.pci};
  std::unique_ptr<pdcch_resource_allocator> pdcch_alloc;
  pucch_allocator_impl                      pucch_alloc;
  const unsigned                            delay_tx_rx_slots;

  // -- Derived
  /// Maximum of all k values.
  /// \note We use this value to account for the case when the PDSCH or PUSCH is allocated several slots in advance.
  const uint8_t max_k_value;

  // -- State

  slot_point_extended next_slot{test_helper::generate_random_slot_point(cell_cfg.scs_common())};

private:
  /// Type erased run_slot for pdcch_allocator.
  /// \note we need this because slot_indication is not part of the pdcch_resource_allocator interface.
  std::function<void(slot_point)> pdcch_alloc_slot_ind_fn;
};

} // namespace ocudu
