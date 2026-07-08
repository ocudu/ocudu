// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/ntn.h"
#include <chrono>
#include <optional>
#include <variant>

namespace ocudu {

namespace ocudu_ntn {

/// NTN Config update for a single cell.
struct ntn_cell_config_update_info {
  using time_point = std::chrono::system_clock::time_point;
  nr_cell_global_id_t                   nr_cgi;
  time_point                            epoch_time;
  unsigned                              ntn_ul_sync_validity_duration;
  ntn_ephemeris_info_t                  ephemeris_info;
  std::optional<ta_info_t>              ta_info;
  std::optional<feeder_link_info_t>     feeder_link_info;
  std::optional<geodetic_coordinates_t> ntn_gateway_location;
  // SIB19 value-tag-tracked fields (changes require SIB1 systemInfoValueTag increment).
  std::optional<geodetic_coordinates_t> reference_location;
  std::optional<unsigned>               distance_threshold;
  std::optional<time_point>             t_service;
  std::optional<ntn_polarization_t>     polarization;
  std::optional<bool>                   ta_report;
  // SIB19 tracked neighbor-cell list.
  std::optional<std::vector<neighbor_ntn_cell>> ncells;
  // SIB19 exempt fields (changes do not trigger SIB1 systemInfoValueTag increment).
  std::optional<geodetic_coordinates_t> moving_ref_location;
  // SIB19 tracked sat-switch field.
  std::optional<sat_switch_with_resync_t> sat_switch_with_resync;
};

/// NTN Config update message to be received over a websocket interface.
struct ntn_config_update_info {
  /// Per-cell NTN config updates.
  std::vector<ntn_cell_config_update_info> cells;
};

/// Result of NTN configuration update operation.
struct ntn_config_update_result {
  /// Cells that were successfully updated.
  std::vector<nr_cell_global_id_t> succeeded;
  /// Cells that failed to update.
  std::vector<nr_cell_global_id_t> failed;
};

/// Public NTN configuration manager interface.
class ntn_configuration_manager
{
public:
  virtual ~ntn_configuration_manager() = default;

  /// \brief Handle NTN configuration update request for one or more cells.
  ///
  /// This function processes NTN configuration updates, including generating timestamped SIB19 PDUs and setting the
  /// Doppler shift for PHY layer pre- and post-compensation for the feeder link.
  /// \param req NTN configuration update request containing one or more cell configurations.
  /// \return Result containing lists of successfully updated and failed cells.
  virtual ntn_config_update_result handle_ntn_config_update(const ntn_config_update_info& req) = 0;
};

} // namespace ocudu_ntn
} // namespace ocudu
