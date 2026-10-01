// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mac_subframe_time_mapper_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/ocudulog/ocudulog.h"

using namespace ocudu;

bool atomic_subframe_time_mapper::store(slot_point slot, time_point time)
{
  if (slot.subframe_slot_index() != 0) {
    return false;
  }

  uint64_t new_packed_mapping = pack_sfn_time_mapping(slot, time);

  // Load current state.
  uint64_t current_mapping = mapping.load(std::memory_order_relaxed);
  // Note: When current_mapping is 0, load_sfn would return slot_point(0,0,0) which is valid,
  // so we check if current_mapping == 0.
  slot_point current_slot = load_sfn(current_mapping, slot.numerology());

  while (current_mapping == 0 || slot > current_slot) {
    if (mapping.compare_exchange_weak(
            current_mapping, new_packed_mapping, std::memory_order_release, std::memory_order_relaxed)) {
      return true;
    }
    // Reload slot for condition recheck.
    current_slot = load_sfn(current_mapping, slot.numerology());
  }

  // Current slot is >= new slot, reject update.
  return false;
}

mac_slot_time_info atomic_subframe_time_mapper::load(unsigned numerology, time_point now) const
{
  uint64_t packed_mapping = mapping.load(std::memory_order_relaxed);
  if (packed_mapping == 0) {
    return mac_slot_time_info{};
  }
  return {load_sfn(packed_mapping, numerology), load_time(packed_mapping, now)};
}

uint64_t atomic_subframe_time_mapper::pack_sfn_time_mapping(slot_point slot, time_point time)
{
  uint64_t ns_since_epoch    = std::chrono::duration_cast<std::chrono::nanoseconds>(time.time_since_epoch()).count();
  uint64_t truncated_ts_ns   = ns_since_epoch & timestamp_mask;
  uint64_t packed_sfn_sf_idx = slot.sfn() << sf_idx_bits | slot.subframe_index();
  return (packed_sfn_sf_idx << timestamp_bits) | truncated_ts_ns;
}

slot_point atomic_subframe_time_mapper::load_sfn(uint64_t packed_mapping, unsigned numerology)
{
  uint64_t packed_sfn_sf_idx = (packed_mapping & sfn_mask) >> timestamp_bits;
  uint64_t sf_idx            = packed_sfn_sf_idx & 0xf;
  uint64_t sfn_val           = packed_sfn_sf_idx >> sf_idx_bits;
  unsigned nof_slots_per_sf  = get_nof_slots_per_subframe(to_subcarrier_spacing(numerology));
  return slot_point(numerology, sfn_val, sf_idx * nof_slots_per_sf);
}

atomic_subframe_time_mapper::time_point atomic_subframe_time_mapper::load_time(uint64_t packed_mapping, time_point now)
{
  uint64_t truncated_ts_ns = (packed_mapping & timestamp_mask);
  uint64_t now_ns          = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
  // Align now to period boundary.
  uint64_t base = now_ns & ~timestamp_mask;
  // Initial timestamp reconstruction.
  uint64_t candidate = base + truncated_ts_ns;

  // Adjust if the reconstructed timestamp is more than half a period away from now.
  if (candidate > now_ns + period / 2) {
    candidate -= period;
  } else if (candidate < now_ns - period / 2) {
    candidate += period;
  }

  return time_point(std::chrono::nanoseconds(candidate));
}

uint64_t atomic_subframe_time_mapper::extract_sfn_sf_idx(uint64_t packed_mapping)
{
  return (packed_mapping & sfn_mask) >> timestamp_bits;
}

mac_subframe_time_mapper_impl::mac_subframe_time_mapper_impl() : logger(ocudulog::fetch_basic_logger("MAC")) {}

void mac_subframe_time_mapper_impl::handle_slot_indication(const mac_cell_timing_context& context)
{
  if (context.sl_tx.subframe_slot_index() == 0) {
    cur_slot_time_mapping.store(context.sl_tx.without_hyper_sfn(), context.time_point);
  }
}

std::optional<mac_slot_time_info> mac_subframe_time_mapper_impl::get_last_mapping(subcarrier_spacing scs) const
{
  const auto& last = cur_slot_time_mapping.load(to_numerology_value(scs));
  if (OCUDU_UNLIKELY(not last.sl_tx.valid())) {
    logger.warning("Slot point to time point mapping not available.");
    return std::nullopt;
  }

  return mac_slot_time_info{last.sl_tx, last.time_point};
}

std::optional<mac_subframe_time_mapper::time_point> mac_subframe_time_mapper_impl::get_time_point(slot_point slot) const
{
  // Check slot validity first before accessing its numerology
  if (not slot.valid()) {
    logger.warning("Cannot provide time point for the uninitialized slot point.");
    return std::nullopt;
  }

  // Use the slot's own numerology to get the last mapping
  const auto& last = cur_slot_time_mapping.load(slot.numerology());
  if (OCUDU_UNLIKELY(not last.sl_tx.valid())) {
    logger.warning("Slot point to time point mapping not available");
    return std::nullopt;
  }

  if (slot == last.sl_tx) {
    return last.time_point;
  }

  // Calculate slot duration based on the slot's numerology
  usecs      slot_dur        = usecs{1000U >> slot.numerology()};
  const auto time_difference = (slot - last.sl_tx) * slot_dur;
  return last.time_point + time_difference;
}

std::optional<slot_point> mac_subframe_time_mapper_impl::get_slot_point(time_point time, subcarrier_spacing scs) const
{
  unsigned    numerology = to_numerology_value(scs);
  const auto& last       = cur_slot_time_mapping.load(numerology);
  if (OCUDU_UNLIKELY(not last.sl_tx.valid())) {
    logger.warning("Slot point to time point mapping not available.");
    return std::nullopt;
  }

  if (time.time_since_epoch().count() == 0) {
    logger.warning("Cannot provide slot point for the uninitialized time point.");
    return std::nullopt;
  }

  // Calculate slot duration based on the given subcarrier spacing
  usecs      slot_dur        = usecs{1000U >> numerology};
  const auto time_difference = std::chrono::duration_cast<std::chrono::microseconds>(time - last.time_point);
  const auto nof_slots       = time_difference.count() / slot_dur.count();
  return last.sl_tx + nof_slots;
}
