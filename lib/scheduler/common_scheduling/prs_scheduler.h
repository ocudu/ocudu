// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cell/resource_grid.h"
#include "../config/cell_configuration.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/prs/prs_constants.h"
#include <vector>

namespace ocudu {

/// Scheduler of DL-PRS transmission occasions of a cell.
class prs_scheduler
{
public:
  explicit prs_scheduler(const cell_configuration& cell_cfg);

  /// \brief Schedules the DL-PRS occasions of the cell.
  ///
  /// The occasions are scheduled as far in advance as the resource grid allows, so that they are reserved before any
  /// other DL allocation of the same slot takes place.
  void run_slot(cell_resource_allocator& res_alloc);

  /// Called when the cell is deactivated, so that the resource grid is repopulated on the next activation.
  void stop();

private:
  struct cached_resource_set {
    /// Resource set periodicity, in slots. See \ref prs_constants::VALID_PERIODICITIES.
    uint32_t period;
    /// First slot of the transmission window within the period, i.e., the earliest occasion of the earliest resource.
    uint32_t window_start;
    /// Length of the transmission window, in slots.
    uint16_t span;
    /// Muting Option 1 pattern, governs muted resource set instances.
    uint32_t muting1_pattern;
    /// Size of \ref muting1_pattern, in bits. Zero when Muting Option 1 is not configured.
    uint8_t muting1_pattern_size;
    /// Number of consecutive resource set instances muted by each bit of \ref muting1_pattern.
    uint8_t muting1_rep_factor;
    /// \brief Muting Option 2 pattern, governs muted repetitions.
    ///
    /// Bits at or above the repetition factor are cleared, so that repetitions past the last one of a resource are
    /// discarded without wrapping the repetition index, as the formula of TS 38.211, Section 7.4.1.7.4 does.
    uint32_t muting2_pattern;
    /// Time gap between repetitions. See \ref prs_time_gap.
    uint8_t time_gap;
    /// Index in \ref cached_pdus of the PDU of the first resource of the set. The PDUs of a set are contiguous.
    uint16_t first_pdu_idx;
    /// \brief Slot offset of the first transmitted repetition of each resource, relative to \ref window_start.
    static_vector<uint16_t, prs_constants::MAX_NOF_RESOURCES_PER_SET> res_offsets;
  };

  /// Schedule DL-PRS occasions on a given slot.
  void schedule_slot(cell_slot_resource_allocator& slot_alloc);

  ocudulog::basic_logger& logger;

  /// Flag indicating whether \c run_slot is called for the first time or not.
  bool first_run_slot{true};

  /// \brief PDU of each resource of each resource set.
  ///
  /// No field of a PDU depends on the slot, so all the repetitions of a resource share a single entry.
  std::vector<prs_info> cached_pdus;

  /// Cached information about each resource set.
  std::vector<cached_resource_set> sets;
};

} // namespace ocudu
