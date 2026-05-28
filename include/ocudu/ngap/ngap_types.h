// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cu_cp_paging.h"
#include "ocudu/ran/tai.h"

namespace ocudu::ocucp {

/// \brief AMF_UE_ID (non ASN1 type of AMF_UE_NGAP_ID) used to identify the UE in the AMF.
/// \remark See TS 38.413 Section 9.3.3.1: AMF_UE_NGAP_ID valid values: (0..2^40-1)
constexpr uint64_t MAX_NOF_AMF_UES = ((uint64_t)1 << 40);
enum class amf_ue_id_t : uint64_t { min = 0, max = MAX_NOF_AMF_UES - 1, invalid = 0x1ffffffffff };

/// Convert AMF_UE_ID type to integer.
constexpr uint64_t amf_ue_id_to_uint(amf_ue_id_t id)
{
  return static_cast<uint64_t>(id);
}

/// Convert integer to AMF_UE_ID type.
constexpr amf_ue_id_t uint_to_amf_ue_id(std::underlying_type_t<amf_ue_id_t> id)
{
  return static_cast<amf_ue_id_t>(id);
}

enum class ngap_rrc_inactive_transition_report_request : uint8_t {
  // Send RRC Inactive Transition Reports when the UE enters or leaves RRC_INACTIVE state.
  subsequent_state_transition_report = 0,
  // Send one single RRC Inactive Transition Report when UE transitions from RRC_INACTIVE to RRC_IDLE.
  single_rrc_connected_state_report,
  // Stop sending RRC Inactive Transition Reports.
  cancel_report
};

struct ngap_core_network_assist_info_for_inactive_ie_exts {
  std::optional<uint64_t>               extended_ue_idx_value;
  std::optional<cu_cp_paging_edrx_info> nr_paging_drx_info;
};

struct ngap_core_network_assist_info_for_inactive {
  uint64_t                            ue_id_idx_value = 0;
  std::optional<uint16_t>             ue_specific_drx;
  std::optional<std::chrono::seconds> periodic_registration_update_timer;
  // If mico_mode_ind is true, the NG-RAN node shall, if supported, consider that the registration area
  // for the UE is the full PLMN and ignore the TAI List for RRC Inactive IE, see 3GPP TS 38.413 section 8.3.1.2.
  bool                                                              mico_mode_ind = false;
  std::vector<tai_t>                                                tai_list_for_inactive;
  std::optional<ngap_core_network_assist_info_for_inactive_ie_exts> ie_exts;
};

} // namespace ocudu::ocucp

// AMF UE ID formatter.
template <>
struct fmt::formatter<ocudu::ocucp::amf_ue_id_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::ocucp::amf_ue_id_t& idx, FormatContext& ctx) const
  {
    if (idx == ocudu::ocucp::amf_ue_id_t::invalid) {
      return format_to(ctx.out(), "invalid");
    }
    return format_to(ctx.out(), "{}", (unsigned)idx);
  }
};
