// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cell_group_event_handler.h"
#include "logging/cell_event_tracer.h"
#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/csi_report/csi_report_data.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/scheduler/scheduler_dl_buffer_state_indication_handler.h"
#include "ocudu/scheduler/scheduler_feedback_handler.h"
#include "ocudu/scheduler/scheduler_positioning_handler.h"
#include "ocudu/scheduler/scheduler_slot_handler.h"
#include <memory>

namespace ocudu {

class cell_configuration;
class cell_event_dispatcher;
class paging_scheduler;
class cell_metrics_handler;
class ra_scheduler;
class scheduler_event_logger;
class si_scheduler;
class srs_scheduler;
class ra_ue_repository;
class uci_indication_selector;
class ue_cell;
class ue_cell_repository;
struct cell_resource_allocator;
struct pws_si_scheduling_update_request;
struct srs_indication;
struct rach_indication_message;
struct sched_paging_information;
struct si_scheduling_update_request;
struct ul_crc_indication;
struct ul_crc_pdu_indication;
struct uci_action;
struct uci_indication;

/// \brief Handler of the events of a cell that require no access to the state shared by the UEs of the cell group.
///
/// Events are enqueued from any executor and processed at the start of the cell slot indication, so that their
/// handling runs in the cell scheduler executor.
class cell_event_manager final : public scheduler_cell_positioning_handler,
                                 public sched_ue_configuration_handler,
                                 public ue_feedback_handler,
                                 public scheduler_dl_buffer_state_indication_handler
{
public:
  cell_event_manager(const cell_configuration&      cell_cfg,
                     cell_resource_allocator&       res_grid,
                     ue_cell_repository&            ue_cell_db,
                     si_scheduler&                  si_sch,
                     paging_scheduler&              pg_sch,
                     ra_scheduler&                  ra_sch,
                     srs_scheduler&                 srs_sch,
                     cell_group_event_handler&      ue_ev_handler,
                     ra_ue_repository&              ra_ue_repo,
                     uci_indication_selector&       uci_sel,
                     cell_metrics_handler&          metrics,
                     scheduler_event_logger&        ev_logger,
                     schedtrace::cell_event_tracer& ev_tracer,
                     ocudulog::basic_logger&        logger);
  ~cell_event_manager() override;

  /// Activate event processing.
  void start();

  /// Deactivate event processing and discard any pending event.
  void stop();

  /// Process the events pending for this cell.
  void run_slot(slot_point sl_tx);

  /// Enqueue paging information reported by upper layers.
  void handle_paging_information(const sched_paging_information& pi);

  /// Enqueue a request to update the system information of the cell.
  void handle_si_update_request(const si_scheduling_update_request& req);

  /// Enqueue a request to update the ETWS/CMAS system information of the cell.
  void handle_pws_si_update_request(const pws_si_scheduling_update_request& req);

  /// Enqueue a RACH indication coming from lower layers.
  void handle_rach_indication(const rach_indication_message& msg);

  /// Enqueue a UL CRC indication coming from lower layers.
  void handle_crc_indication(const ul_crc_indication& crc_ind);

  /// Enqueue an SRS indication coming from lower layers.
  void handle_srs_indication(const srs_indication& srs);

  /// Enqueue a UCI indication coming from lower layers.
  void handle_uci_indication(const uci_indication& uci);

  /// Handle a UCI grant whose indication did not arrive before its deadline.
  void handle_uci_indication_timeout(slot_point uci_slot, rnti_t crnti, const uci_action& action);

  /// Enqueue an error indication of a past slot coming from lower layers.
  void handle_error_indication(slot_point sl_tx, scheduler_slot_handler::error_outcome event);

  // sched_ue_configuration_handler methods.
  void handle_ue_creation(ue_config_update_event ev) override;
  void handle_ue_reconfiguration(ue_config_update_event ev) override;
  void handle_ue_deletion(ue_config_delete_event ev) override;
  void handle_ue_config_applied(du_ue_index_t ue_idx) override;
  void handle_ue_deactivation_request(du_ue_index_t ue_idx) override;

  // ue_feedback_handler methods.
  void handle_ul_bsr_indication(const ul_bsr_indication_message& bsr) override;
  void handle_ul_phr_indication(const ul_phr_indication_message& phr) override;
  void handle_ul_ta_report_indication(const ul_ta_report_indication_message& ta_report) override;
  void handle_dl_mac_ce_indication(const dl_mac_ce_indication& ce) override;
  void handle_crnti_ce_received(du_ue_index_t ue_index) override;

  /// Enqueue a request to reconfigure the slices of the cell.
  void handle_slice_reconfiguration_request(const du_cell_slice_reconfig_request& req);

  // scheduler_dl_buffer_state_indication_handler methods.
  void handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& dl_bo) override;

  // scheduler_cell_positioning_handler methods.
  void handle_positioning_measurement_request(const positioning_measurement_request::cell_info& req) override;
  void handle_positioning_measurement_stop(rnti_t pos_rnti) override;

private:
  class ue_dl_buffer_occupancy_manager;

  /// Handle a CRC that ACKs/NACKs a HARQ of a UE of this cell.
  void handle_ue_crc(slot_point sl_rx, const ul_crc_pdu_indication& crc);

  /// Handle a single UCI PDU of a UE of this cell.
  void handle_uci_pdu(slot_point uci_sl, const uci_indication::uci_pdu& uci_pdu);

  /// \brief Whether the given rnti/slot pair identifies the successRAR's own HARQ-ACK PUCCH (2-step RACH), allocated by
  /// the RA scheduler against a common PUCCH resource rather than tracked as a per-UE DL HARQ. Only meant to be checked
  /// as a fallback, once the UE lookup it would otherwise explain has already failed.
  bool is_msgb_harq_ack_slot(rnti_t rnti, slot_point uci_sl) const;

  void handle_harq_ind(ue_cell&                             ue_cc,
                       slot_point                           uci_sl,
                       bool                                 uci_valid,
                       const bounded_bitset<MAX_NOF_HARQS>& harq_bits,
                       std::optional<float>                 pucch_snr);

  void handle_csi(ue_cell& ue_cc, slot_point sl_rx, const csi_report_data& csi_rep);

  const cell_configuration&      cell_cfg;
  cell_resource_allocator&       res_grid;
  ue_cell_repository&            ue_cell_db;
  si_scheduler&                  si_sch;
  paging_scheduler&              pg_sch;
  ra_scheduler&                  ra_sch;
  srs_scheduler&                 srs_sch;
  cell_group_event_handler&      ue_ev_handler;
  ra_ue_repository&              ra_ue_repo;
  uci_indication_selector&       uci_sel;
  cell_metrics_handler&          metrics;
  scheduler_event_logger&        ev_logger;
  schedtrace::cell_event_tracer& ev_tracer;
  ocudulog::basic_logger&        logger;

  // Queue of pending events and pools of the event payloads that do not fit in an event callback.
  std::unique_ptr<cell_event_dispatcher> dispatcher;

  /// Aggregator of the DL buffer occupancy updates received since the last slot.
  std::unique_ptr<ue_dl_buffer_occupancy_manager> dl_bo_mng;
};

} // namespace ocudu
