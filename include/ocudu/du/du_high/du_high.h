// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/du/f1ap_du.h"
#include "ocudu/mac/mac_cell_control_information_handler.h"
#include "ocudu/mac/mac_cell_rach_handler.h"
#include "ocudu/mac/mac_cell_slot_handler.h"
#include "ocudu/mac/mac_pdu_handler.h"
#include "ocudu/mac/mac_subframe_time_mapper.h"
#include "ocudu/ran/du_types.h"

namespace ocudu {

namespace ocudu_ntn {
class ntn_configuration_manager;
} // namespace ocudu_ntn

namespace odu {

class du_configurator;

/// Interface to DU-high class, which owns and manages the interaction between MAC, RLC and F1 layers.
class du_high
{
public:
  virtual ~du_high() = default;

  /// Starts this DU, setting it in operational mode. If the DU was already in operational mode, it does nothing.
  virtual void start() = 0;

  /// Stops this DU operation.
  virtual void stop() = 0;

  /// Get handler to received F1AP PDUs.
  virtual f1ap_message_handler& get_f1ap_pdu_handler() = 0;

  /// Get handler to F1AP ID translator.
  virtual f1ap_ue_id_translator& get_f1ap_ue_id_translator() = 0;

  /// \brief Returns handler of slot indications for a cell with provided cell_index.
  /// \param cell_index Index of cell currently activated in the DU.
  virtual mac_cell_slot_handler& get_slot_handler(du_cell_index_t cell_index) = 0;

  /// \brief Returns handler of PRACHs for a given cell.
  /// \param cell_index Index of cell currently activated in the DU.
  virtual mac_cell_rach_handler& get_rach_handler(du_cell_index_t cell_index) = 0;

  /// \brief Returns handler for UL and DL control information for a given cell.
  /// \param cell_index Index of cell currently activated in the DU.
  virtual mac_cell_control_information_handler& get_control_info_handler(du_cell_index_t cell_index) = 0;

  /// \brief Returns MAC Rx PDU handler.
  /// The lower layers will use this interface to forward decoded UL PDUs to the MAC.
  virtual mac_pdu_handler& get_pdu_handler() = 0;

  /// Returns handler to configure the operational serving cells and UEs attached to this DU.
  virtual du_configurator& get_du_configurator() = 0;

  /// Returns a mapper of slots to system time.
  virtual mac_subframe_time_mapper& get_subframe_time_mapper() = 0;

  /// Returns the NTN configuration manager of this DU, or nullptr when no NTN cell is configured.
  virtual ocudu_ntn::ntn_configuration_manager* get_ntn_configuration_manager() = 0;
};

} // namespace odu
} // namespace ocudu
