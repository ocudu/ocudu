// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue.h"
#include "ocudu/ocudulog/ocudulog.h"

using namespace ocudu;

ue::ue(const ue_configuration&       cfg,
       ue_logical_channel_repository lch_repo,
       ue_drx_controller&            drx_ctrl,
       ue_ta_report_tracker&         ta_report_tracker_,
       ue_ta_manager                 ta_mgr_,
       const ue_cell_lookup&         ue_cells_) :
  ue_index(cfg.ue_index),
  crnti(cfg.crnti),
  expert_cfg(cfg.expert_cfg()),
  cell_cfg_common(cfg.pcell_cfg().cell_cfg_common),
  ue_ded_cfg(&cfg),
  lc_ch_mgr(std::move(lch_repo)),
  drx(drx_ctrl),
  ta_report(ta_report_tracker_),
  ta_mgr(std::move(ta_mgr_)),
  cells(ue_cells_)
{
}

void ue::slot_indication(slot_point sl_tx)
{
  last_sl_tx = sl_tx;
  drx.slot_indication(sl_tx);
}

void ue::deactivate()
{
  // Disable SRBs and DRBs.
  // Note: We assume that when this function is called any pending RRC container (e.g. containing RRC Release) has
  // already been Tx+ACKed or an upper layer timeout has triggered.
  lc_ch_mgr.deactivate();

  // Cancel HARQ retransmissions in all UE cells.
  for (auto& cell : cells.du_cells) {
    cell->deactivate();
  }
}

void ue::handle_reconfiguration_request(const ue_configuration& new_cfg)
{
  // Update UE config.
  ue_ded_cfg = &new_cfg;
}

void ue::handle_dl_buffer_state_indication(lcid_t lcid, unsigned bs, slot_point hol_toa)
{
  unsigned pending_bytes = bs;

  // Subtract bytes pending for this LCID in scheduled DL HARQ allocations (but not yet sent to the lower layers)
  // before forwarding to DL logical channel manager.
  // Note: The RLC buffer occupancy updates never account for bytes associated with future HARQ transmissions.
  // Note: The TB in the HARQ should also include RLC header segmentation overhead which is not accounted yet in the
  // reported RLC DL buffer occupancy report (reminder: we haven't built the RLC PDU yet!). If we account for this
  // overhead in the computation of pending bytes, the final value will be too low, which will lead to one extra
  // tiny grant. To avoid this, we make the pessimization that every HARQ contains one RLC header due to segmentation.
  static constexpr unsigned RLC_AM_HEADER_SIZE_ESTIM = 4;
  for (unsigned c = 0, ce = nof_cells(); c != ce; ++c) {
    auto& ue_cc = *cells.ue_cells[c];

    if (last_sl_tx.valid() and ue_cc.harqs.last_pdsch_slot().valid() and ue_cc.harqs.last_pdsch_slot() > last_sl_tx) {
      unsigned rem_harqs = ue_cc.harqs.nof_dl_harqs() - ue_cc.harqs.nof_empty_dl_harqs();
      for (unsigned i = 0, e = ue_cc.harqs.nof_dl_harqs(); i != e and rem_harqs > 0; ++i) {
        auto h_dl = ue_cc.harqs.dl_harq(to_harq_id(i));
        if (h_dl.has_value()) {
          rem_harqs--;
          if (h_dl->pdsch_slot() > last_sl_tx and h_dl->nof_retxs() == 0 and h_dl->is_waiting_ack()) {
            for (const auto& lc : h_dl->get_grant_params().lc_sched_info) {
              if (lc.lcid.is_sdu() and lc.lcid.to_lcid() == lcid) {
                unsigned bytes_sched =
                    lc.sched_bytes.value() - std::min(lc.sched_bytes.value(), RLC_AM_HEADER_SIZE_ESTIM);
                pending_bytes -= std::min(pending_bytes, bytes_sched);
              }
            }
          }
        }
      }
    }
  }

  lc_ch_mgr.handle_dl_buffer_status_indication(lcid, pending_bytes, hol_toa);
}

units::bytes ue::pending_ul_newtx_bytes() const
{
  static constexpr unsigned SR_GRANT_BYTES = 512;

  // Sum the last BSRs.
  units::bytes pending_bytes{lc_ch_mgr.ul_pending_bytes()};

  // Subtract the bytes already allocated in UL HARQs.
  for (const ue_cell* ue_cc : cells.ue_cells) {
    if (pending_bytes.value() == 0) {
      break;
    }
    units::bytes harq_bytes = ue_cc->harqs.total_ul_bytes_waiting_ack();
    pending_bytes -= std::min(pending_bytes, harq_bytes);
  }

  // If there are no pending bytes, check if a SR is pending.
  return pending_bytes.value() > 0 ? pending_bytes : units::bytes{lc_ch_mgr.has_pending_sr() ? SR_GRANT_BYTES : 0};
}
