// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/harq_id.h"
#include "ocudu/ran/logical_channel/bsr_format.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/pusch/pusch_tpmi_select.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/scheduler/input/uci_inputs.h"
#include "ocudu/scheduler/scheduler_configurator.h"
#include "ocudu/scheduler/scheduler_dl_buffer_state_indication_handler.h"
#include "ocudu/scheduler/scheduler_feedback_handler.h"
#include "ocudu/scheduler/scheduler_rach_handler.h"
#include "ocudu/scheduler/scheduler_slot_handler.h"

namespace ocudu {

class scheduler_event_logger
{
public:
  struct prach_event {
    slot_point      slot_rx;
    du_cell_index_t cell_index;
    unsigned        preamble_id;
    rnti_t          ra_rnti;
    rnti_t          tc_rnti;
    unsigned        ta;
    bool            is_msga;
  };
  struct ue_creation_event {
    du_ue_index_t   ue_index;
    rnti_t          rnti;
    du_cell_index_t pcell_index;
  };
  struct ue_reconf_event {
    du_ue_index_t ue_index;
    rnti_t        rnti;
  };
  struct ue_cfg_applied_event {
    du_ue_index_t ue_index;
    rnti_t        rnti;
  };
  struct ue_deactivation_event {
    du_ue_index_t ue_index;
    rnti_t        rnti;
  };
  struct crc_event {
    du_ue_index_t        ue_index;
    rnti_t               rnti;
    du_cell_index_t      cell_index;
    slot_point           sl_rx;
    harq_id_t            h_id;
    bool                 crc;
    std::optional<float> ul_sinr_db;
  };
  struct bsr_event {
    du_ue_index_t          ue_index;
    rnti_t                 rnti;
    bsr_format             type;
    ul_bsr_lcg_report_list reported_lcgs;
    units::bytes           tot_ul_pending_bytes;
  };
  struct phr_event {
    du_ue_index_t                   ue_index;
    rnti_t                          rnti;
    du_cell_index_t                 cell_index;
    ph_db_range                     ph;
    std::optional<p_cmax_dbm_range> p_cmax;
  };
  struct error_indication_event {
    slot_point                            sl_tx;
    scheduler_slot_handler::error_outcome outcome;
  };
  struct srs_indication_event {
    du_ue_index_t                         ue_index;
    rnti_t                                rnti;
    std::optional<pusch_tpmi_select_info> tpmi_info;
  };
  struct slice_reconfiguration_event {
    du_cell_index_t cell_index;
  };

  scheduler_event_logger(du_cell_index_t cell_index_, pci_t pci_);
  ~scheduler_event_logger();

  void log()
  {
    if (mode == mode_t::none) {
      return;
    }
    log_impl();
  }

  template <typename Event>
  void enqueue(Event&& ev)
  {
    if (mode == mode_t::none) {
      return;
    }
    enqueue_impl(std::forward<Event>(ev));
  }

  bool enabled() const { return mode != mode_t::none; }

private:
  /// Level of verbosity of the scheduler event logger.
  enum class mode_t { none, info, debug };

  class event_buffer_writer;

  void log_impl();

  void enqueue_impl(const prach_event& rach_ev);
  void enqueue_impl(const rach_indication_message& rach_ind);

  void enqueue_impl(const ue_creation_event& ue_request);
  void enqueue_impl(const ue_reconf_event& ue_request);
  void enqueue_impl(const sched_ue_delete_message& ue_request);
  void enqueue_impl(const ue_cfg_applied_event& ev);
  void enqueue_impl(const ue_deactivation_event& req);

  void enqueue_impl(const error_indication_event& err_ind);

  void enqueue_impl(const sr_event& sr);
  void enqueue_impl(const bsr_event& bsr);
  void enqueue_impl(const harq_ack_event& harq_ev);
  void enqueue_impl(const csi_report_event& csi);
  void enqueue_impl(const crc_event& crc_ev);
  void enqueue_impl(const dl_mac_ce_indication& mac_ce);
  void enqueue_impl(const dl_buffer_state_indication_message& bs);
  void enqueue_impl(const phr_event& phr_ev);
  void enqueue_impl(const srs_indication_event& srs_ev);
  void enqueue_impl(const slice_reconfiguration_event& slice_reconf_ev);

  const du_cell_index_t   cell_index;
  const pci_t             pci;
  ocudulog::basic_logger& logger;
  mode_t                  mode = mode_t::none;

  std::unique_ptr<event_buffer_writer> slot_buffer_writer;
};

} // namespace ocudu
