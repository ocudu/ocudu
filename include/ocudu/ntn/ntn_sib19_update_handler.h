// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/sib/system_info_config.h"
#include "ocudu/ran/slot_point.h"
#include <chrono>
#include <optional>

namespace ocudu {
namespace ocudu_ntn {

/// Request structure for SIB19 PDU update operations.
struct ntn_sib19_update_request {
  using time_point = std::chrono::system_clock::time_point;

  /// NR Cell Global ID of the cell being configured.
  nr_cell_global_id_t nr_cgi;
  /// Slot at which the SIB19 is transmitted.
  slot_point slot;
  /// SI period in number of slots.
  std::optional<unsigned> si_slot_period;
  /// SIB19 information to be packed and transmitted.
  sib19_info sib19;
  /// Epoch time for the SIB19 update.
  time_point epoch_time;
  /// \brief Uplink timing advance T_TA of a UE at the cell reference location (TS 38.211, Section 4.3.1).
  ///
  /// Not broadcast: it is a gNB-internal estimate, see \c ntn_cell_params::ref_location_ul_ta. Absent for TN cells and
  /// whenever it could not be computed, e.g. when no reference location is configured.
  std::optional<std::chrono::microseconds> ref_location_ul_ta;
  /// When true, the handler must set sib19 in the DU request to trigger
  /// a SIB1 systemInfoValueTag increment.
  bool si_valuetag_change = false;
};

/// Interface for handling SIB19 updates in NTN context.
class ntn_sib19_update_handler
{
public:
  virtual ~ntn_sib19_update_handler() = default;

  /// \brief Handle a SIB19 update request.
  ///
  /// This function processes a SIB19 update request, including validation and forwarding to the appropriate DU
  /// components.
  /// \param req Information required to perform the SIB19 update.
  virtual void handle_sib19_msg_update(const ntn_sib19_update_request& req) = 0;
};

} // namespace ocudu_ntn
} // namespace ocudu
