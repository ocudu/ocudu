// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rrc_ue_capability_transfer_procedure.h"
#include "ue/rrc_ue_helpers.h"
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
    if (transaction.response().msg.c1().ue_cap_info().crit_exts.ue_cap_info().ue_cap_rat_container_list_present) {
      context.capabilities_list.emplace(
          transaction.response().msg.c1().ue_cap_info().crit_exts.ue_cap_info().ue_cap_rat_container_list);
      for (const auto& ue_cap_rat_container :
           transaction.response().msg.c1().ue_cap_info().crit_exts.ue_cap_info().ue_cap_rat_container_list) {
        if (ue_cap_rat_container.rat_type.value == asn1::rrc_nr::rat_type_e::nr) {
          asn1::cbit_ref            bref{ue_cap_rat_container.ue_cap_rat_container.copy()};
          asn1::rrc_nr::ue_nr_cap_s ue_nr_cap;
          if (ue_nr_cap.unpack(bref) != asn1::OCUDUASN_SUCCESS) {
            logger.log_error("Error unpacking UE Capabilities");
            continue;
          }

          if (logger.get_basic_logger().debug.enabled()) {
            asn1::json_writer json_writer;
            ue_nr_cap.to_json(json_writer);
            logger.log_debug("UE Capabilities:\n{}", json_writer.to_string().c_str());
          }

          // Store unpacked capabilities.
          context.capabilities = get_capabilities(ue_nr_cap, logger);

        } else {
          logger.log_warning("Unsupported RAT type {}", fmt::underlying(ue_cap_rat_container.rat_type.value));
        }
      }
    }
    procedure_result = context.capabilities_list.has_value();
  } else {
    logger.log_warning("\"{}\" timed out after {}ms", name(), procedure_timeout.count());
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
