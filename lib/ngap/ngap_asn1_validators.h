// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ngap_asn1_converters.h"
#include "ocudu/asn1/asn1_dispatch.h"
#include "ocudu/asn1/ngap/ngap_ies.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include <string>

namespace ocudu::ocucp {

/// \brief Checks the GUAMIs of a served GUAMI list.
static inline error_type<std::string> validate_served_guami_list(const asn1::ngap::served_guami_list_l& list)
{
  for (const auto& served_guami_item : list) {
    if (!asn1_to_guami(served_guami_item.guami).has_value()) {
      return make_unexpected("Invalid GUAMI in served GUAMI list");
    }
  }
  return {};
}

/// \brief Checks the PLMN identities and the slices of a PLMN support list.
static inline error_type<std::string> validate_plmn_support_list(const asn1::ngap::plmn_support_list_l& list)
{
  for (const auto& plmn_support_item : list) {
    if (!plmn_identity::from_bytes(plmn_support_item.plmn_id.to_bytes()).has_value()) {
      return make_unexpected("Invalid PLMN identity in PLMN support list");
    }
    for (const auto& slice_support_item : plmn_support_item.slice_support_list) {
      if (slice_support_item.s_nssai.sd_present) {
        if (!slice_differentiator::create(slice_support_item.s_nssai.sd.to_number()).has_value()) {
          return make_unexpected("Invalid SD in slice support item");
        }
      }
    }
  }
  return {};
}

static inline error_type<std::pair<asn1::ngap::cause_c, std::string>>
validate_ng_setup_response(const asn1::ngap::ng_setup_resp_s& response)
{
  asn1::ngap::cause_c cause;
  cause.set_protocol().value = asn1::ngap::cause_protocol_opts::unspecified;

  if (response->served_guami_list.size() == 0) {
    return make_unexpected(std::make_pair(cause, "Served GUAMI list is empty"));
  }
  if (auto result = validate_served_guami_list(response->served_guami_list); not result.has_value()) {
    return make_unexpected(std::make_pair(cause, result.error()));
  }

  if (response->plmn_support_list.size() == 0) {
    return make_unexpected(std::make_pair(cause, "PLMN support list is empty"));
  }
  if (auto result = validate_plmn_support_list(response->plmn_support_list); not result.has_value()) {
    return make_unexpected(std::make_pair(cause, result.error()));
  }

  return {};
}

static inline error_type<std::string> validate_paging(const asn1::ngap::paging_s& paging)
{
  asn1::ngap::cause_c cause;

  if (paging->ue_paging_id.type() != asn1::ngap::ue_paging_id_c::types::five_g_s_tmsi) {
    return make_unexpected("Unsupported UE Paging ID");
  }

  if (paging->tai_list_for_paging.size() == 0) {
    return make_unexpected("TAI list for paging is empty");
  }

  return {};
}

} // namespace ocudu::ocucp
