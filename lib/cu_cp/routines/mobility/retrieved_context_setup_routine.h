// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../ue_manager/ue_manager_impl.h"
#include "../../xnap_repository.h"
#include "ocudu/cu_cp/ue_configuration.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp_bearer_context_update.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

/// \brief Establishes the bearers of a UE whose context was retrieved from a peer NG-RAN node, and moves the user
/// plane over to this node (TS 38.300 section 9.2.3.3, steps after the RRCReestablishmentComplete).
///
/// The peer holds the UE's CU-UP context, so the bearers are established at the CU-UP from the retrieved PDU session
/// list and added to the UE context the DU created for the random access.
///
/// Once the bearers are up, the AMF is asked to switch the DL path to this node, and the UE context at the peer is
/// released, so the UE keeps its PDU sessions throughout.
///
/// \remark PDCP SDUs still in flight at the peer are lost.
class retrieved_context_setup_routine
{
public:
  retrieved_context_setup_routine(cu_cp_ue&                    ue_,
                                  e1ap_bearer_context_manager& e1ap_bearer_ctxt_mng_,
                                  f1ap_ue_context_manager&     f1ap_ue_ctxt_mng_,
                                  ngap_interface&              ngap_,
                                  xnap_interface*              xnap_,
                                  const ue_configuration&      ue_cfg_,
                                  const security_indication_t& default_security_indication_,
                                  ocudulog::basic_logger&      logger_);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "Retrieved UE Context Setup Routine"; }

private:
  /// Adds SRB2 and the retrieved DRBs to the UE context the DU already holds for this UE.
  bool fill_ue_context_modification_request();
  bool fill_rrc_reconfiguration_args();

  cu_cp_ue&                    ue;
  e1ap_bearer_context_manager& e1ap_bearer_ctxt_mng;
  f1ap_ue_context_manager&     f1ap_ue_ctxt_mng;
  ngap_interface&              ngap;
  xnap_interface*              xnap;
  const ue_configuration&      ue_cfg;
  const security_indication_t& default_security_indication;
  ocudulog::basic_logger&      logger;

  const cu_cp_ue_index_t ue_index;

  up_config_update next_config;

  e1ap_bearer_context_setup_request         bearer_context_setup_request;
  e1ap_bearer_context_setup_response        bearer_context_setup_response;
  f1ap_ue_context_modification_request      ue_context_mod_request;
  f1ap_ue_context_modification_response     ue_context_mod_response;
  e1ap_bearer_context_modification_request  bearer_context_mod_request;
  e1ap_bearer_context_modification_response bearer_context_mod_response;
  rrc_reconfiguration_procedure_request     rrc_reconfig_args;
  bool                                      rrc_reconfig_result = false;
  bool                                      path_switch_success = false;
};

} // namespace ocudu::ocucp
