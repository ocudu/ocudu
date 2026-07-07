// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/metrics/metrics_config.h"
#include "apps/helpers/network/sctp_appconfig.h"
#include "apps/units/o_cu_cp/cu_cp/cu_cp_unit_pcap_config.h"
#include "cu_cp_unit_logger_config.h"
#include "ocudu/ran/arfcn.h"
#include "ocudu/ran/gnb_id.h"
#include "ocudu/ran/meas_types.h"
#include "ocudu/ran/nr_band.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/qos/five_qi.h"
#include "ocudu/ran/s_nssai.h"
#include "ocudu/ran/tac.h"
#include <chrono>
#include <optional>
#include <vector>

namespace ocudu {

struct cu_cp_unit_plmn_item {
  struct tai_slice_t {
    uint8_t  sst = 0;
    uint32_t sd  = 0xffffffU;
  };

  std::string plmn_id;
  /// Supported Slices by the RAN node.
  std::vector<tai_slice_t> tai_slice_support_list;
};

struct cu_cp_unit_supported_ta_item {
  tac_t                             tac;
  std::vector<cu_cp_unit_plmn_item> plmn_list;
};

struct cu_cp_unit_amf_config_item {
  std::vector<std::string> ip_addrs       = {"127.0.1.100"};
  uint16_t                 port           = 38412;
  std::vector<std::string> bind_addrs     = {"127.0.0.1"};
  std::string              bind_interface = "auto";
  sctp_appconfig           sctp;

  /// List of all tracking areas supported by the AMF.
  std::vector<cu_cp_unit_supported_ta_item> supported_tas = {{7, {{"00101", {cu_cp_unit_plmn_item::tai_slice_t{1}}}}}};
  bool                                      is_default_supported_tas = true;
};

struct cu_cp_unit_amf_config {
  cu_cp_unit_amf_config_item amf;
  /// Allow CU-CP to run without a core, e.g. for test mode.
  bool no_core = false;
  /// Time to wait after a failed AMF reconnection attempt in ms.
  unsigned amf_reconnection_retry_time = 1000;
  /// Time that the NGAP waits for a response from the AMF in milliseconds.
  unsigned procedure_timeout = 5000;
};

/// Report configuration for periodical, event-triggered and conditional-trigger report types.
/// Event-triggered reports support events A1–A6 as defined in 3GPP TS 38.331.
struct cu_cp_unit_report_config {
  unsigned    report_cfg_id;
  std::string report_type;
  unsigned    report_interval_ms;

  std::optional<std::string> event_triggered_report_type;
  std::optional<std::string> meas_trigger_quantity;              ///< "rsrp", "rsrq", "sinr"
  std::optional<int>         meas_trigger_quantity_threshold_db; ///< Threshold for A5: RSRP[-156..-31],
                                                                 ///< RSRQ[-43..20], SINR[-23..40]
  std::optional<int> meas_trigger_quantity_threshold_2_db;       ///< Threshold 2 for A5: RSRP[-156..-31],
                                                                 /// <RSRQ[-43..20], SINR[-23..40]
  std::optional<int>      meas_trigger_quantity_offset_db;       ///< [-15..15] dB
  std::optional<unsigned> hysteresis_db;                         ///< [0..15] dB (0.5 dB steps in ASN.1)
  std::optional<unsigned> time_to_trigger_ms;                    ///< Enumerated values
  std::optional<unsigned> t312_ms;
  int                     periodic_ho_rsrp_offset =
      -1; ///< -1 disables handovers from periodic measurements. [0..30] Note the actual value is field value * 0.5 dB.
          ///< E.g. putting a value of -6 here results in -3dB offset.

  // D1/D2 conditional event fields
  std::optional<double> distance_thresh_from_ref1_km;            ///< D1/D2: distance threshold 1 in km [0..3276.75]
  std::optional<double> distance_thresh_from_ref2_km;            ///< D1/D2: distance threshold 2 in km [0..3276.75]
  std::optional<ocucp::rrc_geo_location> ref_location1;          ///< D1: reference location for serving cell
  std::optional<ocucp::rrc_geo_location> ref_location2;          ///< D1: reference location for target cell
  std::optional<double>                  hysteresis_location_km; ///< D1/D2: hysteresis in km (10m steps, max 327.68)

  // T1 conditional event fields
  std::optional<std::chrono::system_clock::time_point> t1_thres; ///< T1: UTC time threshold
  std::optional<std::chrono::duration<double>>
      duration; ///< T1: duration in seconds (each step=100ms, range [0.1..600])
};

struct cu_cp_unit_neighbor_cell_config_item {
  /// Cell id.
  uint64_t nr_cell_id;
  /// Report config ids.
  std::vector<uint64_t> report_cfg_ids;
};

/// Each item describes the relationship between one cell to all other cells.
struct cu_cp_unit_cell_config_item {
  /// Cell id.
  uint64_t                nr_cell_id;
  std::optional<unsigned> periodic_report_cfg_id;

  // These parameters must only be set for external cells
  /// gNodeB identifier bit length.
  std::optional<unsigned> gnb_id_bit_length;
  /// PCI.
  std::optional<pci_t> pci;
  /// PLMN.
  std::optional<std::string> plmn_id;
  /// TAC.
  std::optional<tac_t> tac;
  /// NR band.
  std::optional<nr_band> band;
  /// SSB ARFCN.
  std::optional<arfcn_t> ssb_arfcn;
  /// SSB subcarrier spacing.
  std::optional<unsigned> ssb_scs;
  /// SSB period.
  std::optional<unsigned> ssb_period;
  /// SSB offset.
  std::optional<unsigned> ssb_offset;
  /// SSB duration.
  std::optional<unsigned> ssb_duration;
  /// Vector of cells that are a neighbor of this cell.
  std::vector<cu_cp_unit_neighbor_cell_config_item> ncells;
  // TODO: Add optional SSB parameters.
};

/// All mobility related configuration parameters.
struct cu_cp_unit_mobility_config {
  /// List of all cells known to the CU-CP.
  std::vector<cu_cp_unit_cell_config_item> cells;
  /// Report config.
  std::vector<cu_cp_unit_report_config> report_configs;
  /// Whether to start HO if neighbor cell measurements arrive.
  bool trigger_handover_from_measurements = false;
  /// Whether to auto-trigger CHO after UE setup/capability phase when readiness checks pass.
  bool trigger_cho_on_ue_setup = false;
  /// Timeout used for auto-triggered CHO and as default timeout for manual CHO command in milliseconds.
  unsigned cho_timeout_ms = 10000;
};

/// RRC specific configuration parameters.
struct cu_cp_unit_rrc_config {
  bool force_reestablishment_fallback = false;
  bool force_resume_fallback          = false;
  /// Guard time in ms that is added to the RRC procedure timeout.
  /// NOTE: Guard time needs to be larger then SRB max retx thres * t-PollRetransmit.
  /// (2 * default SRB maxRetxThreshold * t-PollRetransmit = 2 * 8 * 45ms = 720ms, see TS 38.331 Sec 9.2.1)
  unsigned rrc_procedure_guard_time_ms = 1000;
  /// waitTime [s] (1..16) for the RRC Reject waitTime IE.
  /// waitTime is Optional, -- Need N.
  /// If not provided, no waitTime field will be sent with RRC Reject.
  std::optional<unsigned> rrc_reject_wait_time_s;
};

/// Security configuration parameters.
struct cu_cp_unit_security_config {
  std::string integrity_protection       = "not_needed";
  std::string confidentiality_protection = "required";
  std::string nea_preference_list        = "nea0,nea2,nea1,nea3";
  std::string nia_preference_list        = "nia2,nia1,nia3";
};

/// F1AP-CU configuration parameters.
struct cu_cp_unit_f1ap_config {
  /// Timeout for the F1AP procedures in milliseconds.
  unsigned procedure_timeout = 1000;
  /// Enable sending a REFERENCE TIME INFORMATION REPORTING CONTROL to each DU on connection (TS 38.473 section 8.12).
  bool ref_time_reporting_enabled = false;
  /// Reporting mode: "on_demand" (single report) or "periodic" (recurring reports).
  std::string ref_time_reporting_event_type = "periodic";
  /// Reporting period in radio frames (1 RF = 10 ms). Range: 1..512. Used only when event_type is "periodic".
  uint16_t ref_time_reporting_periodicity_rf = 100;
};

/// E1AP-CU-CP configuration parameters.
struct cu_cp_unit_e1ap_config {
  /// Timeout for the E1AP procedures in milliseconds.
  unsigned procedure_timeout = 1000;
};

/// XnAP peer configuration parameters.
struct cu_cp_unit_xnap_peer_config {
  std::vector<std::string> peer_addrs;
};

/// XnAP gateway configuration parameters.
/// XnAP gateway maps to one SCTP socket with configured:
///   - list of bind_addrs (with SCTP multi-homing possible),
///   - list of XnAP connections to peer gNBs (with SCTP multi-homing possible),
///   - its own sctp_appconfig SCTP socket options.
struct cu_cp_unit_xnap_gateway_config {
  std::vector<std::string>                 bind_addrs = {"127.0.30.1"};
  std::vector<cu_cp_unit_xnap_peer_config> connections;
  sctp_appconfig                           sctp;
};

struct cu_cp_unit_xnap_config {
  /// Timeout for the XNAP procedures in milliseconds.
  unsigned procedure_timeout = 5000;
  /// Timer for the XNAP reconnect in milliseconds.
  unsigned reconnect_timer = 10000;
  /// When true, the CU-CP will not initiate outbound XNAP connections but will accept inbound ones.
  bool no_connection_init = false;

  /// XnAP gateways list configuration.
  /// Multiple gateways allow XnAP to operate across different subnets, each with independent SCTP tuning.
  std::vector<cu_cp_unit_xnap_gateway_config> gateways;
};

/// RLC UM TX configuration
struct cu_cp_unit_rlc_tx_um_config {
  /// Number of bits used for sequence number.
  uint16_t sn_field_length;
  /// RLC SDU queue size.
  uint32_t queue_size;
};

/// RLC UM RX configuration
struct cu_cp_unit_rlc_rx_um_config {
  /// Number of bits used for sequence number.
  uint16_t sn_field_length;
  /// Timer used by rx to detect PDU loss (ms).
  int32_t t_reassembly;
};

/// RLC UM configuration
struct cu_cp_unit_rlc_um_config {
  cu_cp_unit_rlc_tx_um_config tx;
  cu_cp_unit_rlc_rx_um_config rx;
};

/// RLC UM TX configuration
struct cu_cp_unit_rlc_tx_am_config {
  /// Number of bits used for sequence number.
  uint16_t sn_field_length;
  /// Poll retx timeout (ms).
  int32_t t_poll_retx;
  /// Max retx threshold.
  uint32_t max_retx_thresh;
  /// Insert poll bit after this many PDUs.
  int32_t poll_pdu;
  /// Insert poll bit after this much data (bytes).
  int32_t poll_byte;
  /// Custom parameter to limit the maximum window size for memory reasons. 0 means no limit.
  uint32_t max_window = 0;
  /// RLC SDU queue size.
  uint32_t queue_size = 4096;
};

/// RLC UM RX configuration
struct cu_cp_unit_rlc_rx_am_config {
  /// Number of bits used for sequence number.
  uint16_t sn_field_length;
  /// Timer used by rx to detect PDU loss (ms).
  int32_t t_reassembly;
  /// Timer used by rx to prohibit tx of status PDU (ms).
  int32_t t_status_prohibit;

  /// Implementation-specific parameters that are not specified by 3GPP

  /// Maximum number of visited SNs in the RX window when building a status report. 0 means no limit.
  uint32_t max_sn_per_status = 0;
};

/// RLC AM configuration
struct cu_cp_unit_rlc_am_config {
  cu_cp_unit_rlc_tx_am_config tx;
  cu_cp_unit_rlc_rx_am_config rx;
};

/// RLC configuration
struct cu_cp_unit_rlc_config {
  std::string              mode = "am";
  cu_cp_unit_rlc_um_config um;
  cu_cp_unit_rlc_am_config am;
};

enum class cu_cp_unit_pdcp_rohc_type { none, rohc, uplink_only_rohc };

inline const char* to_string(cu_cp_unit_pdcp_rohc_type type)
{
  switch (type) {
    case cu_cp_unit_pdcp_rohc_type::none:
      return "none";
    case cu_cp_unit_pdcp_rohc_type::rohc:
      return "rohc";
    case cu_cp_unit_pdcp_rohc_type::uplink_only_rohc:
      return "uplink_only_rohc";
    default:
      return "UNKNOWN";
  }
}

struct cu_cp_unit_pdcp_rohc_config {
  /// ROHC type.
  cu_cp_unit_pdcp_rohc_type rohc_type = cu_cp_unit_pdcp_rohc_type::none;
  /// Maximum CID.
  uint16_t max_cid = 15;
  /// Configure profile0x0001 (ROHCv1 RTP/UDP/IP)
  bool profile0x0001 = false;
  /// Configure profile0x0002 (ROHCv1 UDP/IP)
  bool profile0x0002 = false;
  /// Configure profile0x0003 (ROHCv1 ESP/IP)
  bool profile0x0003 = false;
  /// Configure profile0x0004 (ROHCv1 IP)
  bool profile0x0004 = false;
  /// Configure profile0x0006 (ROHCv1 TCP/IP)
  bool profile0x0006 = false;
  /// Configure profile0x0101 (ROHCv2 RTP/UDP/IP)
  bool profile0x0101 = false;
  /// Configure profile0x0102 (ROHCv2 UDP/IP)
  bool profile0x0102 = false;
  /// Configure profile0x0103 (ROHCv2 ESP/IP)
  bool profile0x0103 = false;
  /// Configure profile0x0104 (ROHCv2 IP)
  bool profile0x0104 = false;
};

struct cu_cp_unit_pdcp_rx_config {
  /// Number of bits used for sequence number.
  uint16_t sn_field_length;
  /// Timer used to detect PDUs losses (ms).
  int32_t t_reordering;
  /// Whether out-of-order delivery to upper layers is enabled.
  bool out_of_order_delivery;
};

struct cu_cp_unit_pdcp_tx_config {
  /// Number of bits used for sequence number.
  uint16_t sn_field_length;
  /// Timer used to notify lower layers to discard PDUs (ms).
  int32_t discard_timer;
  /// Whether PDCP status report is required.
  bool status_report_required;
};

struct cu_cp_unit_pdcp_config {
  cu_cp_unit_pdcp_rohc_config rohc;
  cu_cp_unit_pdcp_tx_config   tx;
  cu_cp_unit_pdcp_rx_config   rx;
};

/// QoS configuration.
struct cu_cp_unit_qos_config {
  five_qi_t              five_qi = uint_to_five_qi(9);
  cu_cp_unit_rlc_config  rlc;
  cu_cp_unit_pdcp_config pdcp;
};

/// Configuration to enable/disable metrics per layer.
struct cu_cp_unit_metrics_layer_config {
  bool enable_ngap           = false;
  bool enable_pdcp           = false;
  bool enable_rrc            = false;
  bool enable_cu_cp_executor = false;

  /// Returns true if one or more layers are enabled, false otherwise.
  bool are_metrics_enabled() const { return enable_ngap || enable_pdcp || enable_rrc; }
};

/// Metrics configuration.
struct cu_cp_unit_metrics_config {
  /// CU-CP statistics report period in milliseconds.
  unsigned                        cu_cp_report_period = 1000;
  app_helpers::metrics_config     common_metrics_cfg;
  cu_cp_unit_metrics_layer_config layers_cfg;
};

/// CU-CP application unit configuration.
struct cu_cp_unit_config {
  /// Node name.
  std::string ran_node_name = "ocucp01";
  /// gNB identifier.
  gnb_id_t gnb_id = {411, 22};
  /// Maximum number of DUs.
  uint16_t max_nof_dus = 6;
  /// Maximum number of CU-UPs.
  uint16_t max_nof_cu_ups = 6;
  /// Maximum number of UEs.
  uint32_t max_nof_ues = 8192;
  /// Maximum number of DRBs per UE.
  uint8_t max_nof_drbs_per_ue = 8;
  /// Inactivity timer in seconds.
  int inactivity_timer = 120;
  /// Enable RRC inactive state.
  bool enable_rrc_inactive = false;
  /// RAN Paging cycle for RRC inactive UEs in number of radio frames.
  uint8_t ran_paging_cycle = 32;
  /// T380 timer value in minutes.
  int t380 = 10;
  /// Number of bits used for the UE id in short and full I-RNTI.
  uint8_t nof_i_rnti_ue_bits = 13;
  /// PDU session request timeout in seconds (must be larger than T310).
  unsigned request_pdu_session_timeout = 3;
  /// Loggers configuration.
  cu_cp_unit_logger_config loggers;
  /// PCAPs configuration.
  cu_cp_unit_pcap_config pcap_cfg;
  /// Metrics configuration.
  cu_cp_unit_metrics_config metrics;
  /// AMF configuration.
  cu_cp_unit_amf_config amf_config;
  // List of all AMFs the CU-CP should connect to.
  std::vector<cu_cp_unit_amf_config_item> extra_amfs;
  /// XNAP configurations.
  cu_cp_unit_xnap_config xnap_config;
  /// Mobility configuration.
  cu_cp_unit_mobility_config mobility_config;
  /// RRC configuration.
  cu_cp_unit_rrc_config rrc_config;
  /// Security configuration.
  cu_cp_unit_security_config security_config;
  /// F1AP configuration.
  cu_cp_unit_f1ap_config f1ap_config;
  /// E1AP configuration.
  cu_cp_unit_e1ap_config e1ap_config;
  /// QoS configuration.
  std::vector<cu_cp_unit_qos_config> qos_cfg;
  /// Network slice configuration.
  std::vector<s_nssai_t> slice_cfg = {s_nssai_t{slice_service_type{1}}};
};

} // namespace ocudu
