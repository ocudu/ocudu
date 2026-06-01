// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cause/ngap_cause.h"
#include "ocudu/ran/crit_diagnostics.h"
#include "ocudu/ran/cu_cp_paging.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include <chrono>

namespace ocudu::ocucp {

/// NR carrier target for RRC Release with redirection (TS 38.331 Sec. 5.3.8.3).
struct cu_cp_release_redirect_nr_info {
  uint32_t           arfcn;                               ///< Target NR downlink ARFCN
  subcarrier_spacing ssb_scs = subcarrier_spacing::kHz15; ///< SSB subcarrier spacing
};

/// Command sent from the CU-CP to the lower layers to release the UE context.
struct cu_cp_ue_context_release_command {
  cu_cp_ue_index_t ue_index = cu_cp_ue_index_t::invalid;
  ngap_cause_t     cause;
  // If true, the lower layers will send an RRC message e.g. RRCReject or RRCRelease to the UE as part of the context
  // release procedure.
  bool                                          requires_rrc_message = true;
  std::optional<std::chrono::seconds>           release_wait_time    = std::nullopt;
  std::optional<cu_cp_release_redirect_nr_info> redirect_nr_info     = std::nullopt;
};

/// Request sent from the lower layers to the CU-CP to request the release of an UE context.
struct cu_cp_ue_context_release_request {
  cu_cp_ue_index_t                              ue_index = cu_cp_ue_index_t::invalid;
  std::vector<pdu_session_id_t>                 pdu_session_res_list_cxt_rel_req;
  ngap_cause_t                                  cause;
  std::optional<cu_cp_release_redirect_nr_info> redirect_nr_info = std::nullopt;
};

struct cu_cp_ue_context_release_complete {
  cu_cp_ue_index_t                           ue_index = cu_cp_ue_index_t::invalid;
  std::optional<cu_cp_user_location_info_nr> user_location_info;
  std::optional<cu_cp_info_on_recommended_cells_and_ran_nodes_for_paging>
                                    info_on_recommended_cells_and_ran_nodes_for_paging;
  std::vector<pdu_session_id_t>     pdu_session_res_list_cxt_rel_cpl;
  std::optional<crit_diagnostics_t> crit_diagnostics;
};

} // namespace ocudu::ocucp
