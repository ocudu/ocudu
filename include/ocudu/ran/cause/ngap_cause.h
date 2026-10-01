// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cause/common.h"
#include "fmt/base.h"
#include <variant>

namespace ocudu {

/// The NGAP radio network cause, see TS 38.413 section 9.3.1.2.
enum class ngap_cause_radio_network_t : uint8_t {
  unspecified = 0,
  txnrelocoverall_expiry,
  successful_ho,
  release_due_to_ngran_generated_reason,
  release_due_to_5gc_generated_reason,
  ho_cancelled,
  partial_ho,
  ho_fail_in_target_5_gc_ngran_node_or_target_sys,
  ho_target_not_allowed,
  tngrelocoverall_expiry,
  tngrelocprep_expiry,
  cell_not_available,
  unknown_target_id,
  no_radio_res_available_in_target_cell,
  unknown_local_ue_ngap_id,
  inconsistent_remote_ue_ngap_id,
  ho_desirable_for_radio_reason,
  time_crit_ho,
  res_optim_ho,
  reduce_load_in_serving_cell,
  user_inactivity,
  radio_conn_with_ue_lost,
  radio_res_not_available,
  invalid_qos_combination,
  fail_in_radio_interface_proc,
  interaction_with_other_proc,
  unknown_pdu_session_id,
  unkown_qos_flow_id,
  multiple_pdu_session_id_instances,
  multiple_qos_flow_id_instances,
  encryption_and_or_integrity_protection_algorithms_not_supported,
  ng_intra_sys_ho_triggered,
  ng_inter_sys_ho_triggered,
  xn_ho_triggered,
  not_supported_5qi_value,
  ue_context_transfer,
  ims_voice_eps_fallback_or_rat_fallback_triggered,
  up_integrity_protection_not_possible,
  up_confidentiality_protection_not_possible,
  slice_not_supported,
  ue_in_rrc_inactive_state_not_reachable,
  redirection,
  res_not_available_for_the_slice,
  ue_max_integrity_protected_data_rate_reason,
  release_due_to_cn_detected_mob,
  n26_interface_not_available,
  release_due_to_pre_emption,
  multiple_location_report_ref_id_instances,
  rsn_not_available_for_the_up,
  npn_access_denied,
  cag_only_access_denied,
  insufficient_ue_cap,
  redcap_ue_not_supported,
  unknown_mbs_session_id,
  indicated_mbs_session_area_info_not_served_by_the_gnb,
  inconsistent_slice_info_for_the_session,
  misaligned_assoc_for_multicast_unicast
};

/// The NGAP transport cause, see TS 38.413 section 9.3.1.2.
enum class ngap_cause_transport_t : uint8_t {
  transport_res_unavailable = 0,
  unspecified,
};

/// The NGAP NAS cause, see TS 38.413 section 9.3.1.2.
enum class cause_nas_t : uint8_t { normal_release = 0, authentication_fail, deregister, unspecified }; // only NGAP

/// The NGAP misc cause, see TS 38.413 section 9.3.1.2.
enum class ngap_cause_misc_t : uint8_t {
  ctrl_processing_overload = 0,
  not_enough_user_plane_processing_res,
  hardware_fail,
  om_intervention,
  unknown_plmn_or_sn_pn,
  unspecified
};

/// The NGAP cause to indicate the reason for a particular event, see TS 38.413 section 9.3.1.2.
/// The NGAP cause is a union of the radio network cause, transport cause, nas cause, protocol cause and misc cause.
using ngap_cause_t =
    std::variant<ngap_cause_radio_network_t, ngap_cause_transport_t, cause_nas_t, cause_protocol_t, ngap_cause_misc_t>;

constexpr const char* format_as(ngap_cause_radio_network_t cause)
{
  switch (cause) {
    case ngap_cause_radio_network_t::unspecified:
      return "unspecified";
    case ngap_cause_radio_network_t::txnrelocoverall_expiry:
      return "txnrelocoverall_expiry";
    case ngap_cause_radio_network_t::successful_ho:
      return "successful_ho";
    case ngap_cause_radio_network_t::release_due_to_ngran_generated_reason:
      return "release_due_to_ngran_generated_reason";
    case ngap_cause_radio_network_t::release_due_to_5gc_generated_reason:
      return "release_due_to_5gc_generated_reason";
    case ngap_cause_radio_network_t::ho_cancelled:
      return "ho_cancelled";
    case ngap_cause_radio_network_t::partial_ho:
      return "partial_ho";
    case ngap_cause_radio_network_t::ho_fail_in_target_5_gc_ngran_node_or_target_sys:
      return "ho_fail_in_target_5_gc_ngran_node_or_target_sys";
    case ngap_cause_radio_network_t::ho_target_not_allowed:
      return "ho_target_not_allowed";
    case ngap_cause_radio_network_t::tngrelocoverall_expiry:
      return "tngrelocoverall_expiry";
    case ngap_cause_radio_network_t::tngrelocprep_expiry:
      return "tngrelocprep_expiry";
    case ngap_cause_radio_network_t::cell_not_available:
      return "cell_not_available";
    case ngap_cause_radio_network_t::unknown_target_id:
      return "unknown_target_id";
    case ngap_cause_radio_network_t::no_radio_res_available_in_target_cell:
      return "no_radio_res_available_in_target_cell";
    case ngap_cause_radio_network_t::unknown_local_ue_ngap_id:
      return "unknown_local_ue_ngap_id";
    case ngap_cause_radio_network_t::inconsistent_remote_ue_ngap_id:
      return "inconsistent_remote_ue_ngap_id";
    case ngap_cause_radio_network_t::ho_desirable_for_radio_reason:
      return "ho_desirable_for_radio_reason";
    case ngap_cause_radio_network_t::time_crit_ho:
      return "time_crit_ho";
    case ngap_cause_radio_network_t::res_optim_ho:
      return "res_optim_ho";
    case ngap_cause_radio_network_t::reduce_load_in_serving_cell:
      return "reduce_load_in_serving_cell";
    case ngap_cause_radio_network_t::user_inactivity:
      return "user_inactivity";
    case ngap_cause_radio_network_t::radio_conn_with_ue_lost:
      return "radio_conn_with_ue_lost";
    case ngap_cause_radio_network_t::radio_res_not_available:
      return "radio_res_not_available";
    case ngap_cause_radio_network_t::invalid_qos_combination:
      return "invalid_qos_combination";
    case ngap_cause_radio_network_t::fail_in_radio_interface_proc:
      return "fail_in_radio_interface_proc";
    case ngap_cause_radio_network_t::interaction_with_other_proc:
      return "interaction_with_other_proc";
    case ngap_cause_radio_network_t::unknown_pdu_session_id:
      return "unknown_pdu_session_id";
    case ngap_cause_radio_network_t::unkown_qos_flow_id:
      return "unkown_qos_flow_id";
    case ngap_cause_radio_network_t::multiple_pdu_session_id_instances:
      return "multiple_pdu_session_id_instances";
    case ngap_cause_radio_network_t::multiple_qos_flow_id_instances:
      return "multiple_qos_flow_id_instances";
    case ngap_cause_radio_network_t::encryption_and_or_integrity_protection_algorithms_not_supported:
      return "encryption_and_or_integrity_protection_algorithms_not_supported";
    case ngap_cause_radio_network_t::ng_intra_sys_ho_triggered:
      return "ng_intra_sys_ho_triggered";
    case ngap_cause_radio_network_t::ng_inter_sys_ho_triggered:
      return "ng_inter_sys_ho_triggered";
    case ngap_cause_radio_network_t::xn_ho_triggered:
      return "xn_ho_triggered";
    case ngap_cause_radio_network_t::not_supported_5qi_value:
      return "not_supported_5qi_value";
    case ngap_cause_radio_network_t::ue_context_transfer:
      return "ue_context_transfer";
    case ngap_cause_radio_network_t::ims_voice_eps_fallback_or_rat_fallback_triggered:
      return "ims_voice_eps_fallback_or_rat_fallback_triggered";
    case ngap_cause_radio_network_t::up_integrity_protection_not_possible:
      return "up_integrity_protection_not_possible";
    case ngap_cause_radio_network_t::up_confidentiality_protection_not_possible:
      return "up_confidentiality_protection_not_possible";
    case ngap_cause_radio_network_t::slice_not_supported:
      return "slice_not_supported";
    case ngap_cause_radio_network_t::ue_in_rrc_inactive_state_not_reachable:
      return "ue_in_rrc_inactive_state_not_reachable";
    case ngap_cause_radio_network_t::redirection:
      return "redirection";
    case ngap_cause_radio_network_t::res_not_available_for_the_slice:
      return "res_not_available_for_the_slice";
    case ngap_cause_radio_network_t::ue_max_integrity_protected_data_rate_reason:
      return "ue_max_integrity_protected_data_rate_reason";
    case ngap_cause_radio_network_t::release_due_to_cn_detected_mob:
      return "release_due_to_cn_detected_mob";
    case ngap_cause_radio_network_t::n26_interface_not_available:
      return "n26_interface_not_available";
    case ngap_cause_radio_network_t::release_due_to_pre_emption:
      return "release_due_to_pre_emption";
    case ngap_cause_radio_network_t::multiple_location_report_ref_id_instances:
      return "multiple_location_report_ref_id_instances";
    case ngap_cause_radio_network_t::rsn_not_available_for_the_up:
      return "rsn_not_available_for_the_up";
    case ngap_cause_radio_network_t::npn_access_denied:
      return "npn_access_denied";
    case ngap_cause_radio_network_t::cag_only_access_denied:
      return "cag_only_access_denied";
    case ngap_cause_radio_network_t::insufficient_ue_cap:
      return "insufficient_ue_cap";
    case ngap_cause_radio_network_t::redcap_ue_not_supported:
      return "redcap_ue_not_supported";
    case ngap_cause_radio_network_t::unknown_mbs_session_id:
      return "unknown_mbs_session_id";
    case ngap_cause_radio_network_t::indicated_mbs_session_area_info_not_served_by_the_gnb:
      return "indicated_mbs_session_area_info_not_served_by_the_gnb";
    case ngap_cause_radio_network_t::inconsistent_slice_info_for_the_session:
      return "inconsistent_slice_info_for_the_session";
    case ngap_cause_radio_network_t::misaligned_assoc_for_multicast_unicast:
      return "misaligned_assoc_for_multicast_unicast";
    default:
      return "unknown";
  }
}

constexpr const char* format_as(ngap_cause_transport_t cause)
{
  switch (cause) {
    case ngap_cause_transport_t::transport_res_unavailable:
      return "transport_res_unavailable";
    case ngap_cause_transport_t::unspecified:
      return "unspecified";
    default:
      return "unknown";
  }
}

constexpr const char* format_as(cause_nas_t cause)
{
  switch (cause) {
    case cause_nas_t::normal_release:
      return "normal_release";
    case cause_nas_t::authentication_fail:
      return "authentication_fail";
    case cause_nas_t::deregister:
      return "deregister";
    case cause_nas_t::unspecified:
      return "unspecified";
    default:
      return "unknown";
  }
}

constexpr const char* format_as(ngap_cause_misc_t cause)
{
  switch (cause) {
    case ngap_cause_misc_t::ctrl_processing_overload:
      return "ctrl_processing_overload";
    case ngap_cause_misc_t::not_enough_user_plane_processing_res:
      return "not_enough_user_plane_processing_res";
    case ngap_cause_misc_t::hardware_fail:
      return "hardware_fail";
    case ngap_cause_misc_t::om_intervention:
      return "om_intervention";
    case ngap_cause_misc_t::unknown_plmn_or_sn_pn:
      return "unknown_plmn_or_sn_pn";
    case ngap_cause_misc_t::unspecified:
      return "unspecified";
    default:
      return "unknown";
  }
}

} // namespace ocudu

namespace fmt {

template <>
struct formatter<ocudu::ngap_cause_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::ngap_cause_t o, FormatContext& ctx) const
  {
    if (const auto* result = std::get_if<ocudu::ngap_cause_radio_network_t>(&o)) {
      return format_to(ctx.out(), "radio_network-{}", *result);
    }
    if (const auto* result = std::get_if<ocudu::ngap_cause_transport_t>(&o)) {
      return format_to(ctx.out(), "transport-{}", *result);
    }
    if (const auto* result = std::get_if<ocudu::cause_nas_t>(&o)) {
      return format_to(ctx.out(), "nas-{}", *result);
    }
    if (const auto* result = std::get_if<ocudu::cause_protocol_t>(&o)) {
      return format_to(ctx.out(), "protocol-{}", *result);
    }
    return format_to(ctx.out(), "misc-{}", std::get<ocudu::ngap_cause_misc_t>(o));
  }
};

} // namespace fmt
