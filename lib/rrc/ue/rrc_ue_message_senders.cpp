// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rrc_asn1_helpers.h"
#include "rrc_ue_helpers.h"
#include "rrc_ue_impl.h"
#include "ocudu/asn1/rrc_nr/dl_ccch_msg.h"
#include "ocudu/asn1/rrc_nr/dl_dcch_msg.h"

using namespace ocudu;
using namespace ocucp;
using namespace asn1::rrc_nr;

void rrc_ue_impl::send_dl_ccch(const dl_ccch_msg_s& dl_ccch_msg)
{
  // Pack DL CCCH msg.
  byte_buffer pdu = pack_into_pdu(dl_ccch_msg, "DL-CCCH-Message");

  // Log Tx message
  log_rrc_message(logger, Tx, pdu, dl_ccch_msg, srb_id_t::srb0, "CCCH DL");

  // Send down the stack.
  logger.log_debug(pdu.begin(), pdu.end(), "Tx {} PDU", srb_id_t::srb0);
  f1ap_pdu_notifier.on_new_rrc_pdu(srb_id_t::srb0, std::move(pdu));
}

void rrc_ue_impl::send_dl_dcch(srb_id_t srb_id, const dl_dcch_msg_s& dl_dcch_msg)
{
  if (!context.pdcp_notifier->has_srb(srb_id)) {
    logger.log_error("Dropping DlDcchMessage. Tx {} is not set up", srb_id);
    return;
  }

  // Pack DL-DCCH message.
  byte_buffer pdu = pack_into_pdu(dl_dcch_msg, "DL-DCCH-Message");

  // Log Tx message.
  log_rrc_message(logger, Tx, pdu, dl_dcch_msg, srb_id, "DCCH DL");

  // Encrypt via PDCP and send down to F1AP.
  pdcp_tx_result encrypt_result = context.pdcp_notifier->encrypt_pdu(srb_id, std::move(pdu));
  if (!encrypt_result.is_successful()) {
    logger.log_info("Requesting UE release. Cause: PDCP packing failed with {}", encrypt_result.get_failure_cause());
    on_ue_release_required(encrypt_result.get_failure_cause());
    return;
  }

  byte_buffer pdcp_pdu = encrypt_result.pop_pdu();
  logger.log_debug(pdcp_pdu.begin(), pdcp_pdu.end(), "Tx {} PDU", context.ue_index, context.c_rnti, srb_id);
  f1ap_pdu_notifier.on_new_rrc_pdu(srb_id, std::move(pdcp_pdu));
}
