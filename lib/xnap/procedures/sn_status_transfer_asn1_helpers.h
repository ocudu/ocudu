// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/xnap/xnap_pdu_contents.h"
#include "ocudu/ran/inter_cu_handover_messages.h"

namespace ocudu::ocucp {

/// \brief Convert the SN Status Transfer ASN.1 struct to the internal representation used by the CU-CP.
/// \param[out] sn_status_transfer The SN Status Transfer struct to fill.
/// \param[in] asn1_sn_status_transfer The SN Status Transfer ASN.1 struct to convert.
/// \return true if the conversion was successful, false otherwise.
inline bool asn1_to_sn_status_transfer(cu_cp_status_transfer&                  sn_status_transfer,
                                       const asn1::xnap::sn_status_transfer_s& asn1_sn_status_transfer)
{
  for (const asn1::xnap::drbs_subject_to_status_transfer_item_s& drb_item_asn1 :
       asn1_sn_status_transfer->drbs_subject_to_status_transfer_list) {
    cu_cp_drbs_subject_to_status_transfer_item drb_item;

    drb_item.drb_id = uint_to_drb_id(drb_item_asn1.drb_id);
    // Fill DL status
    if (drb_item_asn1.pdcp_status_transfer_dl.type() ==
        asn1::xnap::drb_b_status_transfer_choice_c::types_opts::pdcp_sn_12bits) {
      drb_item.drb_status_dl.sn_size     = pdcp_sn_size::size12bits;
      drb_item.drb_status_dl.dl_count.sn = drb_item_asn1.pdcp_status_transfer_dl.pdcp_sn_12bits().count_value.pdcp_sn12;
      drb_item.drb_status_dl.dl_count.hfn =
          drb_item_asn1.pdcp_status_transfer_dl.pdcp_sn_12bits().count_value.hfn_pdcp_sn12;
    } else if (drb_item_asn1.pdcp_status_transfer_dl.type() ==
               asn1::xnap::drb_b_status_transfer_choice_c::types_opts::pdcp_sn_18bits) {
      drb_item.drb_status_dl.sn_size     = pdcp_sn_size::size18bits;
      drb_item.drb_status_dl.dl_count.sn = drb_item_asn1.pdcp_status_transfer_dl.pdcp_sn_18bits().count_value.pdcp_sn18;
      drb_item.drb_status_dl.dl_count.hfn =
          drb_item_asn1.pdcp_status_transfer_dl.pdcp_sn_18bits().count_value.hfn_pdcp_sn18;
    } else {
      return false;
    }
    // Fill UL status.
    if (drb_item_asn1.pdcp_status_transfer_ul.type() ==
        asn1::xnap::drb_b_status_transfer_choice_c::types_opts::pdcp_sn_12bits) {
      drb_item.drb_status_ul.sn_size     = pdcp_sn_size::size12bits;
      drb_item.drb_status_ul.ul_count.sn = drb_item_asn1.pdcp_status_transfer_ul.pdcp_sn_12bits().count_value.pdcp_sn12;
      drb_item.drb_status_ul.ul_count.hfn =
          drb_item_asn1.pdcp_status_transfer_ul.pdcp_sn_12bits().count_value.hfn_pdcp_sn12;
    } else if (drb_item_asn1.pdcp_status_transfer_ul.type() ==
               asn1::xnap::drb_b_status_transfer_choice_c::types_opts::pdcp_sn_18bits) {
      drb_item.drb_status_ul.sn_size     = pdcp_sn_size::size18bits;
      drb_item.drb_status_ul.ul_count.sn = drb_item_asn1.pdcp_status_transfer_ul.pdcp_sn_18bits().count_value.pdcp_sn18;
      drb_item.drb_status_ul.ul_count.hfn =
          drb_item_asn1.pdcp_status_transfer_ul.pdcp_sn_18bits().count_value.hfn_pdcp_sn18;
    } else {
      return false;
    }
    sn_status_transfer.drbs_subject_to_status_transfer_list.emplace(drb_item.drb_id, drb_item);
  }
  return true;
}

/// \brief Convert the SN Status Transfer struct to ASN.1.
/// \param[out] asn1_msg The SN Status Transfer ASN1 struct to fill.
/// \param[in] drb_list The list of DRB status transfer information.
inline void
sn_status_transfer_to_asn1(asn1::xnap::sn_status_transfer_s&                                              asn1_msg,
                           const slotted_id_vector<drb_id_t, cu_cp_drbs_subject_to_status_transfer_item>& drb_list)
{
  asn1::xnap::drbs_subject_to_status_transfer_list_l& asn1_drb_list = asn1_msg->drbs_subject_to_status_transfer_list;
  for (const cu_cp_drbs_subject_to_status_transfer_item& drb : drb_list) {
    asn1::xnap::drbs_subject_to_status_transfer_item_s asn1_drb_item = {};
    asn1_drb_item.drb_id                                             = to_underlying(drb.drb_id);
    if (drb.drb_status_ul.sn_size == pdcp_sn_size::size12bits) {
      asn1_drb_item.pdcp_status_transfer_ul.set_pdcp_sn_12bits();
      asn1_drb_item.pdcp_status_transfer_ul.pdcp_sn_12bits().count_value.hfn_pdcp_sn12 = drb.drb_status_ul.ul_count.hfn;
      asn1_drb_item.pdcp_status_transfer_ul.pdcp_sn_12bits().count_value.pdcp_sn12     = drb.drb_status_ul.ul_count.sn;
    } else {
      asn1_drb_item.pdcp_status_transfer_ul.set_pdcp_sn_18bits();
      asn1_drb_item.pdcp_status_transfer_ul.pdcp_sn_18bits().count_value.hfn_pdcp_sn18 = drb.drb_status_ul.ul_count.hfn;
      asn1_drb_item.pdcp_status_transfer_ul.pdcp_sn_18bits().count_value.pdcp_sn18     = drb.drb_status_ul.ul_count.sn;
    }
    if (drb.drb_status_dl.sn_size == pdcp_sn_size::size12bits) {
      asn1_drb_item.pdcp_status_transfer_dl.set_pdcp_sn_12bits();
      asn1_drb_item.pdcp_status_transfer_dl.pdcp_sn_12bits().count_value.hfn_pdcp_sn12 = drb.drb_status_dl.dl_count.hfn;
      asn1_drb_item.pdcp_status_transfer_dl.pdcp_sn_12bits().count_value.pdcp_sn12     = drb.drb_status_dl.dl_count.sn;
    } else {
      asn1_drb_item.pdcp_status_transfer_dl.set_pdcp_sn_18bits();
      asn1_drb_item.pdcp_status_transfer_dl.pdcp_sn_18bits().count_value.hfn_pdcp_sn18 = drb.drb_status_dl.dl_count.hfn;
      asn1_drb_item.pdcp_status_transfer_dl.pdcp_sn_18bits().count_value.pdcp_sn18     = drb.drb_status_dl.dl_count.sn;
    }
    asn1_drb_list.push_back(asn1_drb_item);
  }
}

} // namespace ocudu::ocucp
