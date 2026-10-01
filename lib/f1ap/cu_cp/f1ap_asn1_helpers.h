// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/ran/five_g_s_tmsi.h"
#include "ocudu/ran/i_rnti.h"
#include <variant>

namespace ocudu::ocucp {

/// \brief Convert the \c cu_cp_paging_message type to ASN.1.
/// \param[out] asn1_paging The ASN.1 type struct to store the result.
/// \param[in] paging The common type Paging message.
inline void fill_asn1_paging_message(asn1::f1ap::paging_s& asn1_paging, const cu_cp_paging_message& paging)
{
  // Add ue id idx value.
  asn1_paging->ue_id_idx_value.set_idx_len10().from_number(paging.ue_id_idx_value);

  // Add paging id.
  if (std::holds_alternative<five_g_s_tmsi_t>(paging.ue_paging_id)) {
    five_g_s_tmsi_t five_g_s_tmsi = std::get<five_g_s_tmsi_t>(paging.ue_paging_id);
    asn1_paging->paging_id.set_cn_ue_paging_id().set_five_g_s_tmsi().from_number(five_g_s_tmsi.to_number());
  } else {
    asn1_paging->paging_id.set_ran_ue_paging_id().irnti.from_number(
        std::get<full_i_rnti_t>(paging.ue_paging_id).value());
  }

  // Add paging drx
  if (paging.paging_drx.has_value()) {
    asn1_paging->paging_drx_present = true;
    asn1::number_to_enum(asn1_paging->paging_drx, paging.paging_drx.value());
  }

  // Add paging prio
  if (paging.paging_prio.has_value()) {
    asn1_paging->paging_prio_present = true;
    asn1::number_to_enum(asn1_paging->paging_prio, paging.paging_prio.value());
  }

  // Add paging cell list
  for (const auto& cell_item : paging.assist_data_for_paging.value()
                                   .assist_data_for_recommended_cells.value()
                                   .recommended_cells_for_paging.recommended_cell_list) {
    asn1::protocol_ie_single_container_s<asn1::f1ap::paging_cell_item_ies_o> asn1_paging_cell_item_container;
    auto& asn1_paging_cell_item = asn1_paging_cell_item_container->paging_cell_item();

    asn1_paging_cell_item.nr_cgi.nr_cell_id.from_number(cell_item.ngran_cgi.nci.value());
    asn1_paging_cell_item.nr_cgi.plmn_id = cell_item.ngran_cgi.plmn_id.to_bytes();

    asn1_paging->paging_cell_list.push_back(asn1_paging_cell_item_container);
  }

  // Add paging origin
  if (paging.paging_origin.has_value()) {
    asn1_paging->paging_origin_present = true;
    asn1::bool_to_enum(asn1_paging->paging_origin, paging.paging_origin.value());
  }

  // Add paging eDRX information.
  if (paging.paging_edrx_info.has_value()) {
    asn1_paging->nr_paginge_drx_info_present = true;
    asn1::float_number_to_enum(asn1_paging->nr_paginge_drx_info.nrpaging_e_drx_cycle_idle,
                               paging.paging_edrx_info->nr_paging_edrx_cycle,
                               0.001f);
    if (paging.paging_edrx_info->nr_paging_time_window.has_value()) {
      asn1_paging->nr_paginge_drx_info.nrpaging_time_win_present = true;
      asn1::number_to_enum(asn1_paging->nr_paginge_drx_info.nrpaging_time_win,
                           *paging.paging_edrx_info->nr_paging_time_window);
    }
  }
}

} // namespace ocudu::ocucp
