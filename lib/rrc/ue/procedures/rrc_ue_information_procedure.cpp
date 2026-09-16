// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rrc_ue_information_procedure.h"
#include "ue/rrc_ue_helpers.h"
#include "ocudu/asn1/rrc_nr/dl_dcch_msg.h"

using namespace ocudu;
using namespace ocudu::ocucp;
using namespace asn1::rrc_nr;

rrc_ue_information_procedure::rrc_ue_information_procedure(rrc_ue_context_t&                           context_,
                                                           rrc_ue_security_mode_command_proc_notifier& rrc_ue_notifier_,
                                                           rrc_ue_context_update_notifier&             cu_cp_notifier_,
                                                           rrc_ue_cu_cp_ue_notifier& cu_cp_ue_notifier_,
                                                           rrc_ue_event_manager&     event_mng_,
                                                           rrc_ue_logger&            logger_) :
  context(context_),
  rrc_ue(rrc_ue_notifier_),
  cu_cp_notifier(cu_cp_notifier_),
  cu_cp_ue_notifier(cu_cp_ue_notifier_),
  event_mng(event_mng_),
  logger(logger_)
{
  procedure_timeout = context.cfg.rrc_procedure_guard_time_ms;
}

void rrc_ue_information_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Checked here rather than where the request was queued: the UE may have moved on in between. TS 38.331
  // sec. 5.7.10.2 allows the request only once AS security is active, and has nothing to say to a UE that left
  // connected mode.
  if (context.state != rrc_state::connected or
      cu_cp_ue_notifier.get_security_context().state != security::security_state::fully_enabled) {
    logger.log_debug("Skipping \"{}\". Cause: the UE is not in RRC connected state with AS security active", name());
    CORO_EARLY_RETURN();
  }

  logger.log_info("\"{}\" started...", name());
  transaction = event_mng.transactions.create_transaction(procedure_timeout);

  send_rrc_ue_information_request();

  CORO_AWAIT(transaction);

  if (not transaction.has_response()) {
    logger.log_warning("\"{}\" timed out after {}ms", name(), procedure_timeout.count());
    CORO_EARLY_RETURN();
  }

  // The UE may answer with any message carrying this transaction id. Accessing the wrong choice branch only logs an
  // ASN.1 error and then reinterprets the union, so the type has to be checked here.
  const ul_dcch_msg_type_c& response = transaction.response().msg;
  if (response.type().value != ul_dcch_msg_type_c::types_opts::msg_class_ext or
      response.msg_class_ext().type().value != ul_dcch_msg_type_c::msg_class_ext_c_::types_opts::c2 or
      response.msg_class_ext().c2().type().value !=
          ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::types_opts::ue_info_resp_r16) {
    logger.log_warning("Received an unexpected message in place of UEInformationResponse");
    CORO_EARLY_RETURN();
  }

  store_coarse_location(response.msg_class_ext().c2().ue_info_resp_r16());

  logger.log_info("\"{}\" finished successfully", name());
  CORO_RETURN();
}

void rrc_ue_information_procedure::send_rrc_ue_information_request()
{
  dl_dcch_msg_s          dl_dcch_msg;
  ue_info_request_r16_s& ue_info_request = dl_dcch_msg.msg.set_c1().set_ue_info_request_r16();
  ue_info_request.rrc_transaction_id     = transaction.id();

  // coarseLocationRequest lives in the v1700 extension, so the extension has to be marked present as well.
  ue_info_request_r16_ies_s& ies                       = ue_info_request.crit_exts.set_ue_info_request_r16();
  ies.non_crit_ext_present                             = true;
  ies.non_crit_ext.coarse_location_request_r17_present = true;

  rrc_ue.on_new_dl_dcch(srb_id_t::srb1, dl_dcch_msg);
}

void rrc_ue_information_procedure::store_coarse_location(const ue_info_resp_r16_s& ue_info_resp)
{
  if (ue_info_resp.crit_exts.type().value != ue_info_resp_r16_s::crit_exts_c_::types_opts::ue_info_resp_r16) {
    logger.log_warning("Unsupported UEInformationResponse critical extension");
    return;
  }

  const ue_info_resp_r16_ies_s& ies = ue_info_resp.crit_exts.ue_info_resp_r16();
  if (not ies.non_crit_ext_present) {
    logger.log_debug("No coarse UE location reported. Cause: the UE did not include the v1700 extension");
    return;
  }

  store_coarse_ue_location(context.coarse_location, ies.non_crit_ext.coarse_location_info_r17, cu_cp_notifier, logger);
}
