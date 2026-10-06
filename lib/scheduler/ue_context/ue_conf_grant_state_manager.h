// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/configured_grant/cg_configuration.h"
#include "ocudu/ran/resource_allocation/rb_interval.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/support/units.h"
#include <optional>

namespace ocudu {

/// \brief Parameters of an active CG allocation.
///
/// The parameters are only meaningful as a set: either the UE has an active CG and all of them are valid, or it has
/// none and none of them are. \ref ue_conf_grant_state_manager hands them out wrapped in a \c std::optional, so that
/// the activation check cannot be skipped.
struct cg_grant_params {
  unsigned     periodicity_slots;
  unsigned     time_offset;
  units::bytes tbs;
  vrb_interval vrbs;
  uint8_t      mcs;
  uint8_t      time_domain_allocation;
  /// Resource id is only applicable to CG type 2.
  unsigned resource_id;
};

/// /brief Class that keeps track of CG state variables.
///
/// Motivated by CG type 2, in which TBS, MCS, VRBs and slot offset and TD allocation can potentially change during the
/// scheduling life. The class provides an API that can be used for both CG type 1 and type 2.
class ue_conf_grant_state_manager
{
public:
  explicit ue_conf_grant_state_manager() = default;

  /// Test whether this is a slot with CG opportunity.
  bool is_cg_slot(slot_point slot) const
  {
    if (not state.has_value()) {
      return false;
    }

    return slot.count() % state->periodicity_slots == state->time_offset;
  }

  bool ue_cg_activated() const { return state.has_value(); }

  /// Update CG state variables.
  void update_state(unsigned     period_slots,
                    units::bytes tbs,
                    vrb_interval vrbs,
                    uint8_t      mcs,
                    uint8_t      td_alloc,
                    unsigned     t_offset,
                    unsigned     res_id)
  {
    state = cg_grant_params{period_slots, t_offset, tbs, vrbs, mcs, td_alloc, res_id};
  }

  void reset_state() { state.reset(); }

  /// Parameters of the active CG allocation, or \c std::nullopt if the UE has no active CG.
  const std::optional<cg_grant_params>& get_grant_params() const { return state; }

private:
  // Values that are subject to change during scheduling life.
  std::optional<cg_grant_params> state;
};

} // namespace ocudu
