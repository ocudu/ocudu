// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_unit_config_yaml_writer.h"
#include "apps/helpers/metrics/metrics_config_yaml_writer.h"
#include "apps/helpers/network/sctp_config_yaml_writer.h"
#include "cu_cp_unit_config.h"
#include "ocudu/adt/span.h"

using namespace ocudu;

static YAML::Node build_cu_cp_tai_slice_section(const cu_cp_unit_plmn_item::tai_slice_t& config)
{
  YAML::Node node;

  node["sst"] = static_cast<unsigned>(config.sst);
  if (config.sd) {
    node["sd"] = config.sd;
  }

  return node;
}

static YAML::Node build_cu_cp_plmn_list_section(const cu_cp_unit_plmn_item& config)
{
  YAML::Node node;

  node["plmn"]        = config.plmn_id;
  auto tai_slice_node = node["tai_slice_support_list"];
  for (const auto& slice : config.tai_slice_support_list) {
    tai_slice_node.push_back(build_cu_cp_tai_slice_section(slice));
  }

  return node;
}

static YAML::Node build_cu_cp_supported_tas_section(const cu_cp_unit_supported_ta_item& config)
{
  YAML::Node node;

  node["tac"]    = config.tac;
  auto plmn_node = node["plmn_list"];
  for (const auto& plmn_item : config.plmn_list) {
    plmn_node.push_back(build_cu_cp_plmn_list_section(plmn_item));
  }

  return node;
}

static YAML::Node build_cu_cp_extra_amfs_item_section(const cu_cp_unit_amf_config_item& config)
{
  YAML::Node node;

  node["addrs"]          = config.ip_addrs;
  node["port"]           = config.port;
  node["bind_addrs"]     = config.bind_addrs;
  node["bind_interface"] = config.bind_interface;
  fill_sctp_config_in_yaml_schema(node, config.sctp);

  auto sta_node = node["supported_tracking_areas"];
  for (const auto& ta : config.supported_tas) {
    sta_node.push_back(build_cu_cp_supported_tas_section(ta));
  }

  return node;
}

static YAML::Node build_cu_cp_extra_amfs_section(const std::vector<cu_cp_unit_amf_config_item>& amfs)
{
  YAML::Node node;

  for (const auto& amf : amfs) {
    node.push_back(build_cu_cp_extra_amfs_item_section(amf));
  }

  return node;
}

static YAML::Node build_cu_cp_amf_section(const cu_cp_unit_amf_config& config)
{
  // The CLI schema inlines the primary AMF item options into the cu_cp.amf subcommand, so emit those
  // fields at the same level here rather than under a nested "amf" key.
  YAML::Node node = build_cu_cp_extra_amfs_item_section(config.amf);

  node["no_core"]                     = config.no_core;
  node["amf_reconnection_retry_time"] = config.amf_reconnection_retry_time;
  node["procedure_timeout"]           = config.procedure_timeout;

  return node;
}

static YAML::Node build_cu_cp_xnap_peer_section(const cu_cp_unit_xnap_peer_config& config)
{
  YAML::Node node;

  node["peer_addrs"] = config.peer_addrs;

  return node;
}

static YAML::Node build_cu_cp_xnap_gateway_section(const cu_cp_unit_xnap_gateway_config& config)
{
  YAML::Node node;

  node["bind_addrs"] = config.bind_addrs;
  fill_sctp_config_in_yaml_schema(node["sctp"], config.sctp);
  for (const auto& peer : config.connections) {
    node["connections"].push_back(build_cu_cp_xnap_peer_section(peer));
  }

  return node;
}

static YAML::Node build_cu_cp_xnap_section(const cu_cp_unit_xnap_config& xnap_config)
{
  YAML::Node node;

  node["procedure_timeout"]  = xnap_config.procedure_timeout;
  node["no_connection_init"] = xnap_config.no_connection_init;

  for (const auto& gateway : xnap_config.gateways) {
    node["gateways"].push_back(build_cu_cp_xnap_gateway_section(gateway));
  }

  return node;
}

static YAML::Node build_cu_cp_mobility_ncells_section(const cu_cp_unit_neighbor_cell_config_item& config)
{
  YAML::Node node;

  node["nr_cell_id"] = config.nr_cell_id;
  for (auto report_id : config.report_cfg_ids) {
    node["report_configs"] = report_id;
  }

  node["report_configs"].SetStyle(YAML::EmitterStyle::Flow);

  return node;
}

static YAML::Node build_cu_cp_mobility_cells_section(const cu_cp_unit_cell_config_item& config)
{
  YAML::Node node;

  node["nr_cell_id"] = config.nr_cell_id;
  if (config.periodic_report_cfg_id) {
    node["periodic_report_cfg_id"] = config.periodic_report_cfg_id.value();
  }
  if (config.band) {
    node["band"] = static_cast<unsigned>(config.band.value());
  }
  node["gnb_id_bit_length"] = config.gnb_id_bit_length.value();
  if (config.pci) {
    node["pci"] = config.pci.value();
  }
  if (config.plmn_id) {
    node["plmn"] = config.plmn_id.value();
  }
  if (config.tac) {
    node["tac"] = config.tac.value();
  }
  if (config.ssb_arfcn) {
    node["ssb_arfcn"] = config.ssb_arfcn.value().value();
  }
  if (config.ssb_scs) {
    node["ssb_scs"] = config.ssb_scs.value();
  }
  if (config.ssb_period) {
    node["ssb_period"] = config.ssb_period.value();
  }
  if (config.ssb_offset) {
    node["ssb_offset"] = config.ssb_offset.value();
  }
  if (config.ssb_duration) {
    node["ssb_duration"] = config.ssb_duration.value();
  }

  for (const auto& ncell : config.ncells) {
    node["ncells"] = build_cu_cp_mobility_ncells_section(ncell);
  }

  return node;
}

/// Convert time_point to ISO 8601 format string (YYYY-MM-DDTHH:MM:SS.mmm).
static std::string timepoint_to_iso8601(const std::chrono::system_clock::time_point& tp)
{
  auto   ms           = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
  auto   secs         = std::chrono::duration_cast<std::chrono::seconds>(ms);
  time_t time         = secs.count();
  int    milliseconds = (ms.count() % 1000);

  std::tm tm_utc = *std::gmtime(&time);
  char    buf[32];
  std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm_utc);

  // Format milliseconds with leading zeros.
  char ms_buf[8];
  std::snprintf(ms_buf, sizeof(ms_buf), ".%03d", milliseconds);

  return std::string(buf) + ms_buf;
}

static YAML::Node build_cu_cp_mobility_report_section(const cu_cp_unit_report_config& config)
{
  YAML::Node node;

  node["report_cfg_id"] = config.report_cfg_id;
  node["report_type"]   = config.report_type;
  // Cond-trigger report configs do not have report interval semantics.
  if (config.report_type != "cond_trigger") {
    node["report_interval_ms"] = config.report_interval_ms;
  }

  if (!config.event_triggered_report_type) {
    return node;
  }

  // Helper: emit an optional field only when present.
  auto add_opt = [&](const char* key, const auto& opt) {
    if (opt.has_value()) {
      node[key] = opt.value();
    }
  };

  const std::string& ev               = config.event_triggered_report_type.value();
  node["event_triggered_report_type"] = ev;

  // Parameters common to all event types.
  add_opt("meas_trigger_quantity", config.meas_trigger_quantity);
  add_opt("hysteresis_db", config.hysteresis_db);
  add_opt("time_to_trigger_ms", config.time_to_trigger_ms);
  add_opt("t312_ms", config.t312_ms);

  // A1, A2, A4, A5 - absolute threshold on one measurement quantity.
  if (ev == "a1" or ev == "a2" or ev == "a4" or ev == "a5") {
    add_opt("meas_trigger_quantity_threshold_db", config.meas_trigger_quantity_threshold_db);
  }

  // A5 only - second absolute threshold (serving < T1 AND neighbour > T2).
  if (ev == "a5") {
    add_opt("meas_trigger_quantity_threshold_2_db", config.meas_trigger_quantity_threshold_2_db);
  }

  // A3, A6 - neighbour offset relative to serving cell.
  if (ev == "a3" or ev == "a6") {
    add_opt("meas_trigger_quantity_offset_db", config.meas_trigger_quantity_offset_db);
  }

  // D1/D2 - distance-based conditional events.
  if (ev == "d1" or ev == "d2") {
    add_opt("distance_thresh_from_ref1_km", config.distance_thresh_from_ref1_km);
    add_opt("distance_thresh_from_ref2_km", config.distance_thresh_from_ref2_km);
    add_opt("hysteresis_location_km", config.hysteresis_location_km);
  }

  // D1 - reference locations (serving and target cell).
  if (ev == "d1") {
    if (config.ref_location1.has_value()) {
      node["ref_location1"]["latitude"]  = config.ref_location1->latitude;
      node["ref_location1"]["longitude"] = config.ref_location1->longitude;
    }
    if (config.ref_location2.has_value()) {
      node["ref_location2"]["latitude"]  = config.ref_location2->latitude;
      node["ref_location2"]["longitude"] = config.ref_location2->longitude;
    }
  }

  // T1 - time-based conditional event.
  if (ev == "t1") {
    if (config.t1_thres.has_value()) {
      node["t1_thres"] = timepoint_to_iso8601(config.t1_thres.value());
    }
    if (config.duration.has_value()) {
      node["duration_s"] = config.duration->count();
    }
  }

  return node;
}

static YAML::Node build_cu_cp_mobility_section(const cu_cp_unit_mobility_config& config)
{
  YAML::Node node;

  node["trigger_handover_from_measurements"] = config.trigger_handover_from_measurements;
  node["trigger_cho_on_ue_setup"]            = config.trigger_cho_on_ue_setup;
  node["cho_timeout_ms"]                     = config.cho_timeout_ms;
  for (const auto& cell : config.cells) {
    node["cells"].push_back(build_cu_cp_mobility_cells_section(cell));
  }
  for (const auto& report : config.report_configs) {
    node["report_configs"].push_back(build_cu_cp_mobility_report_section(report));
  }

  return node;
}

static YAML::Node build_cu_cp_rrc_section(const cu_cp_unit_rrc_config& config)
{
  YAML::Node node;

  node["force_reestablishment_fallback"] = config.force_reestablishment_fallback;
  node["force_resume_fallback"]          = config.force_resume_fallback;
  node["rrc_procedure_guard_time_ms"]    = config.rrc_procedure_guard_time_ms;
  /// Only set rrc_reject_wait_time_s when provided.
  if (config.rrc_reject_wait_time_s.has_value()) {
    node["rrc_reject_wait_time_s"] = config.rrc_reject_wait_time_s.value();
  }

  return node;
}

static YAML::Node build_cu_cp_security_section(const cu_cp_unit_security_config& config)
{
  YAML::Node node;

  node["integrity"]       = config.integrity_protection;
  node["confidentiality"] = config.confidentiality_protection;
  node["nea_pref_list"]   = config.nea_preference_list;
  node["nia_pref_list"]   = config.nia_preference_list;

  return node;
}

static void fill_cu_cp_section(YAML::Node node, const cu_cp_unit_config& config)
{
  node["max_nof_dus"]                 = config.max_nof_dus;
  node["max_nof_cu_ups"]              = config.max_nof_cu_ups;
  node["max_nof_ues"]                 = config.max_nof_ues;
  node["max_nof_drbs_per_ue"]         = static_cast<unsigned>(config.max_nof_drbs_per_ue);
  node["inactivity_timer"]            = config.inactivity_timer;
  node["enable_rrc_inactive"]         = config.enable_rrc_inactive;
  node["ran_paging_cycle"]            = static_cast<unsigned>(config.ran_paging_cycle);
  node["t380"]                        = config.t380;
  node["nof_i_rnti_ue_bits"]          = static_cast<unsigned>(config.nof_i_rnti_ue_bits);
  node["request_pdu_session_timeout"] = config.request_pdu_session_timeout;

  node["amf"] = build_cu_cp_amf_section(config.amf_config);
  if (!config.extra_amfs.empty()) {
    node["extra_amfs"] = build_cu_cp_extra_amfs_section(config.extra_amfs);
  }
  if (!config.xnap_config.gateways.empty()) {
    node["xnap"] = build_cu_cp_xnap_section(config.xnap_config);
  }
  node["mobility"] = build_cu_cp_mobility_section(config.mobility_config);
  node["rrc"]      = build_cu_cp_rrc_section(config.rrc_config);
  node["security"] = build_cu_cp_security_section(config.security_config);
  // Merge into any existing f1ap/e1ap nodes the appconfig writer may have populated (bind_addrs, sctp...).
  node["f1ap"]["procedure_timeout"]                    = config.f1ap_config.procedure_timeout;
  node["f1ap"]["ref_time_reporting"]["enabled"]        = config.f1ap_config.ref_time_reporting_enabled;
  node["f1ap"]["ref_time_reporting"]["event_type"]     = config.f1ap_config.ref_time_reporting_event_type;
  node["f1ap"]["ref_time_reporting"]["periodicity_rf"] = config.f1ap_config.ref_time_reporting_periodicity_rf;

  node["e1ap"]["procedure_timeout"] = config.e1ap_config.procedure_timeout;
}

static void fill_cu_cp_log_section(YAML::Node node, const cu_cp_unit_logger_config& config)
{
  node["pdcp_level"]        = ocudulog::basic_level_to_string(config.pdcp_level);
  node["rrc_level"]         = ocudulog::basic_level_to_string(config.rrc_level);
  node["ngap_level"]        = ocudulog::basic_level_to_string(config.ngap_level);
  node["xnap_level"]        = ocudulog::basic_level_to_string(config.xnap_level);
  node["nrppa_level"]       = ocudulog::basic_level_to_string(config.nrppa_level);
  node["e1ap_level"]        = ocudulog::basic_level_to_string(config.e1ap_level);
  node["f1ap_level"]        = ocudulog::basic_level_to_string(config.f1ap_level);
  node["cu_level"]          = ocudulog::basic_level_to_string(config.cu_level);
  node["sec_level"]         = ocudulog::basic_level_to_string(config.sec_level);
  node["hex_max_size"]      = config.hex_max_size;
  node["e1ap_json_enabled"] = config.e1ap_json_enabled;
  node["f1ap_json_enabled"] = config.f1ap_json_enabled;
}

static void fill_cu_cp_pcap_section(YAML::Node node, const cu_cp_unit_pcap_config& config)
{
  node["ngap_filename"] = config.ngap.filename;
  node["ngap_enable"]   = config.ngap.enabled;
  node["xnap_filename"] = config.xnap.filename;
  node["xnap_enable"]   = config.xnap.enabled;
  node["f1ap_filename"] = config.f1ap.filename;
  node["f1ap_enable"]   = config.f1ap.enabled;
  node["e1ap_filename"] = config.e1ap.filename;
  node["e1ap_enable"]   = config.e1ap.enabled;
}

static void fill_cu_cp_metrics_layers_section(YAML::Node node, const cu_cp_unit_metrics_layer_config& config)
{
  node["enable_ngap"] = config.enable_ngap;
  node["enable_pdcp"] = config.enable_pdcp;
  node["enable_rrc"]  = config.enable_rrc;
}

static void fill_cu_cp_metrics_section(YAML::Node node, const cu_cp_unit_metrics_config& config)
{
  auto perdiodicity_node                   = node["periodicity"];
  perdiodicity_node["cu_cp_report_period"] = config.cu_cp_report_period;

  fill_cu_cp_metrics_layers_section(node["layers"], config.layers_cfg);
}

static void fill_cu_cp_am_section(YAML::Node node, const cu_cp_unit_rlc_am_config& config)
{
  {
    YAML::Node tx_node            = node["tx"];
    tx_node["sn"]                 = config.tx.sn_field_length;
    tx_node["t-poll-retransmit"]  = config.tx.t_poll_retx;
    tx_node["max-retx-threshold"] = config.tx.max_retx_thresh;
    tx_node["poll-pdu"]           = config.tx.poll_pdu;
    tx_node["poll-byte"]          = config.tx.poll_byte;
    tx_node["max_window"]         = config.tx.max_window;
    tx_node["queue-size"]         = config.tx.queue_size;
  }
  {
    YAML::Node rx_node           = node["rx"];
    rx_node["sn"]                = config.rx.sn_field_length;
    rx_node["t-reassembly"]      = config.rx.t_reassembly;
    rx_node["t-status-prohibit"] = config.rx.t_status_prohibit;
    rx_node["max_sn_per_status"] = config.rx.max_sn_per_status;
  }
}

static void fill_cu_cp_um_bidir_section(YAML::Node node, const cu_cp_unit_rlc_um_config& config)
{
  {
    YAML::Node tx_node    = node["tx"];
    tx_node["sn"]         = config.tx.sn_field_length;
    tx_node["queue-size"] = config.tx.queue_size;
  }
  {
    YAML::Node rx_node      = node["rx"];
    rx_node["sn"]           = config.rx.sn_field_length;
    rx_node["t-reassembly"] = config.rx.t_reassembly;
  }
}

static void fill_cu_cp_rlc_qos_section(YAML::Node node, const cu_cp_unit_rlc_config& config)
{
  node["mode"] = config.mode;
  if (config.mode == "am") {
    fill_cu_cp_am_section(node["am"], config.am);
  }

  if (config.mode == "um-bidir") {
    fill_cu_cp_um_bidir_section(node["um-bidir"], config.um);
  }
}

static void fill_cu_cp_pdcp_qos_section(YAML::Node node, const cu_cp_unit_pdcp_config& config)
{
  {
    YAML::Node rohc_node   = node["rohc"];
    rohc_node["rohc_type"] = to_string(config.rohc.rohc_type);
    if (config.rohc.rohc_type != cu_cp_unit_pdcp_rohc_type::none) {
      rohc_node["max_cid"] = config.rohc.max_cid;
      if (config.rohc.rohc_type == cu_cp_unit_pdcp_rohc_type::rohc) {
        rohc_node["profile0x0001"] = config.rohc.profile0x0001;
        rohc_node["profile0x0002"] = config.rohc.profile0x0002;
        rohc_node["profile0x0003"] = config.rohc.profile0x0003;
        rohc_node["profile0x0004"] = config.rohc.profile0x0004;
        rohc_node["profile0x0006"] = config.rohc.profile0x0006;
        rohc_node["profile0x0101"] = config.rohc.profile0x0101;
        rohc_node["profile0x0102"] = config.rohc.profile0x0102;
        rohc_node["profile0x0103"] = config.rohc.profile0x0103;
        rohc_node["profile0x0104"] = config.rohc.profile0x0104;
      } else if (config.rohc.rohc_type == cu_cp_unit_pdcp_rohc_type::uplink_only_rohc) {
        rohc_node["profile0x0006"] = config.rohc.profile0x0006;
      }
    }
  }
  {
    YAML::Node tx_node                = node["tx"];
    tx_node["sn"]                     = config.tx.sn_field_length;
    tx_node["discard_timer"]          = config.tx.discard_timer;
    tx_node["status_report_required"] = config.tx.status_report_required;
  }
  {
    YAML::Node rx_node               = node["rx"];
    rx_node["sn"]                    = config.rx.sn_field_length;
    rx_node["t_reordering"]          = config.rx.t_reordering;
    rx_node["out_of_order_delivery"] = config.rx.out_of_order_delivery;
  }
}

static void fill_cu_cp_qos_entry(YAML::Node node, const cu_cp_unit_qos_config& config)
{
  node["five_qi"] = five_qi_to_uint(config.five_qi);
  fill_cu_cp_rlc_qos_section(node["rlc"], config.rlc);
  fill_cu_cp_pdcp_qos_section(node["pdcp"], config.pdcp);
}

static YAML::Node get_last_entry(YAML::Node node)
{
  ocudu_assert(node.size() > 0, "Node is empty");

  auto it = node.begin();
  for (unsigned i = 1; i != node.size(); ++i) {
    ++it;
  }
  return *it;
}

static void fill_cu_cp_qos_section(YAML::Node node, span<const cu_cp_unit_qos_config> qos_cfg)
{
  auto qos_node = node["qos"];
  for (const auto& qos : qos_cfg) {
    auto node_entry = std::find_if(qos_node.begin(), qos_node.end(), [five = qos.five_qi](const YAML::Node& tmp) {
      return static_cast<uint16_t>(five) == tmp["five_qi"].as<uint16_t>();
    });
    if (node_entry != qos_node.end()) {
      YAML::Node node_five = *node_entry;
      fill_cu_cp_qos_entry(node_five, qos);
    } else {
      qos_node.push_back(YAML::Node());
      fill_cu_cp_qos_entry(get_last_entry(qos_node), qos);
    }
  }
}

void ocudu::fill_cu_cp_config_in_yaml_schema(YAML::Node& node, const cu_cp_unit_config& config)
{
  node["gnb_id"]            = config.gnb_id.id;
  node["gnb_id_bit_length"] = static_cast<unsigned>(config.gnb_id.bit_length);
  node["ran_node_name"]     = config.ran_node_name;

  app_helpers::fill_metrics_appconfig_in_yaml_schema(node, config.metrics.common_metrics_cfg);

  fill_cu_cp_section(node["cu_cp"], config);
  fill_cu_cp_log_section(node["log"], config.loggers);
  fill_cu_cp_pcap_section(node["pcap"], config.pcap_cfg);
  fill_cu_cp_metrics_section(node["metrics"], config.metrics);
  fill_cu_cp_qos_section(node, config.qos_cfg);
}
