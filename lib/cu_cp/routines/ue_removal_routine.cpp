// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_removal_routine.h"

using namespace ocudu;
using namespace ocudu::ocucp;

ue_removal_routine::ue_removal_routine(cu_cp_ue_index_t                         ue_index_,
                                       rrc_ue_handler*                          rrc_du_notifier_,
                                       du_processor_ue_context_removal_handler* du_proc_removal_handler_,
                                       pdcp_ue_context_removal_handler*         pdcp_removal_handler_,
                                       e1ap_bearer_context_removal_handler*     e1ap_removal_handler_,
                                       f1ap_ue_context_removal_handler*         f1ap_removal_handler_,
                                       ngap_ue_context_removal_handler*         ngap_removal_handler_,
                                       nrppa_ue_context_removal_handler*        nrppa_removal_handler_,
                                       xnap_ue_context_removal_handler*         xnap_removal_handler_,
                                       ue_manager&                              ue_mng_,
                                       ocudulog::basic_logger&                  logger_) :
  ue_index(ue_index_),
  rrc_du_notifier(rrc_du_notifier_),
  du_proc_removal_handler(du_proc_removal_handler_),
  pdcp_removal_handler(pdcp_removal_handler_),
  e1ap_removal_handler(e1ap_removal_handler_),
  f1ap_removal_handler(f1ap_removal_handler_),
  ngap_removal_handler(ngap_removal_handler_),
  nrppa_removal_handler(nrppa_removal_handler_),
  xnap_removal_handler(xnap_removal_handler_),
  ue_mng(ue_mng_),
  logger(logger_)
{
}

void ue_removal_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" started...", ue_index, name());

  // Remove RRC UE if it exists.
  if (rrc_du_notifier != nullptr) {
    rrc_du_notifier->remove_ue(ue_index);
  }

  // Remove DU processor per-UE adapter state (F1AP to PDCP and RRC to F1AP routing).
  if (du_proc_removal_handler != nullptr) {
    du_proc_removal_handler->remove_ue_context(ue_index);
  }

  // Remove per-UE SRB PDCP entities.
  if (pdcp_removal_handler != nullptr) {
    pdcp_removal_handler->remove_ue_context(ue_index);
  }

  // Remove Bearer Context from E1AP if it exists.
  if (e1ap_removal_handler != nullptr) {
    e1ap_removal_handler->remove_bearer_context(ue_index);
  }

  // Remove UE Context from F1AP if it exists.
  if (f1ap_removal_handler != nullptr) {
    f1ap_removal_handler->remove_ue_context(ue_index);
  }
  // Remove UE Context from NGAP if it exists.
  if (ngap_removal_handler != nullptr) {
    ngap_removal_handler->remove_ue_context(ue_index);
  }

  // Remove UE Context from NRPPa if it exists.
  if (nrppa_removal_handler != nullptr) {
    nrppa_removal_handler->remove_ue_context(ue_index);
  }

  // Remove UE Context from XNAP if it exists.
  if (xnap_removal_handler != nullptr) {
    xnap_removal_handler->remove_ue_context(ue_index);
  }

  // Remove UE from UE manager.
  ue_mng.remove_ue(ue_index);

  logger.info("ue={}: \"{}\" finished successfully", ue_index, name());

  CORO_RETURN();
}
