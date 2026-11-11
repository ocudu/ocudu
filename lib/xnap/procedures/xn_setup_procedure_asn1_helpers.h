// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../xnap_asn1_converters.h"
#include "ocudu/asn1/asn1_dispatch.h"
#include "ocudu/asn1/xnap/common.h"
#include "ocudu/asn1/xnap/xnap_pdu_contents.h"
#include "ocudu/ran/cause/xnap_cause.h"
#include "ocudu/xnap/xnap_configuration.h"
#include "ocudu/xnap/xnap_message.h"
#include <algorithm>
#include <utility>
#include <vector>

namespace ocudu::ocucp {

/// \brief Fill the list of NR cells served by this node in an XN Setup Request/Response.
/// \param[out] asn1_ies The IEs of the XN Setup Request/Response to fill.
/// \param[in] served_cells The cells served by this node.
template <typename T>
inline void fill_asn1_list_of_served_cells_nr(T& asn1_ies, span<const cu_cp_served_cell_info> served_cells)
{
  if (served_cells.empty()) {
    return;
  }

  asn1_ies->list_of_served_cells_nr_present = true;
  for (const auto& served_cell : served_cells) {
    asn1::xnap::served_cells_nr_item_s asn1_served_cell;
    asn1_served_cell.served_cell_info_nr = served_cell_info_nr_to_asn1(served_cell);
    asn1_ies->list_of_served_cells_nr.push_back(asn1_served_cell);
  }
}

/// \brief Merges tracking areas with the same TAC, and PLMNs with the same PLMN identity within a tracking area.
///
/// The CU-CP configures the tracking areas for each AMF. Two AMFs can have the same TAC for different PLMNs, or the
/// same TAI.
inline std::vector<supported_tracking_area> merge_tai_support_list(span<const supported_tracking_area> tai_support_list)
{
  std::vector<supported_tracking_area> merged;
  for (const auto& ta : tai_support_list) {
    auto it = std::find_if(
        merged.begin(), merged.end(), [&ta](const supported_tracking_area& item) { return item.tac == ta.tac; });
    supported_tracking_area* merged_ta = nullptr;
    if (it != merged.end()) {
      merged_ta = &*it;
    } else {
      merged.push_back(supported_tracking_area{ta.tac, {}, ta.satellite_rat});
      merged_ta = &merged.back();
    }

    for (const auto& plmn : ta.plmn_list) {
      auto merged_plmn = std::find_if(merged_ta->plmn_list.begin(),
                                      merged_ta->plmn_list.end(),
                                      [&plmn](const plmn_item& item) { return item.plmn_id == plmn.plmn_id; });
      if (merged_plmn == merged_ta->plmn_list.end()) {
        merged_ta->plmn_list.push_back(plmn);
        continue;
      }
      for (const auto& slice : plmn.slice_support_list) {
        if (std::find(merged_plmn->slice_support_list.begin(), merged_plmn->slice_support_list.end(), slice) ==
            merged_plmn->slice_support_list.end()) {
          merged_plmn->slice_support_list.push_back(slice);
        }
      }
    }
  }
  return merged;
}

/// \brief Fill the TAI Support List in an XN Setup Request/Response.
/// \param[out] asn1_ies The IEs of the XN Setup Request/Response to fill.
/// \param[in] tai_support_list The tracking areas of all AMFs.
template <typename T>
inline void fill_asn1_tai_support_list(T& asn1_ies, span<const supported_tracking_area> tai_support_list)
{
  for (const auto& tai_support_item : merge_tai_support_list(tai_support_list)) {
    asn1::xnap::tai_support_item_s asn1_tai_support_item;

    // Fill TAC.
    asn1_tai_support_item.tac.from_number(tai_support_item.tac);

    // Fill broadcast PLMN list.
    for (const auto& plmn_item : tai_support_item.plmn_list) {
      asn1::xnap::broadcast_plmn_in_tai_support_item_s asn1_broadcast_plmn;

      // Fill PLMN id.
      asn1_broadcast_plmn.plmn_id.from_number(plmn_item.plmn_id.to_bcd());

      // Fill TAI slice support list.
      for (const auto& snssai_item : plmn_item.slice_support_list) {
        asn1::xnap::s_nssai_s asn1_tai_slice;
        asn1_tai_slice.sst.from_number(snssai_item.sst.value());
        if (snssai_item.sd.is_set()) {
          asn1_tai_slice.sd_present = true;
          asn1_tai_slice.sd.from_number(snssai_item.sd.value());
        }
        asn1_broadcast_plmn.tai_slice_support_list.push_back(asn1_tai_slice);
      }
      asn1_tai_support_item.broadcast_plmns.push_back(asn1_broadcast_plmn);
    }
    asn1_ies->tai_support_list.push_back(asn1_tai_support_item);
  }
}

inline xnap_message generate_asn1_xn_setup_request(const xnap_configuration&          xnap_cfg,
                                                   span<const cu_cp_served_cell_info> served_cells)
{
  xnap_message xn_setup_req;
  xn_setup_req.pdu.set_init_msg();
  xn_setup_req.pdu.init_msg().load_info_obj(ASN1_XNAP_ID_XN_SETUP);

  asn1::xnap::xn_setup_request_s& asn1_ies = xn_setup_req.pdu.init_msg().value.xn_setup_request();

  // Fill global RAN node id.
  auto& global_gnb = asn1_ies->global_ng_ran_node_id.set_gnb();
  global_gnb.gnb_id.set_gnb_id();
  global_gnb.gnb_id.gnb_id().from_number(xnap_cfg.gnb_id.id, xnap_cfg.gnb_id.bit_length);
  global_gnb.plmn_id = xnap_cfg.tai_support_list.front().plmn_list.front().plmn_id.to_bytes();

  // Fill TAI support list.
  fill_asn1_tai_support_list(asn1_ies, xnap_cfg.tai_support_list);

  // Fill AMF region information.
  // Several GUAMIs, of one AMF or of several AMFs, can share an AMF Region.
  std::vector<std::pair<plmn_identity, uint8_t>> amf_regions;
  for (const auto& guami : xnap_cfg.guami_list) {
    std::pair<plmn_identity, uint8_t> amf_region{guami.plmn, guami.amf_region_id};
    if (std::find(amf_regions.begin(), amf_regions.end(), amf_region) != amf_regions.end()) {
      continue;
    }
    amf_regions.push_back(amf_region);

    asn1::xnap::global_amf_region_info_s amf_region_info;
    amf_region_info.plmn_id = guami.plmn.to_bytes();
    amf_region_info.amf_region_id.from_number(guami.amf_region_id);
    asn1_ies->amf_region_info.push_back(amf_region_info);
  }

  // Fill list of served cells.
  fill_asn1_list_of_served_cells_nr(asn1_ies, served_cells);

  return xn_setup_req;
}

inline xnap_message generate_asn1_xn_setup_response(const xnap_configuration&          xnap_cfg,
                                                    span<const cu_cp_served_cell_info> served_cells)
{
  xnap_message xn_setup_resp;

  xn_setup_resp.pdu.set_successful_outcome();
  xn_setup_resp.pdu.successful_outcome().load_info_obj(ASN1_XNAP_ID_XN_SETUP);

  asn1::xnap::xn_setup_resp_s& asn1_ies = xn_setup_resp.pdu.successful_outcome().value.xn_setup_resp();

  // Fill global RAN node id.
  auto& global_gnb = asn1_ies->global_ng_ran_node_id.set_gnb();
  global_gnb.gnb_id.set_gnb_id();
  global_gnb.gnb_id.gnb_id().from_number(xnap_cfg.gnb_id.id, xnap_cfg.gnb_id.bit_length);
  global_gnb.plmn_id = xnap_cfg.tai_support_list.front().plmn_list.front().plmn_id.to_bytes();

  // Fill TAI support list.
  fill_asn1_tai_support_list(asn1_ies, xnap_cfg.tai_support_list);

  // Fill list of served cells.
  fill_asn1_list_of_served_cells_nr(asn1_ies, served_cells);

  return xn_setup_resp;
}

inline xnap_message
generate_asn1_xn_setup_failure(xnap_cause_t                              cause,
                               std::optional<asn1::xnap::time_to_wait_e> time_to_wait = std::nullopt)
{
  xnap_message xn_setup_fail;

  xn_setup_fail.pdu.set_unsuccessful_outcome();
  xn_setup_fail.pdu.unsuccessful_outcome().load_info_obj(ASN1_XNAP_ID_XN_SETUP);

  asn1::xnap::xn_setup_fail_s& asn1_ies = xn_setup_fail.pdu.unsuccessful_outcome().value.xn_setup_fail();

  // Fill cause.
  asn1_ies->cause = cause_to_asn1(cause);

  // Fill Time to Wait IE.
  if (time_to_wait.has_value()) {
    asn1_ies->time_to_wait_present = true;
    asn1_ies->time_to_wait         = time_to_wait.value();
  }

  return xn_setup_fail;
}

} // namespace ocudu::ocucp
