// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

class mac_cell_manager;
class mac_ue_configurator;
class mac_positioning_measurement_handler;
class mac_subframe_time_mapper;

/// Interface used by the management plane of the DU.
class mac_manager
{
public:
  virtual ~mac_manager() = default;

  /// Interface to manage the creation, reconfiguration, deletion, activation and deactivation of cells.
  virtual mac_cell_manager& get_cell_manager() = 0;

  /// Interface to manage the creation, reconfiguration and deletion of UEs in the MAC.
  virtual mac_ue_configurator& get_ue_configurator() = 0;

  /// Fetch positioning measurement handler.
  virtual mac_positioning_measurement_handler& get_positioning_handler() = 0;

  /// Get the DU-wide subframe-time mapper that aggregates timing from all cells.
  /// This mapper receives slot indications from all cells but only stores
  /// the first indication for each new subframe.
  virtual mac_subframe_time_mapper& get_subframe_time_mapper() = 0;
};

} // namespace ocudu
