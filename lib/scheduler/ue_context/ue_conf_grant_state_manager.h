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
    if (not time_offset.has_value()) {
      return false;
    }

    return slot.count() % periodicity_slots == time_offset.value();
  }

  bool ue_cg_activated() const { return time_offset.has_value(); }

  /// Update CG state variables.
  void update_state(unsigned                period_slots,
                    units::bytes            tbs_,
                    vrb_interval            vrbs_,
                    uint8_t                 mcs_,
                    uint8_t                 td_alloc,
                    std::optional<unsigned> t_offset,
                    unsigned                res_id)
  {
    periodicity_slots      = period_slots;
    tbs                    = tbs_;
    vrbs                   = vrbs_;
    time_domain_allocation = td_alloc;
    mcs                    = mcs_;
    time_offset            = t_offset;
    resource_id            = res_id;
  }

  void reset_state() { update_state(0U, units::bytes{0U}, {0U, 0U}, 0U, 0U, std::nullopt, 0U); }

  /// Getters for state variables.
  units::bytes get_tbs() const { return time_offset.has_value() ? tbs : units::bytes{0U}; }
  uint8_t      get_mcs() const { return time_offset.has_value() ? mcs : uint8_t{0U}; }
  vrb_interval get_vrbs() const { return time_offset.has_value() ? vrbs : vrb_interval{0U, 0U}; }
  unsigned     get_resource_id() const { return time_offset.has_value() ? resource_id : 0U; }
  uint8_t      get_td_allocation() const { return time_offset.has_value() ? time_domain_allocation : 0U; }
  unsigned     get_cg_time_offset() const { return time_offset.value_or(0U); }
  unsigned     get_periodicity_slots() const { return periodicity_slots; }

private:
  // Values that are subject to change during scheduling life.
  unsigned                periodicity_slots{0U};
  units::bytes            tbs{0U};
  std::optional<unsigned> time_offset;
  uint8_t                 mcs                    = 0U;
  vrb_interval            vrbs                   = {0U, 0U};
  uint8_t                 time_domain_allocation = 0U;
  unsigned                resource_id            = 0U;
};

} // namespace ocudu
