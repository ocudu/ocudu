// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/slot_point.h"
#include <chrono>
#include <optional>

namespace ocudu {
namespace ocudu_ntn {

/// Time slot mapping information for NTN context.
struct ntn_time_slot_mapping {
  /// Slot point for transmission.
  slot_point slot_tx;
  /// Corresponding time point.
  std::chrono::time_point<std::chrono::system_clock> time_point;
};

/// Interface for providing time information in NTN context.
class ntn_time_provider
{
public:
  virtual ~ntn_time_provider() = default;

  /// \brief Get the last time slot mapping for the timeline of the given cell.
  ///
  /// This function provides access to the most recent time slot mapping for time synchronization operations in NTN
  /// context. Implementations serving a single SFN timeline (e.g. one DU) may ignore the cell key; implementations
  /// spanning multiple DUs (e.g. the CU-CP) use it to select the timeline of the DU serving the cell.
  /// \param nr_cgi NR Cell Global ID identifying the timeline to query.
  /// \param scs Subcarrier spacing of the returned slot point.
  /// \return Optional containing the time slot mapping if available.
  virtual std::optional<ntn_time_slot_mapping> get_last_mapping(const nr_cell_global_id_t& nr_cgi,
                                                                subcarrier_spacing         scs) = 0;
};

} // namespace ocudu_ntn
} // namespace ocudu
