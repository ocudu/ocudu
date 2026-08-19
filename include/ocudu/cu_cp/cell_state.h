// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu::ocucp {

/// \brief Administrative state of a CU-CP logical cell: what the operator wants the cell to do.
///
/// Declared in configuration and changed by operator commands; the CU-CP holds shutting_down itself while a
/// graceful stop is in progress.
enum class cell_admin_state : uint8_t {
  /// The CU-CP activates the cell whenever its DU reports it.
  unlocked,
  /// The CU-CP keeps the cell deactivated until it is unlocked by command.
  locked,
  /// Transient state held while a graceful stop (bar, UE release, deactivation) drains the cell; the cell
  /// becomes locked when the stop completes, and returns to its previous state when the stop fails.
  shutting_down
};

/// \brief Operational state of a CU-CP logical cell: whether the cell is active at its realizing DU.
///
/// Recorded by the CU-CP at every activation/deactivation it drives, so an unlocked cell whose activation
/// failed (operational state disabled) is distinguishable from an active one.
enum class cell_operational_state : uint8_t { disabled, enabled };

/// Return a string representation of the given administrative state.
inline const char* to_string(cell_admin_state state)
{
  switch (state) {
    case cell_admin_state::unlocked:
      return "unlocked";
    case cell_admin_state::locked:
      return "locked";
    case cell_admin_state::shutting_down:
      return "shutting_down";
    default:
      break;
  }
  return "invalid";
}

/// Return a string representation of the given operational state.
inline const char* to_string(cell_operational_state state)
{
  switch (state) {
    case cell_operational_state::disabled:
      return "disabled";
    case cell_operational_state::enabled:
      return "enabled";
    default:
      break;
  }
  return "invalid";
}

} // namespace ocudu::ocucp
