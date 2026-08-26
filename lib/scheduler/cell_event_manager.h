// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/scheduler/scheduler_positioning_handler.h"
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
class ue_cell_repository;
struct pws_si_scheduling_update_request;
struct srs_indication;
struct rach_indication_message;
struct sched_paging_information;
struct si_scheduling_update_request;
struct ul_crc_indication;

/// \brief Handler of the events of a cell that require no access to the state shared by the UEs of the cell group.
///
/// Events are enqueued from any executor and processed at the start of the cell slot indication, so that their
/// handling runs in the cell scheduler executor.
class cell_event_manager final : public scheduler_cell_positioning_handler
{
public:
  cell_event_manager(const cell_configuration& cell_cfg,
                     ue_cell_repository&       ue_cell_db,
                     si_scheduler&             si_sch,
                     paging_scheduler&         pg_sch,
                     ra_scheduler&             ra_sch,
                     srs_scheduler&            srs_sch,
                     cell_metrics_handler&     metrics,
                     scheduler_event_logger&   ev_logger,
                     ocudulog::basic_logger&   logger);
  ~cell_event_manager() override;

  /// Activate event processing.
  void start();

  /// Deactivate event processing and discard any pending event.
  void stop();

  /// Process the events pending for this cell.
  void run_slot();

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

  // scheduler_cell_positioning_handler methods.
  void handle_positioning_measurement_request(const positioning_measurement_request::cell_info& req) override;
  void handle_positioning_measurement_stop(rnti_t pos_rnti) override;

private:
  const cell_configuration& cell_cfg;
  ue_cell_repository&       ue_cell_db;
  si_scheduler&             si_sch;
  paging_scheduler&         pg_sch;
  ra_scheduler&             ra_sch;
  srs_scheduler&            srs_sch;
  cell_metrics_handler&     metrics;
  scheduler_event_logger&   ev_logger;

  // Queue of pending events and pools of the event payloads that do not fit in an event callback.
  std::unique_ptr<cell_event_dispatcher> dispatcher;
};

} // namespace ocudu
