// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_config_translators.h"
#include "apps/helpers/network/sctp_config_translators.h"
#include "apps/helpers/ntn/ntn_config_translators.h"
#include "apps/services/worker_manager/worker_manager_config.h"
#include "cu_cp_unit_config.h"
#include "ocudu/adt/format.h"
#include "ocudu/cu_cp/cu_cp_configuration_helpers.h"
#include "ocudu/ran/plmn_identity.h"

using namespace ocudu;

/// Translates the given RLC mode into a PDCP RLC mode.
static pdcp_rlc_mode rlc_mode_to_pdcp_rlc_mode(rlc_mode mode)
{
  if (mode == rlc_mode::um_bidir || mode == rlc_mode::um_unidir_ul || mode == rlc_mode::um_unidir_dl) {
    return pdcp_rlc_mode::um;
  }
  if (mode == rlc_mode::am) {
    return pdcp_rlc_mode::am;
  }

  report_error("Invalid RLC mode: {}\n", format_as(mode));
}

/// Gets the ROHC profile from the given type and configuration.
static rohc_profile_config get_rohc_profile(cu_cp_unit_pdcp_rohc_type          rohc_type,
                                            const cu_cp_unit_pdcp_rohc_config& rohc_cfg)
{
  if (rohc_type == cu_cp_unit_pdcp_rohc_type::rohc) {
    rohc_profile_config profiles;

    profiles.set_profile(rohc_profile::profile0x0001, rohc_cfg.profile0x0001)
        .set_profile(rohc_profile::profile0x0002, rohc_cfg.profile0x0002)
        .set_profile(rohc_profile::profile0x0003, rohc_cfg.profile0x0003)
        .set_profile(rohc_profile::profile0x0004, rohc_cfg.profile0x0004)
        .set_profile(rohc_profile::profile0x0006, rohc_cfg.profile0x0006)
        .set_profile(rohc_profile::profile0x0101, rohc_cfg.profile0x0101)
        .set_profile(rohc_profile::profile0x0102, rohc_cfg.profile0x0102)
        .set_profile(rohc_profile::profile0x0103, rohc_cfg.profile0x0103)
        .set_profile(rohc_profile::profile0x0104, rohc_cfg.profile0x0104);

    return profiles;
  }

  if (rohc_type == cu_cp_unit_pdcp_rohc_type::uplink_only_rohc) {
    rohc_profile_config profiles;

    profiles.set_profile(rohc_profile::profile0x0006, rohc_cfg.profile0x0006);

    return profiles;
  }

  report_error("Invalid ROHC type: {}\n", to_string(rohc_type));
  return rohc_profile_config{};
}

/// Gets the given header compression for the given ROHC type and configuration.
static std::optional<rohc_config> get_header_compression(const cu_cp_unit_pdcp_rohc_config& rohc_cfg)
{
  if (rohc_cfg.rohc_type == cu_cp_unit_pdcp_rohc_type::none) {
    return std::nullopt;
  }

  return rohc_config{
      .rohc_type =
          (rohc_cfg.rohc_type == cu_cp_unit_pdcp_rohc_type::rohc ? rohc_type_t::rohc : rohc_type_t::uplink_only_rohc),
      .max_cid  = rohc_cfg.max_cid,
      .profiles = get_rohc_profile(rohc_cfg.rohc_type, rohc_cfg)};
}

/// Generates the CU-CP QoS configuration.
static std::map<five_qi_t, ocucp::cu_cp_qos_config> generate_cu_cp_qos_config(span<const cu_cp_unit_qos_config> qos_cfg)
{
  if (qos_cfg.empty()) {
    return config_helpers::make_default_cu_cp_qos_config_list();
  }

  std::map<five_qi_t, ocucp::cu_cp_qos_config> out_cfg;

  for (const auto& qos : qos_cfg) {
    if (out_cfg.find(qos.five_qi) != out_cfg.end()) {
      report_error("Duplicate 5QI configuration: {}\n", qos.five_qi);
    }

    // Convert PDCP config
    out_cfg[qos.five_qi].pdcp =
        pdcp_config{.rb_type                       = pdcp_rb_type::drb,
                    .rlc_mode                      = rlc_mode_to_pdcp_rlc_mode(qos.rlc.mode),
                    .header_compression            = get_header_compression(qos.pdcp.rohc),
                    .integrity_protection_required = false,
                    .ciphering_required            = true,
                    .tx                            = pdcp_config::tx_config{.sn_size                = qos.pdcp.tx.sn_field_length,
                                                                            .direction              = pdcp_security_direction::uplink,
                                                                            .discard_timer          = qos.pdcp.tx.discard_timer,
                                                                            .status_report_required = qos.pdcp.tx.status_report_required},
                    .rx                            = pdcp_config::rx_config{.sn_size               = qos.pdcp.rx.sn_field_length,
                                                                            .direction             = pdcp_security_direction::downlink,
                                                                            .out_of_order_delivery = qos.pdcp.rx.out_of_order_delivery,
                                                                            .t_reordering          = qos.pdcp.rx.t_reordering}};
  }
  return out_cfg;
}

/// Generates the RRC SSB MTC.
static ocucp::rrc_ssb_mtc generate_rrc_ssb_mtc(unsigned period, unsigned offset, unsigned duration)
{
  return ocucp::rrc_ssb_mtc{
      .periodicity_and_offset =
          ocucp::rrc_periodicity_and_offset{.periodicity =
                                                static_cast<ocucp::rrc_periodicity_and_offset::periodicity_t>(period),
                                            .offset = static_cast<uint8_t>(offset)},
      .dur = static_cast<uint8_t>(duration)};
}

/// Generates the CU-CP periodical report configuration.
static ocucp::rrc_periodical_report_cfg
generate_cu_cp_periodical_report_config(const cu_cp_unit_report_config& report_cfg_item)
{
  return ocucp::rrc_periodical_report_cfg{
      .rs_type                     = ocucp::rrc_nr_rs_type::ssb,
      .report_interv               = report_cfg_item.report_interval_ms,
      .report_amount               = -1,
      .report_quant_cell           = ocucp::rrc_meas_report_quant{.rsrp = true, .rsrq = true, .sinr = true},
      .max_report_cells            = 4,
      .report_quant_rs_idxes       = ocucp::rrc_meas_report_quant{.rsrp = true, .rsrq = true, .sinr = true},
      .max_nrof_rs_idxes_to_report = 4,
      .include_beam_meass          = true,
      .use_allowed_cell_list       = false,
      .periodic_ho_rsrp_offset     = static_cast<int8_t>(report_cfg_item.periodic_ho_rsrp_offset)

  };
}

/// Build a measurement trigger quantity for absolute thresholds (A1, A2, A4, A5).
/// Applies 3GPP TS 38.331 encoding:
///   RSRP [dBm]:  ASN.1 = value + 156      (range [-156..-31] -> [0..125])
///   RSRQ [dB]:   ASN.1 = (value + 43) x 2 (range [-43..20]   -> [0..126])
///   SINR [dB]:   ASN.1 = (value + 23) x 2 (range [-23..40]   -> [0..126])
static ocucp::rrc_meas_trigger_quant build_meas_trigger_threshold(std::string_view qty, int db_val)
{
  if (qty == "rsrp") {
    return ocucp::rrc_meas_trigger_quant{
        .rsrp = static_cast<uint8_t>(db_val + 156), .rsrq = std::nullopt, .sinr = std::nullopt};
  }

  if (qty == "rsrq") {
    return ocucp::rrc_meas_trigger_quant{
        .rsrp = std::nullopt, .rsrq = static_cast<uint8_t>((db_val + 43) * 2), .sinr = std::nullopt};
  }

  if (qty == "sinr") {
    return ocucp::rrc_meas_trigger_quant{
        .rsrp = std::nullopt, .rsrq = std::nullopt, .sinr = static_cast<uint8_t>((db_val + 23) * 2)};
  }

  report_error("Invalid measurement trigger quantity: {}\n", qty);
  return {};
}

/// Build a measurement trigger quantity for relative offsets (A3, A6).
/// Applies 3GPP TS 38.331 encoding: ASN.1 = value x 2 (dB -> 0.5 dB steps).
static ocucp::rrc_meas_trigger_quant build_meas_trigger_offset(std::string_view qty, int db_val)
{
  if (qty == "rsrp") {
    return ocucp::rrc_meas_trigger_quant{
        .rsrp = static_cast<uint8_t>(db_val * 2), .rsrq = std::nullopt, .sinr = std::nullopt};
  }

  if (qty == "rsrq") {
    return ocucp::rrc_meas_trigger_quant{
        .rsrp = std::nullopt, .rsrq = static_cast<uint8_t>(db_val * 2), .sinr = std::nullopt};
  }
  if (qty == "sinr") {
    return ocucp::rrc_meas_trigger_quant{
        .rsrp = std::nullopt, .rsrq = std::nullopt, .sinr = static_cast<uint8_t>(db_val * 2)};
  }

  report_error("Invalid measurement trigger quantity: {}\n", qty);
  return {};
}

/// Creates an event for a distance or time based identifier and returns it.
static ocucp::rrc_event_id
create_event_id_for_distance_or_time_based_id(const cu_cp_unit_report_config& report_cfg_item)
{
  ocudu_assert(report_cfg_item.event_triggered_report_type, "Invalid event triggered report type");
  ocudu_assert(report_cfg_item.time_to_trigger_ms, "Invalid time to trigger");
  ocudu_assert(report_cfg_item.distance_thresh_from_ref1_km, "Invalid distance threshold from reference one");
  ocudu_assert(report_cfg_item.distance_thresh_from_ref2_km, "Invalid distance threshold from reference two");
  ocudu_assert(report_cfg_item.ref_location1, "Invalid reference location one");
  ocudu_assert(report_cfg_item.ref_location2, "Invalid reference location two");
  ocudu_assert(report_cfg_item.hysteresis_location_km, "Invalid hysteresis location");
  ocudu_assert(report_cfg_item.t1_thres, "Invalid T1 threshold");
  ocudu_assert(report_cfg_item.duration, "Invalid duration");

  const bool is_distance = (report_cfg_item.event_triggered_report_type == ocucp::rrc_event_id::event_id_t::d1 ||
                            report_cfg_item.event_triggered_report_type == ocucp::rrc_event_id::event_id_t::d2);
  const bool is_d1       = (report_cfg_item.event_triggered_report_type == ocucp::rrc_event_id::event_id_t::d1);
  const bool is_time     = (report_cfg_item.event_triggered_report_type == ocucp::rrc_event_id::event_id_t::t1);

  return ocucp::rrc_event_id{
      .id              = *report_cfg_item.event_triggered_report_type,
      .report_on_leave = false,
      .hysteresis      = 0,
      .time_to_trigger =
          is_distance ? static_cast<uint16_t>(*report_cfg_item.time_to_trigger_ms) : static_cast<uint16_t>(0U),
      .meas_trigger_quant_thres_or_offset = std::nullopt,
      .meas_trigger_quant_thres_2         = std::nullopt,
      .use_allowed_cell_list              = std::nullopt,
      .distance_thresh_from_ref1          = is_distance ? std::make_optional<uint32_t>(static_cast<uint32_t>(
                                                     *report_cfg_item.distance_thresh_from_ref1_km * 1000.0))
                                                        : std::nullopt,
      .distance_thresh_from_ref2          = is_distance ? std::make_optional<uint32_t>(static_cast<uint32_t>(
                                                     *report_cfg_item.distance_thresh_from_ref2_km * 1000.0))
                                                        : std::nullopt,
      .ref_location1 = is_d1 ? std::make_optional<reference_location>(*report_cfg_item.ref_location1) : std::nullopt,
      .ref_location2 = is_d1 ? std::make_optional<reference_location>(*report_cfg_item.ref_location2) : std::nullopt,
      .hysteresis_location = is_distance ? std::make_optional<uint32_t>(
                                               static_cast<uint32_t>(*report_cfg_item.hysteresis_location_km * 1000.0))
                                         : std::nullopt,
      .t1_thres =
          is_time ? std::make_optional<std::chrono::system_clock::time_point>(*report_cfg_item.t1_thres) : std::nullopt,
      .duration = is_time
                      ? std::make_optional<unsigned>(static_cast<unsigned>(
                            std::chrono::duration_cast<std::chrono::milliseconds>(*report_cfg_item.duration).count()))
                      : std::nullopt

  };
}

/// Creates an event if for a conditional identifier and returns it.
static ocucp::rrc_event_id create_event_id_for_conditional_trigger(const cu_cp_unit_report_config& report_cfg_item)
{
  ocudu_assert(report_cfg_item.event_triggered_report_type, "Invalid event triggered report type");
  ocudu_assert(report_cfg_item.hysteresis_db, "Invalid hysteresis");
  ocudu_assert(report_cfg_item.time_to_trigger_ms, "Invalid time to trigger");
  ocudu_assert(report_cfg_item.meas_trigger_quantity, "Invalid MEAS trigger quantity");

  const bool is_a3_or_a6 = (report_cfg_item.event_triggered_report_type == ocucp::rrc_event_id::event_id_t::a3 ||
                            report_cfg_item.event_triggered_report_type == ocucp::rrc_event_id::event_id_t::a6);
  const bool is_a5       = report_cfg_item.event_triggered_report_type == ocucp::rrc_event_id::event_id_t::a5;

  // TS 38.331 EventTriggerConfig: a3-Offset/a6-Offset are only present for A3/A6; a1/a2/a4-Threshold and
  // a5-Threshold1 are only present for A1/A2/A4/A5; a5-Threshold2 is only present for A5.
  if (is_a3_or_a6) {
    ocudu_assert(report_cfg_item.meas_trigger_quantity_offset_db, "Invalid MEAS trigger quantity offset");
  } else {
    ocudu_assert(report_cfg_item.meas_trigger_quantity_threshold_db, "Invalid MEAS trigger threshold");
  }

  if (is_a5) {
    ocudu_assert(report_cfg_item.meas_trigger_quantity_threshold_2_db, "Invalid MEAS trigger threshold two");
  }

  return ocucp::rrc_event_id{
      .id              = *report_cfg_item.event_triggered_report_type,
      .report_on_leave = false,
      // Hysteresis: convert dB to 0.5 dB ASN.1 units.
      .hysteresis      = static_cast<uint8_t>(*report_cfg_item.hysteresis_db * 2),
      .time_to_trigger = static_cast<uint16_t>(*report_cfg_item.time_to_trigger_ms),
      .meas_trigger_quant_thres_or_offset =
          is_a3_or_a6
              ? std::make_optional<ocucp::rrc_meas_trigger_quant>(build_meas_trigger_offset(
                    *report_cfg_item.meas_trigger_quantity, *report_cfg_item.meas_trigger_quantity_offset_db))
              : std::make_optional<ocucp::rrc_meas_trigger_quant>(build_meas_trigger_threshold(
                    *report_cfg_item.meas_trigger_quantity, *report_cfg_item.meas_trigger_quantity_threshold_db)),
      .meas_trigger_quant_thres_2 =
          is_a5 ? std::make_optional<ocucp::rrc_meas_trigger_quant>(build_meas_trigger_threshold(
                      *report_cfg_item.meas_trigger_quantity, *report_cfg_item.meas_trigger_quantity_threshold_2_db))
                : std::nullopt,
      .use_allowed_cell_list     = std::nullopt,
      .distance_thresh_from_ref1 = std::nullopt,
      .distance_thresh_from_ref2 = std::nullopt,
      .ref_location1             = std::nullopt,
      .ref_location2             = std::nullopt,
      .hysteresis_location       = std::nullopt,
      .t1_thres                  = std::nullopt,
      .duration                  = std::nullopt};
}

/// Creates an event trigger configuration and returns it.
static ocucp::rrc_event_trigger_cfg create_event_trigger_cfg(const cu_cp_unit_report_config& report_cfg_item)
{
  report_error_if_not(report_cfg_item.event_triggered_report_type, "Invalid event triggered report");
  report_error_if_not(report_cfg_item.hysteresis_db, "Invalid hysteresis");
  report_error_if_not(report_cfg_item.time_to_trigger_ms, "Invalid time to trigger");
  report_error_if_not(report_cfg_item.meas_trigger_quantity, "Invalid MEAS trigger quantity");

  const bool is_a3_or_a6       = (report_cfg_item.event_triggered_report_type == ocucp::rrc_event_id::event_id_t::a3 ||
                            report_cfg_item.event_triggered_report_type == ocucp::rrc_event_id::event_id_t::a6);
  const bool is_a5             = report_cfg_item.event_triggered_report_type == ocucp::rrc_event_id::event_id_t::a5;
  const bool is_a3_a4_a5_or_a6 = (report_cfg_item.event_triggered_report_type == ocucp::rrc_event_id::event_id_t::a3 ||
                                  report_cfg_item.event_triggered_report_type == ocucp::rrc_event_id::event_id_t::a4 ||
                                  report_cfg_item.event_triggered_report_type == ocucp::rrc_event_id::event_id_t::a5 ||
                                  report_cfg_item.event_triggered_report_type == ocucp::rrc_event_id::event_id_t::a6);

  // TS 38.331 EventTriggerConfig: a3-Offset/a6-Offset are only present for A3/A6; a1/a2/a4-Threshold and
  // a5-Threshold1 are only present for A1/A2/A4/A5; a5-Threshold2 is only present for A5.
  if (is_a3_or_a6) {
    report_error_if_not(report_cfg_item.meas_trigger_quantity_offset_db, "Invalid MEAS trigger quantity offset");
  } else {
    report_error_if_not(report_cfg_item.meas_trigger_quantity_threshold_db, "Invalid MEAS trigger threshold");
  }

  if (is_a5) {
    report_error_if_not(report_cfg_item.meas_trigger_quantity_threshold_2_db, "Invalid MEAS trigger threshold two");
  }

  return ocucp::rrc_event_trigger_cfg{
      .report_add_neigh_meas_present = true,
      .event_id =
          ocucp::rrc_event_id{
              .id              = *report_cfg_item.event_triggered_report_type,
              .report_on_leave = false,
              // Hysteresis: convert dB to 0.5 dB ASN.1 units.
              .hysteresis      = static_cast<uint8_t>(*report_cfg_item.hysteresis_db * 2),
              .time_to_trigger = static_cast<uint16_t>(*report_cfg_item.time_to_trigger_ms),
              .meas_trigger_quant_thres_or_offset =
                  is_a3_or_a6
                      ? std::make_optional<ocucp::rrc_meas_trigger_quant>(build_meas_trigger_offset(
                            *report_cfg_item.meas_trigger_quantity, *report_cfg_item.meas_trigger_quantity_offset_db))
                      : std::make_optional<ocucp::rrc_meas_trigger_quant>(
                            build_meas_trigger_threshold(*report_cfg_item.meas_trigger_quantity,
                                                         *report_cfg_item.meas_trigger_quantity_threshold_db)),
              .meas_trigger_quant_thres_2 =
                  is_a5 ? std::make_optional<ocucp::rrc_meas_trigger_quant>(
                              build_meas_trigger_threshold(*report_cfg_item.meas_trigger_quantity,
                                                           *report_cfg_item.meas_trigger_quantity_threshold_2_db))
                        : std::nullopt,
              .use_allowed_cell_list     = is_a3_a4_a5_or_a6 ? std::make_optional<bool>(false) : std::nullopt,
              .distance_thresh_from_ref1 = std::nullopt,
              .distance_thresh_from_ref2 = std::nullopt,
              .ref_location1             = std::nullopt,
              .ref_location2             = std::nullopt,
              .hysteresis_location       = std::nullopt,
              .t1_thres                  = std::nullopt,
              .duration                  = std::nullopt},
      .rs_type                     = ocucp::rrc_nr_rs_type::ssb,
      .report_interv               = static_cast<uint16_t>(report_cfg_item.report_interval_ms),
      .report_amount               = -1,
      .report_quant_cell           = ocucp::rrc_meas_report_quant{.rsrp = true, .rsrq = true, .sinr = true},
      .max_report_cells            = 4,
      .report_quant_rs_idxes       = ocucp::rrc_meas_report_quant{.rsrp = true, .rsrq = true, .sinr = true},
      .max_nrof_rs_idxes_to_report = std::nullopt,
      .include_beam_meass          = true,
      .t312                        = report_cfg_item.t312_ms};
}

/// Generates the CU-CP trigger report configuration and returns it.
static ocucp::rrc_report_cfg_nr generate_cu_cp_trigger_report_config(const cu_cp_unit_report_config& report_cfg_item)
{
  ocudu_assert(report_cfg_item.event_triggered_report_type, "Invalid event triggered report");

  const bool is_distance_or_time_based =
      (*report_cfg_item.event_triggered_report_type == ocucp::rrc_event_id::event_id_t::d1 ||
       *report_cfg_item.event_triggered_report_type == ocucp::rrc_event_id::event_id_t::t1 ||
       *report_cfg_item.event_triggered_report_type == ocucp::rrc_event_id::event_id_t::d2);

  // Distance-based and time-based events are only valid for cond_trigger.
  if (is_distance_or_time_based) {
    return ocucp::rrc_cond_trigger_cfg{.cond_event_id = create_event_id_for_distance_or_time_based_id(report_cfg_item),
                                       .rs_type       = ocucp::rrc_nr_rs_type::ssb};
  }

  // Conditional-trigger: wrap in rrc_cond_trigger_cfg (no report interval/amount fields).
  if (report_cfg_item.report_type == "cond_trigger") {
    return ocucp::rrc_cond_trigger_cfg{.cond_event_id = create_event_id_for_conditional_trigger(report_cfg_item),
                                       .rs_type       = ocucp::rrc_nr_rs_type::ssb};
  }

  return create_event_trigger_cfg(report_cfg_item);
}

/// Generates the admission configuration and returns it.
static ocucp::cu_cp_configuration::admission_params generate_admission_conf(const cu_cp_unit_config& cu_cfg)
{
  return ocucp::cu_cp_configuration::admission_params{.max_nof_dus         = cu_cfg.max_nof_dus,
                                                      .max_nof_cu_ups      = cu_cfg.max_nof_cu_ups,
                                                      .max_nof_ues         = cu_cfg.max_nof_ues,
                                                      .max_nof_drbs_per_ue = cu_cfg.max_nof_drbs_per_ue};
}

/// Generates the node configuration and returns it.
static ocucp::ran_node_configuration generate_node_conf(const cu_cp_unit_config& cu_cfg)
{
  return ocucp::ran_node_configuration{.gnb_id = cu_cfg.gnb_id, .ran_node_name = cu_cfg.ran_node_name};
}

/// Gets the supported Tracking Areas for the given items.
static std::vector<ocucp::supported_tracking_area> get_supported_tas(span<const cu_cp_unit_supported_ta_item> items)
{
  std::vector<ocucp::supported_tracking_area> supported_tas;
  for (const auto& supported_ta : items) {
    std::vector<ocucp::plmn_item> plmn_list;
    for (const auto& plmn_item : supported_ta.plmn_list) {
      expected<plmn_identity> plmn = plmn_identity::parse(plmn_item.plmn_id);
      if (!plmn.has_value()) {
        report_error("Invalid PLMN: {}\n", plmn_item.plmn_id);
      }
      auto& slice = plmn_list.emplace_back(ocucp::plmn_item{plmn.value(), {}});
      for (const auto& elem : plmn_item.tai_slice_support_list) {
        slice.slice_support_list.push_back(
            s_nssai_t{slice_service_type{elem.sst}, slice_differentiator::create(elem.sd).value()});
      }
    }
    supported_tas.push_back({supported_ta.tac, plmn_list});
  }
  return supported_tas;
}

/// Gets the address of the SCTP association with the given AMF.
static transport_layer_address get_amf_address(const cu_cp_unit_amf_config_item& amf_cfg)
{
  if (amf_cfg.ip_addrs.empty()) {
    report_error("No address configured for the AMF.\n");
  }

  // TODO: Take the address the SCTP association actually established, which the gateway resolves with
  // sctp_getpaddrs() but does not expose. The remaining configured addresses are multihoming alternatives of the same
  // association, so an AMF whose primary path is not the first of them is reported wrongly.
  transport_layer_address amf_addr = transport_layer_address::create_from_string(amf_cfg.ip_addrs.front());
  amf_addr.set_port(amf_cfg.port);

  return amf_addr;
}

/// Generates the NGAP configuration and returns it.
static ocucp::cu_cp_configuration::ngap_params generate_ngap_conf(const cu_cp_unit_config& cu_cfg)
{
  const std::vector<ocucp::supported_tracking_area> amf_supported_tas =
      get_supported_tas(cu_cfg.amf_config.amf.supported_tas);

  std::vector<ocucp::cu_cp_configuration::ngap_config> ngaps{
      ocucp::cu_cp_configuration::ngap_config{amf_supported_tas, get_amf_address(cu_cfg.amf_config.amf)}};

  for (const auto& cfg : cu_cfg.extra_amfs) {
    ngaps.push_back(
        ocucp::cu_cp_configuration::ngap_config{get_supported_tas(cfg.supported_tas), get_amf_address(cfg)});
  }

  return ocucp::cu_cp_configuration::ngap_params{
      .ngaps                       = std::move(ngaps),
      .amf_reconnection_retry_time = std::chrono::milliseconds{cu_cfg.amf_config.amf_reconnection_retry_time},
      .procedure_timeout           = std::chrono::milliseconds{cu_cfg.amf_config.procedure_timeout},
      .no_core                     = cu_cfg.amf_config.no_core,
      .ng_setup_notifier           = nullptr};
}

/// Generates the XNAP configuration and returns it.
static ocucp::cu_cp_configuration::xnap_params generate_xnap_conf(const cu_cp_unit_xnap_config& xnap_cfg)
{
  std::vector<ocucp::cu_cp_configuration::xnap_config> xnaps;
  std::map<xnc_peer_index_t, xnc_gateway_index_t>      peer_to_gateway;

  unsigned peer_idx = 0;
  for (unsigned gw_idx = 0, gw_e = xnap_cfg.gateways.size(); gw_idx != gw_e; ++gw_idx) {
    for (const auto& conn : xnap_cfg.gateways[gw_idx].connections) {
      ocucp::cu_cp_configuration::xnap_config xn_config;
      for (const auto& addr_str : conn.peer_addrs) {
        transport_layer_address addr = transport_layer_address::create_from_string(addr_str);
        addr.set_port(XNAP_PORT);
        xn_config.peer_addrs.push_back(addr);
      }
      xnaps.push_back(xn_config);
      peer_to_gateway[uint_to_xnc_peer_index(peer_idx)] = uint_to_xnc_gateway_index(gw_idx);
      ++peer_idx;
    }
  }

  return ocucp::cu_cp_configuration::xnap_params{.procedure_timeout =
                                                     std::chrono::milliseconds{xnap_cfg.procedure_timeout},
                                                 .reconnect_timer = std::chrono::milliseconds{xnap_cfg.reconnect_timer},
                                                 .no_connection_init = xnap_cfg.no_connection_init,
                                                 .xnaps              = xnaps,
                                                 .xnc_gws            = {},
                                                 .peer_to_gateway    = peer_to_gateway};
}

/// Generates the RRC configuration and returns it.
static ocucp::cu_cp_configuration::rrc_params generate_rrc_conf(const cu_cp_unit_rrc_config& rrc_cfg)
{
  return ocucp::cu_cp_configuration::rrc_params{
      .force_reestablishment_fallback = rrc_cfg.force_reestablishment_fallback,
      .force_resume_fallback          = rrc_cfg.force_resume_fallback,
      .rrc_procedure_guard_time_ms    = std::chrono::milliseconds{rrc_cfg.rrc_procedure_guard_time_ms},
      .rrc_version                    = ocucp::RRC_VERSION};
}

/// Generates the bearers configuration and returns it.
static ocucp::cu_cp_configuration::bearer_params generate_bearers_conf(span<const cu_cp_unit_qos_config> qos_cfg)
{
  return ocucp::cu_cp_configuration::bearer_params{
      .srb2_cfg   = ocucp::srb_pdcp_config{.t_reordering = pdcp_t_reordering::infinity},
      .drb_config = generate_cu_cp_qos_config(qos_cfg)};
}

/// Generates the security configuration and returns it.
static ocucp::cu_cp_configuration::security_params
generate_security_conf(const cu_cp_unit_security_config& security_config)
{
  return ocucp::cu_cp_configuration::security_params{
      .int_algo_pref_list = security_config.nia_preference_list,
      .enc_algo_pref_list = security_config.nea_preference_list,
      .default_security_indication =
          security_indication_t{.integrity_protection_ind       = security_config.integrity_protection,
                                .confidentiality_protection_ind = security_config.confidentiality_protection}};
}

/// Generates the UE configuration and returns it.
static ocucp::ue_configuration generate_ue_conf(const cu_cp_unit_config& cu_cfg)
{
  ocucp::ue_configuration ue_cfg{.inactivity_timer = std::chrono::seconds{cu_cfg.inactivity_timer},
                                 .request_pdu_session_timeout =
                                     std::chrono::seconds{cu_cfg.request_pdu_session_timeout},
                                 .enable_rrc_inactive = cu_cfg.enable_rrc_inactive,
                                 .ran_paging_cycle    = cu_cfg.ran_paging_cycle,
                                 .t380                = std::chrono::minutes{cu_cfg.t380}};

  if (!from_string(ue_cfg.full_i_rnti_prof, cu_cfg.full_i_rnti_profile)) {
    report_error("Invalid value for full_i_rnti_profile={}.\n", cu_cfg.full_i_rnti_profile);
  }
  if (!from_string(ue_cfg.short_i_rnti_prof, cu_cfg.short_i_rnti_profile)) {
    report_error("Invalid value for short_i_rnti_profile={}.\n", cu_cfg.short_i_rnti_profile);
  }

  return ue_cfg;
}

/// Generates the metrics configuration and returns it.
static ocucp::cu_cp_configuration::metrics_params generate_metrics_conf(const cu_cp_unit_metrics_config& metrics_cfg)
{
  return ocucp::cu_cp_configuration::metrics_params{
      .statistics_report_period = std::chrono::seconds{metrics_cfg.cu_cp_report_period},
      .metrics_report_period    = std::chrono::milliseconds(metrics_cfg.cu_cp_report_period),
      .layers_cfg               = ocucp::cu_cp_configuration::metrics_layers_config{
                        .enable_ngap_metrics = metrics_cfg.layers_cfg.enable_ngap_metrics,
                        .enable_rrc_metrics  = metrics_cfg.layers_cfg.enable_rrc_metrics}};
}

/// Generates the F1AP configuration.
static ocucp::f1ap_configuration generate_f1ap_conf(const cu_cp_unit_config& cu_cfg)
{
  return ocucp::f1ap_configuration{.proc_timeout     = std::chrono::milliseconds{cu_cfg.f1ap_config.procedure_timeout},
                                   .json_log_enabled = cu_cfg.loggers.f1ap_json_enabled};
}

/// Generates the E1AP configuration.
static ocucp::e1ap_configuration generate_e1ap_conf(const cu_cp_unit_config& cu_cfg)
{
  return ocucp::e1ap_configuration{.proc_timeout     = std::chrono::milliseconds{cu_cfg.e1ap_config.procedure_timeout},
                                   .json_log_enabled = cu_cfg.loggers.e1ap_json_enabled};
}

/// Generates the RRC SSB MTC.
static std::optional<ocucp::rrc_ssb_mtc> generate_rrc_ssb_mtc(const cu_cp_unit_cell_config_item& app_cfg_item)
{
  if (!app_cfg_item.ssb_duration.has_value() || !app_cfg_item.ssb_offset.has_value() ||
      !app_cfg_item.ssb_period.has_value()) {
    return std::nullopt;
  }

  return generate_rrc_ssb_mtc(
      app_cfg_item.ssb_period.value(), app_cfg_item.ssb_offset.value(), app_cfg_item.ssb_duration.value());
}

/// Generates the SSB subcarrier spacing.
static std::optional<subcarrier_spacing>
generate_ssb_subcarrier_spacing(const cu_cp_unit_cell_config_item& app_cfg_item)
{
  if (!app_cfg_item.ssb_scs.has_value()) {
    return std::nullopt;
  }

  return to_subcarrier_spacing(std::to_string(app_cfg_item.ssb_scs.value()));
}

/// Generates the SSB report config id.
static std::optional<ocucp::report_cfg_id_t> generate_ssb_report_cfg_id(const cu_cp_unit_cell_config_item& app_cfg_item)
{
  if (!app_cfg_item.periodic_report_cfg_id.has_value()) {
    return std::nullopt;
  }

  return ocucp::uint_to_report_cfg_id(app_cfg_item.periodic_report_cfg_id.value());
}

/// Generates the cell MEAS configuration.
static std::map<nr_cell_identity, ocucp::cell_meas_config> ge_cell_meas_config(const cu_cp_unit_config& cu_cfg)
{
  std::map<nr_cell_identity, ocucp::cell_meas_config> cells;

  // Convert appconfig's cell list into cell manager type.
  for (const auto& app_cfg_item : cu_cfg.mobility_config.cells) {
    nr_cell_identity nci = nr_cell_identity::create(app_cfg_item.nr_cell_id).value();

    if (app_cfg_item.plmn_id.has_value()) {
      expected<plmn_identity> plmn = plmn_identity::parse(app_cfg_item.plmn_id.value());
      if (!plmn.has_value()) {
        report_error("External cell (nci={:#x}) has invalid PLMN: {}\n", nci, app_cfg_item.plmn_id.value());
      }
    }

    std::vector<ocucp::neighbor_cell_meas_config> ncells;
    for (const auto& ncell : app_cfg_item.ncells) {
      ocucp::neighbor_cell_meas_config ncell_meas_cfg;
      ncell_meas_cfg.nci = nr_cell_identity::create(ncell.nr_cell_id).value();
      for (const auto& report_id : ncell.report_cfg_ids) {
        ncell_meas_cfg.report_cfg_ids.push_back(ocucp::uint_to_report_cfg_id(report_id));
      }

      ncells.push_back(ncell_meas_cfg);
    }

    cells[nci] = ocucp::cell_meas_config{
        .serving_cell_cfg =
            ocucp::serving_cell_meas_config{
                .nci               = nci,
                .gnb_id_bit_length = app_cfg_item.gnb_id_bit_length.value(),
                .plmn              = app_cfg_item.plmn_id.has_value()
                                         ? plmn_identity::parse(app_cfg_item.plmn_id.value()).value_or(plmn_identity::test_value())
                                         : plmn_identity::test_value(),
                .tac               = app_cfg_item.tac,
                .pci               = app_cfg_item.pci,
                .band              = app_cfg_item.band,
                .ssb_mtc           = generate_rrc_ssb_mtc(app_cfg_item),
                .ssb_arfcn         = app_cfg_item.ssb_arfcn,
                .ssb_scs           = generate_ssb_subcarrier_spacing(app_cfg_item)},
        .periodic_report_cfg_id = generate_ssb_report_cfg_id(app_cfg_item),
        .ncells                 = ncells};
  }

  return cells;
}

/// Gets the RRC report configuration NR.
static std::map<ocucp::report_cfg_id_t, ocucp::rrc_report_cfg_nr>
get_rrc_report_config_nr(const cu_cp_unit_config& cu_cfg)
{
  std::map<ocucp::report_cfg_id_t, ocucp::rrc_report_cfg_nr> report_config_ids;

  // Convert report config.
  for (const auto& report_cfg_item : cu_cfg.mobility_config.report_configs) {
    ocucp::rrc_report_cfg_nr report_cfg;

    if (report_cfg_item.report_type == "periodical") {
      report_cfg = generate_cu_cp_periodical_report_config(report_cfg_item);
    } else {
      report_cfg = generate_cu_cp_trigger_report_config(report_cfg_item);
    }

    report_config_ids[ocucp::uint_to_report_cfg_id(report_cfg_item.report_cfg_id)] = report_cfg;
  }
  return report_config_ids;
}

/// Generates the mobility configuration and returns it.
static ocucp::mobility_configuration generate_mobility_conf(const cu_cp_unit_config& cu_cfg)
{
  return ocucp::mobility_configuration{
      .meas_mgr_config     = ocucp::cell_meas_manager_config{.cells             = ge_cell_meas_config(cu_cfg),
                                                             .report_config_ids = get_rrc_report_config_nr(cu_cfg)},
      .mobility_mgr_config = ocucp::mobility_manager_config{
          .trigger_handover_from_measurements = cu_cfg.mobility_config.trigger_handover_from_measurements,
          .enable_ngap_metrics                = cu_cfg.metrics.layers_cfg.enable_ngap_metrics,
          .enable_rrc_metrics                 = cu_cfg.metrics.layers_cfg.enable_rrc_metrics,
          .trigger_cho_on_ue_setup            = cu_cfg.mobility_config.trigger_cho_on_ue_setup,
          .cho_timeout                        = std::chrono::milliseconds{cu_cfg.mobility_config.cho_timeout_ms}}};
}

/// Generates the services configuration and returns it.
static ocucp::cu_cp_configuration::service_params generate_services_conf()
{
  return ocucp::cu_cp_configuration::service_params{
      .cu_cp_executor = nullptr, .cu_cp_e2_exec = nullptr, .timers = nullptr};
}

ocucp::cu_cp_configuration ocudu::generate_cu_cp_config(const cu_cp_unit_config& cu_cfg)
{
  auto out_cfg = ocucp::cu_cp_configuration{.node             = generate_node_conf(cu_cfg),
                                            .admission        = generate_admission_conf(cu_cfg),
                                            .ngap             = generate_ngap_conf(cu_cfg),
                                            .xnap             = generate_xnap_conf(cu_cfg.xnap_config),
                                            .rrc              = generate_rrc_conf(cu_cfg.rrc_config),
                                            .f1ap             = generate_f1ap_conf(cu_cfg),
                                            .e1ap             = generate_e1ap_conf(cu_cfg),
                                            .security         = generate_security_conf(cu_cfg.security_config),
                                            .bearers          = generate_bearers_conf(cu_cfg.qos_cfg),
                                            .ue               = generate_ue_conf(cu_cfg),
                                            .mobility         = generate_mobility_conf(cu_cfg),
                                            .metrics          = generate_metrics_conf(cu_cfg.metrics),
                                            .services         = generate_services_conf(),
                                            .metrics_notifier = nullptr};

  // Logical cells: derive the NCI from the gNB Id and the sector ID, matching the DU's NCI derivation, so
  // the entry addresses the DU cell with the same sector ID.
  for (const cu_cp_unit_logical_cell_config& cell : cu_cfg.cells_cfg) {
    ocucp::cu_cp_logical_cell_config out_cell;
    out_cell.nci         = nr_cell_identity::create(cu_cfg.gnb_id, cell.sector_id).value();
    out_cell.admin_state = cell.admin_state;
    out_cell.barred      = cell.cell_barred;
    out_cfg.cells.push_back(out_cell);
  }

  if (!config_helpers::is_valid_configuration(out_cfg)) {
    report_error("Invalid CU-CP configuration.\n");
  }

  // Populate the NTN configuration manager config only when at least one neighbour cell configures NTN, so that the
  // optional reflects whether NTN is actually configured.
  if (auto ntn_cfg = generate_cu_cp_ntn_configuration_manager_config(cu_cfg); !ntn_cfg.cells.empty()) {
    out_cfg.ntn = std::move(ntn_cfg);
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

ocudu_ntn::ntn_configuration_manager_config
ocudu::generate_cu_cp_ntn_configuration_manager_config(const cu_cp_unit_config& cu_cfg)
{
  ocudu_ntn::ntn_configuration_manager_config out_cfg;

  // Add globally-defined satellites first. Use user-defined satellite_idx as internal satellite_index.
  unsigned next_satellite_idx = add_global_ntn_satellites(cu_cfg.ntn_satellites, out_cfg.satellites);

  // Look up a cell definition by its NR cell id. The NTN configuration lives on the cell itself, so a neighbour's
  // NTN info is taken from that neighbour's own cell entry.
  auto find_cell = [&cu_cfg](uint64_t nr_cell_id) -> const cu_cp_unit_cell_config_item* {
    for (const cu_cp_unit_cell_config_item& c : cu_cfg.mobility_config.cells) {
      if (c.nr_cell_id == nr_cell_id) {
        return &c;
      }
    }
    return nullptr;
  };

  for (const cu_cp_unit_cell_config_item& cell : cu_cfg.mobility_config.cells) {
    ocudu_ntn::ntn_cell_config out_cell;

    for (const cu_cp_unit_neighbor_cell_config_item& ncell : cell.ncells) {
      const cu_cp_unit_cell_config_item* ncell_def = find_cell(ncell.nr_cell_id);
      if (ncell_def == nullptr || !ncell_def->ntn_cfg.has_value()) {
        continue;
      }
      // Work on a copy: the resolution sets satellite_idx for inline satellite definitions.
      cu_cp_unit_cell_ntn_config ntn_cfg = *ncell_def->ntn_cfg;
      resolve_ntn_satellite_ref(ntn_cfg.sat_ref,
                                out_cfg.satellites,
                                next_satellite_idx,
                                ntn_cfg.sat_ref.ta_info,
                                fmt::format("cells[nci={:#x}].ntn", ncell.nr_cell_id));

      ocudu_ntn::ntn_neighbor_cell_config& nc = out_cell.ncells.emplace_back();
      nc.satellite_index                      = *ntn_cfg.sat_ref.satellite_idx;
      nc.nci                                  = nr_cell_identity::create(ncell.nr_cell_id).value();
      nc.reference_location                   = ntn_cfg.reference_location;
      nc.polarization                         = ntn_cfg.polarization;
      nc.use_state_vector =
          derive_use_state_vector(std::nullopt, ntn_cfg.sat_ref.ephemeris_info, nc.satellite_index, out_cfg.satellites);
    }

    // Skip cells whose neighbours carry no NTN configuration.
    if (out_cell.ncells.empty()) {
      continue;
    }

    out_cell.nr_cgi.nci    = nr_cell_identity::create(cell.nr_cell_id).value();
    out_cell.update_period = std::chrono::milliseconds(cu_cfg.mobility_config.ntn_update_period_ms);
    // Identity within the manager is by NCI alone; the serving PLMN is unused in the CU-CP path, so nr_cgi.plmn_id is
    // left at its default. The manager keys its cell map by NCI, forwards updates by NCI, and DU reference-time
    // reports are matched by NCI. The real PLMN that reaches RRC is resolved downstream by the cell measurement
    // manager from the DU's F1 data, not from here.

    out_cfg.cells.push_back(std::move(out_cell));
  }

  return out_cfg;
}
