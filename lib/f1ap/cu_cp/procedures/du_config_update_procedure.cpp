// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_config_update_procedure.h"
#include "../f1ap_asn1_converters.h"
#include "asn1_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/f1ap/cu_cp/f1ap_du_context.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/ran/cause/f1ap_cause.h"

using namespace ocudu;
using namespace ocucp;

du_config_update_request ocudu::ocucp::create_du_config_update_request(const asn1::f1ap::gnb_du_cfg_upd_s& asn1_request,
                                                                       const f1ap_du_context&              du_ctxt,
                                                                       ocudulog::basic_logger&             logger)
{
  du_config_update_request request;
  request.gnb_du_id =
      asn1_request->gnb_du_id_present ? static_cast<gnb_du_id_t>(asn1_request->gnb_du_id) : du_ctxt.gnb_du_id;

  if (asn1_request->served_cells_to_add_list_present) {
    for (const auto& asn1_item : asn1_request->served_cells_to_add_list) {
      const auto&                               asn1_cell = asn1_item.value().served_cells_to_add_item();
      std::optional<cu_cp_du_served_cells_item> cell      = f1ap_asn1_to_du_served_cell(
          asn1_cell.served_cell_info, asn1_cell.gnb_du_sys_info_present ? &asn1_cell.gnb_du_sys_info : nullptr);
      if (cell.has_value()) {
        request.served_cells_to_add.push_back(std::move(cell.value()));
      } else {
        logger.warning("Not adding a served cell of the gNB-DU Configuration Update. Cause: The CU-CP cannot read "
                       "its ASN.1 information");
      }
    }
  }

  if (asn1_request->served_cells_to_modify_list_present) {
    for (const auto& asn1_item : asn1_request->served_cells_to_modify_list) {
      const auto&                   asn1_cell = asn1_item.value().served_cells_to_modify_item();
      expected<nr_cell_global_id_t> old_cgi   = cgi_from_asn1(asn1_cell.old_nr_cgi);
      if (not old_cgi.has_value()) {
        logger.warning("Not modifying a served cell of the gNB-DU Configuration Update. Cause: Invalid old NR CGI");
        continue;
      }
      std::optional<cu_cp_du_served_cells_item> cell = f1ap_asn1_to_du_served_cell(
          asn1_cell.served_cell_info, asn1_cell.gnb_du_sys_info_present ? &asn1_cell.gnb_du_sys_info : nullptr);
      if (cell.has_value()) {
        request.served_cells_to_mod.push_back({old_cgi.value(), std::move(cell.value())});
      } else {
        logger.warning("Not modifying cell nci={}. Cause: The CU-CP cannot read its ASN.1 information",
                       old_cgi.value().nci);
      }
    }
  }

  if (asn1_request->served_cells_to_delete_list_present) {
    for (const auto& asn1_item : asn1_request->served_cells_to_delete_list) {
      expected<nr_cell_global_id_t> cgi = cgi_from_asn1(asn1_item.value().served_cells_to_delete_item().old_nr_cgi);
      if (cgi.has_value()) {
        request.served_cells_to_rem.push_back(cgi.value());
      } else {
        logger.warning("Not deleting a served cell of the gNB-DU Configuration Update. Cause: Invalid old NR CGI");
      }
    }
  }

  return request;
}

/// Creates the gNB-DU Configuration Update Acknowledge, as per TS 38.473, Section 9.2.1.8.
static f1ap_message create_du_config_update_acknowledge(const asn1::f1ap::gnb_du_cfg_upd_s&      request,
                                                        const du_config_update_result::accepted& cu_response)
{
  f1ap_message f1ap_msg;

  f1ap_msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_GNB_DU_CFG_UPD);
  auto& ack = f1ap_msg.pdu.successful_outcome().value.gnb_du_cfg_upd_ack();

  ack->transaction_id = request->transaction_id;

  if (not cu_response.cells_to_be_activ_list.empty()) {
    ack->cells_to_be_activ_list_present = true;
    for (const auto& du_cell : cu_response.cells_to_be_activ_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_activ_list_item_ies_o> ack_cell;
      ack_cell->cells_to_be_activ_list_item().nr_cgi.plmn_id = du_cell.nr_cgi.plmn_id.to_bytes();
      ack_cell->cells_to_be_activ_list_item().nr_cgi.nr_cell_id.from_number(du_cell.nr_cgi.nci.value());

      if (du_cell.nr_pci.has_value()) {
        ack_cell->cells_to_be_activ_list_item().nr_pci_present = true;
        ack_cell->cells_to_be_activ_list_item().nr_pci         = du_cell.nr_pci.value();
      }

      ack->cells_to_be_activ_list.push_back(ack_cell);
    }
  }

  if (not cu_response.cells_to_be_deactiv_list.empty()) {
    ack->cells_to_be_deactiv_list_present = true;
    for (const nr_cell_global_id_t& cgi : cu_response.cells_to_be_deactiv_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_deactiv_list_item_ies_o> ack_cell;
      ack_cell->cells_to_be_deactiv_list_item().nr_cgi.plmn_id = cgi.plmn_id.to_bytes();
      ack_cell->cells_to_be_deactiv_list_item().nr_cgi.nr_cell_id.from_number(cgi.nci.value());

      ack->cells_to_be_deactiv_list.push_back(ack_cell);
    }
  }

  return f1ap_msg;
}

/// Creates the gNB-DU Configuration Update Failure, as per TS 38.473, Section 9.2.1.9.
static f1ap_message create_du_config_update_failure(const asn1::f1ap::gnb_du_cfg_upd_s& request,
                                                    const asn1::f1ap::cause_c&          cause)
{
  f1ap_message f1ap_msg;

  f1ap_msg.pdu.set_unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_GNB_DU_CFG_UPD);
  auto& fail = f1ap_msg.pdu.unsuccessful_outcome().value.gnb_du_cfg_upd_fail();

  fail->transaction_id = request->transaction_id;
  fail->cause          = cause;

  return f1ap_msg;
}

void ocudu::ocucp::handle_du_config_update_procedure(const asn1::f1ap::gnb_du_cfg_upd_s& request,
                                                     f1ap_du_context&                    du_ctxt,
                                                     f1ap_message_notifier&              pdu_notifier,
                                                     du_setup_notifier&                  du_setup_notif,
                                                     ocudulog::basic_logger&             logger)
{
  du_config_update_request du_req = create_du_config_update_request(request, du_ctxt, logger);

  du_config_update_result request_outcome = du_setup_notif.on_new_du_config_update(du_req);
  if (not request_outcome.is_accepted()) {
    const auto& fail_resp = std::get<du_config_update_result::rejected>(request_outcome.result);
    logger.warning("Rejecting gNB-DU Configuration Update. Cause: {}", fail_resp.cause_str);
    pdu_notifier.on_new_message(create_du_config_update_failure(request, cause_to_asn1(fail_resp.cause)));
    return;
  }

  // The CU-CP rejects an update that carries a different gNB-DU ID, so only the name changes here.
  if (request->gnb_du_name_present) {
    du_ctxt.gnb_du_name = request->gnb_du_name.to_string();
  }

  pdu_notifier.on_new_message(create_du_config_update_acknowledge(
      request, std::get<du_config_update_result::accepted>(request_outcome.result)));
}
