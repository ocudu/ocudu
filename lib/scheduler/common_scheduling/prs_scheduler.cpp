// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "prs_scheduler.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/prs/prs_constants.h"
#include "ocudu/support/math/bit_ops.h"
#include "ocudu/support/ocudu_assert.h"
#include <algorithm>
#include <limits>

using namespace ocudu;

/// Builds the PDU of a DL-PRS resource. None of its fields depends on the slot of the transmission occasion.
static prs_info
build_prs_info(const bwp_configuration& bwp_cfg, const prs_resource_set& res_set, const prs_resource& res)
{
  prs_info pdu;

  pdu.scs             = bwp_cfg.scs;
  pdu.cp              = bwp_cfg.cp;
  pdu.n_id_prs        = res.sequence_id;
  pdu.comb_size       = res_set.comb_size;
  pdu.comb_offset     = res.re_offset;
  pdu.nof_symbols     = res_set.nof_symbols;
  pdu.symbols         = {res.symbol_offset, res.symbol_offset + to_underlying(res_set.nof_symbols)};
  pdu.crbs            = {res_set.start_prb, res_set.start_prb + res_set.bandwidth_prbs};
  pdu.power_offset_db = res_set.power_offset_db;

  return pdu;
}

prs_scheduler::prs_scheduler(const cell_configuration& cell_cfg) : logger(ocudulog::fetch_basic_logger("SCHED"))
{
  const prs_config&        prs_cfg = cell_cfg.params.prs_cfg;
  const bwp_configuration& bwp_cfg = cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params;

  for (const auto& set : prs_cfg.resource_sets) {
    const unsigned time_gap = to_underlying(set.time_gap);
    const unsigned nof_reps = to_underlying(set.repetition_factor);

    // Resource set instances that are transmitted. Every instance is, when Muting Option 1 is not configured.
    const bool     has_muting1 = set.muting_option1.has_value();
    const uint32_t muting_pattern1 =
        has_muting1 ? static_cast<uint32_t>(set.muting_option1->muting_pattern.to_uint64()) : ~0U;
    const uint8_t muting_pattern1_size =
        has_muting1 ? static_cast<uint8_t>(set.muting_option1->muting_pattern.size()) : 0U;
    const uint8_t muting_bit_rep_factor =
        has_muting1 ? static_cast<uint8_t>(to_underlying(set.muting_option1->muting_bit_repetition_factor)) : 1U;

    // Repetitions that are transmitted. Every repetition is, when Muting Option 2 is not configured.
    const uint32_t muting_pattern2 = set.muting_option2.has_value()
                                         ? static_cast<uint32_t>(set.muting_option2->muting_pattern.to_uint64())
                                         : static_cast<uint32_t>((uint64_t{1} << nof_reps) - 1U);

    if (muting_pattern1 == 0 or muting_pattern2 == 0) {
      // Every instance, or every repetition, of the resource set is muted, so it never transmits.
      continue;
    }

    // Compute the window from the earliest and latest transmissions in the period. It starts at the first transmitted
    // repetition of the earliest resource, and ends at the last transmitted repetition of the latest one.
    const auto [first_res, last_res] =
        std::minmax_element(set.resources.begin(),
                            set.resources.end(),
                            [](const prs_resource& a, const prs_resource& b) { return a.slot_offset < b.slot_offset; });

    // First and last repetitions that are transmitted, derived from the muting pattern. The pattern is not empty, as
    // sets that never transmit are discarded above.
    static constexpr unsigned nof_pattern_bits = std::numeric_limits<uint32_t>::digits;
    const unsigned            first_rep_idx    = zero_lsb_count(muting_pattern2);
    const unsigned            last_rep_idx     = nof_pattern_bits - 1U - zero_msb_count(muting_pattern2);

    const uint32_t window_start = set.slot_offset + first_res->slot_offset + (first_rep_idx * time_gap);
    const auto     span         = static_cast<uint16_t>((last_res->slot_offset - first_res->slot_offset) +
                                            ((last_rep_idx - first_rep_idx) * time_gap) + 1U);

    // schedule_slot compares the slot phase against the window without wrapping around the period.
    ocudu_assert(window_start + span <= set.periodicity_slots,
                 "DL-PRS window [{}, {}) does not fit in the period of {} slots",
                 window_start,
                 window_start + span,
                 set.periodicity_slots);

    auto& cached_set =
        sets.emplace_back(cached_resource_set{.period               = set.periodicity_slots,
                                              .window_start         = window_start,
                                              .span                 = span,
                                              .muting1_pattern      = muting_pattern1,
                                              .muting1_pattern_size = muting_pattern1_size,
                                              .muting1_rep_factor   = muting_bit_rep_factor,
                                              .muting2_pattern      = muting_pattern2,
                                              .time_gap             = static_cast<uint8_t>(time_gap),
                                              .first_pdu_idx        = static_cast<uint16_t>(cached_pdus.size())});

    // Save the position of the first repetition of each resource relative to the window start, and pre-generate its
    // PDU. The PDUs of the resources of a set are contiguous, so the set only stores the index of the first one.
    for (const prs_resource& res : set.resources) {
      cached_set.res_offsets.push_back(static_cast<uint16_t>(res.slot_offset - first_res->slot_offset));
      cached_pdus.push_back(build_prs_info(bwp_cfg, set, res));
    }
  }
}

void prs_scheduler::run_slot(cell_resource_allocator& res_alloc)
{
  if (sets.empty()) {
    return;
  }

  if (OCUDU_UNLIKELY(first_run_slot)) {
    // First call to run_slot. Schedule PRS across cell resource grid.
    for (unsigned i = 0; i != res_alloc.max_dl_slot_alloc_delay + 1; ++i) {
      schedule_slot(res_alloc[i]);
    }
    first_run_slot = false;
  } else {
    // Schedule PRS as far in advance as possible.
    schedule_slot(res_alloc[res_alloc.max_dl_slot_alloc_delay]);
  }
}

void prs_scheduler::stop()
{
  first_run_slot = true;
}

void prs_scheduler::schedule_slot(cell_slot_resource_allocator& slot_alloc)
{
  const unsigned slot = slot_alloc.slot.count();
  for (const auto& set : sets) {
    const unsigned phase = slot % set.period;

    if (phase < set.window_start or phase >= (set.window_start + set.span)) {
      // We are outside this set's transmission window.
      continue;
    }

    if (set.muting1_pattern_size != 0) {
      // Which instance of the resource set this slot falls in. TS 38.211, Section 7.4.1.7.4 also subtracts the set and
      // resource slot offsets, which is omitted here: every occasion is within one period, so the count is the same.
      const unsigned instance = slot / set.period;

      // Each bit of the pattern mutes a group of consecutive instances, whose size is always a power of two.
      const unsigned bit_idx = (instance >> zero_lsb_count(set.muting1_rep_factor)) % set.muting1_pattern_size;
      if (((set.muting1_pattern >> bit_idx) & 1U) == 0) {
        // This resource set instance is muted by DL-PRS Muting Option 1.
        continue;
      }
    }

    // The time gap is always a power of two, so the modulo and the division by it are a mask and a shift.
    const unsigned time_gap_mask  = set.time_gap - 1;
    const unsigned time_gap_shift = zero_lsb_count(set.time_gap);

    // The window starts at the first transmitted repetition, so offsets within it are counted from that repetition.
    const unsigned first_rep_idx = zero_lsb_count(set.muting2_pattern);

    // Offset relative to window start.
    const unsigned win_offset = phase - set.window_start;
    for (unsigned r = 0, nof_res = set.res_offsets.size(); r != nof_res; ++r) {
      // Check if a non-muted repetition of this resource falls in this slot.

      // Offset relative to the first transmitted repetition of the resource. It wraps around for resources that start
      // later in the window, yielding a repetition index above any valid one.
      const unsigned rel_offset = win_offset - set.res_offsets[r];
      if ((rel_offset & time_gap_mask) != 0) {
        // This slot does not fall on a repetition boundary of the resource.
        continue;
      }

      // muting_pattern2 holds one bit per repetition, so larger indices cannot be tested against it.
      const unsigned rep_idx = first_rep_idx + (rel_offset >> time_gap_shift);
      if (rep_idx >= prs_constants::VALID_REPETITION_FACTORS.back()) {
        continue;
      }

      if (((set.muting2_pattern >> rep_idx) & 1U) == 0) {
        // The repetition is above the repetition factor or muted by DL-PRS Muting Option 2.
        continue;
      }

      if (slot_alloc.result.dl.prs.full()) {
        logger.error("Failed to allocate DL-PRS PDU in slot={}", slot_alloc.slot);
        return;
      }

      // The resource grid is RB-granular, so the whole PRS bandwidth is reserved even though the resource only uses
      // one subcarrier out of every comb size. There is no PDSCH rate matching around DL-PRS.
      const prs_info& pdu = cached_pdus[set.first_pdu_idx + r];
      slot_alloc.result.dl.prs.push_back(pdu);
      slot_alloc.dl_res_grid.fill(grant_info{pdu.scs, pdu.symbols, pdu.crbs});
    }
  }
}
