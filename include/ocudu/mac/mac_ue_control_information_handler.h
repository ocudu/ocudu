// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/du_types.h"
#include "ocudu/ran/logical_channel/lcid.h"
#include <chrono>
#include <optional>

namespace ocudu {

/// DL Buffer state for a given RLC bearer.
struct mac_dl_buffer_state_indication_message {
  du_ue_index_t ue_index;
  lcid_t        lcid;
  /// Buffer Occupancy value in bytes.
  unsigned bs;
  /// \brief Time-of-arrival of the oldest PDU in the RLC entity Tx buffer. This metric is relevant for delay
  /// prioritization in the scheduler.
  std::optional<std::chrono::steady_clock::time_point> hol_toa;
};

class mac_ue_control_information_handler
{
public:
  virtual ~mac_ue_control_information_handler() = default;

  /// Forwards a new DL buffer state for a given UE and logical channel to the MAC.
  /// \param dl_bs Updated DL buffer state information for a logical channel.
  virtual void handle_dl_buffer_state_update(const mac_dl_buffer_state_indication_message& dl_bs) = 0;
};

} // namespace ocudu
