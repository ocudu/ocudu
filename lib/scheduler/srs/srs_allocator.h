// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/slot_point.h"

namespace ocudu {

struct cell_resource_allocator;
class ue_cell_configuration;

/// Structs with aperiodic SRS allocation results.
struct aperiodic_srs_alloc_info {
  /// \c aperiodicSRS-ResourceTrigger, as per \c SRS-Config, TS 38.331.
  unsigned aperiodic_srs_res_trigger = 0U;
  /// \c slotOffset, as per \c SRS-Config, TS 38.331. This applies only if aperiodic_srs_res_trigger != 0.
  unsigned slot_offset = 0U;
};

/// SRS allocator interface, which handles the allocation of aperiodic SRS grants.
class srs_allocator
{
public:
  virtual ~srs_allocator() = default;

  /// \brief Signals the SRS allocator the start of a new slot
  virtual void slot_indication(slot_point slot_tx) = 0;

  /// \brief Attempts to allocate an aperiodic SRS.
  /// \param[out,in] res_alloc struct with scheduling results.
  /// \param[in] last_srs_slot slot at which this UE has its last aperiodic SRS allocated.
  /// \param[in] ue_cfg user configuration.
  /// \return Returns the SRS allocation result. \ref aperiodic_srs_alloc_info::aperiodic_srs_res_trigger is different
  /// from 0 if the allocation is successful, 0 otherwise.
  virtual aperiodic_srs_alloc_info allocate_aperiodic_srs(cell_resource_allocator&     res_alloc,
                                                          slot_point                   last_srs_slot,
                                                          const ue_cell_configuration& ue_cfg) = 0;
};

} // namespace ocudu
