// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/mac/cell_configuration.h"
#include "ocudu/mac/mac_ue_configurator.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/slot_point.h"

namespace ocudu {

class mac_subframe_time_mapper;
class mac_positioning_measurement_handler;

/// Start configured cell.
struct mac_cell_start {
  du_cell_index_t cell_index;
  slot_point      slot;
};

/// Network slice configuration
struct mac_slice_configuration {
  // TODO: Fill remaining fields
};

/// \brief Requests the MAC/scheduler to repeat a PWS (ETWS/CMAS) short-message broadcast indication a given number of
/// times at a given cadence, as per TS 38.473, Section 8.5.1 "Repetition Period"/"Number of Broadcasts Requested".
struct pws_broadcast_indication {
  /// Repetition Period.
  std::chrono::seconds repeat_period;
  /// Number of Broadcasts Requested.
  uint32_t nof_broadcasts_requested;
};

/// Structure used to update SI PDU messages, without SI change notifications nor in a modification of valueTag in SIB1.
struct mac_cell_sys_info_pdu_update {
  /// SI message index.
  unsigned si_msg_idx;
  /// SIB index (e.g., sib2 => value 2).
  uint8_t sib_idx;
  /// Slot at which the new SI is transmitted. If absent, the update is applied as soon as possible (e.g. PWS).
  std::optional<slot_point> slot;
  /// SI period in nof slots, required if more than one are SI PDU passed.
  std::optional<unsigned> si_slot_period;
  /// Packed content of SIB messages.
  span<byte_buffer> si_messages;
  /// If not empty, requests a repeated PWS short-message broadcast indication for this SI PDU update.
  std::optional<pws_broadcast_indication> pws_broadcast;
};

/// Reconfiguration of a MAC cell during its operation.
struct mac_cell_reconfig_request {
  /// If not empty, contains the new system information to broadcast.
  std::optional<mac_cell_sys_info_config> new_sys_info;
  /// If not empty, contains the new SI PDU to be updated.
  std::optional<mac_cell_sys_info_pdu_update> new_si_pdu_info;
  /// If not empty, contains the updates to be applied to the RRM policies.
  std::optional<du_cell_slice_reconfig_request> slice_reconf_req;
};

struct mac_cell_reconfig_response {
  /// Whether a pending SI reconfiguration was successful.
  bool si_updated = false;
  /// Whether a SI PDUs were successfully enqueued.
  bool si_pdus_enqueued = false;
};

/// Interface used to handle a MAC cell activation/reconfiguration/deactivation.
class mac_cell_controller
{
public:
  virtual ~mac_cell_controller() = default;

  /// Start the cell.
  virtual async_task<void> start() = 0;

  /// Stop the cell.
  virtual async_task<void> stop() = 0;

  /// Reconfigure operation cell.
  virtual async_task<mac_cell_reconfig_response> reconfigure(const mac_cell_reconfig_request& request) = 0;
};

/// Class used to setup the MAC cells and slices.
class mac_cell_manager
{
public:
  virtual ~mac_cell_manager() = default;

  /// Add new cell configuration.
  virtual mac_cell_controller& add_cell(const mac_cell_creation_request& cell_cfg) = 0;

  /// Remove an existing cell configuration.
  virtual void remove_cell(du_cell_index_t cell_index) = 0;

  /// Fetch MAC cell state controller.
  virtual mac_cell_controller& get_cell_controller(du_cell_index_t cell_index) = 0;

  /// Fetch MAC subframe-time mapper.
  virtual mac_subframe_time_mapper& get_subframe_time_mapper() = 0;
};

} // namespace ocudu
