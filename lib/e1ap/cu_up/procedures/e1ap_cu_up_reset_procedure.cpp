// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e1ap_cu_up_reset_procedure.h"
#include "ocudu/asn1/e1ap/common.h"
#include "ocudu/e1ap/common/e1ap_message.h"

using namespace ocudu;
using namespace ocuup;

e1ap_cu_up_reset_procedure::e1ap_cu_up_reset_procedure(const e1ap_cu_up_reset_procedure_configuration& cfg,
                                                       const e1ap_cu_up_reset_procedure_dependencies&  dependencies) :
  reset_msg(cfg.reset_msg),
  ue_ctxt_list(dependencies.ue_ctxt_list),
  cu_up_notifier(dependencies.cu_up_notifier),
  tx_pdu_notifier(dependencies.tx_pdu_notifier),
  logger(dependencies.logger)
{
}

void e1ap_cu_up_reset_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_debug("\"{}\" started...", name());

  // Release CU-UP Bearer contexts.
  if (reset_msg->reset_type.type() == asn1::e1ap::reset_type_c::types_opts::e1_interface) {
    CORO_AWAIT(handle_e1_interface_reset(reset_msg->reset_type.e1_interface()));
  } else if (reset_msg->reset_type.type() == asn1::e1ap::reset_type_c::types_opts::part_of_e1_interface) {
    CORO_AWAIT(handle_part_of_e1_interface_reset(reset_msg->reset_type.part_of_e1_interface()));
  } else {
    CORO_EARLY_RETURN();
  }

  send_e1ap_reset_ack();
  logger.log_debug("\"{}\" finished successfully", name());
  CORO_RETURN();
}

async_task<void> e1ap_cu_up_reset_procedure::handle_e1_interface_reset(const asn1::e1ap::reset_all_e& reset_all)
{
  // Remove the E1AP IDs of all UEs, as per TS 37.483 section 8.2.1.2.1.
  ue_ctxt_list.clear();

  return cu_up_notifier.on_e1_reset_received({e1ap_reset::full, {}});
}

async_task<void> e1ap_cu_up_reset_procedure::handle_part_of_e1_interface_reset(
    const asn1::e1ap::ue_associated_lc_e1_conn_list_res_l& ue_reset_list)
{
  std::vector<cu_up_ue_index_t> ues;
  for (const auto& ue : ue_reset_list) {
    if (ue->type() !=
        asn1::e1ap::ue_associated_lc_e1_conn_item_res_o::value_c::types_opts::ue_associated_lc_e1_conn_item) {
      logger.log_warning("\"{}\": Unsupported UE associated logical connection", name());
      continue;
    }
    const asn1::e1ap::ue_associated_lc_e1_conn_item_s& lc = ue->ue_associated_lc_e1_conn_item();

    // Identify the UE by the gNB-CU-UP UE E1AP ID and/or the gNB-CU-CP UE E1AP ID, as per TS 37.483 section 8.2.1.2.1.
    const e1ap_ue_context* ue_ctx = nullptr;
    if (lc.gnb_cu_up_ue_e1ap_id_present) {
      gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id = int_to_gnb_cu_up_ue_e1ap_id(lc.gnb_cu_up_ue_e1ap_id);
      if (cu_up_e1ap_id == gnb_cu_up_ue_e1ap_id_t::invalid) {
        logger.log_warning("\"{}\": Invalid gNB-CU-UP-UE-E1AP-ID", name());
        continue;
      }
      ue_ctx = ue_ctxt_list.find_ue(cu_up_e1ap_id);
      if (ue_ctx == nullptr) {
        logger.log_info("\"{}\": Could not find UE. cu_up_e1ap_id={}", name(), to_underlying(cu_up_e1ap_id));
        continue;
      }
    } else if (lc.gnb_cu_cp_ue_e1ap_id_present) {
      gnb_cu_cp_ue_e1ap_id_t cu_cp_e1ap_id = int_to_gnb_cu_cp_ue_e1ap_id(lc.gnb_cu_cp_ue_e1ap_id);
      ue_ctx                               = ue_ctxt_list.find_ue(cu_cp_e1ap_id);
      if (ue_ctx == nullptr) {
        logger.log_info("\"{}\": Could not find UE. cu_cp_e1ap_id={}", name(), to_underlying(cu_cp_e1ap_id));
        continue;
      }
    } else {
      // Empty item.
      continue;
    }

    // Remove the E1AP ID of the UE, as per TS 37.483 section 8.2.1.2.1.
    const cu_up_ue_index_t ue_index = ue_ctx->ue_ids.ue_index;
    ue_ctxt_list.remove_ue(ue_index);
    ues.push_back(ue_index);
  }
  return cu_up_notifier.on_e1_reset_received({e1ap_reset::partial, ues});
}

void e1ap_cu_up_reset_procedure::send_e1ap_reset_ack()
{
  e1ap_message e1ap_msg;
  e1ap_msg.pdu.set_successful_outcome().load_info_obj(ASN1_E1AP_ID_RESET);
  asn1::e1ap::reset_ack_s& e1_reset_ack = e1ap_msg.pdu.successful_outcome().value.reset_ack();
  e1_reset_ack->transaction_id          = reset_msg->transaction_id;

  // Include an item for each reset UE association, as per TS 37.483 section 8.2.1.2.1.
  if (reset_msg->reset_type.type() == asn1::e1ap::reset_type_c::types_opts::part_of_e1_interface) {
    for (const auto& item : reset_msg->reset_type.part_of_e1_interface()) {
      if (item->type() !=
          asn1::e1ap::ue_associated_lc_e1_conn_item_res_o::value_c::types_opts::ue_associated_lc_e1_conn_item) {
        continue;
      }
      const asn1::e1ap::ue_associated_lc_e1_conn_item_s& conn_item = item->ue_associated_lc_e1_conn_item();
      if (not conn_item.gnb_cu_cp_ue_e1ap_id_present and not conn_item.gnb_cu_up_ue_e1ap_id_present) {
        continue;
      }
      asn1::protocol_ie_single_container_s<asn1::e1ap::ue_associated_lc_e1_conn_item_res_ack_o> ack_item;
      ack_item->ue_associated_lc_e1_conn_item() = conn_item;
      e1_reset_ack->ue_associated_lc_e1_conn_list_res_ack.push_back(ack_item);
    }
    e1_reset_ack->ue_associated_lc_e1_conn_list_res_ack_present =
        e1_reset_ack->ue_associated_lc_e1_conn_list_res_ack.size() > 0;
  }

  tx_pdu_notifier.on_new_message(e1ap_msg);
}
