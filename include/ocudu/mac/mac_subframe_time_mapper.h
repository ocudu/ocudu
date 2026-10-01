// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/slot_point.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include <chrono>
#include <optional>

namespace ocudu {

struct mac_cell_timing_context;

/// Contains the mapping between slot point and time point.
struct mac_slot_time_info {
  /// Indicates the current slot.
  slot_point sl_tx;
  /// Indicates the system time point associated to the current slot.
  std::chrono::time_point<std::chrono::system_clock> time_point;
};

/// Interface used to handle slot indications.
class mac_slot_time_handler
{
public:
  virtual ~mac_slot_time_handler() = default;

  /// \brief Processing of a new Slot Indication.
  /// \param[in] context Indication context.
  virtual void handle_slot_indication(const mac_cell_timing_context& context) = 0;
};

/// Interface used to retrieve slot point-to-time point mapping at subframe level.
/// This mapper receives indications from all cells and stores only the first mapping per subframe.
class mac_subframe_time_mapper
{
public:
  using time_point                    = std::chrono::time_point<std::chrono::system_clock>;
  virtual ~mac_subframe_time_mapper() = default;

  /// \brief Provides the last slot point to time point mapping for the given subcarrier spacing.
  /// This function returns empty optional if the mapping is not available.
  /// \param scs The subcarrier spacing to use for slot point reconstruction.
  /// \return Mapping between slot point and time point or empty.
  virtual std::optional<mac_slot_time_info> get_last_mapping(subcarrier_spacing scs) const = 0;

  /// \brief Provides time point for the given slot point.
  /// This function returns empty optional if the mapping is not available or if the slot point is not initialized.
  /// \param slot The slot point to convert.
  /// \return Time point for the given slot point or empty.
  virtual std::optional<time_point> get_time_point(slot_point slot) const = 0;

  /// \brief Provides slot point for the given time point and subcarrier spacing.
  /// This function returns empty optional if the mapping is not available or if the time point is not initialized.
  /// \param time The time point to convert.
  /// \param scs The subcarrier spacing to use for slot point reconstruction.
  /// \return Slot point for the given time point or empty.
  virtual std::optional<slot_point> get_slot_point(time_point time, subcarrier_spacing scs) const = 0;
};

} // namespace ocudu
