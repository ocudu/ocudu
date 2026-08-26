// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cell_event_manager.h"
#include "cell/resource_grid.h"
#include "common_scheduling/paging_scheduler.h"
#include "common_scheduling/ra_scheduler.h"
#include "common_scheduling/ra_ue_repository.h"
#include "common_scheduling/si_scheduler.h"
#include "config/cell_configuration.h"
#include "logging/cell_metrics_handler.h"
#include "logging/scheduler_event_logger.h"
#include "srs/srs_scheduler.h"
#include "uci_scheduling/uci_indication_selector.h"
#include "ue_context/ue_cell_repository.h"
#include "ocudu/adt/mpmc_queue.h"
#include "ocudu/adt/unique_function.h"
#include "ocudu/support/math/math_utils.h"
#include "ocudu/support/memory_pool/bounded_object_pool.h"

using namespace ocudu;

/// \brief Queue of the events pending to be processed by a cell, and pools of their payloads.
///
/// Events are pushed from any executor and dispatched in the cell scheduler executor.
class ocudu::cell_event_dispatcher
{
  // [Implementation defined] Number of paging requests that can be in flight at any moment.
  static constexpr size_t PAGING_POOL_SIZE = 128;
  // [Implementation defined] System information updates are rare, so only a few can be in flight at any moment.
  static constexpr size_t SI_POOL_SIZE = 4;
  // [Implementation defined] Lower layers report at most one RACH and one CRC indication per slot, so the pools only
  // need to cover the slots that can elapse before the events are dispatched.
  static constexpr size_t PHY_IND_POOL_SIZE = 8;
  // [Implementation defined] Positioning measurements are rare, so only a few can be in flight at any moment.
  static constexpr size_t POSITIONING_POOL_SIZE = 4;
  // [Implementation defined] Number of UCI PDUs that can be in flight at any moment.
  static constexpr size_t UCI_POOL_SIZE = MAX_PUCCH_PDUS_PER_SLOT * 4;
  // [Implementation defined] Number of SRS PDUs that can be in flight at any moment.
  static constexpr size_t SRS_POOL_SIZE = MAX_SRS_PDUS_PER_SLOT * 4;

  /// \brief Capacity of the queue of pending events.
  ///
  /// It holds every payload the pools can hand out, so that an event type is only ever limited by its own pool and
  /// never by another type having filled the queue.
  static constexpr size_t EVENT_QUEUE_SIZE = PAGING_POOL_SIZE +     // paging
                                             2 * SI_POOL_SIZE +     // SI and ETWS/CMAS SI updates
                                             2 * PHY_IND_POOL_SIZE; // RACH and CRC indications

  using paging_pool  = bounded_object_pool<sched_paging_information>;
  using si_pool      = bounded_object_pool<si_scheduling_update_request>;
  using pws_si_pool  = bounded_object_pool<pws_si_scheduling_update_request>;
  using rach_pool    = bounded_object_pool<rach_indication_message>;
  using crc_pool     = bounded_object_pool<ul_crc_indication>;
  using pos_req_pool = bounded_object_pool<positioning_measurement_request::cell_info>;
  using srs_pool     = bounded_object_pool<srs_indication::srs_indication_pdu>;
  using uci_pool     = bounded_object_pool<uci_indication::uci_pdu>;

  /// Event enqueued and dispatched by this class.
  struct event_t {
    static constexpr size_t callback_capacity = 48;
    using callback_type                       = unique_function<void(), callback_capacity, true>;

    callback_type callback;
    const char*   ev_name = "invalid";

    event_t() = default;
    template <typename Callable>
    event_t(const char* ev_name_, Callable&& callable) : callback(std::forward<Callable>(callable)), ev_name(ev_name_)
    {
    }
  };

  using event_queue = concurrent_queue<event_t, concurrent_queue_policy::lockfree_mpmc>;

public:
  cell_event_dispatcher(const cell_configuration& cell_cfg_, ocudulog::basic_logger& logger_) :
    cell_cfg(cell_cfg_),
    logger(logger_),
    pending_pagings(PAGING_POOL_SIZE),
    pending_si_reqs(SI_POOL_SIZE),
    pending_pws_si_reqs(SI_POOL_SIZE),
    pending_rachs(PHY_IND_POOL_SIZE),
    pending_crcs(PHY_IND_POOL_SIZE),
    pending_pos_reqs(POSITIONING_POOL_SIZE),
    pending_srss(SRS_POOL_SIZE),
    pending_ucis(UCI_POOL_SIZE),
    pending_events(EVENT_QUEUE_SIZE)
  {
  }

  /// Allow events to be pushed.
  void start() { active.store(true, std::memory_order_release); }

  /// Stop accepting events and discard the pending ones.
  void stop()
  {
    active.store(false, std::memory_order_release);

    event_t ev;
    while (pending_events.try_pop(ev)) {
    }
  }

  /// Dispatch the pending events.
  void run()
  {
    event_t ev;
    while (pending_events.try_pop(ev)) {
      ev.callback();
    }
  }

  /// \brief Create an event payload managed by an object pool.
  /// \return \c nullptr if the respective pool is exhausted.
  template <typename PDUType>
  auto create_pdu(const PDUType& pdu)
  {
    auto* pool = std::get<bounded_object_pool<std::decay_t<PDUType>>*>(pools);
    auto  ret  = pool->get();
    if (ret != nullptr) {
      *ret = pdu;
    } else {
      logger.warning("cell={}: Discarding {} event. Cause: Payload pool is empty",
                     fmt::underlying(cell_cfg.cell_index),
                     pdu_type_name<std::decay_t<PDUType>>());
    }
    return ret;
  }

  /// Enqueue an event to be dispatched in the next slot indication of this cell.
  template <typename Callable>
  void push(const char* ev_name, Callable&& callable)
  {
    if (OCUDU_UNLIKELY(not active.load(std::memory_order_acquire))) {
      logger.warning(
          "cell={}: Discarding {} event. Cause: Cell is not active", fmt::underlying(cell_cfg.cell_index), ev_name);
      return;
    }

    if (not pending_events.try_push(event_t{ev_name, std::forward<Callable>(callable)})) {
      logger.warning(
          "cell={}: Discarding {} event. Cause: Event queue is full", fmt::underlying(cell_cfg.cell_index), ev_name);
    }
  }

private:
  /// Returns a human-readable name for an event payload type.
  template <typename PDUType>
  static constexpr const char* pdu_type_name()
  {
    if constexpr (std::is_same_v<PDUType, sched_paging_information>) {
      return "paging";
    } else if constexpr (std::is_same_v<PDUType, si_scheduling_update_request>) {
      return "SI update";
    } else if constexpr (std::is_same_v<PDUType, pws_si_scheduling_update_request>) {
      return "ETWS/CMAS SI update";
    } else if constexpr (std::is_same_v<PDUType, rach_indication_message>) {
      return "RACH";
    } else if constexpr (std::is_same_v<PDUType, ul_crc_indication>) {
      return "CRC";
    } else if constexpr (std::is_same_v<PDUType, positioning_measurement_request::cell_info>) {
      return "positioning request";
    } else if constexpr (std::is_same_v<PDUType, srs_indication::srs_indication_pdu>) {
      return "SRS";
    } else if constexpr (std::is_same_v<PDUType, uci_indication::uci_pdu>) {
      return "UCI";
    } else {
      return "unknown";
    }
  }

  const cell_configuration& cell_cfg;
  ocudulog::basic_logger&   logger;

  paging_pool  pending_pagings;
  si_pool      pending_si_reqs;
  pws_si_pool  pending_pws_si_reqs;
  rach_pool    pending_rachs;
  crc_pool     pending_crcs;
  pos_req_pool pending_pos_reqs;
  srs_pool     pending_srss;
  uci_pool     pending_ucis;

  std::tuple<paging_pool*, si_pool*, pws_si_pool*, rach_pool*, crc_pool*, pos_req_pool*, srs_pool*, uci_pool*> pools{
      &pending_pagings,
      &pending_si_reqs,
      &pending_pws_si_reqs,
      &pending_rachs,
      &pending_crcs,
      &pending_pos_reqs,
      &pending_srss,
      &pending_ucis};

  event_queue pending_events;

  // Whether the cell is currently accepting events.
  std::atomic<bool> active{true};
};

cell_event_manager::cell_event_manager(const cell_configuration&      cell_cfg_,
                                       cell_resource_allocator&       res_grid_,
                                       ue_cell_repository&            ue_cell_db_,
                                       si_scheduler&                  si_sch_,
                                       paging_scheduler&              pg_sch_,
                                       ra_scheduler&                  ra_sch_,
                                       srs_scheduler&                 srs_sch_,
                                       cell_ue_event_notifier&        ue_ev_notifier_,
                                       ra_ue_repository&              ra_ue_repo_,
                                       uci_indication_selector&       uci_sel_,
                                       cell_metrics_handler&          metrics_,
                                       scheduler_event_logger&        ev_logger_,
                                       schedtrace::cell_event_tracer& ev_tracer_,
                                       ocudulog::basic_logger&        logger_) :
  cell_cfg(cell_cfg_),
  res_grid(res_grid_),
  ue_cell_db(ue_cell_db_),
  si_sch(si_sch_),
  pg_sch(pg_sch_),
  ra_sch(ra_sch_),
  srs_sch(srs_sch_),
  ue_ev_notifier(ue_ev_notifier_),
  ra_ue_repo(ra_ue_repo_),
  uci_sel(uci_sel_),
  metrics(metrics_),
  ev_logger(ev_logger_),
  ev_tracer(ev_tracer_),
  logger(logger_),
  dispatcher(std::make_unique<cell_event_dispatcher>(cell_cfg_, logger_))
{
}

cell_event_manager::~cell_event_manager() = default;

void cell_event_manager::start()
{
  dispatcher->start();
}

void cell_event_manager::stop()
{
  dispatcher->stop();
}

void cell_event_manager::run_slot()
{
  dispatcher->run();
}

void cell_event_manager::handle_paging_information(const sched_paging_information& pi)
{
  auto pi_ptr = dispatcher->create_pdu(pi);
  if (pi_ptr == nullptr) {
    return;
  }

  dispatcher->push("paging", [this, pi_ptr = std::move(pi_ptr)]() { pg_sch.handle_paging_information(*pi_ptr); });
}

void cell_event_manager::handle_si_update_request(const si_scheduling_update_request& req)
{
  auto req_ptr = dispatcher->create_pdu(req);
  if (req_ptr == nullptr) {
    return;
  }

  dispatcher->push("SI update", [this, req_ptr = std::move(req_ptr)]() { si_sch.handle_si_update_request(*req_ptr); });
}

void cell_event_manager::handle_pws_si_update_request(const pws_si_scheduling_update_request& req)
{
  auto req_ptr = dispatcher->create_pdu(req);
  if (req_ptr == nullptr) {
    return;
  }

  dispatcher->push("PWS SI update",
                   [this, req_ptr = std::move(req_ptr)]() { si_sch.handle_pws_si_update_request(*req_ptr); });
}

void cell_event_manager::handle_rach_indication(const rach_indication_message& msg)
{
  auto msg_ptr = dispatcher->create_pdu(msg);
  if (msg_ptr == nullptr) {
    return;
  }

  dispatcher->push("RACH", [this, msg_ptr = std::move(msg_ptr)]() { ra_sch.handle_rach_indication(*msg_ptr); });
}

void cell_event_manager::handle_crc_indication(const ul_crc_indication& crc_ind)
{
  auto crc_ptr = dispatcher->create_pdu(crc_ind);
  if (crc_ptr == nullptr) {
    return;
  }

  dispatcher->push("CRC", [this, crc_ptr = std::move(crc_ptr)]() {
    // The RA scheduler selects the CRCs that belong to the RA procedure.
    ra_sch.handle_crc_indication(*crc_ptr);

    for (const ul_crc_pdu_indication& crc : crc_ptr->crcs) {
      if (crc.ue_index == INVALID_DU_UE_INDEX) {
        // The CRC of a UE that has not been created yet, and is therefore only of interest to the RA scheduler.
        continue;
      }
      handle_ue_crc(crc_ptr->sl_rx, crc);
    }
  });
}

void cell_event_manager::handle_ue_crc(slot_point sl_rx, const ul_crc_pdu_indication& crc)
{
  ue_cell* ue_cc = ue_cell_db.find(crc.ue_index);
  if (ue_cc == nullptr) {
    return;
  }

  // Update HARQ.
  const bool was_pending_cfra = ue_cc->get_pcell_state().conres_st == ue_conres_state::pending_cfra;
  const auto crc_process_res  = ue_cc->handle_crc_pdu(sl_rx, crc);
  if (not crc_process_res.has_value()) {
    // The Msg3 HARQ of a contention-free access is owned by the RA scheduler. Completing the contention resolution
    // needs the state shared by the UEs of the cell group, so hand it over to the UE scheduler.
    if (was_pending_cfra and crc.tb_crc_success) {
      ue_ev_notifier.on_cfra_msg3_acked(crc.ue_index);
    }
    return;
  }

  // \ref pusch_transmitted is true if the gnb detected that the PUSCH was transmitted, false if it was DTX.
  auto [tbs, pusch_transmitted] = crc_process_res.value();
  if (not pusch_transmitted) {
    ev_logger.enqueue(scheduler_event_logger::crc_event{crc.ue_index,
                                                        crc.rnti,
                                                        cell_cfg.cell_index,
                                                        sl_rx,
                                                        crc.harq_id,
                                                        scheduler_event_logger::crc_event::crc_res_t::dtx,
                                                        crc.ul_sinr_dB});
    return;
  }

  // Process Timing Advance Offset.
  if (crc.tb_crc_success and crc.time_advance_offset.has_value() and crc.ul_sinr_dB.has_value()) {
    ue_cc->handle_ul_n_ta_update_indication(crc.ul_sinr_dB.value(), crc.time_advance_offset.value());
  }

  // Log event.
  ev_logger.enqueue(scheduler_event_logger::crc_event{crc.ue_index,
                                                      crc.rnti,
                                                      cell_cfg.cell_index,
                                                      sl_rx,
                                                      crc.harq_id,
                                                      crc.tb_crc_success
                                                          ? scheduler_event_logger::crc_event::crc_res_t::ok
                                                          : scheduler_event_logger::crc_event::crc_res_t::ko,
                                                      crc.ul_sinr_dB});

  // Notify metrics handler.
  metrics.handle_crc_indication(sl_rx, crc, tbs);
}

void cell_event_manager::handle_positioning_measurement_request(const positioning_measurement_request::cell_info& req)
{
  ocudu_assert(req.cell_index == cell_cfg.cell_index, "Received positioning request for wrong cell");

  auto req_ptr = dispatcher->create_pdu(req);
  if (req_ptr == nullptr) {
    return;
  }

  dispatcher->push("positioning request", [this, req_ptr = std::move(req_ptr)]() {
    srs_sch.handle_positioning_measurement_request(*req_ptr);
  });
}

void cell_event_manager::handle_positioning_measurement_stop(rnti_t pos_rnti)
{
  dispatcher->push("positioning stop", [this, pos_rnti]() { srs_sch.handle_positioning_measurement_stop(pos_rnti); });
}

void cell_event_manager::handle_srs_indication(const srs_indication& srs)
{
  for (const srs_indication::srs_indication_pdu& srs_pdu : srs.srss) {
    auto srs_ptr = dispatcher->create_pdu(srs_pdu);
    if (srs_ptr == nullptr) {
      return;
    }

    dispatcher->push("SRS", [this, srs_ptr = std::move(srs_ptr)]() {
      ue_cell* ue_cc = ue_cell_db.find(srs_ptr->ue_index);
      if (ue_cc == nullptr) {
        return;
      }

      // Indicate the channel matrix.
      ue_cc->handle_srs_channel_matrix(srs_ptr->channel_matrix);

      // Log event.
      ev_logger.enqueue(scheduler_event_logger::srs_indication_event{
          srs_ptr->ue_index, srs_ptr->rnti, ue_cc->channel_state_manager().get_latest_tpmi_select_info()});

      // Handle time aligment measurement if present.
      if (srs_ptr->time_advance_offset.has_value()) {
        // Assume some SINR for the TA feedback using the channel matrix topology and near zero noise variance.
        const float frobenius_norm = srs_ptr->channel_matrix.frobenius_norm();
        const float noise_var      = near_zero;
        const float sinr_dB        = convert_power_to_dB(frobenius_norm * frobenius_norm / noise_var);

        // Notify UL TA update.
        ue_cc->handle_ul_n_ta_update_indication(sinr_dB, srs_ptr->time_advance_offset.value());

        // Report the SRS PDU to the metrics handler.
        metrics.handle_srs_indication(*srs_ptr, ue_cc->channel_state_manager().get_nof_ul_layers());
      }
    });
  }
}

void cell_event_manager::handle_uci_indication(const uci_indication& uci)
{
  for (const uci_indication::uci_pdu& pdu : uci.ucis) {
    auto uci_ptr = dispatcher->create_pdu(pdu);
    if (uci_ptr == nullptr) {
      return;
    }

    // Note: The removal of a UE is deferred until it has no HARQ awaiting feedback, so a UCI of a grant that was
    // scheduled for it is not lost. Its CSI and SR carry none, and it keeps sending them until it processes the RRC
    // Release, so a UCI of an unknown UE is expected and not worth reporting.
    dispatcher->push(
        "UCI", [this, uci_sl = uci.slot_rx, uci_ptr = std::move(uci_ptr)]() { handle_uci_pdu(uci_sl, *uci_ptr); });
  }
}

void cell_event_manager::handle_uci_pdu(slot_point uci_sl, const uci_indication::uci_pdu& uci_pdu)
{
  // The UE context may not exist yet if this is the successRAR's own HARQ-ACK PUCCH (2-step RACH), whose common PUCCH
  // resource is allocated by the RA scheduler before UE creation completes.
  ue_cell* ue_cc = ue_cell_db.find(uci_pdu.ue_index);
  if (ue_cc == nullptr) {
    return;
  }

  // Process the PDU and determine resulting action.
  auto action = uci_sel.handle_uci_ind_pdu(uci_sl, uci_pdu);
  if (not action.has_value()) {
    // No action came out of this UCI PDU (likely needs to combine more UCI PDUs).
    return;
  }

  // Process DL HARQ-ACK bits.
  // Note: the slot of the UCI grant is used, rather than the slot in which the PDU was received, as they differ in the
  // case of a multi-slot PUCCH repetition burst, and it is the former that the DL HARQ processes are keyed on.
  if (not action->harq_ack_bits.empty()) {
    handle_harq_ind(*ue_cc, action->uci_slot, action->uci_valid, action->harq_ack_bits, action->ul_sinr_dB);
  }

  // Process SRs.
  if (action->sr_detected) {
    // Serving the SR needs the logical channels of the UE, so hand it over to the UE scheduler.
    ue_ev_notifier.on_sr_detected(ue_cc->ue_index, uci_sl);

    // Log SR event.
    sr_event event{ue_cc->ue_index, ue_cc->rnti()};
    ev_tracer.on_event(event);
    ev_logger.enqueue(event);

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
      ue_cc->handle_ul_n_ta_update_indication(action->ul_sinr_dB.value(), *action->time_advance_offset);
    }
  }

  // Report the UCI PDU to the metrics handler.
  metrics.handle_uci_pdu_indication(uci_pdu.ue_index, *action);
}

bool cell_event_manager::is_msgb_harq_ack_slot(rnti_t rnti, slot_point uci_sl) const
{
  auto ra_it = ra_ue_repo.find(rnti);
  return ra_it != ra_ue_repo.end() and ra_it->msgb_ack_slot_tx.has_value() and *ra_it->msgb_ack_slot_tx == uci_sl;
}

void cell_event_manager::handle_harq_ind(ue_cell&                             ue_cc,
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
      // HARQ process was not found or in invalid state. This is expected for the successRAR's own HARQ-ACK PUCCH
      // (2-step RACH), allocated by the RA scheduler against a common PUCCH resource rather than tracked here; only
      // warn when that fallback explanation doesn't hold.
      if (not is_msgb_harq_ack_slot(ue_cc.rnti(), uci_sl)) {
        logger.warning("rnti={}: Discarding ACK info. Cause: DL HARQ for uci slot={} and HARQ-ACK bit={} not found.",
                       ue_cc.rnti(),
                       uci_sl,
                       harq_idx);
      }
      continue;
    }
    const units::bytes tbs{h_dl->get_grant_params().tbs};

    // Log Event.
    harq_ack_event event{ue_cc.ue_index, ue_cc.rnti(), ue_cc.cell_index, uci_sl, h_dl->id(), status, tbs};
    ev_tracer.on_event(event);
    ev_logger.enqueue(event);

    // NOTE: this is for the first attachment only. In this case, the first ACK is the one that acks the ConRes or the
    // ConRes + MSG4; there is only 1 HARQ process waiting for ACKs, which acks the ConRes.
    if (h_dl->empty() and ue_cc.is_pcell() and
        ue_cc.get_pcell_state().conres_st == ue_conres_state::pending_conres_ce) {
      // Completing the contention resolution needs the state shared by the UEs of the cell group.
      ue_ev_notifier.on_conres_ce_acked(ue_cc.ue_index);
    }

    // Notify metrics handler with HARQ outcome.
    metrics.handle_dl_harq_ack(ue_cc.ue_index, status == mac_harq_ack_report_status::ack, tbs);
  }
}

void cell_event_manager::handle_csi(ue_cell& ue_cc, slot_point sl_rx, const csi_report_data& csi_rep)
{
  // Forward CSI bits to UE.
  ue_cc.handle_csi_report(csi_rep);

  // Log event.
  csi_report_event event{ue_cc.ue_index, ue_cc.rnti(), sl_rx, csi_rep};
  ev_tracer.on_event(event);
  ev_logger.enqueue(event);
}

void cell_event_manager::handle_uci_indication_timeout(slot_point uci_slot, rnti_t crnti, const uci_action& action)
{
  // Notify respective DL HARQ that the UCI went missing.
  ue_cell* ue_cc = ue_cell_db.find_by_rnti(crnti);
  if (ue_cc == nullptr) {
    // The UE context may not exist yet if this is the successRAR's own HARQ-ACK PUCCH (2-step RACH), whose common
    // PUCCH resource is allocated by the RA scheduler before UE creation completes.
    if (not is_msgb_harq_ack_slot(crnti, uci_slot)) {
      logger.warning("rnti={}: UCI timeout detected for unknown UE at UCI slot={}", crnti, uci_slot);
    }
    return;
  }

  // Forward HARQ-ACK bits.
  if (not action.harq_ack_bits.empty()) {
    handle_harq_ind(*ue_cc, uci_slot, action.uci_valid, action.harq_ack_bits, action.ul_sinr_dB);
  }

  // Forward SR indication, if pending.
  if (action.sr_detected) {
    ue_ev_notifier.on_sr_detected(ue_cc->ue_index, uci_slot);

    // Log SR event.
    sr_event event{ue_cc->ue_index, ue_cc->rnti()};
    ev_tracer.on_event(event);
    ev_logger.enqueue(event);

    // Report SR to metrics.
    metrics.handle_sr_indication(ue_cc->ue_index, uci_slot);
  }
}

static void handle_discarded_pusch(const cell_slot_resource_allocator& prev_slot_result, ue_cell_repository& ue_cell_db)
{
  for (const ul_sched_info& grant : prev_slot_result.result.ul.puschs) {
    ue_cell* ue_cc = ue_cell_db.find_by_rnti(grant.pusch_cfg.rnti);
    if (ue_cc == nullptr) {
      // UE has been removed.
      continue;
    }

    // - The lower layers will not attempt to decode the PUSCH and will not send any CRC indication.
    std::optional<ul_harq_process_handle> h_ul = ue_cc->harqs.ul_harq(to_harq_id(grant.pusch_cfg.harq_id));
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

void cell_event_manager::handle_error_indication(slot_point sl_tx, scheduler_slot_handler::error_outcome event)
{
  auto handle_error_impl = [this, sl_tx, event]() {
    // Handle Error Indication.

    const cell_slot_resource_allocator* prev_slot_result = res_grid.get_history(sl_tx);
    if (prev_slot_result == nullptr) {
      logger.warning("cell={}, slot={}: Discarding error indication. Cause: Scheduler results associated with the slot "
                     "of the error indication have already been erased (current slot={})",
                     cell_cfg.cell_index,
                     sl_tx,
                     res_grid.slot_tx());
      return;
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
      handle_discarded_pusch(*prev_slot_result, ue_cell_db);

      uci_sel.handle_discarded_ucis(sl_tx);
    }

    // Log event.
    ev_logger.enqueue(scheduler_event_logger::error_indication_event{sl_tx, event});

    // Report metrics.
    metrics.handle_error_indication();
  };

  dispatcher->push("error indication", std::move(handle_error_impl));
}
