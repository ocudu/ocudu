// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cell/resource_grid.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/prach/ssb_to_ro_mapping.h"
#include "ocudu/ran/slot_point.h"

namespace ocudu {

class prach_scheduler
{
public:
  explicit prach_scheduler(const cell_configuration& cfg_);

  /// Allocate RACH PDUs for the given slot.
  void run_slot(cell_resource_allocator& res_grid);

  void stop();

private:
  // [Implementation-defined] This is the maximum number of slots per PRACH preamble. It's obtained by considering the
  // longest preamble length (which can be derived from Table 6.3.3.1-1, TS 38.211 for Format 2) and the shortest slot
  // duration currently supported by the GNB, which is 0.5ms for SCS 30KHz.
  static constexpr unsigned MAX_SLOTS_PER_PRACH = 7;

  struct cached_prach_occasion {
    /// RB x symbol resources used for the PRACH.
    static_vector<grant_info, MAX_SLOTS_PER_PRACH> grant_list;
    /// Pre-generated PRACH occasion.
    prach_occasion_info occasion;
  };

  const prach_helper::preamble_slot_mapping& td_mapper() const { return ssb_ro_map.td_slot_mapping(); }

  const rach_config_common& rach_cfg_common() const
  {
    return *cell_cfg.params.ul_cfg_common.init_ul_bwp.rach_cfg_common;
  }

  /// \brief Performs PRACH allocation for this slot.
  ///
  /// One PRACH PDU per preamble for long preambles, and 1 or 2 PRACH PDUs for short preambles depending on
  /// prach_length_slots. In the case of short preamble, when the burst of PRACH opportunities spans over 2 (SCS common)
  /// slots,  we allocate a PRACH PDU for each of these 2 slots, as expected from the scheduler output interface.
  void allocate_slot_prach_pdus(cell_resource_allocator& res_grid, slot_point sl);

  const cell_configuration& cell_cfg;
  ocudulog::basic_logger&   logger;
  /// Helper to determine the slot positioning of the PRACH preambles and which of their occasions are valid.
  prach_helper::ssb_to_ro_mapping ssb_ro_map;

  bool first_slot_ind = true;
  /// Pre-generated PRACH occasions.
  static_vector<cached_prach_occasion, MAX_PRACH_OCCASIONS_PER_SLOT> cached_prachs;
};

} // namespace ocudu
