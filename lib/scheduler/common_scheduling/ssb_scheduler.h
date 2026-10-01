// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cell/resource_grid.h"
#include "../config/cell_configuration.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/slot_point.h"

namespace ocudu {

enum class ssb_pattern_case;
struct cell_slot_resource_allocator;

using ssb_information_list = static_vector<ssb_information, MAX_SSB_PER_SLOT>;

class ssb_scheduler
{
public:
  explicit ssb_scheduler(const cell_configuration& cfg_);

  /// \brief Schedule grant(s) for SSB.
  ///
  /// The functions schedules the SSB according to a given periodicity, depending on the frequency and SSB case for up
  /// to N slots when run for first time. In subsequent runs schedules SSB on N + 1 slot.
  ///
  /// \param[out] res_allocator  Cell resource grid.
  /// \param[in]  sl_point       Slot point carrying information about current slot.
  ///
  /// \remark This function only works for FR1, or L_max = 4 or 8.
  void run_slot(cell_resource_allocator& res_allocator, slot_point sl_point);

  /// \brief Schedule grant for SSB.
  ///
  /// The functions schedules the SSB per slot according to a given periodicity, depending on the frequency and
  /// SSB case.
  ///
  /// \param[out] slot_allocator Slot allocator object with SSB grants and DL PRBs.
  ///
  /// \remark This function only works for FR1, or L_max = 4 or 8.
  void schedule_ssb(cell_slot_resource_allocator& slot_allocator) const;

  /// Called when cell is deactivated.
  void stop();

private:
  /// \brief Perform allocation for case A and C (both paired and unpaired spectrum) - TS 38.213, Section 4.1.
  void ssb_alloc_case_A_C(ssb_information_list& ssb_list, arfcn_t freq_arfcn_cut_off, slot_point sl_point_mod) const;

  /// \brief Perform SSB allocation for case B (both paired and unpaired spectrum) - TS 38.213, Section 4.1.
  void ssb_alloc_case_B(ssb_information_list& ssb_list, slot_point sl_point_mod) const;

  /// \brief Perform SSB allocation for case D for FR2 120kHz SCS - TS 38.213, Section 4.1.
  void ssb_alloc_case_D(ssb_information_list& ssb_list, slot_point sl_point_mod) const;

  static void fill_ssb_parameters(ssb_information_list& ssb_list,
                                  ssb_offset_to_pointA  offset_to_point_A,
                                  ssb_subcarrier_offset ssb_subc_offset,
                                  subcarrier_spacing    ssb_scs,
                                  subcarrier_spacing    scs_common,
                                  uint8_t               ssb_burst_symb_idx,
                                  uint8_t               ssb_idx);

  /// Flag indicating whether \c run_slot is called for the first time or not.
  bool first_run_slot{true};

  /// Periodicity of SSB in milliseconds (or in nof. subframes).
  uint8_t ssb_period;

  /// SSB pattern case, derived from the cell band and SSB SCS at construction.
  ssb_pattern_case ssb_case;

  const cell_configuration& cell_cfg;
  ocudulog::basic_logger&   logger;
};

} // namespace ocudu
