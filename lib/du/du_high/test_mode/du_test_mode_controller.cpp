// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_test_mode_controller.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/support/executors/task_executor.h"
#include <atomic>

using namespace ocudu;
using namespace odu;

// ---- cell_handler ----

class du_test_mode_controller::cell_controller
{
public:
  cell_controller(du_test_mode_controller& parent_,
                  du_cell_index_t          cell_index_,
                  timer_manager&           timers_,
                  task_executor&           ctrl_exec_) :
    parent(parent_),
    cell_index(cell_index_),
    ulcch_buf(byte_buffer::create({0x34, 0x1e, 0x4f, 0xc0, 0x4f, 0xa6, 0x06, 0x3f, 0x00, 0x00, 0x00}).value()),
    ues(parent.cfg.nof_ues)
  {
    if (parent.cfg.attach_detach_duration.has_value()) {
      // In case attach-detach cycling is enabled.
      attach_detach_timer = timers_.create_unique_timer(ctrl_exec_);
      attach_detach_timer.set(*parent.cfg.attach_detach_duration,
                              [this](timer_id_t) { this->handle_attach_detach_timer(); });
      guard_timer = timers_.create_unique_timer(ctrl_exec_);
      guard_timer.set(parent.cfg.attach_detach_guard_duration, [this](timer_id_t) { this->handle_guard_timer(); });
    }

    free_list_rnti.reserve(ues.size());
  }

  void start() { start_next_ue_creation_cycle(); }

  /// Handles the event of a ConRes completion.
  void handle_conres_completed(rnti_t rnti, bool success)
  {
    if (cycle != cell_cycle_state::creating) {
      parent.logger.warning("TEST_MODE cell={} tc-rnti={}: Unexpected ConRes CE detected", cell_index, rnti);
      return;
    }

    const unsigned ue_offset = get_ue_offset(rnti);
    if (ue_offset >= parent.cfg.nof_ues or ues[ue_offset].conres_complete) {
      parent.logger.warning("TEST_MODE cell={} tc-rnti={}: Unexpected cell or UE for ConRes CE", cell_index, rnti);
      return;
    }

    if (not success) {
      parent.logger.info("TEST_MODE cell={} rnti={}: UE ConRes CE was not scheduled on time", cell_index, rnti);
    }

    // UE fully established or failed ConRes.
    ues[ue_offset].conres_complete = true;
    ++nof_ues_estab;

    if (nof_ues_estab == parent.cfg.nof_ues) {
      cycle = cell_cycle_state::running;

      // Stop of creation of new UEs.
      ue_creation_enabled.store(false, std::memory_order_release);

      if (attach_detach_timer.is_valid()) {
        // Initiate attach-detach timer, if configured.
        attach_detach_timer.run();
        parent.logger.info("TEST_MODE cell={}: All {} UE(s) established. Running for {} ms.",
                           cell_index,
                           parent.cfg.nof_ues,
                           parent.cfg.attach_detach_duration->count());
      } else {
        parent.logger.info("TEST_MODE cell={}: All {} UE(s) established.", cell_index, parent.cfg.nof_ues);
      }
    }
  }

  void handle_slot_completed(slot_point slot)
  {
    // Stagger UE creation.
    const unsigned cell_offset_mod =
        parent.cfg.ue_creation_stagger_slots * static_cast<unsigned>(cell_index) / MAX_NOF_DU_CELLS;
    if (slot.count() % parent.cfg.ue_creation_stagger_slots != cell_offset_mod) {
      return;
    }

    if (not ue_creation_enabled.load(std::memory_order_acquire)) {
      // Not accepting new UEs.
      return;
    }

    // Dispatch task to create UE.
    if (not parent.ctrl_exec.defer([this, slot]() { try_create_ue(slot); })) {
      parent.logger.warning("TEST_MODE cell={}: Failed to dispatch UE creation request", cell_index);
    }
  }

  void on_ue_removed()
  {
    // Note: The UE can be removed during cell_cycle_state::creating (due to rrcReject). In that case, do nothing.
    if (cycle != cell_cycle_state::releasing) {
      return;
    }
    if (nof_ues_pending_remove > 0) {
      --nof_ues_pending_remove;
      if (nof_ues_pending_remove == 0) {
        start_guard_period();
      }
    }
  }

private:
  struct ue_cell_context {
    slot_point ccch_slot;
    bool       conres_complete = false;
  };

  void try_create_ue(slot_point slot)
  {
    if (cycle != cell_cycle_state::creating) {
      return;
    }
    if (free_list_rnti.empty()) {
      // No more UEs to create.

      // Check, however, if there is any UE that already passed its ConRes window.
      auto it = std::find_if(ues.begin(), ues.end(), [slot](const ue_cell_context& u) {
        const unsigned conres_win_guard_slots = 128 * get_nof_slots_per_subframe(slot.scs());
        return u.ccch_slot.valid() and not u.conres_complete and u.ccch_slot + conres_win_guard_slots < slot;
      });
      if (it != ues.end()) {
        // A UE that failed ConRes was detected.
        const rnti_t rnti = get_ue_rnti(std::distance(ues.begin(), it));
        handle_conres_completed(rnti, false);
      }

      return;
    }

    // Pop next RNTI to create.
    const rnti_t test_ue_rnti = free_list_rnti.back();
    free_list_rnti.pop_back();

    // Record the injection slot for the ConRes timeout check.
    ues[get_ue_offset(test_ue_rnti)].ccch_slot = slot;

    // Dispatch UL-CCCH to the DU-high.
    parent.mac_ctrl.inject_ul_ccch_msg(slot, cell_index, test_ue_rnti, ulcch_buf.copy());
  }

  /// Called when the attach-detach timer triggers.
  void handle_attach_detach_timer()
  {
    ocudu_assert(cycle == cell_cycle_state::running, "Invalid state");
    start_release_all_ues();
  }

  void handle_guard_timer()
  {
    ocudu_assert(cycle == cell_cycle_state::guard, "Invalid state");
    parent.logger.info("TEST_MODE cell={}: Guard period elapsed. Starting new creation cycle.", cell_index);
    start_next_ue_creation_cycle();
  }

  /// Starts guard period, during which, no UE is attached to the cell.
  void start_guard_period()
  {
    parent.logger.info("TEST_MODE cell={}: All UE(s) released. Entering guard period.", cell_index);
    cycle = cell_cycle_state::guard;
    guard_timer.run();
  }

  /// Starts the release of all UEs in the cell.
  void start_release_all_ues()
  {
    parent.logger.info(
        "TEST_MODE cell={}: Attach/detach duration elapsed. Releasing {} UE(s).", cell_index, parent.cfg.nof_ues);

    cycle                 = cell_cycle_state::releasing;
    unsigned nof_released = 0;
    for (unsigned u = 0; u < parent.cfg.nof_ues; ++u) {
      const rnti_t rnti = get_ue_rnti(u);
      if (parent.f1c_adapter.try_release_ue(rnti)) {
        ++nof_released;
      }
    }
    nof_ues_pending_remove = nof_released;

    if (nof_released == 0) {
      start_guard_period();
    }
  }

  void start_next_ue_creation_cycle()
  {
    cycle                  = cell_cycle_state::creating;
    nof_ues_estab          = 0;
    nof_ues_pending_remove = 0;
    std::fill(ues.begin(), ues.end(), ue_cell_context{});
    for (unsigned i = 0; i != parent.cfg.nof_ues; ++i) {
      free_list_rnti.push_back(get_ue_rnti(parent.cfg.nof_ues - i - 1));
    }

    // Signal RT executor that it can start triggering UE creations.
    ue_creation_enabled.store(true, std::memory_order_release);
  }

  unsigned get_ue_offset(rnti_t rnti) const
  {
    const unsigned base      = to_value(parent.cfg.rnti) + static_cast<unsigned>(cell_index) * parent.cfg.nof_ues;
    const unsigned ue_offset = to_value(rnti) - base;
    return ue_offset;
  }
  rnti_t get_ue_rnti(unsigned ue_offset) const
  {
    const unsigned base = to_value(parent.cfg.rnti) + static_cast<unsigned>(cell_index) * parent.cfg.nof_ues;
    return to_rnti(ue_offset + base);
  }

  /// States of the cell test mode controller.
  enum class cell_cycle_state { creating, running, releasing, guard };

  du_test_mode_controller& parent;
  du_cell_index_t          cell_index;

  // Pre-canned UL-CCCH message.
  byte_buffer ulcch_buf;

  cell_cycle_state             cycle = cell_cycle_state::creating;
  unique_timer                 attach_detach_timer;
  unique_timer                 guard_timer;
  unsigned                     nof_ues_estab          = 0;
  unsigned                     nof_ues_pending_remove = 0;
  std::vector<ue_cell_context> ues;
  std::vector<rnti_t>          free_list_rnti;

  // Flag accessed from cell RT executor.
  std::atomic<bool> ue_creation_enabled{true};
};

class du_test_mode_controller::mac_event_notifier final : public mac_test_mode_event_notifier
{
public:
  mac_event_notifier(du_test_mode_controller& parent_) : parent(parent_) {}

  void on_con_res_completed(du_cell_index_t cell_index, rnti_t rnti) override
  {
    if (not parent.ctrl_exec.defer(
            [this, cell_index, rnti]() { parent.cells[cell_index]->handle_conres_completed(rnti, true); })) {
      parent.logger.warning("TEST_MODE: Failed to dispatch Msg4 notification for rnti={}", rnti);
    }
  }

  void on_slot_completed(du_cell_index_t cell_index, slot_point sl_tx) override
  {
    parent.cells[cell_index]->handle_slot_completed(sl_tx);
  }

private:
  du_test_mode_controller& parent;
};

class du_test_mode_controller::f1c_event_notifier final : public f1c_test_mode_event_notifier
{
public:
  f1c_event_notifier(du_test_mode_controller& parent_) : parent(parent_) {}

  void on_ue_removed(rnti_t rnti) override { parent.handle_ue_removed(rnti); }

private:
  du_test_mode_controller& parent;
};

// ---- du_test_mode_controller ----

du_test_mode_controller::du_test_mode_controller(const du_test_mode_config::test_mode_ue_config& cfg_,
                                                 f1c_connection_client&                          f1c_client_,
                                                 mac_result_notifier&                            phy_notifier_,
                                                 timer_manager&                                  timers_,
                                                 task_executor&                                  ctrl_exec_,
                                                 unsigned                                        nof_cells_) :
  cfg(cfg_),
  ctrl_exec(ctrl_exec_),
  logger(ocudulog::fetch_basic_logger("DU")),
  f1c_adapter(f1c_client_, std::make_unique<f1c_event_notifier>(*this), logger),
  mac_ctrl(cfg_, phy_notifier_, std::make_unique<mac_event_notifier>(*this), nof_cells_)
{
  cells.reserve(nof_cells_);
  for (unsigned i = 0; i != nof_cells_; ++i) {
    cells.push_back(std::make_unique<cell_controller>(*this, to_du_cell_index(i), timers_, ctrl_exec_));
  }
}

du_test_mode_controller::~du_test_mode_controller() = default;

std::unique_ptr<mac_interface> du_test_mode_controller::decorate(std::unique_ptr<mac_interface> mac_ptr)
{
  auto wrapper = mac_ctrl.decorate(std::move(mac_ptr));
  for (auto& cell : cells) {
    cell->start();
  }
  return wrapper;
}

void du_test_mode_controller::handle_ue_removed(rnti_t rnti)
{
  if (not cfg.attach_detach_duration.has_value()) {
    // Attach-detach mode is not enabled. Early return.
    return;
  }
  for (unsigned c = 0; c < cells.size(); ++c) {
    if (is_test_ue_in_cell(to_du_cell_index(c), rnti)) {
      cells[c]->on_ue_removed();
      return;
    }
  }
}
