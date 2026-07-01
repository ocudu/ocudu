// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ng_reset_procedure.h"
#include "ngap_asn1_converters.h"
#include "ocudu/asn1/ngap/common.h"
#include "ocudu/ngap/ngap_message.h"

using namespace ocudu;
using namespace ocudu::ocucp;
using namespace asn1::ngap;

ng_reset_procedure::ng_reset_procedure(ngap_context_t&           context_,
                                       const cu_cp_reset&        msg_,
                                       ngap_message_notifier&    amf_notif_,
                                       ngap_transaction_manager& ev_mng_,
                                       ngap_ue_context_list&     ue_ctxt_list_,
                                       ocudulog::basic_logger&   logger_) :
  context(context_), msg(msg_), amf_notifier(amf_notif_), ev_mng(ev_mng_), ue_ctxt_list(ue_ctxt_list_), logger(logger_)
{
}

void ng_reset_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("\"{}\" started...", name());

  // Subscribe to respective publisher to receive NG RESET ACKNOWLEDGE message.
  transaction_sink.subscribe_to(ev_mng.ng_reset_outcome, context.procedure_timeout);

  // Forward message to AMF.
  if (!send_ng_reset()) {
    logger.info("AMF notifier is not set. Cannot send NGReset");
    CORO_EARLY_RETURN();
  }

  // Await AMF response.
  CORO_AWAIT(transaction_sink);

  // TODO: Handle NG RESET ACKNOWLEDGE message.

  logger.info("\"{}\" finished successfully", name());

  CORO_RETURN();
}

bool ng_reset_procedure::send_ng_reset()
{
  ngap_message ngap_msg = {};
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_NG_RESET);
  asn1::ngap::ng_reset_s& ng_reset = ngap_msg.pdu.init_msg().value.ng_reset();
  ng_reset->cause                  = cause_to_asn1(std::get<ngap_cause_t>(msg.cause));
  if (msg.interface_reset) {
    // Reset all UEs.
    ng_reset->reset_type.set_ng_interface() = asn1::ngap::reset_all_opts::options::reset_all;
  } else {
    // Reset only specific UEs.
    ue_associated_lc_ng_conn_list_l& reset_part_of_ng_interface = ng_reset->reset_type.set_part_of_ng_interface();
    for (const auto& ue : msg.ues_to_reset) {
      if (!ue_ctxt_list.contains(ue)) {
        logger.debug("ue={}: Excluding UE from NG Reset. UE context does not exist", ue);
      } else {
        auto& ue_ctxt = ue_ctxt_list[ue];

        ue_associated_lc_ng_conn_item_s conn_item;

        if (ue_ctxt.ue_ids.amf_ue_id != amf_ue_id_t::invalid) {
          conn_item.amf_ue_ngap_id_present = true;
          conn_item.amf_ue_ngap_id         = amf_ue_id_to_uint(ue_ctxt.ue_ids.amf_ue_id);
        }
        if (ue_ctxt.ue_ids.ran_ue_id != ran_ue_id_t::invalid) {
          conn_item.ran_ue_ngap_id_present = true;
          conn_item.ran_ue_ngap_id         = ran_ue_id_to_uint(ue_ctxt.ue_ids.ran_ue_id);
        }

        reset_part_of_ng_interface.push_back(conn_item);
      }
    }
  }

  // Send the message to CU-UP.
  return amf_notifier.on_new_message(ngap_msg);
}
