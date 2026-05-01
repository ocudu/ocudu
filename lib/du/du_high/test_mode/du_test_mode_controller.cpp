// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_test_mode_controller.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "ocudu/asn1/rrc_nr/ue_cap.h"
#include "ocudu/f1ap/du/f1ap_du.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/mac/mac_pdu_handler.h"
#include "ocudu/ran/logical_channel/lcid.h"
#include "ocudu/scheduler/result/sched_result.h"
#include "ocudu/support/executors/task_executor.h"
#include <atomic>

using namespace ocudu;
using namespace odu;

// ---- f1c_wrapper_impl ----

/// Intercepts outgoing F1AP PDUs (DU → CU) to capture gnb_du_ue_f1ap_id for test mode UEs.
class du_test_mode_controller::f1c_wrapper_impl : public f1c_connection_client
{
public:
  explicit f1c_wrapper_impl(du_test_mode_controller& parent_) : parent(parent_) {}

  void set_upstream(f1c_connection_client& upstream_) { upstream = &upstream_; }

  std::unique_ptr<f1ap_message_notifier>
  handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier) override
  {
    ocudu_assert(upstream != nullptr, "f1c upstream not set");
    auto upstream_tx = upstream->handle_du_connection_request(std::move(du_rx_pdu_notifier));
    if (upstream_tx == nullptr) {
      return nullptr;
    }
    return std::make_unique<tx_interceptor>(parent, std::move(upstream_tx));
  }

private:
  class tx_interceptor : public f1ap_message_notifier
  {
  public:
    tx_interceptor(du_test_mode_controller& parent_, std::unique_ptr<f1ap_message_notifier> upstream_tx_) :
      parent(parent_), upstream_tx(std::move(upstream_tx_))
    {
    }

    void on_new_message(const f1ap_message& msg) override
    {
      using namespace asn1::f1ap;

      if (msg.pdu.type().value == f1ap_pdu_c::types_opts::init_msg and
          msg.pdu.init_msg().value.type().value ==
              f1ap_elem_procs_o::init_msg_c::types_opts::init_ul_rrc_msg_transfer) {
        const auto& ie   = msg.pdu.init_msg().value.init_ul_rrc_msg_transfer();
        rnti_t      rnti = to_rnti(ie->c_rnti);

        if (ie->du_to_cu_rrc_container_present) {
          for (unsigned c = 0; c < parent.cells.size(); ++c) {
            if (parent.is_test_ue_in_cell(static_cast<du_cell_index_t>(c), rnti)) {
              parent.on_ue_f1ap_id_captured(rnti, int_to_gnb_du_ue_f1ap_id(ie->gnb_du_ue_f1ap_id));
              break;
            }
          }
        }
      }

      upstream_tx->on_new_message(msg);
    }

  private:
    du_test_mode_controller&               parent;
    std::unique_ptr<f1ap_message_notifier> upstream_tx;
  };

  du_test_mode_controller& parent;
  f1c_connection_client*   upstream = nullptr;
};

// ---- cell_notifier_impl ----

/// Wraps a mac_cell_result_notifier to intercept Msg4 events and slot timing for the cycling controller.
class du_test_mode_controller::cell_notifier_impl : public mac_cell_result_notifier
{
public:
  cell_notifier_impl(du_test_mode_controller&  parent_,
                     du_cell_index_t           cell_index_,
                     mac_cell_result_notifier& real_phy_) :
    parent(parent_), cell_index(cell_index_), real_phy(real_phy_)
  {
  }

  void on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res) override
  {
    // Searches ConRes CEs in the scheduler result.
    for (const auto& grant : dl_res.dl_res->ue_grants) {
      // Checks if the first subPDU is ConRes CE. If it is, signal UE connection.
      if (not grant.tb_list.empty() and grant.tb_list[0].lc_chs_to_sched[0].lcid == lcid_dl_sch_t::UE_CON_RES_ID) {
        const rnti_t crnti = grant.pdsch_cfg.rnti;
        ocudu_sanity_check(parent.is_test_ue_in_cell(cell_index, crnti), "Unexpected cell for ConRes CE");
        parent.handle_conres_scheduled(cell_index, crnti);
      }
    }

    // Forward results to the lower layers.
    real_phy.on_new_downlink_scheduler_results(dl_res);
  }

  void on_new_downlink_data(const mac_dl_data_result& dl_data) override { real_phy.on_new_downlink_data(dl_data); }

  void on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res) override
  {
    real_phy.on_new_uplink_scheduler_results(ul_res);
  }

  void on_cell_results_completion(slot_point slot) override
  {
    real_phy.on_cell_results_completion(slot);
    parent.handle_slot_completed(cell_index, slot);
  }

private:
  du_test_mode_controller&  parent;
  du_cell_index_t           cell_index;
  mac_cell_result_notifier& real_phy;
};

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

  /// Handles the event of a ConRes CE scheduled.
  void handle_conres_scheduled(rnti_t rnti, bool success)
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
    if (cycle == cell_cycle_state::releasing and nof_ues_pending_remove > 0) {
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
        handle_conres_scheduled(rnti, false);
      }

      return;
    }

    // Pop next RNTI to create.
    const rnti_t test_ue_rnti = free_list_rnti.back();
    free_list_rnti.pop_back();

    // Record the injection slot for the ConRes timeout check.
    ues[get_ue_offset(test_ue_rnti)].ccch_slot = slot;

    // Dispatch UL-CCCH to the DU-high.
    parent.pdu_handler->handle_rx_data_indication(
        mac_rx_data_indication{slot, cell_index, {mac_rx_pdu{test_ue_rnti, 0, ulcch_buf.copy()}}});
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

    parent.ue_id_table.erase(
        std::remove_if(parent.ue_id_table.begin(),
                       parent.ue_id_table.end(),
                       [&](const ue_entry& e) { return parent.is_test_ue_in_cell(cell_index, e.rnti); }),
        parent.ue_id_table.end());

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
      if (parent.release_ue(rnti)) {
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

// ---- du_test_mode_controller ----

du_test_mode_controller::du_test_mode_controller(const du_test_mode_config::test_mode_ue_config& cfg_,
                                                 timer_manager&                                  timers_,
                                                 task_executor&                                  ctrl_exec_,
                                                 unsigned                                        nof_cells_) :
  cfg(cfg_),
  ctrl_exec(ctrl_exec_),
  logger(ocudulog::fetch_basic_logger("DU")),
  f1c_wrapper(std::make_unique<f1c_wrapper_impl>(*this))
{
  cells.reserve(nof_cells_);
  for (unsigned i = 0; i != nof_cells_; ++i) {
    cells.push_back(std::make_unique<cell_controller>(*this, to_du_cell_index(i), timers_, ctrl_exec_));
  }
}

du_test_mode_controller::~du_test_mode_controller() = default;

void du_test_mode_controller::set_f1c_upstream(f1c_connection_client& upstream)
{
  f1c_wrapper->set_upstream(upstream);
}

f1c_connection_client& du_test_mode_controller::get_f1c_wrapper()
{
  return *f1c_wrapper;
}

mac_cell_result_notifier& du_test_mode_controller::add_cell_notifier(du_cell_index_t           cell_index,
                                                                     mac_cell_result_notifier& real_phy_notifier)
{
  if (cell_notifiers.size() <= static_cast<unsigned>(cell_index)) {
    cell_notifiers.resize(static_cast<unsigned>(cell_index) + 1);
  }
  cell_notifiers[cell_index] = std::make_unique<cell_notifier_impl>(*this, cell_index, real_phy_notifier);
  return *cell_notifiers[cell_index];
}

void du_test_mode_controller::connect(mac_pdu_handler& pdu_handler_, f1ap_du& f1ap_)
{
  pdu_handler  = &pdu_handler_;
  f1ap_handler = &f1ap_;
  for (auto& cell : cells) {
    cell->start();
  }
}

void du_test_mode_controller::handle_conres_scheduled(du_cell_index_t cell_index, rnti_t rnti)
{
  if (not ctrl_exec.defer([this, cell_index, rnti]() { cells[cell_index]->handle_conres_scheduled(rnti, true); })) {
    logger.warning("TEST_MODE: Failed to dispatch Msg4 notification for rnti={}", rnti);
  }
}

void du_test_mode_controller::handle_slot_completed(du_cell_index_t cell_index, slot_point slot)
{
  cells[cell_index]->handle_slot_completed(slot);
}

void du_test_mode_controller::on_ue_f1ap_id_captured(rnti_t rnti, gnb_du_ue_f1ap_id_t gnb_du_ue_id)
{
  if (not ctrl_exec.defer(
          [this, rnti, gnb_du_ue_id]() { ue_id_table.push_back({.rnti = rnti, .gnb_du_ue_id = gnb_du_ue_id}); })) {
    logger.warning("TEST_MODE: Failed to dispatch F1AP ID capture for rnti={}", rnti);
  }
}

void du_test_mode_controller::on_ue_removed(rnti_t rnti)
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

bool du_test_mode_controller::release_ue(rnti_t rnti)
{
  auto it = std::find_if(ue_id_table.begin(), ue_id_table.end(), [rnti](const ue_entry& e) { return e.rnti == rnti; });
  if (it == ue_id_table.end()) {
    return false;
  }

  auto gnb_cu_ue_id = f1ap_handler->get_gnb_cu_ue_f1ap_id(it->gnb_du_ue_id);
  if (not gnb_cu_ue_id.has_value()) {
    logger.warning("TEST_MODE: Cannot release rnti={}: gnb_cu_ue_f1ap_id not found", rnti);
    return false;
  }

  // Prepare F1AP UE Context Release Command.
  f1ap_message rel_cmd;
  rel_cmd.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_RELEASE);
  auto& cmd                            = rel_cmd.pdu.init_msg().value.ue_context_release_cmd();
  cmd->gnb_du_ue_f1ap_id               = gnb_du_ue_f1ap_id_to_uint(it->gnb_du_ue_id);
  cmd->gnb_cu_ue_f1ap_id               = gnb_cu_ue_f1ap_id_to_uint(*gnb_cu_ue_id);
  cmd->cause.set_radio_network().value = asn1::f1ap::cause_radio_network_opts::options::normal_release;

  logger.debug("TEST_MODE rnti={}: Injecting UE Context Release Command", rnti);
  f1ap_handler->handle_message(rel_cmd);
  return true;
}
