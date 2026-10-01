// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "mac_scheduler_cell_configurator.h"
#include "ocudu/mac/mac_cell_manager.h"
#include "ocudu/mac/mac_cell_rach_handler.h"
#include "ocudu/mac/mac_cell_slot_handler.h"
#include "ocudu/mac/mac_ue_control_information_handler.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/slot_point_extended.h"
#include "ocudu/support/units.h"
#include <optional>

namespace ocudu {

struct sched_result;

/// \brief Interface used by MAC Cell Processor to interact with the MAC scheduler.
class mac_scheduler_cell_info_handler : public mac_ue_control_information_handler,
                                        public mac_scheduler_cell_configurator
{
public:
  ~mac_scheduler_cell_info_handler() override = default;

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

  /// \brief Gets the RACH handler for a given cell.
  /// \param cell_index DU-specific index of the cell for which a RACH handler is being retrieved.
  /// \return Cell-specific RACH handler.
  virtual mac_cell_rach_handler& get_cell_rach_handler(du_cell_index_t cell_index) = 0;
};

} // namespace ocudu
