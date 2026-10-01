// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../xnap_asn1_converters.h"
#include "../xnap_context.h"
#include "ocudu/asn1/xnap/xnap_ies.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/plmn_identity.h"

namespace ocudu::ocucp {

/// \brief Validate a PLMN ID encoded as an ASN.1 fixed octet string of length 3 and return whether it is valid.
/// \param[in] asn1_plmn The ASN.1 fixed octet string of length 3 representing the PLMN ID.
/// \return true if the PLMN ID is valid, false otherwise.
static inline bool is_valid_plmn(const asn1::fixed_octstring<3, true>& asn1_plmn)
{
  auto result = plmn_identity::from_bytes(asn1_plmn.to_bytes());
  return result.has_value();
}

/// \brief Validate the XN Setup Request/Response and return the cause and error message if the validation fails.
/// \param[in] asn1_request_response The XN Setup Request/Response to validate.
/// \return An error type containing the cause and error message if the validation fails, or an empty value if the
/// validation succeeds.
template <typename T>
static inline error_type<std::pair<asn1::xnap::cause_c, std::string>>
validate_xn_setup_request_response(const T& asn1_request_response)
{
  asn1::xnap::cause_c cause;
  cause.set_protocol().value = asn1::xnap::cause_protocol_opts::unspecified;
  if (asn1_request_response->global_ng_ran_node_id.type() !=
          asn1::xnap::global_ng_ran_node_id_c::types_opts::options::gnb ||
      asn1_request_response->global_ng_ran_node_id.gnb().gnb_id.type() !=
          asn1::xnap::gnb_id_choice_c::types_opts::options::gnb_id) {
    return make_unexpected(std::make_pair(cause, "Unsupported global NG RAN node ID type"));
  }
  for (const auto& asn1_tai_support_item : asn1_request_response->tai_support_list) {
    for (const auto& asn1_broadcast_plmn : asn1_tai_support_item.broadcast_plmns) {
      if (not is_valid_plmn(asn1_broadcast_plmn.plmn_id)) {
        return make_unexpected(std::make_pair(cause, "Invalid PLMN ID in TAI support list"));
      }
    }
  }
  for (const auto& asn1_amf_region_info_item : asn1_request_response->amf_region_info) {
    if (not is_valid_plmn(asn1_amf_region_info_item.plmn_id)) {
      return make_unexpected(std::make_pair(cause, "Invalid PLMN ID in AMF region info"));
    }
  }
  for (const auto& asn1_served_cell : asn1_request_response->list_of_served_cells_nr) {
    if (not is_valid(static_cast<pci_t>(asn1_served_cell.served_cell_info_nr.nr_pci))) {
      return make_unexpected(std::make_pair(cause, "Invalid PCI in served cell info"));
    }
    for (const auto& asn1_plmn : asn1_served_cell.served_cell_info_nr.broadcast_plmn) {
      if (not is_valid_plmn(asn1_plmn)) {
        return make_unexpected(std::make_pair(cause, "Invalid PLMN ID in served cell info"));
      }
    }
  }

  return {};
}

/// \brief Generate the XNAP context for the peer CU-CP based on the information provided in the XN Setup
/// Request/Response.
/// \param[in] asn1_request_response The ASN.1 type XN Setup Request/Response containing the information to generate the
/// XNAP context for the peer CU-CP.
/// \return The generated XNAP context for the peer CU-CP.
template <typename T>
static inline xnap_context create_peer_xnap_context(const T& asn1_request_response)
{
  xnap_context context;

  // Fill gNB ID.
  context.gnb_id = gnb_id_t{
      .id = static_cast<uint32_t>(asn1_request_response->global_ng_ran_node_id.gnb().gnb_id.gnb_id().to_number()),
      .bit_length = static_cast<uint8_t>(asn1_request_response->global_ng_ran_node_id.gnb().gnb_id.gnb_id().length())};

  // Fill TAI support list.
  for (const auto& asn1_tai_support_item : asn1_request_response->tai_support_list) {
    supported_tracking_area tai_support_item;
    tai_support_item.tac = asn1_tai_support_item.tac.to_number();
    for (const auto& asn1_broadcast_plmn : asn1_tai_support_item.broadcast_plmns) {
      std::vector<s_nssai_t> slice_support_list;
      for (const auto& asn1_tai_slice : asn1_broadcast_plmn.tai_slice_support_list) {
        slice_support_list.push_back(asn1_to_s_nssai(asn1_tai_slice));
      }
      // Note: The ASN.1 PLMN ID already validated in validate_xn_setup_request, so it is safe to assume it is valid
      // here.
      plmn_item supported_plmn_item{.plmn_id =
                                        plmn_identity::from_bytes(asn1_broadcast_plmn.plmn_id.to_bytes()).value(),
                                    .slice_support_list = slice_support_list};
      tai_support_item.plmn_list.push_back(supported_plmn_item);
    }
    context.tai_support_list.push_back(tai_support_item);
  }

  // Fill AMF region info.
  for (const auto& asn1_amf_region_info_item : asn1_request_response->amf_region_info) {
    xnap_amf_region_info_item amf_region_info_item;
    // Note: The ASN.1 PLMN ID already validated in validate_xn_setup_request, so it is safe to assume it is valid here.
    amf_region_info_item.plmn = plmn_identity::from_bytes(asn1_amf_region_info_item.plmn_id.to_bytes()).value();
    amf_region_info_item.amf_region_id = asn1_amf_region_info_item.amf_region_id.to_number();

    context.amf_region_info.push_back(amf_region_info_item);
  }

  // Fill served cells list.
  if (asn1_request_response->list_of_served_cells_nr_present) {
    for (const auto& asn1_served_cell : asn1_request_response->list_of_served_cells_nr) {
      context.list_of_served_cells_nr.push_back(asn1_to_served_cell_info(asn1_served_cell.served_cell_info_nr));
    }
  }

  return context;
}

} // namespace ocudu::ocucp
