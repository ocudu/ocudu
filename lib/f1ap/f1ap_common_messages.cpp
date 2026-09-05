// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_common_messages.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/support/enum_utils.h"

using namespace ocudu;
using namespace asn1::f1ap;

f1ap_message ocudu::generate_error_indication(uint8_t                                   transaction_id,
                                              const std::optional<gnb_du_ue_f1ap_id_t>& du_ue_id,
                                              const std::optional<gnb_cu_ue_f1ap_id_t>& cu_ue_id,
                                              const std::optional<asn1::f1ap::cause_c>& cause)
{
  f1ap_message msg;
  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_ERROR_IND);
  asn1::f1ap::error_ind_s error_ind = msg.pdu.init_msg().value.error_ind();

  error_ind->transaction_id = transaction_id;

  error_ind->gnb_du_ue_f1ap_id_present = du_ue_id.has_value();
  if (du_ue_id.has_value()) {
    error_ind->gnb_du_ue_f1ap_id = to_underlying(*du_ue_id);
  }

  error_ind->gnb_cu_ue_f1ap_id_present = cu_ue_id.has_value();
  if (cu_ue_id.has_value()) {
    error_ind->gnb_cu_ue_f1ap_id = to_underlying(*cu_ue_id);
  }

  error_ind->cause_present = cause.has_value();
  if (error_ind->cause_present) {
    error_ind->cause = *cause;
  }

  return msg;
}
