// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../du_processor/du_processor.h"
#include "../../ue_manager/ue_manager_impl.h"
#include "ocudu/cu_cp/ue_configuration.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp.h"
#include "ocudu/rrc/rrc_resume.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

/// \brief Establishes the bearers of a UE that resumed at this node with a context retrieved from a peer NG-RAN node
/// (TS 38.300 section 9.2.2.4.1, step 4).
///
/// The peer holds the UE's CU-UP context and the UE reached this node through a random access, so the bearers are
/// established at both the CU-UP and the DU from the retrieved PDU session list.
///
/// A UE resumes its DRBs upon receiving the RRCResume (TS 38.331 section 5.3.13.4), so this runs before Msg4 and
/// returns the radio bearer configuration the RRC puts into it. The DL path is moved once the UE confirms the
/// RRCResume.
///
/// \remark PDCP SDUs still in flight at the peer are lost.
class retrieved_context_resume_routine
{
public:
  retrieved_context_resume_routine(const rrc_resume_request&    request_,
                                   cu_cp_ue&                    ue_,
                                   e1ap_bearer_context_manager& e1ap_bearer_ctxt_mng_,
                                   du_processor&                du_proc_,
                                   const ue_configuration&      ue_cfg_,
                                   const security_indication_t& default_security_indication_,
                                   ocudulog::basic_logger&      logger_);

  void operator()(coro_context<async_task<rrc_resume_request_response>>& ctx);

  static const char* name() { return "Retrieved UE Context Resume Routine"; }

private:
  /// Sets up SRB1, SRB2 and the retrieved DRBs at the DU, which holds no context for this UE beyond the random access.
  bool fill_ue_context_setup_request();
  /// Fills the radio bearer configuration the RRC sends to the UE in the RRCResume.
  bool fill_rrc_resume_response();

  const rrc_resume_request     request;
  cu_cp_ue&                    ue;
  e1ap_bearer_context_manager& e1ap_bearer_ctxt_mng;
  du_processor&                du_proc;
  const ue_configuration&      ue_cfg;
  const security_indication_t& default_security_indication;
  ocudulog::basic_logger&      logger;

  const cu_cp_ue_index_t ue_index;

  up_config_update next_config;

  e1ap_bearer_context_setup_request         bearer_context_setup_request;
  e1ap_bearer_context_setup_response        bearer_context_setup_response;
  f1ap_ue_context_setup_request             ue_context_setup_request;
  f1ap_ue_context_setup_response            ue_context_setup_response;
  e1ap_bearer_context_modification_request  bearer_context_mod_request;
  e1ap_bearer_context_modification_response bearer_context_mod_response;
  rrc_resume_request_response               response_msg;
};

} // namespace ocudu::ocucp
