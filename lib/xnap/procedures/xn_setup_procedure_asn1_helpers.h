// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../xnap_asn1_converters.h"
#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/asn1/xnap/common.h"
#include "ocudu/asn1/xnap/xnap_pdu_contents.h"
#include "ocudu/ran/cause/xnap_cause.h"
#include "ocudu/xnap/xnap_configuration.h"
#include "ocudu/xnap/xnap_message.h"

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
  asn1_ies->tai_support_list.resize(xnap_cfg.tai_support_list.size());
  for (unsigned i = 0; i < xnap_cfg.tai_support_list.size(); i++) {
    const auto& tai_support_item      = xnap_cfg.tai_support_list[i];
    auto&       asn1_tai_support_item = asn1_ies->tai_support_list[i];

    // Fill TAC.
    asn1_tai_support_item.tac.from_number(tai_support_item.tac);

    // Fill broadcast PLMN list.
    asn1_tai_support_item.broadcast_plmns.resize(tai_support_item.plmn_list.size());
    for (unsigned j = 0; j < xnap_cfg.tai_support_list.size(); j++) {
      const auto& plmn_item           = tai_support_item.plmn_list[j];
      auto&       asn1_broadcast_plmn = asn1_tai_support_item.broadcast_plmns[j];

      // Fill PLMN id.
      asn1_broadcast_plmn.plmn_id.from_number(plmn_item.plmn_id.to_bcd());

      // Fill TAI slice support list.
      asn1_broadcast_plmn.tai_slice_support_list.resize(plmn_item.slice_support_list.size());
      for (unsigned k = 0; k < plmn_item.slice_support_list.size(); k++) {
        // Fill S-NSSAI.
        const auto& snssai_item    = plmn_item.slice_support_list[k];
        auto&       asn1_tai_slice = asn1_broadcast_plmn.tai_slice_support_list[k];
        asn1_tai_slice.sst.from_number(snssai_item.sst.value());

        if (snssai_item.sd.is_set()) {
          asn1_tai_slice.sd_present = true;
          asn1_tai_slice.sd.from_number(snssai_item.sd.value());
        }
      }
    }
  }

  // Fill AMF region information.
  for (const auto& guami : xnap_cfg.guami_list) {
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

  asn1_ies->tai_support_list.resize(xnap_cfg.tai_support_list.size());
  for (unsigned i = 0; i < xnap_cfg.tai_support_list.size(); i++) {
    const auto& tai_support_item      = xnap_cfg.tai_support_list[i];
    auto&       asn1_tai_support_item = asn1_ies->tai_support_list[i];
    // Fill TAC.
    asn1_tai_support_item.tac.from_number(tai_support_item.tac);

    // Fill broadcast PLMN list.
    asn1_tai_support_item.broadcast_plmns.resize(tai_support_item.plmn_list.size());
    for (unsigned j = 0; j < xnap_cfg.tai_support_list.size(); j++) {
      const auto& plmn_item           = tai_support_item.plmn_list[j];
      auto&       asn1_broadcast_plmn = asn1_tai_support_item.broadcast_plmns[j];

      // Fill PLMN id.
      asn1_broadcast_plmn.plmn_id.from_number(plmn_item.plmn_id.to_bcd());

      // Fill TAI slice support list.
      asn1_broadcast_plmn.tai_slice_support_list.resize(plmn_item.slice_support_list.size());
      for (unsigned k = 0; k < plmn_item.slice_support_list.size(); k++) {
        // Fill S-NSSAI.
        const auto& snssai_item    = plmn_item.slice_support_list[k];
        auto&       asn1_tai_slice = asn1_broadcast_plmn.tai_slice_support_list[k];
        asn1_tai_slice.sst.from_number(snssai_item.sst.value());

        if (snssai_item.sd.is_set()) {
          asn1_tai_slice.sd_present = true;
          asn1_tai_slice.sd.from_number(snssai_item.sd.value());
        }
      }
    }
  }

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
