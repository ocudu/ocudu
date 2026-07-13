// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_write_replace_warning_procedure.h"
#include "../../asn1_helpers.h"
#include "../f1ap_du_context.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap_ies.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/f1ap/f1ap_message_notifier.h"

using namespace ocudu;
using namespace odu;
using namespace asn1::f1ap;

f1ap_du_write_replace_warning_procedure::f1ap_du_write_replace_warning_procedure(
    const write_replace_warning_request_s& msg_,
    const f1ap_du_context&                 ctxt_,
    f1ap_du_pws_notifier&                  pws_notifier_,
    f1ap_message_notifier&                 cu_notif_) :
  request(msg_), ctxt(ctxt_), pws_notifier(pws_notifier_), cu_notif(cu_notif_)
{
}

void f1ap_du_write_replace_warning_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  CORO_AWAIT_VALUE(accepted_cells, pws_notifier.on_write_replace_warning_received(build_request()));

  send_response();

  CORO_RETURN();
}

write_replace_warning_information f1ap_du_write_replace_warning_procedure::build_request() const
{
  write_replace_warning_information info;
  info.sib_type                 = request->pws_sys_info.sib_type;
  info.sib_msg                  = request->pws_sys_info.sib_msg.copy();
  info.repeat_period            = std::chrono::seconds{request->repeat_period};
  info.nof_broadcasts_requested = request->numof_broadcast_request;

  if (request->cells_to_be_broadcast_list_present) {
    for (const auto& item : request->cells_to_be_broadcast_list) {
      auto cgi = cgi_from_asn1(item->cells_to_be_broadcast_item().nr_cgi);
      if (not cgi.has_value()) {
        continue;
      }
      const f1ap_du_cell_context* cell = ctxt.find_cell(cgi.value());
      if (cell == nullptr) {
        // Cell not served by this DU.
        continue;
      }
      info.cells.push_back(cell->cell_index);
    }
  } else {
    // Absent list means all served cells are targeted.
    for (const auto& cell : ctxt.served_cells) {
      info.cells.push_back(cell.cell_index);
    }
  }

  return info;
}

void f1ap_du_write_replace_warning_procedure::send_response()
{
  f1ap_message msg;

  msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_WRITE_REPLACE_WARNING);
  write_replace_warning_resp_s& resp = msg.pdu.successful_outcome().value.write_replace_warning_resp();
  resp->transaction_id               = request->transaction_id;

  if (not accepted_cells.empty()) {
    resp->cells_broadcast_completed_list_present = true;
    resp->cells_broadcast_completed_list.resize(accepted_cells.size());
    for (unsigned i = 0, e = accepted_cells.size(); i != e; ++i) {
      // \c accepted_cells is always a subset of the cells resolved from \c ctxt.served_cells in build_request().
      const nr_cell_global_id_t& cgi = ctxt.served_cells[accepted_cells[i]].nr_cgi;
      resp->cells_broadcast_completed_list[i].load_info_obj(ASN1_F1AP_ID_CELLS_BROADCAST_COMPLETED_ITEM);
      resp->cells_broadcast_completed_list[i]->cells_broadcast_completed_item().nr_cgi = cgi_to_asn1(cgi);
    }
  }

  // Send F1AP message.
  cu_notif.on_new_message(msg);
}
