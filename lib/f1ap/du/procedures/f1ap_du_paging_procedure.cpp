// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_paging_procedure.h"
#include "../../asn1_helpers.h"
#include "../../f1ap_asn1_utils.h"
#include "../f1ap_du_context.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/f1ap_ies.h"
#include "ocudu/f1ap/du/f1ap_du.h"
#include "ocudu/ran/paging_information.h"
#include "ocudu/ran/pcch/paging_helper.h"

using namespace ocudu;
using namespace odu;
using namespace asn1::f1ap;

bool odu::handle_paging_request(const paging_s&          msg,
                                f1ap_du_paging_notifier& paging_notifier,
                                f1ap_du_context&         ctxt,
                                ocudulog::basic_logger&  logger)
{
  paging_information info{};

  // Extract UE Identity.
  if (auto id = get_paging_ue_identity_index_value(msg); id.has_value()) {
    info.ue_identity = id.value();
  } else {
    logger.error("Discarding Paging message. Cause: Paging UE Identity index type {} is not supported",
                 msg->ue_id_idx_value.type().to_string());
    return false;
  }

  // Extract Paging Identity Type and Paging Identity.
  if (auto id = get_paging_identity_type(msg); id.has_value()) {
    info.paging_type_indicator = id.value();
    info.paging_identity       = *get_paging_identity(msg);
  } else {
    logger.error("ue_id={}: Discarding Paging message. Cause: Paging Identity type {} is not supported",
                 info.ue_identity,
                 msg->paging_id.type().to_string());
    return false;
  }

  // Extract paging DRX cycle.
  if (msg->paging_drx_present) {
    if (auto ret = get_paging_drx_in_nof_rf(msg); ret.has_value()) {
      info.paging_drx = radio_frames{ret.value()};
    } else {
      logger.error("ue_id={}: Discarding Paging message. Cause: DRX value {} is not supported",
                   info.ue_identity,
                   msg->paging_drx.to_string());
      return false;
    }
  }

  // Extract paging priority.
  if (msg->paging_prio_present) {
    expected<unsigned> paging_priority = get_paging_priority(msg);
    if (not paging_priority.has_value()) {
      logger.error(
          "ue_id={}: Paging priority value {} is not supported", info.ue_identity, msg->paging_prio.to_string());
    } else {
      info.paging_priority = paging_priority.value();
    }
  }
  if (msg->paging_origin_present and msg->paging_origin.value == paging_origin_e::non_neg3gpp) {
    info.is_paging_origin_non_3gpp_access = true;
  }

  for (const auto& asn_nr_cgi : msg->paging_cell_list) {
    const auto ret = cgi_from_asn1(asn_nr_cgi->paging_cell_item().nr_cgi);
    if (not ret.has_value()) {
      logger.error("ue_id={}: Discarding Paging message. Cause: Invalid CGI in paging cell list", info.ue_identity);
      continue;
    }
    auto                        paging_cell_cgi = ret.value();
    const f1ap_du_cell_context* cell_same_cgi   = ctxt.find_cell(paging_cell_cgi);
    if (cell_same_cgi == nullptr) {
      // Cell not served by this DU.
      logger.error("Cell with PLMN={} and NCI={} not handled by DU", paging_cell_cgi.plmn_id, paging_cell_cgi.nci);
      continue;
    }
    info.paging_cells.push_back(cell_same_cgi->cell_index);
  }

  // Extract eDRX information if present.
  if (msg->nr_paginge_drx_info_present) {
    // eDRX info enabled.
    if (msg->paging_id.type().value != paging_id_c::types_opts::cn_ue_paging_id) {
      logger.error("ue_id={}: Discarding Paging message. Cause: RAN UE Paging ID not supported when eDRX is enabled",
                   info.ue_identity);
      return false;
    }
    auto& edrx = info.edrx.emplace();

    // Convert number of HSFNs to radio frames.
    edrx.cycle_idle = radio_frames{
        static_cast<int64_t>(std::round(msg->nr_paginge_drx_info.nrpaging_e_drx_cycle_idle.to_number() * NOF_SFNS))};

    if (msg->nr_paginge_drx_info.nrpaging_time_win_present) {
      // The PTW length is given in multiples of 1.28 seconds.
      auto ptw     = msg->nr_paginge_drx_info.nrpaging_time_win.to_number() * std::chrono::milliseconds{1280};
      edrx.ptw_len = std::chrono::duration_cast<radio_frames>(ptw);
    }

    // Compute UE_ID and UE_ID_H as per TS 38.304, 7.1 and 7.4.
    info.ue_identity              = info.paging_identity % 4096;
    info.edrx->hashed_ue_identity = paging_helper::compute_UE_ID_H(info.paging_identity);
  }

  // Forward paging information to lower layers.
  paging_notifier.on_paging_received(info);

  return true;
}
