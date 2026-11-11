// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_cu_ue_context.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/f1ap/f1ap_message.h"
#include <algorithm>

using namespace ocudu;
using namespace ocucp;

void f1ap_ue_context::handle_dl_rrc_message(const f1ap_dl_rrc_message& msg, f1ap_message_notifier& msg_notifier)
{
  ocudu_sanity_check(ue_ids.du_ue_f1ap_id && ue_ids.du_ue_f1ap_id != gnb_du_ue_f1ap_id_t::invalid,
                     "Invalid gNB-DU-UE-F1AP-Id");

  asn1::f1ap::dl_rrc_msg_transfer_s dl_rrc_msg = {};
  dl_rrc_msg->gnb_cu_ue_f1ap_id                = to_underlying(ue_ids.cu_ue_f1ap_id);
  dl_rrc_msg->gnb_du_ue_f1ap_id                = to_underlying(*ue_ids.du_ue_f1ap_id);
  dl_rrc_msg->srb_id                           = static_cast<uint8_t>(srb_id_to_uint(msg.srb_id));
  dl_rrc_msg->rrc_container                    = msg.rrc_container.copy();

  if (pending_old_ue_id.has_value()) {
    // If the UE requests to reestablish RRC connection in the last serving gNB-DU, the DL RRC MESSAGE TRANSFER message
    // shall include old gNB-DU UE F1AP ID, see TS 38.401 section 8.7.
    dl_rrc_msg->old_gnb_du_ue_f1ap_id_present = true;
    dl_rrc_msg->old_gnb_du_ue_f1ap_id         = to_underlying(pending_old_ue_id.value());

    pending_old_ue_id.reset();
  }

  // Pack message into PDU.
  f1ap_message f1ap_dl_rrc_msg;
  f1ap_dl_rrc_msg.pdu.set_init_msg();
  f1ap_dl_rrc_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_DL_RRC_MSG_TRANSFER);
  f1ap_dl_rrc_msg.pdu.init_msg().value.dl_rrc_msg_transfer() = std::move(dl_rrc_msg);

  // Send DL RRC message.
  msg_notifier.on_new_message(f1ap_dl_rrc_msg);
}

f1ap_ue_context_list::~f1ap_ue_context_list()
{
  // Release the gNB-CU UE F1AP IDs of the remaining UEs, so that other F1AP instances can use them.
  for (const auto& u : ues) {
    cu_ue_id_alloc.release(u.first);
  }
}

bool f1ap_ue_context_list::contains(cu_cp_ue_index_t ue_index) const
{
  if (ue_index_to_ue_f1ap_id.find(ue_index) == ue_index_to_ue_f1ap_id.end()) {
    return false;
  }
  if (ues.find(ue_index_to_ue_f1ap_id.at(ue_index)) == ues.end()) {
    return false;
  }
  return true;
}

f1ap_ue_context& f1ap_ue_context_list::operator[](gnb_cu_ue_f1ap_id_t cu_ue_id)
{
  ocudu_assert(ues.find(cu_ue_id) != ues.end(), "cu_ue={}: F1AP UE context not found", fmt::underlying(cu_ue_id));
  return ues.at(cu_ue_id);
}

f1ap_ue_context& f1ap_ue_context_list::operator[](cu_cp_ue_index_t ue_index)
{
  ocudu_assert(ue_index_to_ue_f1ap_id.find(ue_index) != ue_index_to_ue_f1ap_id.end(),
               "ue={} gNB-CU-UE-F1AP-ID not found",
               ue_index);
  ocudu_assert(ues.find(ue_index_to_ue_f1ap_id.at(ue_index)) != ues.end(),
               "cu_ue={}: F1AP UE context not found",
               fmt::underlying(ue_index_to_ue_f1ap_id.at(ue_index)));
  return ues.at(ue_index_to_ue_f1ap_id.at(ue_index));
}

const f1ap_ue_context* f1ap_ue_context_list::find(gnb_du_ue_f1ap_id_t du_ue_id) const
{
  auto it =
      std::find_if(ues.begin(), ues.end(), [du_ue_id](const std::pair<const gnb_cu_ue_f1ap_id_t, f1ap_ue_context>& u) {
        return u.second.ue_ids.du_ue_f1ap_id && u.second.ue_ids.du_ue_f1ap_id == du_ue_id;
      });
  return it != ues.end() ? &it->second : nullptr;
}

const f1ap_ue_context* f1ap_ue_context_list::find(gnb_cu_ue_f1ap_id_t cu_ue_id) const
{
  auto it = ues.find(cu_ue_id);
  return it != ues.end() ? &it->second : nullptr;
}

f1ap_ue_context* f1ap_ue_context_list::find(gnb_cu_ue_f1ap_id_t cu_ue_id)
{
  auto it = ues.find(cu_ue_id);
  return it != ues.end() ? &it->second : nullptr;
}

const f1ap_ue_context* f1ap_ue_context_list::find(cu_cp_ue_index_t ue_idx) const
{
  auto it = ue_index_to_ue_f1ap_id.find(ue_idx);
  return it != ue_index_to_ue_f1ap_id.end() ? &ues.at(it->second) : nullptr;
}

f1ap_ue_context* f1ap_ue_context_list::find(cu_cp_ue_index_t ue_idx)
{
  auto it = ue_index_to_ue_f1ap_id.find(ue_idx);
  return it != ue_index_to_ue_f1ap_id.end() ? &ues.at(it->second) : nullptr;
}

f1ap_ue_context& f1ap_ue_context_list::add_ue(cu_cp_ue_index_t ue_index, gnb_cu_ue_f1ap_id_t cu_ue_id)
{
  ocudu_assert(ue_index != cu_cp_ue_index_t::invalid, "Invalid ue_index={}", ue_index);
  ocudu_assert(cu_ue_id != gnb_cu_ue_f1ap_id_t::invalid, "Invalid cu_ue={}", fmt::underlying(cu_ue_id));

  logger.debug("ue={} cu_ue={}: Adding F1AP UE context", ue_index, fmt::underlying(cu_ue_id));
  ues.emplace(
      std::piecewise_construct, std::forward_as_tuple(cu_ue_id), std::forward_as_tuple(ue_index, cu_ue_id, timers));
  ue_index_to_ue_f1ap_id.emplace(ue_index, cu_ue_id);
  return ues.at(cu_ue_id);
}

void f1ap_ue_context_list::add_du_ue_f1ap_id(gnb_cu_ue_f1ap_id_t cu_ue_id, gnb_du_ue_f1ap_id_t du_ue_id)
{
  ocudu_assert(cu_ue_id != gnb_cu_ue_f1ap_id_t::invalid, "Invalid cu_ue={}", fmt::underlying(cu_ue_id));
  ocudu_assert(du_ue_id != gnb_du_ue_f1ap_id_t::invalid, "Invalid du_ue={}", fmt::underlying(du_ue_id));
  ocudu_assert(ues.find(cu_ue_id) != ues.end(), "cu_ue={}: F1AP UE context not found", fmt::underlying(cu_ue_id));

  auto& ue = ues.at(cu_ue_id);
  ue.logger.log_debug("Adding du_ue={}", fmt::underlying(du_ue_id));
  ue.ue_ids.du_ue_f1ap_id = du_ue_id;

  ue.logger.set_prefix({ue.ue_ids.ue_index, ue.ue_ids.cu_ue_f1ap_id, ue.ue_ids.du_ue_f1ap_id}, ": ");
}

void f1ap_ue_context_list::remove_ue(cu_cp_ue_index_t ue_index)
{
  ocudu_assert(ue_index != cu_cp_ue_index_t::invalid, "Invalid ue_index={}", ue_index);

  if (ue_index_to_ue_f1ap_id.find(ue_index) == ue_index_to_ue_f1ap_id.end()) {
    logger.warning("ue={}: gNB-CU-UE-F1AP-ID not found", ue_index);
    return;
  }

  // Remove UE from lookup
  gnb_cu_ue_f1ap_id_t cu_ue_id = ue_index_to_ue_f1ap_id.at(ue_index);
  ue_index_to_ue_f1ap_id.erase(ue_index);

  if (ues.find(cu_ue_id) == ues.end()) {
    logger.warning("cu_ue={}: F1AP UE context not found", fmt::underlying(cu_ue_id));
    return;
  }

  logger.debug("ue={} cu_ue={}: Removing F1AP UE context", ue_index, fmt::underlying(cu_ue_id));
  ues.erase(cu_ue_id);
  cu_ue_id_alloc.release(cu_ue_id);
}

void f1ap_ue_context_list::cancel_ue_transactions(cu_cp_ue_index_t ue_index)
{
  auto stop = [this, ue_index](auto f1ap_ue_transaction_manager::* event_source) {
    if (f1ap_ue_context* u = find(ue_index)) {
      (u->ev_mng.*event_source).stop();
    }
  };
  stop(&f1ap_ue_transaction_manager::context_setup_outcome);
  stop(&f1ap_ue_transaction_manager::context_modification_outcome);
  stop(&f1ap_ue_transaction_manager::positioning_information_outcome);
  stop(&f1ap_ue_transaction_manager::positioning_activation_outcome);
  stop(&f1ap_ue_transaction_manager::e_cid_measurement_outcome);
  // The UE Context Release Routine waits for this transaction and removes the UE context. Thus, cancel it last.
  stop(&f1ap_ue_transaction_manager::context_release_complete);
}

void f1ap_ue_context_list::add_srb0_rrc_notifier(cu_cp_ue_index_t ue_index, f1ap_ul_ccch_notifier* srb0_notifier)
{
  ocudu_assert(ue_index != cu_cp_ue_index_t::invalid, "Invalid ue_index={}", ue_index);
  ocudu_assert(ue_index_to_ue_f1ap_id.find(ue_index) != ue_index_to_ue_f1ap_id.end(),
               "ue={}: gNB-CU-UE-F1AP-ID not found",
               ue_index);
  ocudu_assert(ues.find(ue_index_to_ue_f1ap_id.at(ue_index)) != ues.end(),
               "cu_ue={}: F1AP UE context not found",
               fmt::underlying(ue_index_to_ue_f1ap_id.at(ue_index)));
  ocudu_assert(srb0_notifier, "Invalid SRB0 notifier");

  ues.at(ue_index_to_ue_f1ap_id.at(ue_index)).get_ul_bearer_manager().activate_srb0(*srb0_notifier);
}

void f1ap_ue_context_list::add_srb1_rrc_notifier(cu_cp_ue_index_t ue_index, f1ap_ul_dcch_notifier* srb1_notifier)
{
  ocudu_assert(ue_index != cu_cp_ue_index_t::invalid, "Invalid ue_index={}", ue_index);
  ocudu_assert(ue_index_to_ue_f1ap_id.find(ue_index) != ue_index_to_ue_f1ap_id.end(),
               "ue={}: gNB-CU-UE-F1AP-ID not found",
               ue_index);
  ocudu_assert(ues.find(ue_index_to_ue_f1ap_id.at(ue_index)) != ues.end(),
               "cu_ue={}: F1AP UE context not found",
               fmt::underlying(ue_index_to_ue_f1ap_id.at(ue_index)));
  ocudu_assert(srb1_notifier, "Invalid SRB1 notifier");

  ues.at(ue_index_to_ue_f1ap_id.at(ue_index)).get_ul_bearer_manager().activate_srb1(*srb1_notifier);
}

void f1ap_ue_context_list::add_srb2_rrc_notifier(cu_cp_ue_index_t ue_index, f1ap_ul_dcch_notifier* srb2_notifier)
{
  ocudu_assert(ue_index != cu_cp_ue_index_t::invalid, "Invalid ue_index={}", ue_index);
  ocudu_assert(ue_index_to_ue_f1ap_id.find(ue_index) != ue_index_to_ue_f1ap_id.end(),
               "ue={}: gNB-CU-UE-F1AP-ID not found",
               ue_index);
  ocudu_assert(ues.find(ue_index_to_ue_f1ap_id.at(ue_index)) != ues.end(),
               "cu_ue={}: F1AP UE context not found",
               fmt::underlying(ue_index_to_ue_f1ap_id.at(ue_index)));
  ocudu_assert(srb2_notifier, "Invalid SRB2 notifier");

  ues.at(ue_index_to_ue_f1ap_id.at(ue_index)).get_ul_bearer_manager().activate_srb2(*srb2_notifier);
}
