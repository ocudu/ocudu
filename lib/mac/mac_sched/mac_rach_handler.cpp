// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mac_rach_handler.h"
#include "../rnti_manager.h"
#include "ocudu/scheduler/scheduler_configurator.h"
#include "ocudu/scheduler/scheduler_rach_handler.h"

using namespace ocudu;

mac_cell_rach_handler_impl::mac_cell_rach_handler_impl(mac_rach_handler&                               parent_,
                                                       const sched_cell_configuration_request_message& sched_cfg) :
  parent(parent_),
  cell_index(sched_cfg.cell_index),
  cfra_preambles(ra_helper::get_cfra_preambles(*sched_cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common)),
  // Preambles above the CB (4-step) and MsgA (2-step CB) ranges are reserved for CFRA.
  preambles(cfra_preambles.length())
{
  for (auto& preamble : preambles) {
    preamble.store(rnti_t::INVALID_RNTI, std::memory_order_relaxed);
  }
}

unsigned mac_cell_rach_handler_impl::get_cfra_index(unsigned ra_preamble_id) const
{
  ocudu_assert(cfra_preambles.contains(ra_preamble_id), "Invalid CFRA preamble");
  return ra_preamble_id - cfra_preambles.start();
}

void mac_cell_rach_handler_impl::handle_rach_indication(const mac_rach_indication& rach_ind)
{
  // Create Scheduler RACH indication message. Allocate TC-RNTIs in the process.
  rach_indication_message sched_rach{};
  sched_rach.cell_index = cell_index;
  sched_rach.slot_rx    = rach_ind.slot_rx;
  for (const auto& occasion : rach_ind.occasions) {
    auto& sched_occasion           = sched_rach.occasions.emplace_back();
    sched_occasion.start_symbol    = occasion.start_symbol;
    sched_occasion.frequency_index = occasion.frequency_index;
    for (const auto& preamble : occasion.preambles) {
      rnti_t selected_rnti = rnti_t::INVALID_RNTI;
      if (cfra_preambles.contains(preamble.index)) {
        // Fetch C-RNTI if it is Contention-free RACH preamble.
        selected_rnti = preambles[get_cfra_index(preamble.index)].load(std::memory_order_acquire);
        if (selected_rnti == rnti_t::INVALID_RNTI) {
          parent.logger.info("cell={} preamble id={}: Ignoring detected contention-free PRACH preamble. Cause: No "
                             "C-RNTI was allocated this preamble.",
                             fmt::underlying(cell_index),
                             preamble.index);
          continue;
        }
      } else {
        // It is a Contention-based RACH preamble. Allocate TC-RNTI for the UE.
        selected_rnti = parent.rnti_mng.allocate();
        if (selected_rnti == rnti_t::INVALID_RNTI) {
          parent.logger.warning("cell={} preamble id={}: Ignoring PRACH. Cause: Failed to allocate TC-RNTI.",
                                fmt::underlying(cell_index),
                                preamble.index);
          continue;
        }
      }
      auto& sched_preamble        = sched_occasion.preambles.emplace_back();
      sched_preamble.preamble_id  = preamble.index;
      sched_preamble.tc_rnti      = selected_rnti;
      sched_preamble.time_advance = preamble.time_advance;
    }
    if (sched_occasion.preambles.empty()) {
      // No preamble was added. Remove occasion.
      sched_rach.occasions.pop_back();
    }
  }

  // Forward RACH indication to scheduler.
  if (not sched_rach.occasions.empty()) {
    parent.sched.handle_rach_indication(sched_rach);
  }
}

bool mac_cell_rach_handler_impl::handle_cfra_allocation(uint8_t preamble_id, du_ue_index_t ue_idx, rnti_t crnti)
{
  ocudu_assert(cfra_preambles.contains(preamble_id), "Invalid preamble_id={}", preamble_id);
  if (parent.ue_map[ue_idx].cell_idx != INVALID_DU_CELL_INDEX or
      parent.ue_map[ue_idx].preamble_id != MAX_NOF_RA_PREAMBLES_PER_OCCASION) {
    return false;
  }
  const unsigned idx           = get_cfra_index(preamble_id);
  rnti_t         expected_rnti = rnti_t::INVALID_RNTI;
  if (preambles[idx].compare_exchange_strong(expected_rnti, crnti, std::memory_order_acq_rel)) {
    parent.ue_map[ue_idx].cell_idx    = cell_index;
    parent.ue_map[ue_idx].preamble_id = preamble_id;
    return true;
  }
  return false;
}

void mac_cell_rach_handler_impl::handle_cfra_deallocation(du_ue_index_t ue_idx)
{
  auto&         ue_entry    = parent.ue_map[ue_idx];
  const uint8_t preamble_id = ue_entry.preamble_id;
  if (preamble_id != MAX_NOF_RA_PREAMBLES_PER_OCCASION) {
    ue_entry.cell_idx    = INVALID_DU_CELL_INDEX;
    ue_entry.preamble_id = MAX_NOF_RA_PREAMBLES_PER_OCCASION;
    preambles[get_cfra_index(preamble_id)].store(rnti_t::INVALID_RNTI, std::memory_order_release);
  }
}

mac_rach_handler::mac_rach_handler(scheduler_rach_handler& sched_,
                                   rnti_manager&           rnti_mng_,
                                   ocudulog::basic_logger& logger_) :
  sched(sched_),
  rnti_mng(rnti_mng_),
  logger(logger_),
  ue_map(MAX_NOF_DU_UES, cfra_ue_context{INVALID_DU_CELL_INDEX, MAX_NOF_RA_PREAMBLES_PER_OCCASION})
{
}

mac_cell_rach_handler_impl& mac_rach_handler::add_cell(const sched_cell_configuration_request_message& sched_cfg)
{
  ocudu_assert(not cell_map.contains(sched_cfg.cell_index), "Cell already exists");
  cell_map.emplace(sched_cfg.cell_index, std::make_unique<mac_cell_rach_handler_impl>(*this, sched_cfg));
  return *cell_map[sched_cfg.cell_index];
}

void mac_rach_handler::rem_cell(du_cell_index_t cell_index)
{
  ocudu_assert(cell_map.contains(cell_index), "Cell does not exist");
  cell_map.erase(cell_index);
}
