// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rrc_ue_capability_transfer_procedure.h"
#include "ue/rrc_ue_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/rrc_nr/dl_dcch_msg.h"

using namespace ocudu;
using namespace ocudu::ocucp;
using namespace asn1::rrc_nr;

rrc_ue_capability_transfer_procedure::rrc_ue_capability_transfer_procedure(
    rrc_ue_context_t&                           context_,
    rrc_ue_security_mode_command_proc_notifier& rrc_ue_notifier_,
    rrc_ue_event_manager&                       event_mng_,
    rrc_ue_logger&                              logger_) :
  context(context_), rrc_ue(rrc_ue_notifier_), event_mng(event_mng_), logger(logger_)
{
  procedure_timeout = context.cfg.rrc_procedure_guard_time_ms;
}

void rrc_ue_capability_transfer_procedure::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  if (context.capabilities_list.has_value()) {
    logger.log_debug("\"{}\" skipped. Capabilities already present", name());
    CORO_EARLY_RETURN(true);
  }

  logger.log_info("\"{}\" started...", name());
  // Create new transaction for RRCUeCapabilityEnquiry.
  transaction = event_mng.transactions.create_transaction(procedure_timeout);

  // Send RRC UE Capability Enquiry to UE.
  send_rrc_ue_capability_enquiry();

  // Await UE response.
  CORO_AWAIT(transaction);

  if (transaction.has_response()) {
    const ul_dcch_msg_type_c& response = transaction.response().msg;
    if (response.type().value != ul_dcch_msg_type_c::types_opts::c1 or
        response.c1().type().value != ul_dcch_msg_type_c::c1_c_::types_opts::ue_cap_info) {
      logger.log_warning("Received an unexpected message in place of UECapabilityInformation");
      CORO_EARLY_RETURN(false);
    }

    if (response.c1().ue_cap_info().crit_exts.ue_cap_info().ue_cap_rat_container_list_present) {
      const auto& list         = response.c1().ue_cap_info().crit_exts.ue_cap_info().ue_cap_rat_container_list;
      auto        capabilities = get_capabilities(list, logger);
      if (not capabilities.has_value()) {
        logger.log_warning("UE capability transfer failed: invalid NR capabilities");
        CORO_EARLY_RETURN(false);
      }
      context.capabilities_list = list;
      context.capabilities      = capabilities.value();
    }
    procedure_result = context.capabilities_list.has_value();
  } else {
    if (transaction.failure_cause() == protocol_transaction_failure::timeout) {
      logger.log_warning("\"{}\" timed out after {}ms", name(), procedure_timeout.count());
    } else {
      logger.log_info("\"{}\" cancelled", name());
    }
    CORO_EARLY_RETURN(false);
  }

  logger.log_info("\"{}\" finished successfully", name());
  CORO_RETURN(procedure_result);
}

void rrc_ue_capability_transfer_procedure::send_rrc_ue_capability_enquiry()
{
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_ue_cap_enquiry();
  ue_cap_enquiry_s& rrc_ue_cap_enquiry = dl_dcch_msg.msg.c1().set_ue_cap_enquiry();
  fill_asn1_rrc_ue_capability_enquiry(rrc_ue_cap_enquiry, transaction.id(), context.cell.bands);
  rrc_ue.on_new_dl_dcch(srb_id_t::srb1, dl_dcch_msg);
}
