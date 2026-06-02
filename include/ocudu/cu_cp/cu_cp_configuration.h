// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cell_meas_manager_config.h"
#include "ocudu/cu_cp/cu_cp_metrics_notifier.h"
#include "ocudu/cu_cp/cu_cp_ng_setup_notifier.h"
#include "ocudu/cu_cp/cu_cp_types.h"
#include "ocudu/cu_cp/mobility_manager_config.h"
#include "ocudu/cu_cp/ue_configuration.h"
#include "ocudu/e1ap/cu_cp/e1ap_configuration.h"
#include "ocudu/f1ap/cu_cp/f1ap_configuration.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/supported_tracking_area.h"
#include "ocudu/rrc/rrc_ue_config.h"
#include "ocudu/support/executors/task_executor.h"
#include <chrono>
#include <map>

namespace ocudu {

class pdcp_metrics_notifier;

namespace ocucp {
class n2_connection_client;
class ngap_repository;
class xnc_connection_gateway;

/// Parameters of the CU-CP that will reported to the 5G core.
struct ran_node_configuration {
  /// The gNodeB identifier.
  gnb_id_t    gnb_id{411, 22};
  std::string ran_node_name = "gnb01";
};

struct mobility_configuration {
  cell_meas_manager_cfg meas_manager_config;
  mobility_manager_cfg  mobility_manager_config;
};

/// Configuration passed to CU-CP.
struct cu_cp_configuration {
  struct admission_params {
    /// Maximum number of DU connections that the CU-CP may accept.
    unsigned max_nof_dus = 6;
    /// Maximum number of CU-UP connections that the CU-CP may accept.
    unsigned max_nof_cu_ups = 6;
    /// Maximum number of UEs that the CU-CP may accept.
    uint32_t max_nof_ues = 8192;
    /// Maximum number of DRBs per UE that the CU-CP will configure.
    uint8_t max_nof_drbs_per_ue = 8;
  };

  struct service_params {
    task_executor* cu_cp_executor = nullptr;
    task_executor* cu_cp_e2_exec  = nullptr;
    timer_manager* timers         = nullptr;
  };

  struct ngap_config {
    n2_connection_client* n2_gw = nullptr;
    // Supported TAs for each AMF.
    std::vector<supported_tracking_area> supported_tas;
  };

  struct ngap_params {
    /// NGAP configurations.
    std::vector<ngap_config> ngaps;
    /// Time to wait after a failed AMF reconnection attempt in ms.
    std::chrono::milliseconds amf_reconnection_retry_time = std::chrono::milliseconds{1000};
    /// Time that the NGAP waits for a response from the AMF in milliseconds.
    std::chrono::milliseconds procedure_timeout = std::chrono::milliseconds{5000};
    /// Option to run CU-CP without a core.
    bool no_core = false;
    /// Optional notifier invoked once after a successful NG Setup.
    cu_cp_ng_setup_complete_notifier* ng_setup_notifier = nullptr;
  };

  struct xnap_config {
    /// XN-C peer addresses. Multiple addresses can be provided for SCTP multihoming.
    std::vector<transport_layer_address> peer_addrs;
  };

  struct xnap_params {
    /// Time that the XNAP waits for a response in milliseconds (Implementation-defined).
    std::chrono::milliseconds procedure_timeout{1000};
    /// Time that the XNAP waits before retrying the reconnection in milliseconds.
    std::chrono::milliseconds reconnect_timer{10000};
    /// When true, the CU-CP will not initiate outbound XNAP connections but will accept inbound ones.
    bool no_connection_init = false;
    /// XnAP peer configuration.
    std::vector<xnap_config> xnaps;
    /// Xn-C gateways.
    std::vector<xnc_connection_gateway*> xnc_gws;
    /// XnAP peer to Xn-C gateway mapping.
    std::map<xnc_peer_index_t, xnc_gateway_index_t> peer_to_gateway;
  };

  struct rrc_params {
    /// Force re-establishment fallback.
    bool force_reestablishment_fallback = false;
    /// Force resume fallback.
    bool force_resume_fallback = false;
    /// Guard time for RRC procedures.
    std::chrono::milliseconds rrc_procedure_guard_time_ms{1000};
    /// Version of the RRC.
    unsigned rrc_version = 2;
  };

  struct security_params {
    /// Integrity protection algorithms preference list
    security::preferred_integrity_algorithms int_algo_pref_list{security::integrity_algorithm::nia0};
    /// Encryption algorithms preference list
    security::preferred_ciphering_algorithms enc_algo_pref_list{security::ciphering_algorithm::nea0};
    /// Default security if not signaled via NGAP.
    security_indication_t default_security_indication;
  };
  struct bearer_params {
    /// PDCP config to use when UE SRB2 are configured.
    srb_pdcp_config srb2_cfg;
    /// Configuration for available 5QI.
    std::map<five_qi_t, cu_cp_qos_config> drb_config;
  };

  struct metrics_layers_config {
    /// Enable NGAP metrics.
    bool enable_ngap = false;
    /// Enable RRC metrics.
    bool enable_rrc = false;
  };

  struct metrics_params {
    /// CU-CP statistics report period.
    std::chrono::seconds      statistics_report_period{1};
    std::chrono::milliseconds metrics_report_period{0};
    metrics_layers_config     layers_cfg = {};
  };

  /// NG-RAN node parameters.
  ran_node_configuration node;
  /// Parameters to determine the admission of new CU-UP, DU and UE connections.
  admission_params admission;
  /// NGAP layer-specific parameters.
  ngap_params ngap;
  /// XNAP layer-specific parameters.
  xnap_params xnap;
  /// RRC layer-specific parameters.
  rrc_params rrc;
  /// F1AP layer-specific parameters.
  f1ap_configuration f1ap;
  /// E1AP layer-specific parameters.
  e1ap_configuration e1ap;
  /// UE Security-specific parameters.
  security_params security;
  /// SRB and DRB configuration of created UEs.
  bearer_params bearers;
  /// UE-specific parameters.
  ue_configuration ue;
  /// Parameters related with the mobility of UEs.
  mobility_configuration mobility;
  /// Parameters related with CU-CP metrics.
  metrics_params metrics;
  /// Timers, executors, and other services used by the CU-CP.
  service_params services;
  /// CU-CP metrics notifier.
  cu_cp_metrics_report_notifier* metrics_notifier = nullptr;
};

} // namespace ocucp
} // namespace ocudu
