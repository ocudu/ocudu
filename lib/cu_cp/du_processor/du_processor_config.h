// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cu_cp_controller/node_connection_notifier.h"
#include "du_configuration_handler.h"
#include "ocudu/cu_cp/cu_cp_ref_time_report_notifier.h"
#include "ocudu/f1ap/cu_cp/f1ap_configuration.h"
#include "ocudu/f1ap/f1ap_message_notifier.h"

namespace ocudu::ocucp {

/// DU processor configuration.
struct du_processor_config {
  cu_cp_du_index_t                         du_index = cu_cp_du_index_t::invalid;
  gnb_id_t                                 gnb_id;
  std::string                              ran_node_name;
  srb_pdcp_config                          srb2_cfg;
  std::map<five_qi_t, cu_cp_qos_config>    drb_config;
  security::preferred_integrity_algorithms int_algo_pref_list;
  security::preferred_ciphering_algorithms enc_algo_pref_list;
  bool                                     force_reestablishment_fallback;
  bool                                     force_resume_fallback;
  std::chrono::milliseconds                rrc_procedure_guard_time_ms;
  std::optional<std::chrono::seconds>      rrc_reject_wait_time;
  unsigned                                 rrc_version;
  f1ap_configuration                       f1ap;
};

/// DU processor dependencies.
struct du_processor_dependencies {
  task_executor&                            cu_cp_executor;
  timer_manager&                            timers;
  du_connection_notifier&                   du_setup_notif;
  std::unique_ptr<du_configuration_handler> du_cfg_hdlr;
  du_processor_cu_cp_notifier&              cu_cp_notifier;
  f1ap_message_notifier&                    f1ap_pdu_notifier;
  async_task_scheduler&                     common_task_sched;
  ue_manager&                               ue_mng;
  cu_cp_ref_time_report_notifier&           ref_time_report_notifier;
  ocudulog::basic_logger&                   logger;
};

} // namespace ocudu::ocucp
