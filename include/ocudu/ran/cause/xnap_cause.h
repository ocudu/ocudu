// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cause/common.h"
#include "fmt/base.h"
#include <variant>

namespace ocudu {

/// The XNAP radio network cause, see TS 38.423 section 9.2.3.2.
enum class xnap_cause_radio_network_t : uint8_t {
  cell_not_available = 0,
  ho_desirable_for_radio_reasons,
  ho_target_not_allowed,
  invalid_amf_set_id,
  no_radio_res_available_in_target_cell,
  partial_ho,
  reduce_load_in_serving_cell,
  res_optim_ho,
  time_crit_ho,
  txn_relo_coverall_expiry,
  txn_relo_cprep_expiry,
  unknown_guami_id,
  unknown_local_ng_ran_node_ue_xn_ap_id,
  inconsistent_remote_ng_ran_node_ue_xn_ap_id,
  encryption_and_or_integrity_protection_algorithms_not_supported,
  not_used_causes_value_neg1,
  multiple_pdu_session_id_instances,
  unknown_pdu_session_id,
  unknown_qos_flow_id,
  multiple_qos_flow_id_instances,
  switch_off_ongoing,
  not_supported_5qi_value,
  txn_d_coverall_expiry,
  txn_d_cprep_expiry,
  action_desirable_for_radio_reasons,
  reduce_load,
  res_optim,
  time_crit_action,
  target_not_allowed,
  no_radio_res_available,
  invalid_qos_combination,
  encryption_algorithms_not_supported,
  proc_cancelled,
  rrm_purpose,
  improve_user_bit_rate,
  user_inactivity,
  radio_conn_with_ue_lost,
  fail_in_the_radio_interface_proc,
  bearer_option_not_supported,
  up_integrity_protection_not_possible,
  up_confidentiality_protection_not_possible,
  res_not_available_for_the_slice_s,
  ue_max_ip_data_rate_reason,
  cp_integrity_protection_fail,
  up_integrity_protection_fail,
  slice_not_supported_by_ng_ran,
  mn_mob,
  sn_mob,
  count_reaches_max_value,
  unknown_old_ng_ran_node_ue_xn_ap_id,
  pdcp_overload,
  drb_id_not_available,
  unspecified,
  ue_context_id_not_known,
  non_relocation_of_context,
  cho_cpc_res_tobechanged,
  rsn_not_available_for_the_up,
  npn_access_denied,
  report_characteristics_empty,
  existing_meas_id,
  meas_temporarily_not_available,
  meas_not_supported_for_the_obj,
  ue_pwr_saving,
  not_existing_ng_ran_node2_meas_id,
  insufficient_ue_cap,
  normal_release,
  value_out_of_allowed_range,
  scg_activation_deactivation_fail,
  scg_deactivation_fail_due_to_data_tx,
  ssb_not_available,
  ltm_triggered,
  no_backhaul_res,
  miab_node_not_authorized,
  iab_not_authorized,
};

/// The XNAP transport cause, see TS 38.423 section 9.2.3.2.
enum class xnap_cause_transport_t : uint8_t { transport_res_unavailable = 0, unspecified };

/// The XNAP misc cause, see TS 38.423 section 9.2.3.2.
enum class xnap_cause_misc_t : uint8_t {
  ctrl_processing_overload = 0,
  hardware_fail,
  o_and_m_intervention,
  not_enough_user_plane_processing_res,
  unspecified
};

/// The XNAP cause to indicate the reason for a particular event, see TS 38.423 section 9.2.3.2.
/// The XNAP cause is a union of the radio network cause, transport cause, nas cause, protocol cause and misc cause.
using xnap_cause_t =
    std::variant<xnap_cause_radio_network_t, xnap_cause_transport_t, cause_protocol_t, xnap_cause_misc_t>;

constexpr const char* format_as(xnap_cause_radio_network_t cause)
{
  switch (cause) {
    case xnap_cause_radio_network_t::cell_not_available:
      return "cell_not_available";
    case xnap_cause_radio_network_t::ho_desirable_for_radio_reasons:
      return "ho_desirable_for_radio_reasons";
    case xnap_cause_radio_network_t::ho_target_not_allowed:
      return "ho_target_not_allowed";
    case xnap_cause_radio_network_t::invalid_amf_set_id:
      return "invalid_amf_set_id";
    case xnap_cause_radio_network_t::no_radio_res_available_in_target_cell:
      return "no_radio_res_available_in_target_cell";
    case xnap_cause_radio_network_t::partial_ho:
      return "partial_ho";
    case xnap_cause_radio_network_t::reduce_load_in_serving_cell:
      return "reduce_load_in_serving_cell";
    case xnap_cause_radio_network_t::res_optim_ho:
      return "res_optim_ho";
    case xnap_cause_radio_network_t::time_crit_ho:
      return "time_crit_ho";
    case xnap_cause_radio_network_t::txn_relo_coverall_expiry:
      return "txn_relo_coverall_expiry";
    case xnap_cause_radio_network_t::txn_relo_cprep_expiry:
      return "txn_relo_cprep_expiry";
    case xnap_cause_radio_network_t::unknown_guami_id:
      return "unknown_guami_id";
    case xnap_cause_radio_network_t::unknown_local_ng_ran_node_ue_xn_ap_id:
      return "unknown_local_ng_ran_node_ue_xn_ap_id";
    case xnap_cause_radio_network_t::inconsistent_remote_ng_ran_node_ue_xn_ap_id:
      return "inconsistent_remote_ng_ran_node_ue_xn_ap_id";
    case xnap_cause_radio_network_t::encryption_and_or_integrity_protection_algorithms_not_supported:
      return "encryption_and_or_integrity_protection_algorithms_not_supported";
    case xnap_cause_radio_network_t::not_used_causes_value_neg1:
      return "not_used_causes_value_neg1";
    case xnap_cause_radio_network_t::multiple_pdu_session_id_instances:
      return "multiple_pdu_session_id_instances";
    case xnap_cause_radio_network_t::unknown_pdu_session_id:
      return "unknown_pdu_session_id";
    case xnap_cause_radio_network_t::unknown_qos_flow_id:
      return "unknown_qos_flow_id";
    case xnap_cause_radio_network_t::multiple_qos_flow_id_instances:
      return "multiple_qos_flow_id_instances";
    case xnap_cause_radio_network_t::switch_off_ongoing:
      return "switch_off_ongoing";
    case xnap_cause_radio_network_t::not_supported_5qi_value:
      return "not_supported_5qi_value";
    case xnap_cause_radio_network_t::txn_d_coverall_expiry:
      return "txn_d_coverall_expiry";
    case xnap_cause_radio_network_t::txn_d_cprep_expiry:
      return "txn_d_cprep_expiry";
    case xnap_cause_radio_network_t::action_desirable_for_radio_reasons:
      return "action_desirable_for_radio_reasons";
    case xnap_cause_radio_network_t::reduce_load:
      return "reduce_load";
    case xnap_cause_radio_network_t::res_optim:
      return "res_optim";
    case xnap_cause_radio_network_t::time_crit_action:
      return "time_crit_action";
    case xnap_cause_radio_network_t::target_not_allowed:
      return "target_not_allowed";
    case xnap_cause_radio_network_t::no_radio_res_available:
      return "no_radio_res_available";
    case xnap_cause_radio_network_t::invalid_qos_combination:
      return "invalid_qos_combination";
    case xnap_cause_radio_network_t::encryption_algorithms_not_supported:
      return "encryption_algorithms_not_supported";
    case xnap_cause_radio_network_t::proc_cancelled:
      return "proc_cancelled";
    case xnap_cause_radio_network_t::rrm_purpose:
      return "rrm_purpose";
    case xnap_cause_radio_network_t::improve_user_bit_rate:
      return "improve_user_bit_rate";
    case xnap_cause_radio_network_t::user_inactivity:
      return "user_inactivity";
    case xnap_cause_radio_network_t::radio_conn_with_ue_lost:
      return "radio_conn_with_ue_lost";
    case xnap_cause_radio_network_t::fail_in_the_radio_interface_proc:
      return "fail_in_the_radio_interface_proc";
    case xnap_cause_radio_network_t::bearer_option_not_supported:
      return "bearer_option_not_supported";
    case xnap_cause_radio_network_t::up_integrity_protection_not_possible:
      return "up_integrity_protection_not_possible";
    case xnap_cause_radio_network_t::up_confidentiality_protection_not_possible:
      return "up_confidentiality_protection_not_possible";
    case xnap_cause_radio_network_t::res_not_available_for_the_slice_s:
      return "res_not_available_for_the_slice_s";
    case xnap_cause_radio_network_t::ue_max_ip_data_rate_reason:
      return "ue_max_ip_data_rate_reason";
    case xnap_cause_radio_network_t::cp_integrity_protection_fail:
      return "cp_integrity_protection_fail";
    case xnap_cause_radio_network_t::up_integrity_protection_fail:
      return "up_integrity_protection_fail";
    case xnap_cause_radio_network_t::slice_not_supported_by_ng_ran:
      return "slice_not_supported_by_ng_ran";
    case xnap_cause_radio_network_t::mn_mob:
      return "mn_mob";
    case xnap_cause_radio_network_t::sn_mob:
      return "sn_mob";
    case xnap_cause_radio_network_t::count_reaches_max_value:
      return "count_reaches_max_value";
    case xnap_cause_radio_network_t::unknown_old_ng_ran_node_ue_xn_ap_id:
      return "unknown_old_ng_ran_node_ue_xn_ap_id";
    case xnap_cause_radio_network_t::pdcp_overload:
      return "pdcp_overload";
    case xnap_cause_radio_network_t::drb_id_not_available:
      return "drb_id_not_available";
    case xnap_cause_radio_network_t::unspecified:
      return "unspecified";
    case xnap_cause_radio_network_t::ue_context_id_not_known:
      return "ue_context_id_not_known";
    case xnap_cause_radio_network_t::non_relocation_of_context:
      return "non_relocation_of_context";
    case xnap_cause_radio_network_t::cho_cpc_res_tobechanged:
      return "cho_cpc_res_tobechanged";
    case xnap_cause_radio_network_t::rsn_not_available_for_the_up:
      return "rsn_not_available_for_the_up";
    case xnap_cause_radio_network_t::npn_access_denied:
      return "npn_access_denied";
    case xnap_cause_radio_network_t::report_characteristics_empty:
      return "report_characteristics_empty";
    case xnap_cause_radio_network_t::existing_meas_id:
      return "existing_meas_id";
    case xnap_cause_radio_network_t::meas_temporarily_not_available:
      return "meas_temporarily_not_available";
    case xnap_cause_radio_network_t::meas_not_supported_for_the_obj:
      return "meas_not_supported_for_the_obj";
    case xnap_cause_radio_network_t::ue_pwr_saving:
      return "ue_pwr_saving";
    case xnap_cause_radio_network_t::not_existing_ng_ran_node2_meas_id:
      return "not_existing_ng_ran_node2_meas_id";
    case xnap_cause_radio_network_t::insufficient_ue_cap:
      return "insufficient_ue_cap";
    case xnap_cause_radio_network_t::normal_release:
      return "normal_release";
    case xnap_cause_radio_network_t::value_out_of_allowed_range:
      return "value_out_of_allowed_range";
    case xnap_cause_radio_network_t::scg_activation_deactivation_fail:
      return "scg_activation_deactivation_fail";
    case xnap_cause_radio_network_t::scg_deactivation_fail_due_to_data_tx:
      return "scg_deactivation_fail_due_to_data_tx";
    case xnap_cause_radio_network_t::ssb_not_available:
      return "ssb_not_available";
    case xnap_cause_radio_network_t::ltm_triggered:
      return "ltm_triggered";
    case xnap_cause_radio_network_t::no_backhaul_res:
      return "no_backhaul_res";
    case xnap_cause_radio_network_t::miab_node_not_authorized:
      return "miab_node_not_authorized";
    case xnap_cause_radio_network_t::iab_not_authorized:
      return "iab_not_authorized";
    default:
      return "unknown";
  }
}

constexpr const char* format_as(xnap_cause_transport_t cause)
{
  switch (cause) {
    case xnap_cause_transport_t::transport_res_unavailable:
      return "transport_res_unavailable";
    case xnap_cause_transport_t::unspecified:
      return "unspecified";
    default:
      return "unknown";
  }
}

constexpr const char* format_as(xnap_cause_misc_t cause)
{
  switch (cause) {
    case xnap_cause_misc_t::ctrl_processing_overload:
      return "ctrl_processing_overload";
    case xnap_cause_misc_t::hardware_fail:
      return "hardware_fail";
    case xnap_cause_misc_t::o_and_m_intervention:
      return "o_and_m_intervention";
    case xnap_cause_misc_t::not_enough_user_plane_processing_res:
      return "not_enough_user_plane_processing_res";
    case xnap_cause_misc_t::unspecified:
      return "unspecified";
    default:
      return "unknown";
  }
}

} // namespace ocudu

namespace fmt {

template <>
struct formatter<ocudu::xnap_cause_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::xnap_cause_t o, FormatContext& ctx) const
  {
    if (const auto* result = std::get_if<ocudu::xnap_cause_radio_network_t>(&o)) {
      return format_to(ctx.out(), "radio_network-{}", *result);
    }
    if (const auto* result = std::get_if<ocudu::xnap_cause_transport_t>(&o)) {
      return format_to(ctx.out(), "transport-{}", *result);
    }
    if (const auto* result = std::get_if<ocudu::cause_protocol_t>(&o)) {
      return format_to(ctx.out(), "protocol-{}", *result);
    }
    return format_to(ctx.out(), "misc-{}", std::get<ocudu::xnap_cause_misc_t>(o));
  }
};

} // namespace fmt
