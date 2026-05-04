// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_high_ntn_sib19_update_handler_impl.h"
#include "du_manager/converters/asn1_ntn_config_helpers.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/asn1/rrc_nr/bcch_dl_sch_msg.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/du/du_high/du_manager/du_configurator.h"

using namespace ocudu;
using namespace odu;

du_high_ntn_sib19_update_handler_impl::du_high_ntn_sib19_update_handler_impl(du_configurator& du_cfgr_) :
  logger(ocudulog::fetch_basic_logger("DU")), du_cfgr(du_cfgr_)
{
}

void du_high_ntn_sib19_update_handler_impl::handle_sib19_msg_update(const ocudu_ntn::ntn_sib19_update_request& req)
{
  // Convert to DU SI PDU update request.
  using namespace asn1::rrc_nr;

  // Pack SIB19 as BCCH-DL-SCH msg.
  byte_buffer             buf;
  asn1::bit_ref           bref{buf};
  bcch_dl_sch_msg_s       msg;
  sys_info_ies_s&         si_ies = msg.msg.set_c1().set_sys_info().crit_exts.set_sys_info();
  sys_info_ies_s::item_c_ sib_item;
  sib_item.set_sib19_v1700() = make_asn1_rrc_cell_sib19(req.sib19);

  si_ies.sib_type_and_info.push_back(sib_item);

  asn1::OCUDUASN_CODE ret = msg.pack(bref);
  ocudu_assert(ret == asn1::OCUDUASN_SUCCESS, "Failed to pack SIB19");

  std::vector<byte_buffer> msgs;
  msgs.push_back(std::move(buf));

  // Create cell-level request.
  du_cell_ntn_param_update_request cell_req;
  cell_req.nr_cgi         = req.nr_cgi;
  cell_req.si_msg_idx     = req.si_msg_idx;
  cell_req.sib_idx        = req.sib_idx;
  cell_req.slot           = req.slot;
  cell_req.si_slot_period = req.si_slot_period;
  cell_req.si_messages    = span<byte_buffer>(msgs);

  // Populate NTN assistance info.
  ntn_assistance_info_update ntn_info;
  ntn_info.ephemeris_info           = req.sib19.ntn_cfg->ephemeris_info.value();
  ntn_info.ta_info                  = req.sib19.ntn_cfg->ta_info;
  ntn_info.epoch_time               = req.sib19.ntn_cfg->epoch_time;
  ntn_info.ntn_ul_sync_validity_dur = req.sib19.ntn_cfg->ntn_ul_sync_validity_dur;
  cell_req.ntn_assistance_info      = ntn_info;

  // When tracked SIB19 fields have changed, set sib19 to trigger SIB1 systemInfoValueTag increment.
  if (req.si_valuetag_change) {
    cell_req.sib19 = req.sib19;
  }

  // Put in DU request.
  du_ntn_param_update_request du_req;
  du_req.cells.push_back(std::move(cell_req));

  // Forward to DU configurator.
  du_cfgr.handle_ntn_param_update(du_req);
}
