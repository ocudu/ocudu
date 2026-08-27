// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ra_ue_repository.h"
#include "../config/cell_configuration.h"

using namespace ocudu;

namespace {
/// Notifier of MsgB (DL) HARQ process timeouts.
class msgb_harq_timeout_notifier final : public harq_timeout_notifier
{
public:
  msgb_harq_timeout_notifier(pci_t pci_, ocudulog::basic_logger& logger_) : pci(pci_), logger(logger_) {}

  void on_feedback_timeout(du_ue_index_t ue_idx, bool is_dl, bool ack) override
  {
    ocudu_sanity_check(is_dl, "Only DL HARQs are managed in the MsgB HARQ notifier");

    logger.debug("pci={}: MsgB HARQ timed out. Clearing HARQ entity.", pci);
  }

  void on_retx_timeout(du_ue_index_t ue_idx, bool is_dl) override
  {
    ocudu_sanity_check(is_dl, "Only DL HARQs are managed in the MsgB HARQ notifier");

    logger.debug("pci={}: MsgB HARQ retransmission timed out. Clearing HARQ entity.", pci);
  }

  void on_feedback_disabled_harq_timeout(du_ue_index_t ue_idx, bool is_dl, units::bytes tbs) override {}

private:
  const pci_t             pci;
  ocudulog::basic_logger& logger;
};

/// Compute max Msg3 reTx timeout period, based on the fact that it should not be longer than the ConRes timer.
unsigned get_harq_retx_timeout_slots(const cell_configuration& cell_cfg)
{
  const auto conres_timer       = cell_cfg.params.ul_cfg_common.init_ul_bwp.rach_cfg_common->ra_con_res_timer;
  const auto conres_timer_slots = conres_timer.count() * get_nof_slots_per_subframe(cell_cfg.scs_common());
  return conres_timer_slots;
}

} // namespace

/// Notifier of Msg3 (UL) HARQ process timeouts.
class ra_ue_repository::msg3_harq_timeout_notifier final : public harq_timeout_notifier
{
public:
  msg3_harq_timeout_notifier(ra_ue_repository& ra_ue_repo_, pci_t pci_, ocudulog::basic_logger& logger_) :
    ra_ue_repo(ra_ue_repo_), pci(pci_), logger(logger_)
  {
  }

  void on_feedback_timeout(du_ue_index_t ue_idx, bool is_dl, bool ack) override { release(ue_idx, is_dl, true); }

  void on_retx_timeout(du_ue_index_t ue_idx, bool is_dl) override { release(ue_idx, is_dl, false); }

  void on_feedback_disabled_harq_timeout(du_ue_index_t ue_idx, bool is_dl, units::bytes tbs) override {}

private:
  // Common to both timeout causes: the Msg3 HARQ is gone either way, so its ra_ue_repo ring entry must be
  // released too, or the ring slot leaks forever.
  void release(du_ue_index_t ue_idx, bool is_dl, bool is_feedback_timeout)
  {
    ocudu_sanity_check(not is_dl, "Only UL HARQs are managed in the RA scheduler");
    auto it = ra_ue_repo.table.find(static_cast<size_t>(ue_idx));
    ocudu_sanity_check(it != ra_ue_repo.end(), "timeout called but HARQ entity does not exist");

    if (is_feedback_timeout) {
      logger.warning("pci={} tc-rnti={}: Discarding Msg3 HARQ process. Cause: HARQ-ACK/CRC feedback was not received "
                     "in time.",
                     pci,
                     it->preamble.tc_rnti);
    } else {
      logger.warning("pci={} tc-rnti={}: Discarding Msg3 retransmission HARQ process. Cause: Retransmission period "
                     "timed out.",
                     pci,
                     it->preamble.tc_rnti);
    }

    // Erase the entry to make the slot available again.
    ra_ue_repo.erase(it);
  }

  ra_ue_repository&       ra_ue_repo;
  const pci_t             pci;
  ocudulog::basic_logger& logger;
};

ra_ue_repository::ra_ue_repository(const cell_configuration& cell_cfg,
                                   ocudulog::basic_logger&   logger,
                                   size_t                    capacity) :
  ra_con_res_timer_slots(get_harq_retx_timeout_slots(cell_cfg)),
  ring_capacity(static_cast<uint16_t>(capacity)),
  ra_harqs(ring_capacity,
           ring_capacity,
           1,
           std::make_unique<msgb_harq_timeout_notifier>(cell_cfg.params.pci, logger),
           std::make_unique<msg3_harq_timeout_notifier>(*this, cell_cfg.params.pci, logger),
           get_harq_retx_timeout_slots(cell_cfg),
           get_harq_retx_timeout_slots(cell_cfg),
           cell_harq_manager::DEFAULT_ACK_TIMEOUT_SLOTS,
           cell_cfg.ntn_cs_koffset,
           cell_cfg.params.ntn_params.has_value() && cell_cfg.params.ntn_params->ul_harq_mode_b)
{
  report_fatal_error_if_not(capacity > 0, "ra_ue_repository capacity must be greater than 0");
  table.reserve(capacity);
}

void ra_ue_repository::slot_indication(slot_point sl_tx)
{
  ra_harqs.slot_indication(sl_tx);

  // Erase any RA UE entry whose ra-ContentionResolutionTimer has expired, or that was marked pending removal, as
  // long as its Msg3 HARQ is not still awaiting ACK/CRC feedback, so its ring slot is never leaked.
  for (auto it = table.begin(); it != table.end();) {
    if (not it->harq_ent.empty() and it->harq_ent.find_ul_harq_waiting_ack().has_value()) {
      // Msg3 HARQ is still awaiting ACK/CRC feedback.
      ++it;
      continue;
    }
    const slot_point sl_conres = it->prach_slot_rx + ra_con_res_timer_slots;
    if (not it->pending_removal and sl_conres > sl_tx) {
      // ConRes window has not yet elapsed, and removal wasn't otherwise requested.
      ++it;
      continue;
    }
    it = table.erase(it);
  }
}
