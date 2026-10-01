// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/mac/mac_cell_control_information_handler.h"
#include "ocudu/mac/mac_cell_rach_handler.h"
#include "ocudu/mac/mac_cell_slot_handler.h"
#include "ocudu/mac/mac_manager.h"
#include "ocudu/mac/mac_paging_information_handler.h"
#include "ocudu/mac/mac_pdu_handler.h"
#include "ocudu/mac/mac_ue_control_information_handler.h"

namespace ocudu {

class mac_interface : public mac_manager
{
public:
  virtual ~mac_interface() = default;

  /// Returns handler of PRACHs.
  virtual mac_cell_rach_handler& get_rach_handler(du_cell_index_t cell_index) = 0;

  /// Returns handler for UL and DL control information for a given cell.
  virtual mac_cell_control_information_handler& get_control_info_handler(du_cell_index_t cell_index) = 0;

  /// Returns handler for UE control information.
  virtual mac_ue_control_information_handler& get_ue_control_info_handler() = 0;

  /// \brief Returns handler of slot indications for a cell with provided cell_index.
  /// \param cell_index Index of cell currently activated in the DU.
  virtual mac_cell_slot_handler& get_slot_handler(du_cell_index_t cell_index) = 0;

  /// Returns PDU handler.
  virtual mac_pdu_handler& get_pdu_handler() = 0;

  /// Returns Paging handler.
  virtual mac_paging_information_handler& get_cell_paging_info_handler() = 0;
};

} // namespace ocudu
