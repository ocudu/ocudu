// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_pws_broadcast_procedure.h"
#include "../du_cell_manager.h"
#include "ocudu/asn1/rrc_nr/bcch_dl_sch_msg.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/async/async_no_op_task.h"

using namespace ocudu;
using namespace odu;

namespace {

/// Finds the SI-message index at which \c type is statically scheduled in \c cell_cfg, if any. A cell only has a
/// scheduling slot for SIB6/7/8 if it was configured with the corresponding etws_cfg/cmas_cfg at startup.
std::optional<unsigned> find_si_msg_idx_for_sib(const du_cell_config& cell_cfg, sib_type type)
{
  if (not cell_cfg.si.si_config.has_value()) {
    return std::nullopt;
  }
  const auto& si_sched_info = cell_cfg.si.si_config->si_sched_info;
  for (unsigned i = 0, e = si_sched_info.size(); i != e; ++i) {
    const auto& sibs = si_sched_info[i].sib_mapping_info;
    if (std::find(sibs.begin(), sibs.end(), type) != sibs.end()) {
      return i;
    }
  }
  return std::nullopt;
}

/// \brief Packs a single ASN.1 PER-encoded SIB6/7/8 segment \c sib_msg into a full BCCH-DL-SCH-Message envelope.
///
/// SIB6/7/8 are root (non-extension) CHOICE alternatives in SystemInformation-IEs.sib-TypeAndInfo, so, unlike
/// Rel-16+ SIBs, they are not open-type wrapped and cannot be spliced into the envelope as raw bytes -- they must
/// first be unpacked into their generated ASN.1 object so it can be re-packed in place by the envelope's own pack().
///
/// \remark SIB6 is never segmented (always exactly one segment). SIB7/8 may be split into multiple segments by the
/// CU (see \c write_replace_warning_information::sib_msgs); this function must be called once per segment, and each
/// resulting BCCH-DL-SCH-Message is transmitted in its own SI-message window occasion, in order.
expected<byte_buffer> pack_warning_bcch_dl_sch_msg(uint8_t sib_type, const byte_buffer& sib_msg)
{
  using namespace asn1::rrc_nr;

  asn1::cbit_ref bref(sib_msg);

  sys_info_ies_s::item_c_ sib_item;
  switch (sib_type) {
    case 6:
      if (sib_item.set_sib6().unpack(bref) != asn1::OCUDUASN_SUCCESS) {
        return make_unexpected(default_error_t{});
      }
      break;
    case 7:
      if (sib_item.set_sib7().unpack(bref) != asn1::OCUDUASN_SUCCESS) {
        return make_unexpected(default_error_t{});
      }
      break;
    case 8:
      if (sib_item.set_sib8().unpack(bref) != asn1::OCUDUASN_SUCCESS) {
        return make_unexpected(default_error_t{});
      }
      break;
    default:
      return make_unexpected(default_error_t{});
  }

  bcch_dl_sch_msg_s msg;
  msg.msg.set_c1().set_sys_info().crit_exts.set_sys_info().sib_type_and_info.push_back(sib_item);

  byte_buffer   buf;
  asn1::bit_ref bref_out{buf};
  if (msg.pack(bref_out) != asn1::OCUDUASN_SUCCESS) {
    return make_unexpected(default_error_t{});
  }
  return buf;
}

} // namespace

du_pws_broadcast_procedure::du_pws_broadcast_procedure(const write_replace_warning_information& req_,
                                                       const du_manager_params&                 du_params_,
                                                       du_cell_manager&                         cell_mng_) :
  request(req_), du_params(du_params_), cell_mng(cell_mng_), logger(ocudulog::fetch_basic_logger("DU-MNG"))
{
}

void du_pws_broadcast_procedure::operator()(coro_context<async_task<std::vector<du_cell_index_t>>>& ctx)
{
  CORO_BEGIN(ctx);

  for (; next_cell_idx != request.cells.size(); ++next_cell_idx) {
    CORO_AWAIT_VALUE(mac_cell_reconfig_response resp, handle_cell_broadcast(request.cells[next_cell_idx]));
    if (resp.si_pdus_enqueued) {
      accepted_cells.push_back(request.cells[next_cell_idx]);
    }
  }

  CORO_RETURN(accepted_cells);
}

async_task<mac_cell_reconfig_response> du_pws_broadcast_procedure::handle_cell_broadcast(du_cell_index_t cell_index)
{
  const du_cell_config& cell_cfg = cell_mng.get_cell_cfg(cell_index);

  std::optional<unsigned> si_msg_idx = find_si_msg_idx_for_sib(cell_cfg, static_cast<sib_type>(request.sib_type));
  if (not si_msg_idx.has_value()) {
    logger.warning("cell={}: Discarding Write-Replace Warning. Cause: Cell not provisioned for SIB{}",
                   cell_index,
                   request.sib_type);
    return launch_no_op_task(mac_cell_reconfig_response{});
  }

  si_messages.clear();
  si_messages.reserve(request.sib_msgs.size());
  for (const byte_buffer& segment : request.sib_msgs) {
    expected<byte_buffer> pdu = pack_warning_bcch_dl_sch_msg(request.sib_type, segment);
    if (not pdu.has_value()) {
      logger.warning(
          "cell={}: Discarding Write-Replace Warning. Cause: Failed to pack SIB{}", cell_index, request.sib_type);
      return launch_no_op_task(mac_cell_reconfig_response{});
    }
    si_messages.push_back(std::move(pdu.value()));
  }

  mac_cell_reconfig_request req;
  req.new_si_pdu_info = mac_cell_sys_info_pdu_update{
      .si_msg_idx     = si_msg_idx.value(),
      .sib_idx        = request.sib_type,
      .slot           = std::nullopt,
      .si_slot_period = std::nullopt,
      .si_messages    = span<byte_buffer>(si_messages),
      .pws_broadcast  = pws_broadcast_indication{request.repeat_period, request.nof_broadcasts_requested}};

  return du_params.mac.mgr.get_cell_manager().get_cell_controller(cell_index).reconfigure(req);
}
