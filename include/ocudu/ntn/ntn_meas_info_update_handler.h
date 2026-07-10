// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/ntn.h"
#include <optional>
#include <vector>

namespace ocudu {
namespace ocudu_ntn {

/// Propagated NTN assistance information for one neighbour cell, used to populate ntn-NeighbourCellInfo-r18 in
/// MeasObjectNR (TS 38.331).
struct ntn_neighbour_meas_info {
  /// NR cell identity of the neighbour cell.
  nr_cell_identity nci;
  /// Epoch time of the ephemeris, expressed in the serving cell SFN/subframe.
  epoch_time_t epoch_time;
  /// Propagated satellite ephemeris at the epoch time.
  ntn_ephemeris_info_t ephemeris;
  /// 2-D reference location of the neighbour cell (in degrees).
  std::optional<geodetic_coordinates_t> ref_location;
  /// DL/UL polarization of the neighbour cell (ntn-PolarizationDL/UL-r17).
  std::optional<ntn_polarization_t> polarization;
};

/// Request structure for measurement-related NTN neighbour info update operations.
struct ntn_meas_info_update_request {
  /// NR Cell Global ID of the serving cell whose neighbour measurement info is updated.
  nr_cell_global_id_t serving_cgi;
  /// Updated NTN neighbour cell info, one entry per configured NTN neighbour cell.
  std::vector<ntn_neighbour_meas_info> ncells;
};

/// Interface for handling NTN neighbour cell measurement info updates.
class ntn_meas_info_update_handler
{
public:
  virtual ~ntn_meas_info_update_handler() = default;

  /// \brief Handle an NTN neighbour cell measurement info update request.
  ///
  /// This function forwards freshly propagated per-neighbour ephemeris and epoch time to the consumer responsible for
  /// building measurement configurations (e.g. the CU-CP cell measurement manager).
  /// \param req Information required to perform the update.
  virtual void handle_ntn_meas_info_update(const ntn_meas_info_update_request& req) = 0;
};

} // namespace ocudu_ntn
} // namespace ocudu
