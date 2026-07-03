// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "scheduler_test_simulator.h"
#include "../test_utils/indication_generators.h"
#include "sched_random_utils.h"
#include "scheduler_test_suite.h"
#include "ocudu/scheduler/config/scheduler_config.h"
#include "ocudu/scheduler/scheduler_factory.h"

using namespace ocudu;

scheduler_expert_config ocudu::make_custom_scheduler_expert_config(bool enable_csi_rs_pdsch_multiplexing)
{
  scheduler_expert_config exp_cfg             = config_helpers::make_default_scheduler_expert_config();
  exp_cfg.ue.enable_csi_rs_pdsch_multiplexing = enable_csi_rs_pdsch_multiplexing;
  return exp_cfg;
}

scheduler_test_simulator::scheduler_test_simulator(const scheduler_expert_config& sched_cfg_,
                                                   unsigned                       tx_rx_delay_,
                                                   subcarrier_spacing             max_scs) :
  tx_rx_delay(tx_rx_delay_),
  logger([]() -> ocudulog::basic_logger& {
    ocudulog::init();
    auto& l = ocudulog::fetch_basic_logger("SCHED", true);
    l.set_level(ocudulog::basic_levels::debug);
    return l;
  }()),
  test_logger(ocudulog::fetch_basic_logger("TEST", true)),
  sched_cfg(sched_cfg_),
  cfg_mng(sched_cfg_),
  sched(create_scheduler(scheduler_config{sched_cfg, notif})),
  next_slot(test_helper::generate_random_slot_point(max_scs))
{
  test_logger.set_level(ocudulog::basic_levels::debug);
  logger.set_context(next_slot.sfn(), next_slot.slot_index());
  test_logger.set_context(next_slot.sfn(), next_slot.slot_index());
  ocudulog::flush();
}

scheduler_test_simulator::scheduler_test_simulator(unsigned           tx_rx_delay_,
                                                   subcarrier_spacing max_scs,
                                                   bool               enable_csi_rs_pdsch_multiplexing) :
  scheduler_test_simulator(make_custom_scheduler_expert_config(enable_csi_rs_pdsch_multiplexing), tx_rx_delay_, max_scs)
{
}

scheduler_test_simulator::scheduler_test_simulator(const scheduler_test_sim_config& cfg) :
  scheduler_test_simulator(cfg.sched_cfg, cfg.tx_rx_delay, cfg.max_scs)
{
  auto_uci       = cfg.auto_uci;
  auto_crc       = cfg.auto_crc;
  ntn_cs_koffset = cfg.ntn_cs_koffset;
}

scheduler_test_simulator::~scheduler_test_simulator()
{
  // Let any pending allocations complete.
  const unsigned max_k = std::max(SCHEDULER_MAX_K0 + SCHEDULER_MAX_K1, SCHEDULER_MAX_K2);
  for (unsigned i = 0; i < max_k; i++) {
    run_slot();
  }

  // Flush logs before exiting.
  ocudulog::flush();
}

void scheduler_test_simulator::add_cell(const sched_cell_configuration_request_message& cell_cfg_req)
{
  const cell_configuration* cell_cfg_ptr = cfg_mng.add_cell(cell_cfg_req);
  sim_cells.push_back(std::make_unique<sim_cell_context>(cell_cfg_ptr));
  auto cpy             = cell_cfg_req;
  cpy.metrics.notifier = &sim_cells.back()->cell_metrics;
  sched->handle_cell_configuration_request(cpy);
}

void scheduler_test_simulator::add_ue(const sched_ue_creation_request_message& ue_request, bool wait_notification)
{
  static const size_t ADD_TIMEOUT = 100;
  sched->handle_ue_creation_request(ue_request);
  if (not ue_request.ul_ccch_slot_rx.has_value() and ue_request.starts_in_fallback) {
    sched->handle_crnti_ce_received(ue_request.ue_index);
  }
  rnti_to_ue_index.insert(std::make_pair(ue_request.crnti, ue_request.ue_index));
  if (wait_notification) {
    notif.last_ue_index_cfg.reset();
    for (unsigned i = 0; i != ADD_TIMEOUT and notif.last_ue_index_cfg != ue_request.ue_index; ++i) {
      run_slot();
    }
  }
}

void scheduler_test_simulator::rem_ue(du_ue_index_t ue_index)
{
  sched->handle_ue_removal_request(ue_index);
  for (auto it = rnti_to_ue_index.begin(); it != rnti_to_ue_index.end(); ++it) {
    if (it->second == ue_index) {
      rnti_to_ue_index.erase(it);
      break;
    }
  }
}

void scheduler_test_simulator::push_dl_buffer_state(const dl_buffer_state_indication_message& upd)
{
  sched->handle_dl_buffer_state_indication(upd);
}

void scheduler_test_simulator::run_slot(std::optional<du_cell_index_t> cell_idx)
{
  ocudu_assert(not cell_idx.has_value() or contains(*cell_idx), "Invalid cellId={}", fmt::underlying(*cell_idx));
  logger.set_context(next_slot.sfn(), next_slot.slot_index());
  test_logger.set_context(next_slot.sfn(), next_slot.slot_index());

  auto advance_slot = [this](du_cell_index_t cidx) {
    // Run scheduler for the cell.
    sim_cells[cidx]->last_res = &sched->slot_indication(next_slot, cidx);

    // Ensure the scheduler result is consistent with the cell configuration and there are no collisions.
    test_scheduler_result_consistency(cell_cfg(cidx), next_slot.without_hyper_sfn(), *last_sched_result(cidx));

    // Verify HARQ NDI and TBS consistency across retransmissions.
    sim_cells[cidx]->harq_tracker.on_new_result(next_slot.without_hyper_sfn(), *last_sched_result(cidx));

    // In case auto-feedback is enabled, handle it.
    handle_auto_feedback(cidx);
  };

  if (not cell_idx.has_value()) {
    // Advance for all cells.
    for (size_t idx = 0U, sz = sim_cells.size(); idx != sz; ++idx) {
      if (contains(to_du_cell_index(idx))) {
        advance_slot(to_du_cell_index(idx));
      }
    }
  } else {
    advance_slot(*cell_idx);
  }

  ++next_slot;

  // Resume async tasks awaiting the next slot and drop the ones that completed.
  next_slot_signal.set();
  for (auto it = pending_tasks.begin(); it != pending_tasks.end();) {
    if (it->ready()) {
      it = pending_tasks.erase(it);
    } else {
      ++it;
    }
  }
}

bool scheduler_test_simulator::run_slot_until(const std::function<bool()>& cond_func, unsigned slot_timeout)
{
  unsigned count = 0;
  for (; count < slot_timeout; ++count) {
    run_slot();
    if (cond_func()) {
      break;
    }
  }
  return count < slot_timeout;
}

void scheduler_test_simulator::schedule_task(async_task<void> task)
{
  if (task.ready()) {
    return;
  }
  pending_tasks.push_back(launch_async([t = std::move(task)](coro_context<eager_async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);
    CORO_AWAIT(t);
    CORO_RETURN();
  }));
}

void scheduler_test_simulator::run_until_all_pending_tasks_completion()
{
  while (not pending_tasks.empty()) {
    run_slot();
  }
}

async_task<bool> scheduler_test_simulator::launch_run_until(unique_function<bool()> condition, unsigned max_slot_count)
{
  return launch_async([this, condition = std::move(condition), max_slot_count, count = 0U](
                          coro_context<async_task<bool>>& ctx) mutable {
    CORO_BEGIN(ctx);
    for (count = 0; count != max_slot_count; ++count) {
      if (condition()) {
        CORO_EARLY_RETURN(true);
      }
      CORO_AWAIT(next_slot_signal);
    }
    CORO_RETURN(false);
  });
}

async_task<void> scheduler_test_simulator::launch_add_ue_task(sched_ue_creation_request_message ue_request)
{
  const du_ue_index_t ue_index = ue_request.ue_index;
  // Note: the completion condition is built here (evaluated context) and moved into the coroutine frame, as the CORO_*
  // macros cannot take an expression containing a lambda literal.
  return launch_async([this, ue_request = std::move(ue_request), cond = unique_function<bool()>([this, ue_index]() {
                                                                   return notif.ue_creation_completed(ue_index);
                                                                 })](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);
    add_ue(ue_request, false);
    CORO_AWAIT(launch_run_until(std::move(cond)));
    CORO_RETURN();
  });
}

async_task<void> scheduler_test_simulator::launch_rem_ue_task(du_ue_index_t ue_index)
{
  return launch_async([this, ue_index, cond = unique_function<bool()>([this, ue_index]() {
                                         return notif.ue_deletion_completed(ue_index);
                                       })](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);
    sched->handle_ue_removal_request(ue_index);
    // Wait for the scheduler to confirm the UE was fully drained before considering it removed.
    CORO_AWAIT(launch_run_until(std::move(cond)));
    for (auto it = rnti_to_ue_index.begin(); it != rnti_to_ue_index.end(); ++it) {
      if (it->second == ue_index) {
        rnti_to_ue_index.erase(it);
        break;
      }
    }
    CORO_RETURN();
  });
}

void scheduler_test_simulator::handle_auto_feedback(du_cell_index_t cell_idx)
{
  if (not auto_uci and not auto_crc) {
    return;
  }
  // Note: next_slot hasn't been incremented yet.
  slot_point sl_rx = next_slot.without_hyper_sfn();

  uci_indication uci_ind;
  uci_ind.cell_index = cell_idx;
  uci_ind.slot_rx    = sl_rx;
  ul_crc_indication crc_ind;
  crc_ind.cell_index = cell_idx;
  crc_ind.sl_rx      = sl_rx;

  if (auto_uci) {
    // Handle PUCCHs.
    for (const pucch_info& pucch : this->last_sched_result(cell_idx)->ul.pucchs) {
      if (pucch.format() == pucch_format::FORMAT_1 and pucch.uci_bits.sr_bits != sr_nof_bits::no_sr) {
        // Skip SRs.
        continue;
      }

      const du_ue_index_t ue_idx = rnti_to_ue_index.at(pucch.crnti);
      uci_ind.ucis.push_back(test_helper::create_uci_indication_pdu(ue_idx, pucch));
    }
  }

  for (const auto& pusch : this->last_sched_result(cell_idx)->ul.puschs) {
    if (auto_uci and pusch.uci.has_value()) {
      const du_ue_index_t ue_idx = pusch.context.ue_index;
      uci_ind.ucis.push_back(test_helper::create_uci_indication_pdu(pusch.pusch_cfg.rnti, ue_idx, pusch.uci.value()));
    }

    if (auto_crc) {
      crc_ind.crcs.push_back(test_helper::create_crc_pdu_indication(pusch));
    }
  }

  // Forward indications to the scheduler.
  if (not uci_ind.ucis.empty()) {
    on_uci_indication(uci_ind);
    this->sched->handle_uci_indication(uci_ind);
  }
  if (not crc_ind.crcs.empty()) {
    this->sched->handle_crc_indication(crc_ind);
  }
}
