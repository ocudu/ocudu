// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu {

/// The protocol cause.
/// For E1AP see TS 38.463 section 9.3.1.2.
/// For F1AP see TS 38.473 section 9.3.1.2.
/// For NGAP see TS 38.413 section 9.3.1.2.
enum class cause_protocol_t : uint8_t {
  transfer_syntax_error = 0,
  abstract_syntax_error_reject,
  abstract_syntax_error_ignore_and_notify,
  msg_not_compatible_with_receiver_state,
  semantic_error,
  abstract_syntax_error_falsely_constructed_msg,
  unspecified
};

/// The misc cause.
/// For E1AP see TS 38.463 section 9.3.1.2.
/// For F1AP see TS 38.473 section 9.3.1.2.
enum class cause_misc_t : uint8_t {
  ctrl_processing_overload = 0,
  not_enough_user_plane_processing_res,
  hardware_fail,
  om_intervention,
  unspecified
};

/// Provides the establishment cause for the RRCSetupRequest in accordance with the information
/// received from upper layers, see TS 38.331 section 6.2.2.
enum class establishment_cause_t : uint8_t {
  emergency = 0,
  high_prio_access,
  mt_access,
  mo_sig,
  mo_data,
  mo_voice_call,
  mo_video_call,
  mo_sms,
  mps_prio_access,
  mcs_prio_access,
  unknown
};

/// RRC connection establishment failure causes per TS 28.552, section 5.1.1.15.3.
enum class establishment_fail_cause_t : uint8_t { network_reject = 0, no_reply, other };

/// Provides the resume cause for the RRCResumeRequest in accordance with the information
/// received from upper layers, see TS 38.331 section 6.2.2.
enum class resume_cause_t : uint8_t {
  emergency = 0,
  high_prio_access,
  mt_access,
  mo_sig,
  mo_data,
  mo_voice_call,
  mo_video_call,
  mo_sms,
  rna_upd,
  mps_prio_access,
  mcs_prio_access,
  unknown
};

constexpr const char* format_as(cause_protocol_t cause)
{
  switch (cause) {
    case cause_protocol_t::transfer_syntax_error:
      return "transfer_syntax_error";
    case cause_protocol_t::abstract_syntax_error_reject:
      return "abstract_syntax_error_reject";
    case cause_protocol_t::abstract_syntax_error_ignore_and_notify:
      return "abstract_syntax_error_ignore_and_notify";
    case cause_protocol_t::msg_not_compatible_with_receiver_state:
      return "msg_not_compatible_with_receiver_state";
    case cause_protocol_t::semantic_error:
      return "semantic_error";
    case cause_protocol_t::abstract_syntax_error_falsely_constructed_msg:
      return "abstract_syntax_error_falsely_constructed_msg";
    case cause_protocol_t::unspecified:
      return "unspecified";
    default:
      return "unknown";
  }
}

constexpr const char* format_as(cause_misc_t cause)
{
  switch (cause) {
    case cause_misc_t::ctrl_processing_overload:
      return "ctrl_processing_overload";
    case cause_misc_t::not_enough_user_plane_processing_res:
      return "not_enough_user_plane_processing_res";
    case cause_misc_t::hardware_fail:
      return "hardware_fail";
    case cause_misc_t::om_intervention:
      return "om_intervention";
    case cause_misc_t::unspecified:
      return "unspecified";
    default:
      return "unknown";
  }
}

constexpr const char* format_as(establishment_cause_t o)
{
  switch (o) {
    case establishment_cause_t::emergency:
      return "emergency";
    case establishment_cause_t::high_prio_access:
      return "high_prio_access";
    case establishment_cause_t::mt_access:
      return "mt_access";
    case establishment_cause_t::mo_sig:
      return "mo_sig";
    case establishment_cause_t::mo_data:
      return "mo_data";
    case establishment_cause_t::mo_voice_call:
      return "mo_voice_call";
    case establishment_cause_t::mo_video_call:
      return "mo_video_call";
    case establishment_cause_t::mo_sms:
      return "mo_sms";
    case establishment_cause_t::mps_prio_access:
      return "mps_prio_access";
    case establishment_cause_t::mcs_prio_access:
      return "mcs_prio_access";
    default:
      return "unknown";
  }
}

constexpr const char* format_as(resume_cause_t o)
{
  switch (o) {
    case resume_cause_t::emergency:
      return "emergency";
    case resume_cause_t::high_prio_access:
      return "high_prio_access";
    case resume_cause_t::mt_access:
      return "mt_access";
    case resume_cause_t::mo_sig:
      return "mo_sig";
    case resume_cause_t::mo_data:
      return "mo_data";
    case resume_cause_t::mo_voice_call:
      return "mo_voice_call";
    case resume_cause_t::mo_video_call:
      return "mo_video_call";
    case resume_cause_t::mo_sms:
      return "mo_sms";
    case resume_cause_t::rna_upd:
      return "rna_upd";
    case resume_cause_t::mps_prio_access:
      return "mps_prio_access";
    case resume_cause_t::mcs_prio_access:
      return "mcs_prio_access";
    default:
      return "unknown";
  }
}

constexpr const char* format_as(establishment_fail_cause_t o)
{
  switch (o) {
    case establishment_fail_cause_t::network_reject:
      return "network reject";
    case establishment_fail_cause_t::no_reply:
      return "no reply";
    case establishment_fail_cause_t::other:
      return "other";
    default:
      return "unknown";
  }
}

} // namespace ocudu
