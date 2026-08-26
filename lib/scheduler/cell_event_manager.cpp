// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cell_event_manager.h"
#include "common_scheduling/paging_scheduler.h"
#include "common_scheduling/ra_scheduler.h"
#include "common_scheduling/si_scheduler.h"
#include "config/cell_configuration.h"
#include "logging/cell_metrics_handler.h"
#include "logging/scheduler_event_logger.h"
#include "srs/srs_scheduler.h"
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

  std::tuple<paging_pool*, si_pool*, pws_si_pool*, rach_pool*, crc_pool*, pos_req_pool*, srs_pool*> pools{
      &pending_pagings,
      &pending_si_reqs,
      &pending_pws_si_reqs,
      &pending_rachs,
      &pending_crcs,
      &pending_pos_reqs,
      &pending_srss};

  event_queue pending_events;

  // Whether the cell is currently accepting events.
  std::atomic<bool> active{true};
};

cell_event_manager::cell_event_manager(const cell_configuration& cell_cfg_,
                                       ue_cell_repository&       ue_cell_db_,
                                       si_scheduler&             si_sch_,
                                       paging_scheduler&         pg_sch_,
                                       ra_scheduler&             ra_sch_,
                                       srs_scheduler&            srs_sch_,
                                       cell_metrics_handler&     metrics_,
                                       scheduler_event_logger&   ev_logger_,
                                       ocudulog::basic_logger&   logger) :
  cell_cfg(cell_cfg_),
  ue_cell_db(ue_cell_db_),
  si_sch(si_sch_),
  pg_sch(pg_sch_),
  ra_sch(ra_sch_),
  srs_sch(srs_sch_),
  metrics(metrics_),
  ev_logger(ev_logger_),
  dispatcher(std::make_unique<cell_event_dispatcher>(cell_cfg_, logger))
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

  dispatcher->push("CRC", [this, crc_ptr = std::move(crc_ptr)]() { ra_sch.handle_crc_indication(*crc_ptr); });
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
