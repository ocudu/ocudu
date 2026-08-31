// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mac_rach_handler.h"
#include "../rnti_manager.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/prach/ra_helper.h"
#include "ocudu/scheduler/scheduler_configurator.h"
#include "ocudu/scheduler/scheduler_rach_handler.h"
#include <algorithm>

using namespace ocudu;

static unsigned get_total_msga_cb_preambles(const rach_config_common& rach_cfg)
{
  return ra_helper::get_msga_cb_preambles_per_ssb(rach_cfg) * get_nof_ssb_per_ro(rach_cfg.nof_ssb_per_ro);
}

static unsigned get_total_msg1_cfra_preambles(const rach_config_common& rach_cfg)
{
  return ra_helper::get_msg1_cfra_preambles_per_ssb(rach_cfg) * get_nof_ssb_per_ro(rach_cfg.nof_ssb_per_ro);
}

mac_cell_rach_handler_impl::mac_cell_rach_handler_impl(mac_rach_handler&                               parent_,
                                                       const sched_cell_configuration_request_message& sched_cfg) :
  parent(parent_),
  cell_index(sched_cfg.cell_index),
  rach_cfg_common(*sched_cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common),
  msga_tc_rnti_ttl_slots(
      sched_cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common->two_step_rach_cfg.has_value()
          ? static_cast<unsigned>(
                sched_cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common->two_step_rach_cfg->pusch.td_offset) +
                sched_cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common->two_step_rach_cfg->msgB_response_window_slots
          : 0U),
  msg1_cfra_preambles(get_total_msg1_cfra_preambles(rach_cfg_common)),
  msga_tc_rntis(get_total_msga_cb_preambles(rach_cfg_common)),
  msga_con_res_ids(get_total_msga_cb_preambles(rach_cfg_common))
{
  for (auto& preamble : msg1_cfra_preambles) {
    preamble.store(rnti_t::INVALID_RNTI, std::memory_order_relaxed);
  }
  for (auto& entry : msga_tc_rntis) {
    // Zero-initialized entry decodes to ra_rnti()==INVALID_RNTI (0x0), which no real RA-RNTI ever equals, so it is
    // naturally treated as "no entry" on lookup.
    entry.store(0, std::memory_order_relaxed);
  }
  for (auto& entry : msga_con_res_ids) {
    // Zero-initialized entry decodes to a tag of INVALID_RNTI (0), which no real TC-RNTI ever equals, so it is
    // naturally treated as "no entry" on lookup.
    entry.store(0, std::memory_order_relaxed);
  }
}

unsigned mac_cell_rach_handler_impl::get_cfra_index(unsigned ra_preamble_id) const
{
  ocudu_assert(ra_helper::is_msg1_cf_preamble(rach_cfg_common, ra_preamble_id), "Invalid CFRA preamble");
  const uint8_t  preambles_per_ssb    = ra_helper::get_preambles_per_ssb(rach_cfg_common);
  const uint8_t  cf_preambles_per_ssb = ra_helper::get_msg1_cfra_preambles_per_ssb(rach_cfg_common);
  const uint8_t  win_start            = preambles_per_ssb - cf_preambles_per_ssb;
  const uint8_t  local_idx            = (ra_preamble_id % preambles_per_ssb) - win_start;
  const unsigned ssb_idx              = ra_preamble_id / preambles_per_ssb;
  return local_idx + ssb_idx * cf_preambles_per_ssb;
}

unsigned mac_cell_rach_handler_impl::get_msga_cb_index(unsigned ra_preamble_id) const
{
  ocudu_assert(
      ra_helper::is_msga_cb_preamble(rach_cfg_common, ra_preamble_id), "Invalid MsgA preamble id={}", ra_preamble_id);
  const uint8_t  preambles_per_ssb         = ra_helper::get_preambles_per_ssb(rach_cfg_common);
  const uint8_t  msga_cb_preambles_per_ssb = ra_helper::get_msga_cb_preambles_per_ssb(rach_cfg_common);
  const uint8_t  win_start                 = rach_cfg_common.nof_cb_preambles_per_ssb;
  const uint8_t  local_idx                 = (ra_preamble_id % preambles_per_ssb) - win_start;
  const unsigned ssb_idx                   = ra_preamble_id / preambles_per_ssb;
  return local_idx + ssb_idx * msga_cb_preambles_per_ssb;
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
    sched_occasion.slot_index      = occasion.slot_index;
    sched_occasion.frequency_index = occasion.frequency_index;
    const rnti_t occasion_ra_rnti =
        ra_helper::get_ra_rnti(occasion.slot_index, occasion.start_symbol, occasion.frequency_index);
    for (const auto& preamble : occasion.preambles) {
      rnti_t selected_rnti = rnti_t::INVALID_RNTI;
      if (ra_helper::is_msg1_cf_preamble(rach_cfg_common, preamble.index)) {
        // Fetch C-RNTI if it is Contention-free RACH (CFRA) preamble.
        selected_rnti = msg1_cfra_preambles[get_cfra_index(preamble.index)].load(std::memory_order_acquire);
        if (selected_rnti == rnti_t::INVALID_RNTI) {
          parent.logger.info("cell={} preamble id={}: Ignoring detected contention-free PRACH preamble. Cause: No "
                             "C-RNTI was allocated this preamble.",
                             cell_index,
                             preamble.index);
          continue;
        }
      } else {
        // It is a Contention-based RACH preamble. Allocate TC-RNTI for the UE.
        selected_rnti = parent.rnti_mng.allocate();
        if (selected_rnti == rnti_t::INVALID_RNTI) {
          parent.logger.warning(
              "cell={} preamble id={}: Ignoring PRACH. Cause: Failed to allocate TC-RNTI.", cell_index, preamble.index);
          continue;
        }
        if (ra_helper::is_msga_cb_preamble(rach_cfg_common, preamble.index)) {
          // This is a 2-step RACH (MsgA) preamble. The MsgA PUSCH carrying its CCCH payload will be decoded and
          // reported by the lower layers using the RA-RNTI (as per TS 38.211, 6.3.1.1), not this TC-RNTI. Register
          // the mapping so it can be later resolved back to the real TC-RNTI.
          add_msga_tc_rnti(occasion_ra_rnti, static_cast<uint8_t>(preamble.index), selected_rnti, rach_ind.slot_rx);
        }
      }
      auto& sched_preamble        = sched_occasion.preambles.emplace_back();
      sched_preamble.preamble_id  = preamble.index;
      sched_preamble.tc_rnti      = selected_rnti;
      sched_preamble.time_advance = preamble.time_advance;
      sched_preamble.snr_dB       = preamble.snr_dB;
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

void mac_cell_rach_handler_impl::add_msga_tc_rnti(rnti_t ra_rnti, uint8_t rapid, rnti_t tc_rnti, slot_point sl_rx)
{
  ocudu_assert(ra_helper::is_msga_cb_preamble(rach_cfg_common, rapid), "Invalid MsgA preamble id={}", rapid);
  const slot_point expiry = sl_rx + msga_tc_rnti_ttl_slots;
  const unsigned   idx    = get_msga_cb_index(rapid);

  // Unconditional last-writer-wins overwrite. The RA-RNTI recurs periodically, so only the most recent occasion
  // that used a given RAPID is relevant; a single atomic store keeps this update wait-free and lock-free, as this
  // runs on a latency-critical path.
  msga_tc_rntis[idx].store(msga_tc_rnti_entry(ra_rnti, tc_rnti, expiry).to_word(), std::memory_order_release);
}

std::optional<rnti_t>
mac_cell_rach_handler_impl::resolve_msga_tc_rnti(rnti_t ra_rnti, uint8_t rapid, slot_point sl_rx) const
{
  if (not ra_helper::is_msga_cb_preamble(rach_cfg_common, rapid)) {
    return std::nullopt;
  }
  const unsigned           idx = get_msga_cb_index(rapid);
  const msga_tc_rnti_entry entry(msga_tc_rntis[idx].load(std::memory_order_acquire));
  if (entry.ra_rnti() != ra_rnti or entry.expiry() <= sl_rx) {
    return std::nullopt;
  }
  return entry.tc_rnti();
}

std::optional<rnti_t> mac_cell_rach_handler_impl::handle_msga_ccch_sdu(rnti_t                 ra_rnti,
                                                                       uint8_t                rapid,
                                                                       slot_point             sl_rx,
                                                                       const ue_con_res_id_t& con_res_id)
{
  std::optional<rnti_t> tc_rnti = resolve_msga_tc_rnti(ra_rnti, rapid, sl_rx);
  if (tc_rnti.has_value()) {
    add_msga_con_res_id(*tc_rnti, con_res_id);
  }
  return tc_rnti;
}

/// Packs a msga_con_res_ids word: bits [0, 48) hold the Contention Resolution Id bytes, bits [48, 64) hold the
/// owning TC-RNTI, used as a collision-detection tag on resolution.
static uint64_t pack_con_res_id(rnti_t tc_rnti, const ue_con_res_id_t& con_res_id)
{
  uint64_t word = static_cast<uint64_t>(to_value(tc_rnti)) << 48U;
  for (unsigned i = 0; i != UE_CON_RES_ID_LEN; ++i) {
    word |= static_cast<uint64_t>(con_res_id[i]) << (8U * i);
  }
  return word;
}

static rnti_t unpack_con_res_id_tag(uint64_t word)
{
  return to_rnti(static_cast<uint16_t>(word >> 48U));
}

static ue_con_res_id_t unpack_con_res_id(uint64_t word)
{
  ue_con_res_id_t con_res_id;
  for (unsigned i = 0; i != UE_CON_RES_ID_LEN; ++i) {
    con_res_id[i] = static_cast<uint8_t>(word >> (8U * i));
  }
  return con_res_id;
}

unsigned mac_cell_rach_handler_impl::get_con_res_id_index(rnti_t tc_rnti) const
{
  ocudu_assert(is_crnti(tc_rnti), "Invalid TC-RNTI={}", tc_rnti);
  return to_value(tc_rnti) % msga_con_res_ids.size();
}

void mac_cell_rach_handler_impl::add_msga_con_res_id(rnti_t tc_rnti, const ue_con_res_id_t& con_res_id)
{
  msga_con_res_ids[get_con_res_id_index(tc_rnti)].store(pack_con_res_id(tc_rnti, con_res_id),
                                                        std::memory_order_release);
}

std::optional<ue_con_res_id_t> mac_cell_rach_handler_impl::resolve_msga_con_res_id(rnti_t tc_rnti)
{
  ocudu_assert(not msga_con_res_ids.empty(), "No MsgA CB preambles available");
  const unsigned idx  = get_con_res_id_index(tc_rnti);
  const uint64_t word = msga_con_res_ids[idx].load(std::memory_order_acquire);
  if (unpack_con_res_id_tag(word) != tc_rnti) {
    return std::nullopt;
  }
  // Consume the entry, since a successRAR is only ever encoded once per preamble detection.
  msga_con_res_ids[idx].store(0, std::memory_order_release);
  return unpack_con_res_id(word);
}

bool mac_cell_rach_handler_impl::handle_cfra_allocation(uint8_t preamble_id, du_ue_index_t ue_idx, rnti_t crnti)
{
  ocudu_assert(ra_helper::is_msg1_cf_preamble(rach_cfg_common, preamble_id), "Invalid preamble_id={}", preamble_id);
  if (parent.ue_map[ue_idx].preamble_id != MAX_NOF_RA_PREAMBLES_PER_OCCASION) {
    return false;
  }
  const unsigned idx           = get_cfra_index(preamble_id);
  rnti_t         expected_rnti = rnti_t::INVALID_RNTI;
  if (msg1_cfra_preambles[idx].compare_exchange_strong(expected_rnti, crnti, std::memory_order_acq_rel)) {
    parent.ue_map[ue_idx].preamble_id = preamble_id;
    parent.ue_map[ue_idx].cell_index  = cell_index;
    return true;
  }
  return false;
}

void mac_cell_rach_handler_impl::handle_cfra_deallocation(du_ue_index_t ue_idx)
{
  auto&         ue_entry    = parent.ue_map[ue_idx];
  const uint8_t preamble_id = ue_entry.preamble_id;
  if (preamble_id != MAX_NOF_RA_PREAMBLES_PER_OCCASION) {
    ue_entry.preamble_id = MAX_NOF_RA_PREAMBLES_PER_OCCASION;
    msg1_cfra_preambles[get_cfra_index(preamble_id)].store(rnti_t::INVALID_RNTI, std::memory_order_release);
  }
}

mac_rach_handler::mac_rach_handler(scheduler_rach_handler& sched_,
                                   rnti_manager&           rnti_mng_,
                                   ocudulog::basic_logger& logger_) :
  sched(sched_),
  rnti_mng(rnti_mng_),
  logger(logger_),
  ue_map(MAX_NOF_DU_UES, cfra_ue_context{MAX_NOF_RA_PREAMBLES_PER_OCCASION, INVALID_DU_CELL_INDEX})
{
}

void mac_rach_handler::handle_cfra_deallocation(du_ue_index_t ue_idx)
{
  auto& entry = ue_map[ue_idx];
  if (entry.preamble_id == MAX_NOF_RA_PREAMBLES_PER_OCCASION) {
    return;
  }
  if (cell_map.contains(entry.cell_index)) {
    cell_map[entry.cell_index]->handle_cfra_deallocation(ue_idx);
  }
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
