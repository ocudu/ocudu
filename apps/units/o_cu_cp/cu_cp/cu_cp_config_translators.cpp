// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_config_translators.h"
#include "apps/helpers/network/sctp_config_translators.h"
#include "apps/services/worker_manager/worker_manager_config.h"
#include "cu_cp_unit_config.h"
#include "ocudu/cu_cp/cu_cp_configuration_helpers.h"
#include "ocudu/ran/plmn_identity.h"
#include <sstream>

using namespace ocudu;

static std::map<five_qi_t, ocucp::cu_cp_qos_config> generate_cu_cp_qos_config(const cu_cp_unit_config& config)
{
  std::map<five_qi_t, ocucp::cu_cp_qos_config> out_cfg = {};
  if (config.qos_cfg.empty()) {
    out_cfg = config_helpers::make_default_cu_cp_qos_config_list();
    return out_cfg;
  }

  for (const auto& qos : config.qos_cfg) {
    if (out_cfg.find(qos.five_qi) != out_cfg.end()) {
      report_error("Duplicate 5QI configuration: {}\n", qos.five_qi);
    }
    // Convert PDCP config
    pdcp_config& out_pdcp = out_cfg[qos.five_qi].pdcp;

    // RB type
    out_pdcp.rb_type = pdcp_rb_type::drb;

    // RLC mode
    rlc_mode mode = {};
    if (!from_string(mode, qos.rlc.mode)) {
      report_error("Invalid RLC mode: {}, mode={}\n", qos.five_qi, qos.rlc.mode);
    }
    if (mode == rlc_mode::um_bidir || mode == rlc_mode::um_unidir_ul || mode == rlc_mode::um_unidir_dl) {
      out_pdcp.rlc_mode = pdcp_rlc_mode::um;
    } else if (mode == rlc_mode::am) {
      out_pdcp.rlc_mode = pdcp_rlc_mode::am;
    } else {
      report_error("Invalid RLC mode: {}, mode={}\n", qos.five_qi, qos.rlc.mode);
    }

    // Header compression
    if (qos.pdcp.rohc.rohc_type != cu_cp_unit_pdcp_rohc_type::none) {
      out_pdcp.header_compression.emplace(rohc_config{});
      auto& rohc_cfg   = *out_pdcp.header_compression;
      rohc_cfg.max_cid = qos.pdcp.rohc.max_cid;
      if (qos.pdcp.rohc.rohc_type == cu_cp_unit_pdcp_rohc_type::rohc) {
        rohc_cfg.rohc_type = rohc_type_t::rohc;
        rohc_cfg.profiles.set_profile(rohc_profile::profile0x0001, qos.pdcp.rohc.profile0x0001)
            .set_profile(rohc_profile::profile0x0002, qos.pdcp.rohc.profile0x0002)
            .set_profile(rohc_profile::profile0x0003, qos.pdcp.rohc.profile0x0003)
            .set_profile(rohc_profile::profile0x0004, qos.pdcp.rohc.profile0x0004)
            .set_profile(rohc_profile::profile0x0006, qos.pdcp.rohc.profile0x0006)
            .set_profile(rohc_profile::profile0x0101, qos.pdcp.rohc.profile0x0101)
            .set_profile(rohc_profile::profile0x0102, qos.pdcp.rohc.profile0x0102)
            .set_profile(rohc_profile::profile0x0103, qos.pdcp.rohc.profile0x0103)
            .set_profile(rohc_profile::profile0x0104, qos.pdcp.rohc.profile0x0104);
      } else if (qos.pdcp.rohc.rohc_type == cu_cp_unit_pdcp_rohc_type::uplink_only_rohc) {
        rohc_cfg.rohc_type = rohc_type_t::uplink_only_rohc;
        rohc_cfg.profiles.set_profile(rohc_profile::profile0x0006, qos.pdcp.rohc.profile0x0006);
      } else {
        report_error("Invalid ROHC type: {}, type={}\n", qos.five_qi, to_string(qos.pdcp.rohc.rohc_type));
      }
    }

    out_pdcp.integrity_protection_required = false;
    out_pdcp.ciphering_required            = true;

    // > Tx
    // >> SN size
    if (!pdcp_sn_size_from_uint(out_pdcp.tx.sn_size, qos.pdcp.tx.sn_field_length)) {
      report_error("Invalid PDCP TX SN: {}, SN={}\n", qos.five_qi, qos.pdcp.tx.sn_field_length);
    }

    // >> discard timer
    out_pdcp.tx.discard_timer = pdcp_discard_timer{};
    if (!pdcp_discard_timer_from_int(out_pdcp.tx.discard_timer.value(), qos.pdcp.tx.discard_timer)) {
      report_error("Invalid PDCP discard timer. 5QI {} discard_timer {}\n", qos.five_qi, qos.pdcp.tx.discard_timer);
    }

    // >> status report required
    out_pdcp.tx.status_report_required = qos.pdcp.tx.status_report_required;

    // > Rx
    // >> SN size
    if (!pdcp_sn_size_from_uint(out_pdcp.rx.sn_size, qos.pdcp.rx.sn_field_length)) {
      report_error("Invalid PDCP RX SN: {}, SN={}\n", qos.five_qi, qos.pdcp.rx.sn_field_length);
    }

    // >> out of order delivery
    out_pdcp.rx.out_of_order_delivery = qos.pdcp.rx.out_of_order_delivery;

    // >> t-Reordering
    if (!pdcp_t_reordering_from_int(out_pdcp.rx.t_reordering, qos.pdcp.rx.t_reordering)) {
      report_error("Invalid PDCP t-Reordering. {} t-Reordering {}\n", qos.five_qi, qos.pdcp.rx.t_reordering);
    }
  }
  return out_cfg;
}

static security::preferred_integrity_algorithms
generate_preferred_integrity_algorithms_list(const cu_cp_unit_config& config)
{
  // String splitter helper
  auto split = [](const std::string& s, char delim) -> std::vector<std::string> {
    std::vector<std::string> result;
    std::stringstream        ss(s);
    for (std::string item; getline(ss, item, delim);) {
      result.push_back(item);
    }
    return result;
  };

  // > Remove spaces, convert to lower case and split on comma
  std::string nia_preference_list = config.security_config.nia_preference_list;
  nia_preference_list.erase(std::remove_if(nia_preference_list.begin(), nia_preference_list.end(), ::isspace),
                            nia_preference_list.end());
  std::transform(nia_preference_list.begin(),
                 nia_preference_list.end(),
                 nia_preference_list.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  std::vector<std::string> nea_v = split(nia_preference_list, ',');

  security::preferred_integrity_algorithms algo_list = {};
  int                                      idx       = 0;
  for (const std::string& nea : nea_v) {
    if (nea == "nia0") {
      algo_list[idx] = security::integrity_algorithm::nia0;
    } else if (nea == "nia1") {
      algo_list[idx] = security::integrity_algorithm::nia1;
    } else if (nea == "nia2") {
      algo_list[idx] = security::integrity_algorithm::nia2;
    } else if (nea == "nia3") {
      algo_list[idx] = security::integrity_algorithm::nia3;
    }
    idx++;
  }
  return algo_list;
}

static security::preferred_ciphering_algorithms
generate_preferred_ciphering_algorithms_list(const cu_cp_unit_config& config)
{
  // String splitter helper
  auto split = [](const std::string& s, char delim) -> std::vector<std::string> {
    std::vector<std::string> result;
    std::stringstream        ss(s);
    for (std::string item; getline(ss, item, delim);) {
      result.push_back(item);
    }
    return result;
  };

  // > Remove spaces, convert to lower case and split on comma
  std::string nea_preference_list = config.security_config.nea_preference_list;
  nea_preference_list.erase(std::remove_if(nea_preference_list.begin(), nea_preference_list.end(), ::isspace),
                            nea_preference_list.end());
  std::transform(nea_preference_list.begin(),
                 nea_preference_list.end(),
                 nea_preference_list.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  std::vector<std::string> nea_v = split(nea_preference_list, ',');

  security::preferred_ciphering_algorithms algo_list = {};
  int                                      idx       = 0;
  for (const std::string& nea : nea_v) {
    if (nea == "nea0") {
      algo_list[idx] = security::ciphering_algorithm::nea0;
    } else if (nea == "nea1") {
      algo_list[idx] = security::ciphering_algorithm::nea1;
    } else if (nea == "nea2") {
      algo_list[idx] = security::ciphering_algorithm::nea2;
    } else if (nea == "nea3") {
      algo_list[idx] = security::ciphering_algorithm::nea3;
    }
    idx++;
  }
  return algo_list;
}

static ocucp::rrc_ssb_mtc generate_rrc_ssb_mtc(unsigned period, unsigned offset, unsigned duration)
{
  ocucp::rrc_ssb_mtc ssb_mtc;
  ssb_mtc.periodicity_and_offset.periodicity = (ocucp::rrc_periodicity_and_offset::periodicity_t)period;
  ssb_mtc.periodicity_and_offset.offset      = offset;
  ssb_mtc.dur                                = duration;

  return ssb_mtc;
}

static ocucp::rrc_periodical_report_cfg
generate_cu_cp_periodical_report_config(const cu_cp_unit_report_config& report_cfg_item)
{
  ocucp::rrc_periodical_report_cfg periodical;

  periodical.rs_type                = ocucp::rrc_nr_rs_type::ssb;
  periodical.report_interv          = report_cfg_item.report_interval_ms;
  periodical.report_amount          = -1;
  periodical.report_quant_cell.rsrp = true;
  periodical.report_quant_cell.rsrq = true;
  periodical.report_quant_cell.sinr = true;
  periodical.max_report_cells       = 4;

  ocucp::rrc_meas_report_quant report_quant_rs_idxes;
  report_quant_rs_idxes.rsrp       = true;
  report_quant_rs_idxes.rsrq       = true;
  report_quant_rs_idxes.sinr       = true;
  periodical.report_quant_rs_idxes = report_quant_rs_idxes;

  periodical.max_nrof_rs_idxes_to_report = 4;
  periodical.include_beam_meass          = true;
  periodical.use_allowed_cell_list       = false;

  periodical.periodic_ho_rsrp_offset = report_cfg_item.periodic_ho_rsrp_offset;

  return periodical;
}

/// Build a measurement trigger quantity for absolute thresholds (A1, A2, A4, A5).
/// Applies 3GPP TS 38.331 encoding:
///   RSRP [dBm]:  ASN.1 = value + 156      (range [-156..-31] -> [0..125])
///   RSRQ [dB]:   ASN.1 = (value + 43) x 2 (range [-43..20]   -> [0..126])
///   SINR [dB]:   ASN.1 = (value + 23) x 2 (range [-23..40]   -> [0..126])
static ocucp::rrc_meas_trigger_quant build_meas_trigger_threshold(const std::string& qty, int db_val)
{
  ocucp::rrc_meas_trigger_quant q;
  if (qty == "rsrp") {
    q.rsrp = static_cast<uint8_t>(db_val + 156);
  } else if (qty == "rsrq") {
    q.rsrq = static_cast<uint8_t>((db_val + 43) * 2);
  } else if (qty == "sinr") {
    q.sinr = static_cast<uint8_t>((db_val + 23) * 2);
  } else {
    report_error("Invalid measurement trigger quantity: {}\n", qty);
  }
  return q;
}

/// Build a measurement trigger quantity for relative offsets (A3, A6).
/// Applies 3GPP TS 38.331 encoding: ASN.1 = value x 2 (dB -> 0.5 dB steps).
static ocucp::rrc_meas_trigger_quant build_meas_trigger_offset(const std::string& qty, int db_val)
{
  ocucp::rrc_meas_trigger_quant q;
  if (qty == "rsrp") {
    q.rsrp = static_cast<uint8_t>(db_val * 2);
  } else if (qty == "rsrq") {
    q.rsrq = static_cast<uint8_t>(db_val * 2);
  } else if (qty == "sinr") {
    q.sinr = static_cast<uint8_t>(db_val * 2);
  } else {
    report_error("Invalid measurement trigger quantity: {}\n", qty);
  }
  return q;
}

static ocucp::rrc_report_cfg_nr generate_cu_cp_trigger_report_config(const cu_cp_unit_report_config& report_cfg_item)
{
  const std::string& ev = report_cfg_item.event_triggered_report_type.value();

  // Distance-based and time-based events are only valid for cond_trigger.
  if (ev == "d1" or ev == "t1" or ev == "d2") {
    ocucp::rrc_event_id event_id;
    if (ev == "d1" or ev == "d2") {
      event_id.id = (ev == "d1") ? ocucp::rrc_event_id::event_id_t::d1 : ocucp::rrc_event_id::event_id_t::d2;
      event_id.distance_thresh_from_ref1 =
          static_cast<uint32_t>(report_cfg_item.distance_thresh_from_ref1_km.value() * 1000.0);
      event_id.distance_thresh_from_ref2 =
          static_cast<uint32_t>(report_cfg_item.distance_thresh_from_ref2_km.value() * 1000.0);
      event_id.hysteresis_location = static_cast<uint32_t>(report_cfg_item.hysteresis_location_km.value() * 1000.0);
      event_id.time_to_trigger     = report_cfg_item.time_to_trigger_ms.value();
      // D1-only: reference locations.
      if (ev == "d1") {
        event_id.ref_location1 = report_cfg_item.ref_location1.value();
        event_id.ref_location2 = report_cfg_item.ref_location2.value();
      }
    } else if (ev == "t1") {
      event_id.id       = ocucp::rrc_event_id::event_id_t::t1;
      event_id.t1_thres = report_cfg_item.t1_thres.value();
      event_id.duration = static_cast<unsigned>(
          std::chrono::duration_cast<std::chrono::milliseconds>(report_cfg_item.duration.value()).count());
    }
    ocucp::rrc_cond_trigger_cfg cond_trigger_cfg;
    cond_trigger_cfg.cond_event_id = event_id;
    cond_trigger_cfg.rs_type       = ocucp::rrc_nr_rs_type::ssb;
    return cond_trigger_cfg;
  }

  // A-family events (a1-a6).
  const std::string& qty = report_cfg_item.meas_trigger_quantity.value();

  // Build rrc_event_id (common to both event-triggered and conditional-trigger).
  ocucp::rrc_event_id event_id;

  if (ev == "a1") {
    event_id.id = ocucp::rrc_event_id::event_id_t::a1;
  } else if (ev == "a2") {
    event_id.id = ocucp::rrc_event_id::event_id_t::a2;
  } else if (ev == "a3") {
    event_id.id = ocucp::rrc_event_id::event_id_t::a3;
  } else if (ev == "a4") {
    event_id.id = ocucp::rrc_event_id::event_id_t::a4;
  } else if (ev == "a5") {
    event_id.id = ocucp::rrc_event_id::event_id_t::a5;
  } else {
    event_id.id = ocucp::rrc_event_id::event_id_t::a6;
  }

  // Hysteresis: convert dB to 0.5 dB ASN.1 units.
  event_id.hysteresis      = static_cast<uint8_t>(report_cfg_item.hysteresis_db.value() * 2);
  event_id.time_to_trigger = report_cfg_item.time_to_trigger_ms.value();

  // A3, A6: relative offset (neighbour - serving).
  if (ev == "a3" or ev == "a6") {
    event_id.meas_trigger_quant_thres_or_offset =
        build_meas_trigger_offset(qty, report_cfg_item.meas_trigger_quantity_offset_db.value());
  } else {
    // A1, A2, A4, A5: absolute threshold.
    event_id.meas_trigger_quant_thres_or_offset =
        build_meas_trigger_threshold(qty, report_cfg_item.meas_trigger_quantity_threshold_db.value());
    if (ev == "a5") {
      event_id.meas_trigger_quant_thres_2 =
          build_meas_trigger_threshold(qty, report_cfg_item.meas_trigger_quantity_threshold_2_db.value());
    }
  }

  // Conditional-trigger: wrap in rrc_cond_trigger_cfg (no report interval/amount fields).
  if (report_cfg_item.report_type == "cond_trigger") {
    ocucp::rrc_cond_trigger_cfg cond_trigger_cfg;
    cond_trigger_cfg.cond_event_id = event_id;
    cond_trigger_cfg.rs_type       = ocucp::rrc_nr_rs_type::ssb;
    return cond_trigger_cfg;
  }

  // Event-triggered specific parameters.
  event_id.report_on_leave = false;
  if (ev == "a3" or ev == "a4" or ev == "a5" or ev == "a6") {
    event_id.use_allowed_cell_list = false;
  }

  ocucp::rrc_event_trigger_cfg event_trigger_cfg;
  event_trigger_cfg.event_id               = event_id;
  event_trigger_cfg.rs_type                = ocucp::rrc_nr_rs_type::ssb;
  event_trigger_cfg.report_interv          = report_cfg_item.report_interval_ms;
  event_trigger_cfg.report_amount          = -1;
  event_trigger_cfg.report_quant_cell.rsrp = true;
  event_trigger_cfg.report_quant_cell.rsrq = true;
  event_trigger_cfg.report_quant_cell.sinr = true;
  event_trigger_cfg.max_report_cells       = 4;
  event_trigger_cfg.t312                   = report_cfg_item.t312_ms;

  ocucp::rrc_meas_report_quant report_quant_rs_idxes;
  report_quant_rs_idxes.rsrp              = true;
  report_quant_rs_idxes.rsrq              = true;
  report_quant_rs_idxes.sinr              = true;
  event_trigger_cfg.report_quant_rs_idxes = report_quant_rs_idxes;

  return event_trigger_cfg;
}

ocucp::cu_cp_configuration ocudu::generate_cu_cp_config(const cu_cp_unit_config& cu_cfg)
{
  ocucp::cu_cp_configuration out_cfg    = config_helpers::make_default_cu_cp_config();
  out_cfg.admission.max_nof_dus         = cu_cfg.max_nof_dus;
  out_cfg.admission.max_nof_cu_ups      = cu_cfg.max_nof_cu_ups;
  out_cfg.admission.max_nof_ues         = cu_cfg.max_nof_ues;
  out_cfg.admission.max_nof_drbs_per_ue = cu_cfg.max_nof_drbs_per_ue;

  out_cfg.node.gnb_id        = cu_cfg.gnb_id;
  out_cfg.node.ran_node_name = cu_cfg.ran_node_name;

  out_cfg.ngap.procedure_timeout           = std::chrono::milliseconds{cu_cfg.amf_config.procedure_timeout};
  out_cfg.ngap.amf_reconnection_retry_time = std::chrono::milliseconds{cu_cfg.amf_config.amf_reconnection_retry_time};
  out_cfg.ngap.no_core                     = cu_cfg.amf_config.no_core;

  {
    std::vector<ocucp::supported_tracking_area> supported_tas;
    for (const auto& supported_ta : cu_cfg.amf_config.amf.supported_tas) {
      std::vector<ocucp::plmn_item> plmn_list;
      for (const auto& plmn_item : supported_ta.plmn_list) {
        expected<plmn_identity> plmn = plmn_identity::parse(plmn_item.plmn_id);
        if (!plmn.has_value()) {
          report_error("Invalid PLMN: {}\n", plmn_item.plmn_id);
        }
        plmn_list.push_back({plmn.value(), {}});
        for (const auto& elem : plmn_item.tai_slice_support_list) {
          plmn_list.back().slice_support_list.push_back(
              s_nssai_t{slice_service_type{elem.sst}, slice_differentiator::create(elem.sd).value()});
        }
      }
      supported_tas.push_back({supported_ta.tac, plmn_list});
    }
    out_cfg.ngap.ngaps.push_back(ocucp::cu_cp_configuration::ngap_config{nullptr, supported_tas});
  }

  for (const auto& cfg : cu_cfg.extra_amfs) {
    std::vector<ocucp::supported_tracking_area> supported_tas;
    for (const auto& supported_ta : cfg.supported_tas) {
      std::vector<ocucp::plmn_item> plmn_list;
      for (const auto& plmn_item : supported_ta.plmn_list) {
        expected<plmn_identity> plmn = plmn_identity::parse(plmn_item.plmn_id);
        if (!plmn.has_value()) {
          report_error("Invalid PLMN: {}\n", plmn_item.plmn_id);
        }
        plmn_list.push_back({plmn.value(), {}});
        for (const auto& elem : plmn_item.tai_slice_support_list) {
          plmn_list.back().slice_support_list.push_back(
              s_nssai_t{slice_service_type{elem.sst}, slice_differentiator::create(elem.sd).value()});
        }
      }
      supported_tas.push_back({supported_ta.tac, plmn_list});
    }
    out_cfg.ngap.ngaps.push_back(ocucp::cu_cp_configuration::ngap_config{nullptr, supported_tas});
  }

  // XNAP.
  out_cfg.xnap.procedure_timeout  = std::chrono::milliseconds{cu_cfg.xnap_config.procedure_timeout};
  out_cfg.xnap.reconnect_timer    = std::chrono::milliseconds{cu_cfg.xnap_config.reconnect_timer};
  out_cfg.xnap.no_connection_init = cu_cfg.xnap_config.no_connection_init;
  uint32_t peer_idx               = 0;
  uint16_t gw_idx                 = 0;
  for (const auto& gw_cfg : cu_cfg.xnap_config.gateways) {
    for (const auto& peer : gw_cfg.connections) {
      ocucp::cu_cp_configuration::xnap_config xn_config{};
      for (const auto& addr_str : peer.peer_addrs) {
        transport_layer_address addr = transport_layer_address::create_from_string(addr_str);
        addr.set_port(XNAP_PORT);
        xn_config.peer_addrs.push_back(addr);
      }
      out_cfg.xnap.xnaps.push_back(xn_config);
      out_cfg.xnap.peer_to_gateway[uint_to_xnc_peer_index(peer_idx)] = uint_to_xnc_gateway_index(gw_idx);
      ++peer_idx;
    }
    ++gw_idx;
  }

  out_cfg.rrc.force_reestablishment_fallback = cu_cfg.rrc_config.force_reestablishment_fallback;
  out_cfg.rrc.force_resume_fallback          = cu_cfg.rrc_config.force_resume_fallback;
  out_cfg.rrc.rrc_procedure_guard_time_ms    = std::chrono::milliseconds{cu_cfg.rrc_config.rrc_procedure_guard_time_ms};

  out_cfg.bearers.drb_config = generate_cu_cp_qos_config(cu_cfg);

  out_cfg.security.int_algo_pref_list = generate_preferred_integrity_algorithms_list(cu_cfg);
  out_cfg.security.enc_algo_pref_list = generate_preferred_ciphering_algorithms_list(cu_cfg);
  if (!from_string(out_cfg.security.default_security_indication.integrity_protection_ind,
                   cu_cfg.security_config.integrity_protection)) {
    report_error("Invalid value for integrity_protection={}.\n", cu_cfg.security_config.integrity_protection);
  }
  if (!from_string(out_cfg.security.default_security_indication.confidentiality_protection_ind,
                   cu_cfg.security_config.confidentiality_protection)) {
    report_error("Invalid value for confidentiality_protection={}.\n",
                 cu_cfg.security_config.confidentiality_protection);
  }

  // Timers.
  out_cfg.ue.inactivity_timer              = std::chrono::seconds{cu_cfg.inactivity_timer};
  out_cfg.ue.request_pdu_session_timeout   = std::chrono::seconds{cu_cfg.request_pdu_session_timeout};
  out_cfg.metrics.statistics_report_period = std::chrono::seconds{cu_cfg.metrics.cu_cp_report_period};
  out_cfg.ue.t380                          = std::chrono::minutes{cu_cfg.t380};

  // RRC inactive.
  out_cfg.ue.enable_rrc_inactive = cu_cfg.enable_rrc_inactive;
  out_cfg.ue.ran_paging_cycle    = cu_cfg.ran_paging_cycle;
  out_cfg.ue.nof_i_rnti_ue_bits  = cu_cfg.nof_i_rnti_ue_bits;

  // Metrics.
  out_cfg.metrics.layers_cfg.enable_ngap = cu_cfg.metrics.layers_cfg.enable_ngap;
  out_cfg.metrics.layers_cfg.enable_rrc  = cu_cfg.metrics.layers_cfg.enable_rrc;

  // Mobility.
  out_cfg.mobility.mobility_manager_config.trigger_handover_from_measurements =
      cu_cfg.mobility_config.trigger_handover_from_measurements;
  out_cfg.mobility.mobility_manager_config.enable_ngap_metrics     = cu_cfg.metrics.layers_cfg.enable_ngap;
  out_cfg.mobility.mobility_manager_config.enable_rrc_metrics      = cu_cfg.metrics.layers_cfg.enable_rrc;
  out_cfg.mobility.mobility_manager_config.trigger_cho_on_ue_setup = cu_cfg.mobility_config.trigger_cho_on_ue_setup;
  out_cfg.mobility.mobility_manager_config.cho_timeout =
      std::chrono::milliseconds{cu_cfg.mobility_config.cho_timeout_ms};

  // F1AP-CU config.
  out_cfg.f1ap.proc_timeout     = std::chrono::milliseconds{cu_cfg.f1ap_config.procedure_timeout};
  out_cfg.f1ap.json_log_enabled = cu_cfg.loggers.f1ap_json_enabled;

  // E1AP-CU-CP config.
  out_cfg.e1ap.proc_timeout     = std::chrono::milliseconds{cu_cfg.e1ap_config.procedure_timeout};
  out_cfg.e1ap.json_log_enabled = cu_cfg.loggers.e1ap_json_enabled;

  // Convert appconfig's cell list into cell manager type.
  for (const auto& app_cfg_item : cu_cfg.mobility_config.cells) {
    nr_cell_identity        nci = nr_cell_identity::create(app_cfg_item.nr_cell_id).value();
    ocucp::cell_meas_config meas_cfg_item;
    meas_cfg_item.serving_cell_cfg.nci = nci;
    if (app_cfg_item.periodic_report_cfg_id.has_value()) {
      meas_cfg_item.periodic_report_cfg_id = ocucp::uint_to_report_cfg_id(app_cfg_item.periodic_report_cfg_id.value());
    }

    meas_cfg_item.serving_cell_cfg.gnb_id_bit_length = app_cfg_item.gnb_id_bit_length.value();
    meas_cfg_item.serving_cell_cfg.pci               = app_cfg_item.pci;
    if (app_cfg_item.plmn_id.has_value()) {
      expected<plmn_identity> plmn = plmn_identity::parse(app_cfg_item.plmn_id.value());
      if (!plmn.has_value()) {
        report_error("External cell (nci={:#x}) has invalid PLMN: {}\n", nci, app_cfg_item.plmn_id.value());
      }
      meas_cfg_item.serving_cell_cfg.plmn = plmn.value();
    }

    meas_cfg_item.serving_cell_cfg.tac       = app_cfg_item.tac;
    meas_cfg_item.serving_cell_cfg.band      = app_cfg_item.band;
    meas_cfg_item.serving_cell_cfg.ssb_arfcn = app_cfg_item.ssb_arfcn;
    if (app_cfg_item.ssb_scs.has_value()) {
      meas_cfg_item.serving_cell_cfg.ssb_scs.emplace() =
          to_subcarrier_spacing(std::to_string(app_cfg_item.ssb_scs.value()));
    }
    if (app_cfg_item.ssb_duration.has_value() && app_cfg_item.ssb_offset.has_value() &&
        app_cfg_item.ssb_period.has_value()) {
      // Add MTC config.
      meas_cfg_item.serving_cell_cfg.ssb_mtc.emplace() = generate_rrc_ssb_mtc(
          app_cfg_item.ssb_period.value(), app_cfg_item.ssb_offset.value(), app_cfg_item.ssb_duration.value());
    }

    for (const auto& ncell : app_cfg_item.ncells) {
      ocucp::neighbor_cell_meas_config ncell_meas_cfg;
      ncell_meas_cfg.nci = nr_cell_identity::create(ncell.nr_cell_id).value();
      for (const auto& report_id : ncell.report_cfg_ids) {
        ncell_meas_cfg.report_cfg_ids.push_back(ocucp::uint_to_report_cfg_id(report_id));
      }

      meas_cfg_item.ncells.push_back(ncell_meas_cfg);
    }

    // Store config.
    out_cfg.mobility.meas_manager_config.cells[meas_cfg_item.serving_cell_cfg.nci] = meas_cfg_item;
  }

  // Convert report config.
  for (const auto& report_cfg_item : cu_cfg.mobility_config.report_configs) {
    ocucp::rrc_report_cfg_nr report_cfg;

    if (report_cfg_item.report_type == "periodical") {
      report_cfg = generate_cu_cp_periodical_report_config(report_cfg_item);
    } else {
      report_cfg = generate_cu_cp_trigger_report_config(report_cfg_item);
    }

    // Store config.
    out_cfg.mobility.meas_manager_config
        .report_config_ids[ocucp::uint_to_report_cfg_id(report_cfg_item.report_cfg_id)] = report_cfg;
  }

  if (!config_helpers::is_valid_configuration(out_cfg)) {
    report_error("Invalid CU-CP configuration.\n");
  }

  return out_cfg;
}

ocucp::n2_connection_client_config ocudu::generate_n2_client_config(bool                              no_core,
                                                                    const cu_cp_unit_amf_config_item& amf_cfg,
                                                                    dlt_pcap&                         pcap_writer,
                                                                    io_broker&                        broker,
                                                                    task_executor&                    io_rx_executor)
{
  using no_core_mode_t = ocucp::n2_connection_client_config::no_core;
  using network_mode_t = ocucp::n2_connection_client_config::network;
  using ngap_mode_t    = std::variant<no_core_mode_t, network_mode_t>;

  ngap_mode_t mode = no_core ? ngap_mode_t{no_core_mode_t{}} : ngap_mode_t{network_mode_t{broker, io_rx_executor}};
  if (not no_core) {
    auto& nw_mode                  = std::get<network_mode_t>(mode);
    nw_mode.sctp.if_name           = "N2";
    nw_mode.sctp.dest_name         = "AMF";
    nw_mode.sctp.connect_addresses = amf_cfg.ip_addrs;
    nw_mode.sctp.connect_port      = amf_cfg.port;
    nw_mode.sctp.bind_addresses    = amf_cfg.bind_addrs;
    nw_mode.sctp.bind_interface    = amf_cfg.bind_interface;
    nw_mode.sctp.ppid              = NGAP_PPID;
    fill_sctp_network_gateway_config_socket_params(nw_mode.sctp, amf_cfg.sctp);
  }

  return ocucp::n2_connection_client_config{pcap_writer, mode};
}

void ocudu::fill_cu_cp_worker_manager_config(worker_manager_config& config, const cu_cp_unit_config& unit_cfg)
{
  // CU-CP executors are needed.
  config.cu_cp_cfg.emplace();

  // Enable PCAPs.
  auto& pcap_cfg = config.pcap_cfg;
  if (unit_cfg.pcap_cfg.e1ap.enabled) {
    pcap_cfg.is_e1ap_enabled = true;
  }
  if (unit_cfg.pcap_cfg.f1ap.enabled) {
    pcap_cfg.is_f1ap_enabled = true;
  }
  if (unit_cfg.pcap_cfg.ngap.enabled) {
    pcap_cfg.is_ngap_enabled = true;
  }
  if (unit_cfg.pcap_cfg.xnap.enabled) {
    pcap_cfg.is_xnap_enabled = true;
  }
}
