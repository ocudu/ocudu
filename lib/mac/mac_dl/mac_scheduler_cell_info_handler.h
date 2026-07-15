// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/mac/mac_cell_manager.h"
#include "ocudu/mac/mac_cell_slot_handler.h"
#include "ocudu/mac/mac_ue_control_information_handler.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/slot_point_extended.h"
#include "ocudu/support/units.h"
#include <optional>

namespace ocudu {

struct si_scheduling_update_request;
struct sched_result;

/// \brief Interface used by MAC Cell Processor to interact with the MAC scheduler.
class mac_scheduler_cell_info_handler : public mac_ue_control_information_handler
{
public:
  virtual ~mac_scheduler_cell_info_handler() override = default;

  /// \brief Start scheduling for a given cell. If cell was already activated, this operation has no effect.
  /// \param cell_idx DU-specific index of the cell for which the slot is being processed.
  /// \remark This function must be called before the first slot indication is processed.
  virtual void handle_cell_activation(du_cell_index_t cell_idx) = 0;

  /// \brief Stop running cell. If cell was already deactivated, this operation has no effect.
  /// \param cell_idx DU-specific index of the cell for which the slot is being processed.
  /// \remark This function must be called after the last slot indication is processed.
  virtual void handle_cell_deactivation(du_cell_index_t cell_idx) = 0;

  /// \brief Processes a new slot for a specific cell in the MAC scheduler.
  /// \param slot_tx SFN + slot index of the Transmit slot to be processed.
  /// \param cell_idx DU-specific index of the cell for which the slot is being processed.
  /// \return Result of the scheduling operation. It contains both DL and UL scheduling information.
  virtual const sched_result& slot_indication(slot_point_extended slot_tx, du_cell_index_t cell_idx) = 0;

  /// \brief Processes an error indication for a specific cell in the MAC scheduler.
  /// \param slot_tx SFN + slot index of the Transmit slot to be processed.
  /// \param cell_idx DU-specific index of the cell for which the indication is being processed.
  /// \param event Effect that the errors in the lower layers had on the result provided by the scheduler.
  virtual void
  handle_error_indication(slot_point slot_tx, du_cell_index_t cell_idx, mac_cell_slot_handler::error_event event) = 0;

  /// \brief Update SIB1 and SI scheduling information in scheduler.
  /// \param[in] request Request to change SI sched info and messages.
  virtual void handle_si_change_indication(const si_scheduling_update_request& request) = 0;

  /// \brief Request the scheduler to broadcast a PWS (ETWS/CMAS) short-message notification and activate the target
  /// SI-message. If \c nof_segments has a value, the SI-message is activated for one complete broadcast (repetition
  /// is handled entirely by the MAC layer, which re-issues this request once per broadcast occurrence); if
  /// \c std::nullopt, the SI-message broadcasts indefinitely.
  /// \param[in] cell_idx DU-specific index of the cell for which the indication is directed.
  /// \param[in] si_msg_idx Index of the SI-message carrying the SIB6/7/8 to activate.
  /// \param[in] nof_segments Number of segments composing the warning message, or \c std::nullopt for indefinite.
  /// \param[in] msg_len Length, in bytes, of the largest segment of the warning message being activated. Used to
  /// size the PDSCH grant for this SI-message while active.
  virtual void handle_pws_broadcast_indication(du_cell_index_t         cell_idx,
                                               unsigned                si_msg_idx,
                                               std::optional<unsigned> nof_segments,
                                               units::bytes            msg_len) = 0;

  /// \brief Handle request to update the slice configuration of a cell.
  /// \param[in] cell_index Index of the cell for which the measurement is directed.
  /// \param[in] req Request to update the RRM policies.
  virtual void handle_slice_reconfiguration_request(const du_cell_slice_reconfig_request& req) = 0;
};

} // namespace ocudu
