// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_event_manager.h"
#include "../logging/scheduler_event_logger.h"
#include "../logging/scheduler_metrics_handler.h"
#include "../srs/srs_scheduler.h"
#include "../support/sr_helper.h"
#include "../uci_scheduling/uci_indication_selector.h"
#include "../uci_scheduling/uci_scheduler_impl.h"
#include "../ue_scheduling/ue_cell_grid_allocator.h"
#include "ocudu/scheduler/scheduler_feedback_handler.h"
#include "ocudu/support/memory_pool/bounded_object_pool.h"
#include <memory>

using namespace ocudu;

/// \brief More than one DL buffer occupancy update may be received per slot for the same UE and bearer. This class
/// ensures that the UE DL buffer occupancy is updated only once per bearer per slot for efficiency reasons.
class ue_cell_event_manager::ue_dl_buffer_occupancy_manager final : public scheduler_dl_buffer_state_indication_handler
{
  using bearer_key                        = uint32_t;
  static constexpr size_t NOF_BEARER_KEYS = MAX_NOF_DU_UES * MAX_NOF_RB_LCIDS;

  static bearer_key    get_bearer_key(du_ue_index_t ue_index, lcid_t lcid) { return lcid * MAX_NOF_DU_UES + ue_index; }
  static du_ue_index_t get_ue_index(bearer_key key) { return to_du_ue_index(key % MAX_NOF_DU_UES); }
  static lcid_t        get_lcid(bearer_key key) { return uint_to_lcid(key / MAX_NOF_DU_UES); }

public:
  ue_dl_buffer_occupancy_manager(ue_cell_event_manager& parent_) : parent(parent_), pending_evs(NOF_BEARER_KEYS)
  {
    std::fill(ue_dl_bo_table.begin(), ue_dl_bo_table.end(), std::make_pair(-1, 0));
  }

  void handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& rlc_dl_bo) override
  {
    // Update DL Buffer Occupancy for the given UE and bearer.
    unsigned key          = get_bearer_key(rlc_dl_bo.ue_index, rlc_dl_bo.lcid);
    bool     first_rlc_bo = ue_dl_bo_table[key].first.exchange(rlc_dl_bo.bs, std::memory_order_acquire) < 0;
    ue_dl_bo_table[key].second.store(rlc_dl_bo.hol_toa.valid() ? rlc_dl_bo.hol_toa.count_val : -1,
                                     std::memory_order_relaxed);

    if (not first_rlc_bo) {
      // If another DL BO update has been received before for this same bearer, we do not need to enqueue a new event.
      return;
    }

    // Signal that this bearer needs its BO state updated.
    if (not pending_evs.try_push(key)) {
      parent.logger.warning("ue={} lcid={}: Discarding DL buffer occupancy update. Cause: Event queue is full",
                            fmt::underlying(rlc_dl_bo.ue_index),
                            fmt::underlying(rlc_dl_bo.lcid));
    }
  }

  void slot_indication(slot_point sl)
  {
    // Process RLC buffer updates of pending UEs.
    bearer_key key;
    while (pending_evs.try_pop(key)) {
      // Recreate latest DL BO update.
      dl_buffer_state_indication_message dl_bo;
      // > Extract UE index and LCID.
      dl_bo.ue_index = get_ue_index(key);
      dl_bo.lcid     = get_lcid(key);
      int hol_toa    = ue_dl_bo_table[key].second.load(std::memory_order_relaxed);
      if (hol_toa >= 0) {
        dl_bo.hol_toa = std::min(sl, slot_point{sl.numerology(), (unsigned)hol_toa});
      }
      // > Extract last DL BO value for the respective bearer and reset BO table position.
      dl_bo.bs = ue_dl_bo_table[key].first.exchange(-1, std::memory_order_release);
      if (dl_bo.bs < 0) {
        parent.logger.warning("ue={} lcid={}: Invalid DL buffer occupancy value: {}",
                              fmt::underlying(dl_bo.ue_index),
                              fmt::underlying(dl_bo.lcid),
                              dl_bo.bs);
        continue;
      }

      // Retrieve UE.
      if (not parent.ue_db.contains(dl_bo.ue_index)) {
        parent.logger.warning("ue={}: Discarding DL buffer occupancy update. Cause: UE not recognized",
                              fmt::underlying(dl_bo.ue_index));
        continue;
      }
      ue& u = parent.ue_db[dl_bo.ue_index];

      // Forward DL BO update to UE.
      u.handle_dl_buffer_state_indication(dl_bo.lcid, dl_bo.bs, dl_bo.hol_toa);
      if (u.get_pcell().is_in_fallback_mode()) {
        // Signal SRB fallback scheduler with the new SRB0/SRB1 buffer state.
        parent.fallback_sched.handle_dl_buffer_state_indication(dl_bo.ue_index);
      }

      // Log event.
      parent.ev_logger.enqueue(dl_bo);

      // Report event.
      parent.metrics.handle_dl_buffer_state_indication(dl_bo);
    }
  }

private:
  using ue_event_queue =
      concurrent_queue<bearer_key, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>;

  ue_cell_event_manager& parent;

  // Table of pending DL Buffer Occupancy values and HOL TOAs. DL Buffer Occupancy=-1 means that it is not set. HOL
  // ToA of 0 means it is not set.
  std::array<std::pair<std::atomic<int>, std::atomic<int>>, NOF_BEARER_KEYS> ue_dl_bo_table;

  // Queue of {UE Id, LCID} pairs with pending DL Buffer Occupancy updates.
  ue_event_queue pending_evs;
};

class ocudu::pdu_indication_pool
{
  // We use this value as a safety margin to account for skipped slot indications.
  static constexpr size_t MAX_EXPECTED_SLOTS = 4;
  // Although PHR and BSR indications are reported in the same quantity as CRC indications, the formers follow a slower
  // path to reach the scheduler; there's both a thread hop (dispatched to a per-UE MAC UL PDU executor) and processing
  // latency (MAC PDU decoding + subPDU parsing); therefore, there can be a larger accumulation of in-flight BSR/PHR
  // indications compared to CRC.
  static constexpr size_t MAX_BSR_PHR_EXPECTED_SLOTS = 6;
  static constexpr size_t UCI_INITIAL_POOL_SIZE      = MAX_PUCCH_PDUS_PER_SLOT * MAX_EXPECTED_SLOTS;
  static constexpr size_t PHR_INITIAL_POOL_SIZE      = MAX_PUSCH_PDUS_PER_SLOT * MAX_BSR_PHR_EXPECTED_SLOTS;
  static constexpr size_t CRC_INITIAL_POOL_SIZE      = MAX_PUSCH_PDUS_PER_SLOT * MAX_EXPECTED_SLOTS;
  static constexpr size_t SRS_INITIAL_POOL_SIZE      = MAX_SRS_PDUS_PER_SLOT * MAX_EXPECTED_SLOTS;
  static constexpr size_t BSR_INITIAL_POOL_SIZE      = MAX_PUSCH_PDUS_PER_SLOT * MAX_BSR_PHR_EXPECTED_SLOTS;
  static constexpr size_t POSITIONING_REQ_POOL_SIZE  = 1 * MAX_EXPECTED_SLOTS;
  static constexpr size_t SLICE_RECONF_POOL_SIZE     = 1 * MAX_EXPECTED_SLOTS;

  using uci_pool          = bounded_object_pool<uci_indication::uci_pdu>;
  using phr_pool          = bounded_object_pool<ul_phr_indication_message>;
  using crc_pool          = bounded_object_pool<ul_crc_pdu_indication>;
  using srs_pool          = bounded_object_pool<srs_indication::srs_indication_pdu>;
  using bsr_pool          = bounded_object_pool<ul_bsr_indication_message>;
  using pos_req_pool      = bounded_object_pool<positioning_measurement_request::cell_info>;
  using slice_reconf_pool = bounded_object_pool<du_cell_slice_reconfig_request>;

public:
  pdu_indication_pool(ocudulog::basic_logger& logger_) :
    logger(logger_),
    pending_ucis(UCI_INITIAL_POOL_SIZE),
    pending_phrs(PHR_INITIAL_POOL_SIZE),
    pending_crcs(CRC_INITIAL_POOL_SIZE),
    pending_srss(SRS_INITIAL_POOL_SIZE),
    pending_bsrs(BSR_INITIAL_POOL_SIZE),
    pending_pos_reqs(POSITIONING_REQ_POOL_SIZE),
    slice_reconf_reqs(SLICE_RECONF_POOL_SIZE)
  {
  }

  /// Returns a human-readable name for a PDU type.
  template <typename PDUType>
  static constexpr const char* pdu_type_name()
  {
    if constexpr (std::is_same_v<PDUType, uci_indication::uci_pdu>) {
      return "UCI";
    } else if constexpr (std::is_same_v<PDUType, ul_phr_indication_message>) {
      return "PHR";
    } else if constexpr (std::is_same_v<PDUType, ul_crc_pdu_indication>) {
      return "CRC";
    } else if constexpr (std::is_same_v<PDUType, srs_indication::srs_indication_pdu>) {
      return "SRS";
    } else if constexpr (std::is_same_v<PDUType, ul_bsr_indication_message>) {
      return "BSR";
    } else if constexpr (std::is_same_v<PDUType, positioning_measurement_request::cell_info>) {
      return "positioning measurement request";
    } else if constexpr (std::is_same_v<PDUType, du_cell_slice_reconfig_request>) {
      return "slice reconfiguration request";
    } else {
      return "unknown";
    }
  }

  /// Create a PDU managed by an object pool.
  template <typename PDUType>
  auto create_pdu(const PDUType& pdu)
  {
    auto* pool = std::get<bounded_object_pool<std::decay_t<PDUType>>*>(pools);
    auto  ret  = pool->get();
    if (ret != nullptr) {
      *ret = pdu;
    } else {
      logger.warning("Discarding {} indication PDU. Cause: PDU pool is empty", pdu_type_name<std::decay_t<PDUType>>());
    }
    return ret;
  }

private:
  ocudulog::basic_logger& logger;

  uci_pool          pending_ucis;
  phr_pool          pending_phrs;
  crc_pool          pending_crcs;
  srs_pool          pending_srss;
  bsr_pool          pending_bsrs;
  pos_req_pool      pending_pos_reqs;
  slice_reconf_pool slice_reconf_reqs;

  std::tuple<uci_pool*, phr_pool*, crc_pool*, srs_pool*, bsr_pool*, pos_req_pool*, slice_reconf_pool*> pools{
      &pending_ucis,
      &pending_phrs,
      &pending_crcs,
      &pending_srss,
      &pending_bsrs,
      &pending_pos_reqs,
      &slice_reconf_reqs};
};

// Initial capacity for the common and cell event lists, in order to avoid std::vector reallocations. We use the max
// nof UEs as a conservative estimate of the expected number of events per slot.
static constexpr size_t CELL_EVENT_LIST_SIZE = MAX_NOF_DU_UES * 2;

ue_cell_event_manager::ue_cell_event_manager(ue_event_manager&          parent_,
                                             const cell_creation_event& cell_ev,
                                             ue_repository&             ue_db_,
                                             ocudulog::basic_logger&    logger_) :
  parent(parent_),
  ue_db(ue_db_),
  logger(logger_),
  cfg(cell_ev.cell_res_grid.cfg),
  res_grid(cell_ev.cell_res_grid),
  cell_harqs(cell_ev.ue_cell_db.get_cell_harqs()),
  fallback_sched(cell_ev.fallback_sched),
  uci_sched(cell_ev.uci_sched),
  slice_sched(cell_ev.slice_sched),
  srs_sched(cell_ev.srs_sched),
  uci_selector(cell_ev.uci_selector),
  metrics(cell_ev.metrics),
  ev_logger(cell_ev.ev_logger),
  ind_pdu_pool(std::make_unique<pdu_indication_pool>(logger)),
  dl_bo_mng(std::make_unique<ue_dl_buffer_occupancy_manager>(*this)),
  pending_events(CELL_EVENT_LIST_SIZE)
{
}

ue_cell_event_manager::~ue_cell_event_manager()
{
  // Stop cell if not already stopped.
  stop();

  // Deregister cell from ue_event_manager.
  parent.cells[cfg.cell_index] = nullptr;
}

void ue_cell_event_manager::stop()
{
  active.store(false, std::memory_order_release);

  // Clear pending events.
  event_t cell_ev;
  while (pending_events.try_pop(cell_ev)) {
  }
}

void ue_cell_event_manager::run_slot(slot_point sl_tx)
{
  last_sl_tx = sl_tx;

  // Pop and process pending cell-specific events.
  event_t cell_ev;
  while (pending_events.try_pop(cell_ev)) {
    const auto result = cell_ev.callback();
    switch (result) {
      case event_result::invalid_ue:
        log_invalid_ue_index(cell_ev.ue_index, cell_ev.ev_name, cell_ev.warn_if_ignored);
        break;
      case event_result::invalid_ue_cc:
        log_invalid_cc(cell_ev.ue_index, cell_ev.ev_name, cell_ev.warn_if_ignored);
        break;
      default:
        break;
    }
  }

  // Process pending DL buffer occupancy updates.
  dl_bo_mng->slot_indication(sl_tx);
}

void ue_cell_event_manager::handle_ue_creation(ue_config_update_event ev)
{
  const du_cell_index_t ue_pcell_index = ev.next_config().pcell_common_cfg().cell_index;

  auto handle_ue_creation_impl = [this, ev = std::move(ev)]() mutable {
    const du_ue_index_t ue_index = ev.get_ue_index();
    const rnti_t        crnti    = ev.next_config().crnti;
    if (ue_db.contains(ue_index)) {
      logger.error("ue={} rnti={}: Discarding UE creation. Cause: A UE with the same index already exists",
                   fmt::underlying(ue_index),
                   crnti);
      return event_result::processed;
    }

    // Insert UE in UE repository.
    const du_cell_index_t pcell_index    = ev.next_config().pcell_common_cfg().cell_index;
    bool                  is_in_fallback = ev.get_fallback_command().has_value() and ev.get_fallback_command().value();
    ue_db.add_ue(ev.next_config(), is_in_fallback, ev.get_ul_ccch_slot_rx(), ev.get_cfra_enabled());

    auto& u     = ue_db[ue_index];
    auto& ue_cc = u.get_pcell();
    if (ue_cc.get_pcell_state().conres_st != ue_conres_state::pending_conres_crnti_ce and
        ue_cc.get_pcell_state().conres_st != ue_conres_state::pending_cfra) {
      // In case the UE is expecting a C-RNTI CE, defer activation of UCI/SR scheduling.
      uci_sched.add_ue(ue_cc.cfg());
      srs_sched.add_ue(ue_cc.cfg());
    }

    // Add UE to slice scheduler.
    // Note: This action only has effect when UE is created in non-fallback mode.
    slice_sched.add_ue(ue_index);

    if (ue_cc.get_pcell_state().conres_st == ue_conres_state::pending_conres_ce) {
      // Note: In case of RACH-created UE, auto-inject MAC ConRes CE.

      // Forward CE to ue instance.
      u.handle_dl_mac_ce_indication(dl_mac_ce_indication{ue_index, lcid_dl_sch_t::UE_CON_RES_ID});

      // Notify fallback scheduler of a pending ConRes CE.
      fallback_sched.handle_conres_indication(ue_index);
    }

    // Log Event.
    ev_logger.enqueue(scheduler_event_logger::ue_creation_event{ue_index, crnti, pcell_index});

    // Register UE in the cell metrics.
    metrics.handle_ue_creation(ue_index, crnti, ev.next_config().pcell_common_cfg().params.pci);

    // Notify config manager that creation is complete with success.
    ev.notify_completion();

    return event_result::processed;
  };

  // Defer UE object addition to ue list to the slot indication handler.
  push_event(ue_pcell_index, event_t{"ue_add", std::move(handle_ue_creation_impl)});
}

void ue_cell_event_manager::handle_ue_reconfiguration(ue_config_update_event ev)
{
  const du_cell_index_t pcell_index = ev.next_config().pcell_common_cfg().cell_index;
  const du_ue_index_t   ue_index    = ev.get_ue_index();

  auto handle_ue_reconf_impl = [this, ev = std::move(ev)]() mutable {
    const du_ue_index_t ue_idx = ev.get_ue_index();
    if (not ue_db.contains(ue_idx)) {
      return event_result::invalid_ue;
    }
    auto& u = ue_db[ue_idx];

    // Reconfigure PCell
    // Note: Carrier aggregation not yet supported.
    auto& ue_cc = u.get_cell(SERVING_PCELL_IDX);

    if (ue_cc.get_pcell_state().conres_st != ue_conres_state::pending_conres_crnti_ce and
        ue_cc.get_pcell_state().conres_st != ue_conres_state::pending_cfra) {
      uci_sched.reconf_ue(ev.next_config().ue_cell_cfg(ue_cc.cell_index), ue_cc.cfg());
      srs_sched.reconf_ue(ev.next_config().ue_cell_cfg(ue_cc.cell_index), ue_cc.cfg());
    }

    // Configure existing UE.
    ue_db.reconfigure_ue(ev.next_config(), ev.get_cause());

    // Update slice scheduler.
    slice_sched.reconf_ue(u.ue_index);

    // Log event.
    ev_logger.enqueue(scheduler_event_logger::ue_reconf_event{ue_idx, u.crnti});

    // Update the cell metrics.
    metrics.handle_ue_reconfiguration(ue_idx);

    // Notify config manager that creation is complete with success.
    ev.notify_completion();

    return event_result::processed;
  };

  // Defer UE reconf to ue list to the slot indication handler.
  push_event(pcell_index, event_t{"ue_reconf", ue_index, std::move(handle_ue_reconf_impl)});
}

void ue_cell_event_manager::handle_ue_deletion(ue_config_delete_event ev)
{
  const du_ue_index_t   ue_index    = ev.ue_index();
  const du_cell_index_t pcell_index = ev.pcell_index();

  auto handle_ue_deletion_impl = [this, ev = std::move(ev)]() mutable {
    const du_ue_index_t ue_idx = ev.ue_index();
    if (not ue_db.contains(ue_idx)) {
      return event_result::invalid_ue;
    }
    const auto&  u    = ue_db[ue_idx];
    const rnti_t rnti = u.crnti;

    const auto& ue_cc = u.get_pcell();
    if (ue_cc.get_pcell_state().conres_st != ue_conres_state::pending_conres_crnti_ce and
        ue_cc.get_pcell_state().conres_st != ue_conres_state::pending_cfra) {
      // F1AP-created UE was only added to UCI/SRS scheduling after the reception of C-RNTI CE.
      uci_sched.rem_ue(u.get_pcell().cfg());
      srs_sched.rem_ue(u.get_pcell().cfg());
    }
    // Schedule removal of UE from slice scheduler.
    slice_sched.rem_ue(ue_idx);

    // Schedule UE removal from repository.
    ue_db.schedule_ue_rem(std::move(ev));

    // Log UE removal event.
    ev_logger.enqueue(sched_ue_delete_message{ue_idx, rnti});

    return event_result::processed;
  };

  push_event(pcell_index, event_t{"ue_rem", ue_index, std::move(handle_ue_deletion_impl)});
}

void ue_cell_event_manager::handle_ue_config_applied(du_cell_index_t pcell_idx, du_ue_index_t ue_idx)
{
  auto handle_ue_config_applied_impl = [this, ue_idx]() {
    // Confirm that UE applied new config.
    ue_db.ue_config_applied(ue_idx);

    // Notify slice scheduler only when the UE fully exits fallback (conres also done).
    if (not ue_db[ue_idx].get_pcell().is_in_fallback_mode()) {
      slice_sched.config_applied(ue_idx);
    }

    // Log UE config applied event.
    ev_logger.enqueue(scheduler_event_logger::ue_cfg_applied_event{ue_idx, ue_db[ue_idx].crnti});

    return event_result::processed;
  };

  push_event(pcell_idx, event_t{"ue_cfg_applied", ue_idx, std::move(handle_ue_config_applied_impl)});
}

void ue_cell_event_manager::handle_ue_deactivation_request(du_cell_index_t pcell_idx, du_ue_index_t ue_idx)
{
  auto handle_ue_deactivation_impl = [this, ue_idx]() {
    if (not ue_db.contains(ue_idx)) {
      return event_result::invalid_ue;
    }
    auto& u = ue_db[ue_idx];

    // Deactivate the UE (no more grants after this point).
    u.deactivate();

    // Schedule removal of UE from slice scheduler so it doesn't get scheduled PDSCH/PUSCH.
    slice_sched.rem_ue(ue_idx);

    // Log event.
    ev_logger.enqueue(scheduler_event_logger::ue_deactivation_event{ue_idx, u.crnti});

    return event_result::processed;
  };

  push_event(pcell_idx, event_t{"ue_deactivation", ue_idx, std::move(handle_ue_deactivation_impl)});
}

void ue_cell_event_manager::handle_ul_bsr_indication(const ul_bsr_indication_message& bsr_ind)
{
  auto bsr_ind_ptr = ind_pdu_pool->create_pdu(bsr_ind);
  if (bsr_ind_ptr == nullptr) {
    return;
  }
  const du_cell_index_t pcell_index = bsr_ind.cell_index;
  const du_ue_index_t   ue_index    = bsr_ind.ue_index;

  auto handle_ul_bsr_ind_impl = [this, bsr_ind = std::move(bsr_ind_ptr)]() {
    if (not ue_db.contains(bsr_ind->ue_index)) {
      return event_result::invalid_ue;
    }
    auto& u = ue_db[bsr_ind->ue_index];

    // Handle event.
    u.handle_bsr_indication(*bsr_ind);

    if (u.get_pcell().is_in_fallback_mode()) {
      // Signal SRB fallback scheduler with the new SRB0/SRB1 buffer state.
      fallback_sched.handle_ul_bsr_indication(bsr_ind->ue_index, *bsr_ind);
    }

    // Log event.
    if (ev_logger.enabled()) {
      scheduler_event_logger::bsr_event event{};
      event.ue_index             = bsr_ind->ue_index;
      event.rnti                 = bsr_ind->crnti;
      event.type                 = bsr_ind->type;
      event.reported_lcgs        = bsr_ind->reported_lcgs;
      event.tot_ul_pending_bytes = units::bytes{u.pending_ul_newtx_bytes()};
      ev_logger.enqueue(event);
    }

    // Notify metrics handler.
    metrics.handle_ul_bsr_indication(*bsr_ind);

    return event_result::processed;
  };

  push_event(pcell_index, event_t{"BSR", ue_index, std::move(handle_ul_bsr_ind_impl)});
}

void ue_cell_event_manager::handle_crc_indication(const ul_crc_indication& crc_ind)
{
  for (unsigned i = 0, e = crc_ind.crcs.size(); i != e; ++i) {
    if (crc_ind.crcs[i].ue_index == INVALID_DU_UE_INDEX) {
      // CRCs with invalid UE index are ignored (They are handled by the RA scheduler).
      continue;
    }
    auto crc_ind_ptr = ind_pdu_pool->create_pdu(crc_ind.crcs[i]);
    if (crc_ind_ptr == nullptr) {
      return;
    }

    auto crc_handle_impl = [this, sl_rx = crc_ind.sl_rx, crc_ptr = std::move(crc_ind_ptr)]() -> event_result {
      if (not ue_db.contains(crc_ptr->ue_index)) {
        return event_result::invalid_ue;
      }
      ue&      u     = ue_db[crc_ptr->ue_index];
      ue_cell* ue_cc = u.find_cell(cfg.cell_index);
      if (ue_cc == nullptr) {
        return event_result::invalid_ue_cc;
      }

      // Update HARQ.
      const bool was_pending_cfra = ue_cc->get_pcell_state().conres_st == ue_conres_state::pending_cfra;
      const auto tbs              = ue_cc->handle_crc_pdu(sl_rx, *crc_ptr);
      if (not tbs.has_value()) {
        if (was_pending_cfra and crc_ptr->tb_crc_success and ue_db.cfra_msg3_acked(crc_ptr->ue_index)) {
          // CFRA Msg3 ACKed: start UCI/SRS scheduling now that Msg3 is complete.
          uci_sched.add_ue(ue_cc->cfg());
          srs_sched.add_ue(ue_cc->cfg());
          if (not ue_cc->is_in_fallback_mode()) {
            slice_sched.config_applied(crc_ptr->ue_index);
          }
        }
        return event_result::processed;
      }

      // Process Timing Advance Offset.
      if (crc_ptr->tb_crc_success and crc_ptr->time_advance_offset.has_value() and crc_ptr->ul_sinr_dB.has_value()) {
        u.handle_ul_n_ta_update_indication(
            ue_cc->cell_index, crc_ptr->ul_sinr_dB.value(), crc_ptr->time_advance_offset.value());
      }

      // Log event.
      ev_logger.enqueue(scheduler_event_logger::crc_event{crc_ptr->ue_index,
                                                          crc_ptr->rnti,
                                                          cfg.cell_index,
                                                          sl_rx,
                                                          crc_ptr->harq_id,
                                                          crc_ptr->tb_crc_success,
                                                          crc_ptr->ul_sinr_dB});

      // Notify metrics handler.
      metrics.handle_crc_indication(sl_rx, *crc_ptr, tbs.value());

      return event_result::processed;
    };

    // Push UE CRC event.
    push_event(crc_ind.cell_index, event_t{"CRC", crc_ind.crcs[i].ue_index, std::move(crc_handle_impl)});
  }
}

ue_cell_event_manager::event_result ue_cell_event_manager::handle_uci_pdu(slot_point                     uci_sl,
                                                                          const uci_indication::uci_pdu& uci_pdu)
{
  // Fetch UE objects.
  if (not ue_db.contains(uci_pdu.ue_index)) {
    return event_result::invalid_ue;
  }
  ue&      u     = ue_db[uci_pdu.ue_index];
  ue_cell* ue_cc = u.find_cell(cfg.cell_index);
  if (ue_cc == nullptr) {
    return event_result::invalid_ue_cc;
  }

  // Process the PDU and determine resulting action.
  auto action = uci_selector.handle_uci_ind_pdu(uci_sl, uci_pdu);
  if (not action.has_value()) {
    // No action came out of this UCI PDU (likely needs to combine more UCI PDUs).
    return event_result::processed;
  }

  // Process DL HARQ-ACK bits.
  if (not action->harq_ack_bits.empty()) {
    handle_harq_ind(*ue_cc, uci_sl, action->uci_valid, action->harq_ack_bits, action->ul_sinr_dB);
  }

  // Process SRs.
  if (action->sr_detected) {
    // Handle SR indication.
    u.handle_sr_indication(uci_sl);

    if (ue_cc->is_in_fallback_mode()) {
      fallback_sched.handle_sr_indication(ue_cc->ue_index);
    }

    // Log SR event.
    ev_logger.enqueue(scheduler_event_logger::sr_event{ue_cc->ue_index, ue_cc->rnti()});

    // Report SR to metrics.
    metrics.handle_sr_indication(ue_cc->ue_index, uci_sl);
  }

  // Process CSI, if present.
  if (action->csi.has_value()) {
    handle_csi(*ue_cc, uci_sl, *action->csi);
  }

  // Process SINR and Timing Advance Offset.
  if (action->uci_valid and action->ul_sinr_dB.has_value()) {
    if (action->type == uci_action::pdu_type::pucch_f2f3f4) {
      ue_cc->get_pucch_power_controller().update_pucch_sinr_f2_f3_f4(
          uci_sl, action->ul_sinr_dB.value(), not action->harq_ack_bits.empty(), action->csi.has_value());
    } else {
      ue_cc->get_pucch_power_controller().update_pucch_sinr_f0_f1(uci_sl, *action->ul_sinr_dB);
    }

    if (action->time_advance_offset.has_value()) {
      u.handle_ul_n_ta_update_indication(ue_cc->cell_index, action->ul_sinr_dB.value(), *action->time_advance_offset);
    }
  }

  // Report the UCI PDU to the metrics handler.
  metrics.handle_uci_pdu_indication(uci_pdu.ue_index, *action);

  return event_result::processed;
}

void ue_cell_event_manager::handle_uci_indication(const uci_indication& ind)
{
  for (unsigned i = 0, e = ind.ucis.size(); i != e; ++i) {
    auto uci_ptr = ind_pdu_pool->create_pdu(ind.ucis[i]);
    if (uci_ptr == nullptr) {
      return;
    }

    auto uci_handle_impl = [this, uci_sl = ind.slot_rx, uci_pdu = std::move(uci_ptr)]() {
      return handle_uci_pdu(uci_sl, *uci_pdu);
    };
    push_event(ind.cell_index,
               event_t{"UCI",
                       ind.ucis[i].ue_index,
                       std::move(uci_handle_impl),
                       // Note: We do not warn if the UE is not found, because there is this transient
                       // period when the UE is about to receive and process the RRC Release, but it is
                       // still sending CSI or SR in the PUCCH. If we stop the PUCCH scheduling for the
                       // UE about to be released, we could risk interference between UEs in the PUCCH.
                       false});
  }
}

void ue_cell_event_manager::handle_srs_indication(const srs_indication& ind)
{
  for (unsigned i = 0, e = ind.srss.size(); i != e; ++i) {
    const srs_indication::srs_indication_pdu& srs_pdu     = ind.srss[i];
    auto                                      srs_pdu_ptr = ind_pdu_pool->create_pdu(srs_pdu);
    if (srs_pdu_ptr == nullptr) {
      return;
    }

    auto srs_handle_impl = [this, srs_ptr = std::move(srs_pdu_ptr)]() {
      // Fetch UE objects.
      if (not ue_db.contains(srs_ptr->ue_index)) {
        return event_result::invalid_ue;
      }
      ue&      u     = ue_db[srs_ptr->ue_index];
      ue_cell* ue_cc = u.find_cell(cfg.cell_index);
      if (ue_cc == nullptr) {
        return event_result::invalid_ue_cc;
      }

      // Indicate the channel matrix.
      ue_cc->handle_srs_channel_matrix(srs_ptr->channel_matrix);

      // Log event.
      ev_logger.enqueue(scheduler_event_logger::srs_indication_event{
          srs_ptr->ue_index, srs_ptr->rnti, ue_cc->channel_state_manager().get_latest_tpmi_select_info()});

      // Handle time aligment measurement if present.
      if (srs_ptr->time_advance_offset.has_value()) {
        // Assume some SINR for the TA feedback using the channel matrix topology and near zero
        // noise variance.
        float frobenius_norm = srs_ptr->channel_matrix.frobenius_norm();
        float noise_var      = near_zero;
        float sinr_dB        = convert_power_to_dB(frobenius_norm * frobenius_norm / noise_var);

        // Notify UL TA update.
        ue_db[ue_cc->ue_index].handle_ul_n_ta_update_indication(
            ue_cc->cell_index, sinr_dB, srs_ptr->time_advance_offset.value());

        // Report the SRS PDU to the metrics handler.
        metrics.handle_srs_indication(*srs_ptr, ue_cc->channel_state_manager().get_nof_ul_layers());
      }
      return event_result::processed;
    };

    push_event(ind.cell_index, event_t{"SRS", srs_pdu.ue_index, std::move(srs_handle_impl), false});
  }
}

void ue_cell_event_manager::handle_ul_phr_indication(const ul_phr_indication_message& phr_ind)
{
  auto phr_ind_ptr = ind_pdu_pool->create_pdu(phr_ind);
  if (phr_ind_ptr == nullptr) {
    return;
  }
  const du_cell_index_t pcell_index = phr_ind.cell_index;
  const du_ue_index_t   ue_index    = phr_ind.ue_index;

  auto handle_phr_impl = [this, phr_ind = std::move(phr_ind_ptr)]() {
    // Fetch UE objects.
    if (not ue_db.contains(phr_ind->ue_index)) {
      return event_result::invalid_ue;
    }
    auto& u = ue_db[phr_ind->ue_index];

    for (const cell_ph_report& cell_phr : phr_ind->phr.get_phr()) {
      ocudu_sanity_check(cell_phr.serv_cell_id < u.nof_cells(),
                         "Invalid serving cell index={}",
                         fmt::underlying(cell_phr.serv_cell_id));
      auto& ue_cc = u.get_cell(cell_phr.serv_cell_id);

      ue_cc.get_pusch_power_controller().handle_phr(cell_phr, phr_ind->slot_rx);

      // Log event.
      scheduler_event_logger::phr_event event{};
      event.ue_index   = phr_ind->ue_index;
      event.rnti       = phr_ind->rnti;
      event.cell_index = ue_cc.cell_index;
      event.ph         = cell_phr.ph;
      event.p_cmax     = cell_phr.p_cmax;
      ev_logger.enqueue(event);
    }

    // Notify metrics handler.
    metrics.handle_ul_phr_indication(*phr_ind);

    return event_result::processed;
  };

  push_event(pcell_index, event_t{"PHR", ue_index, std::move(handle_phr_impl)});
}

void ue_cell_event_manager::handle_dl_mac_ce_indication(const dl_mac_ce_indication& ce)
{
  auto handle_mac_ce_impl = [this, ce]() {
    if (not ue_db.contains(ce.ue_index)) {
      return event_result::invalid_ue;
    }
    auto& u = ue_db[ce.ue_index];

    // Notify SRB fallback scheduler upon receiving ConRes CE indication.
    if (ce.ce_lcid == lcid_dl_sch_t::UE_CON_RES_ID) {
      logger.warning("cell={} rnti={} ue={}: Discarding ConRes CE indication. Cause: The scheduler automatically "
                     "triggers this type of CE",
                     cfg.cell_index,
                     u.crnti,
                     u.ue_index);
      return event_result::processed;
    }

    // Forward CE to UE instance.
    u.handle_dl_mac_ce_indication(ce);

    // Log event.
    ev_logger.enqueue(ce);

    return event_result::processed;
  };

  push_event(cfg.cell_index, event_t{"DL MAC CE", ce.ue_index, std::move(handle_mac_ce_impl)});
}

void ue_cell_event_manager::handle_crnti_ce_received(du_ue_index_t ue_index)
{
  auto handle_crnti_ce_received = [this, ue_index]() {
    if (not ue_db.contains(ue_index)) {
      return event_result::invalid_ue;
    }
    auto& u     = ue_db[ue_index];
    auto& ue_cc = u.get_pcell();
    if (ue_cc.cell_index != cfg.cell_index) {
      logger.warning("cell={} ue={} rnti={}: Discarding C-RNTI CE. It was received in cell that is not PCell ({})",
                     cfg.cell_index,
                     ue_index,
                     u.crnti,
                     ue_cc.cell_index);
      return event_result::invalid_ue_cc;
    }

    if (ue_db.crnti_ce_received(ue_index)) {
      // C-RNTI CE received: contention resolution completed for this F1AP-created UE.

      // Initiate UCI and SRS schedulers with the confirmed UE resources.
      uci_sched.add_ue(ue_cc.cfg());
      srs_sched.add_ue(ue_cc.cfg());

      // Notify slice scheduler only when the UE fully exits fallback (config also applied).
      if (not ue_cc.is_in_fallback_mode()) {
        slice_sched.config_applied(ue_index);
      }
    }

    return event_result::processed;
  };

  push_event(cfg.cell_index, event_t{"CRNTI CE received", ue_index, std::move(handle_crnti_ce_received)});
}

void ue_cell_event_manager::handle_positioning_measurement_request(
    const positioning_measurement_request::cell_info& req)
{
  ocudu_assert(req.cell_index == cfg.cell_index, "Received positioning request for wrong cell");

  auto req_ptr = ind_pdu_pool->create_pdu(req);
  if (req_ptr == nullptr) {
    return;
  }

  const du_cell_index_t cell_index = req.cell_index;
  auto                  task       = [this, req_ptr = std::move(req_ptr)]() {
    srs_sched.handle_positioning_measurement_request(*req_ptr);
    return event_result::processed;
  };

  push_event(cell_index, event_t{"POS MEAS REQ", std::move(task)});
}

void ue_cell_event_manager::handle_positioning_measurement_stop(rnti_t pos_rnti)
{
  auto task = [this, pos_rnti]() {
    srs_sched.handle_positioning_measurement_stop(pos_rnti);
    return event_result::processed;
  };

  push_event(cfg.cell_index, event_t{"pos_meas_stop", std::move(task)});
}

void ue_cell_event_manager::handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& bs)
{
  dl_bo_mng->handle_dl_buffer_state_indication(bs);
}

static void handle_discarded_pusch(const cell_slot_resource_allocator& prev_slot_result, ue_repository& ue_db)
{
  for (const ul_sched_info& grant : prev_slot_result.result.ul.puschs) {
    ue* u = ue_db.find_by_rnti(grant.pusch_cfg.rnti);
    if (u == nullptr) {
      // UE has been removed.
      continue;
    }

    // - The lower layers will not attempt to decode the PUSCH and will not send any CRC indication.
    std::optional<ul_harq_process_handle> h_ul = u->get_pcell().harqs.ul_harq(to_harq_id(grant.pusch_cfg.harq_id));
    if (h_ul.has_value()) {
      // Note: We don't use this cancellation to update the UL OLLA, as we shouldn't take lates into account in link
      // adaptation.
      if (h_ul->nof_retxs() == 0) {
        // Given that the PUSCH grant was discarded before it reached the PHY, the "new_data" flag was not handled
        // and the UL softbuffer was not reset. To avoid mixing different TBs in the softbuffer, it is important to
        // reset the UL HARQ process.
        h_ul->reset();
      } else {
        // To avoid a long UL HARQ timeout window (due to lack of CRC indication), it is important to force a NACK
        // in the UL HARQ process.
        h_ul->ul_crc_info(false);
      }
    }
  }
}

void ue_cell_event_manager::handle_error_indication(slot_point sl_tx, scheduler_slot_handler::error_outcome event)
{
  auto handle_error_impl = [this, sl_tx, event]() {
    // Handle Error Indication.

    const cell_slot_resource_allocator* prev_slot_result = res_grid.get_history(sl_tx);
    if (prev_slot_result == nullptr) {
      logger.warning("cell={}, slot={}: Discarding error indication. Cause: Scheduler results associated with the slot "
                     "of the error indication have already been erased (current slot={})",
                     fmt::underlying(cfg.cell_index),
                     sl_tx,
                     last_sl_tx);
      return event_result::processed;
    }

    // In case DL PDCCHs were skipped, there will be the following consequences:
    // - The UE will not decode the PDSCH and will not send the respective UCI.
    // - The UE won't update the HARQ NDI, if new HARQ TB.
    // - The UCI indication coming later from the lower layers will likely contain a HARQ-ACK=DTX.
    // In case UL PDCCHs were skipped, there will be the following consequences:
    // - The UE will not decode the PUSCH.
    // - The UE won't update the HARQ NDI, if new HARQ TB.
    // - The CRC indication coming from the lower layers will likely be CRC=KO.
    // - Any UCI in the respective PUSCH will be likely reported as HARQ-ACK=DTX.
    // In neither of the cases, the HARQs will timeout, because we did not lose the UCI/CRC indications in the
    // lower layers. We do not need to cancel associated PUSCH grant (in UL PDCCH case) because it is important
    // that the PUSCH "new_data" flag reaches the lower layers, telling them whether the UL HARQ buffer needs to
    // be reset or not. Cancelling HARQ retransmissions is dangerous as it increases the chances of NDI
    // ambiguity.

    // In case of PDSCH grants being discarded, there will be the following consequences:
    // - If the PDCCH was not discarded,the UE will fail to decode the PDSCH and will send an HARQ-ACK=NACK. The
    // scheduler will retransmit the respective DL HARQ. No actions required.

    // In case of PUCCH and PUSCH grants being discarded.
    if (event.pusch_and_pucch_discarded) {
      handle_discarded_pusch(*prev_slot_result, ue_db);

      uci_selector.handle_discarded_ucis(sl_tx);
    }

    // Log event.
    ev_logger.enqueue(scheduler_event_logger::error_indication_event{sl_tx, event});

    // Report metrics.
    metrics.handle_error_indication();

    return event_result::processed;
  };

  push_event(cfg.cell_index, event_t{"error_ind", std::move(handle_error_impl)});
}

void ue_cell_event_manager::handle_slice_reconfiguration_request(const du_cell_slice_reconfig_request& req)
{
  auto req_ptr = ind_pdu_pool->create_pdu(req);
  if (req_ptr == nullptr) {
    return;
  }

  auto handle_slice_reconfig_impl = [this, req_ptr = std::move(req_ptr)]() {
    // Handle slice reconfiguration.
    slice_sched.handle_slice_reconfiguration_request(*req_ptr);

    // Log event.
    ev_logger.enqueue(scheduler_event_logger::slice_reconfiguration_event{req_ptr->cell_index});

    return event_result::processed;
  };

  push_event(cfg.cell_index, event_t{"slice_reconf", std::move(handle_slice_reconfig_impl)});
}

void ue_cell_event_manager::handle_harq_ind(ue_cell&                             ue_cc,
                                            slot_point                           uci_sl,
                                            bool                                 uci_valid,
                                            const bounded_bitset<MAX_NOF_HARQS>& harq_bits,
                                            std::optional<float>                 pucch_snr)
{
  metrics.handle_uci_with_harq_ack(ue_cc.ue_index, uci_sl, pucch_snr.has_value());

  mac_harq_ack_report_status status = mac_harq_ack_report_status::dtx;
  for (unsigned harq_idx = 0, harq_end_idx = harq_bits.size(); harq_idx != harq_end_idx; ++harq_idx) {
    // Possible report scenarios: (i) ACK, (ii) NACK, (iii) UCI invalid, (iv) UCI timeout.
    // The (iii) and (iv) are treated as HARQ report status "DTX" to not affect the DL OLLA.
    if (uci_valid) {
      status = harq_bits.test(harq_idx) ? mac_harq_ack_report_status::ack : mac_harq_ack_report_status::nack;
    }

    // Update UE HARQ state with received HARQ-ACK.
    std::optional<dl_harq_process_handle> h_dl = ue_cc.handle_dl_ack_info(uci_sl, status, harq_idx, pucch_snr);
    if (not h_dl.has_value()) {
      // HARQ process was not found or in invalid state. Move on to next HARQ bit.
      continue;
    }
    const units::bytes tbs{h_dl->get_grant_params().tbs};

    // Log Event.
    ev_logger.enqueue(scheduler_event_logger::harq_ack_event{
        ue_cc.ue_index, ue_cc.rnti(), ue_cc.cell_index, uci_sl, h_dl->id(), status, tbs});

    // NOTE: this is for the first attachment only. In this case, the first ACK is the one that acks the ConRes or the
    // ConRes + MSG4; there is only 1 HARQ process waiting for ACKs, which acks the ConRes.
    if (h_dl->empty() and ue_cc.is_pcell() and
        ue_cc.get_pcell_state().conres_st == ue_conres_state::pending_conres_ce) {
      ue_db.handle_conres_ce_outcome(ue_cc.ue_index, true);
    }

    // Notify metrics handler with HARQ outcome.
    metrics.handle_dl_harq_ack(ue_cc.ue_index, status == mac_harq_ack_report_status::ack, tbs);
  }
}

void ue_cell_event_manager::handle_csi(ue_cell& ue_cc, slot_point sl_rx, const csi_report_data& csi_rep)
{
  // Forward CSI bits to UE.
  ue_cc.handle_csi_report(csi_rep);

  // Log event.
  ev_logger.enqueue(scheduler_event_logger::csi_report_event{ue_cc.ue_index, ue_cc.rnti(), sl_rx, csi_rep});
}

void ue_cell_event_manager::handle_uci_indication_timeout(slot_point uci_slot, rnti_t crnti, const uci_action& action)
{
  // Notify respective DL HARQ that the UCI went missing.
  ue* u = parent.ue_db.find_by_rnti(crnti);
  if (u == nullptr) {
    parent.logger.warning("rnti={}: UCI timeout detected for unknown UE at UCI slot={}", crnti, uci_slot);
    return;
  }
  ue_cell* ue_cc = u->find_cell(cfg.cell_index);
  if (ue_cc == nullptr) {
    parent.logger.warning("cell={} rnti={} ue={}: UCI timeout detected for unknown UE carrier at UCI slot={}",
                          cfg.cell_index,
                          crnti,
                          u->ue_index,
                          uci_slot);
    return;
  }

  // Forward HARQ-ACK bits.
  if (not action.harq_ack_bits.empty()) {
    handle_harq_ind(*ue_cc, uci_slot, action.uci_valid, action.harq_ack_bits, action.ul_sinr_dB);
  }

  // Forward SR indication, if pending.
  if (action.sr_detected) {
    u->handle_sr_indication(uci_slot);

    if (ue_cc->is_in_fallback_mode()) {
      fallback_sched.handle_sr_indication(ue_cc->ue_index);
    }

    // Log SR event.
    ev_logger.enqueue(scheduler_event_logger::sr_event{ue_cc->ue_index, ue_cc->rnti()});

    // Report SR to metrics.
    metrics.handle_sr_indication(ue_cc->ue_index, uci_slot);
  }
}

void ue_cell_event_manager::push_event(du_cell_index_t cell_index, event_t event)
{
  if (OCUDU_UNLIKELY(not active.load(std::memory_order_acquire))) {
    // Note: PHY events should not arrive after the cell has been stopped.
    if (event.ue_index == INVALID_DU_UE_INDEX) {
      logger.warning(
          "cell={}: Discarding {} event. Cause: Cell is not active", fmt::underlying(cell_index), event.ev_name);
    } else {
      logger.warning("cell={} ue={}: Discarding {} event. Cause: Cell is not active",
                     fmt::underlying(cell_index),
                     fmt::underlying(event.ue_index),
                     event.ev_name);
    }
    return;
  }

  const du_ue_index_t ue_idx  = event.ue_index;
  const char*         ev_name = event.ev_name;
  if (not pending_events.try_push(std::move(event))) {
    if (ue_idx == INVALID_DU_UE_INDEX) {
      logger.warning("cell={}: Discarding {} event. Cause: Event queue is full", fmt::underlying(cell_index), ev_name);
    } else {
      logger.warning("cell={} ue={}: Discarding {} event. Cause: Event queue is full",
                     fmt::underlying(cell_index),
                     fmt::underlying(ue_idx),
                     ev_name);
    }
  }
}

void ue_cell_event_manager::log_invalid_ue_index(du_ue_index_t ue_index,
                                                 const char*   event_name,
                                                 bool          warn_if_ignored) const
{
  ocudulog::log_channel& log_channel = warn_if_ignored ? logger.warning : logger.info;
  log_channel("cell={} ue={}: Discarding {} event. Cause: UE with provided Id does not exist",
              fmt::underlying(cfg.cell_index),
              ue_index,
              event_name);
}

void ue_cell_event_manager::log_invalid_cc(du_ue_index_t ue_idx, const char* event_name, bool warn_if_ignored) const
{
  ocudulog::log_channel& log_channel = warn_if_ignored ? logger.warning : logger.info;
  log_channel("cell={} ue={}: Discarding {} event. Cause: UE is not configured in this cell",
              fmt::underlying(cfg.cell_index),
              ue_idx,
              event_name);
}

ue_event_manager::ue_event_manager(ue_repository& ue_db_) : ue_db(ue_db_), logger(ocudulog::fetch_basic_logger("SCHED"))
{
  std::fill(cells.begin(), cells.end(), nullptr);
}

std::unique_ptr<ue_cell_event_manager> ue_event_manager::add_cell(const cell_creation_event& cell_ev)
{
  const du_cell_index_t cell_index = cell_ev.cell_res_grid.cell_index();
  ocudu_assert(not cell_exists(cell_index), "Overwriting cell configurations not supported");

  // Create ue_cell_event_manager.
  auto cell = std::make_unique<ue_cell_event_manager>(*this, cell_ev, ue_db, logger);

  // Register cell.
  cells[cell_index] = cell.get();

  return cell;
}

bool ue_event_manager::cell_exists(du_cell_index_t cell_index) const
{
  return cell_index < MAX_NOF_DU_CELLS and cells[cell_index] != nullptr;
}
