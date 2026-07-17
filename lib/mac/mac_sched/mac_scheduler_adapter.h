// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../mac_ctrl/mac_scheduler_configurator.h"
#include "../mac_dl/mac_scheduler_cell_info_handler.h"
#include "../mac_ul/mac_scheduler_ce_info_handler.h"
#include "ocudu/mac/mac_cell_control_information_handler.h"
#include "ocudu/mac/mac_cell_rach_handler.h"
#include "ocudu/mac/mac_paging_information_handler.h"

namespace ocudu {

/// \brief Interface from MAC to a scheduler implementation.
class mac_scheduler_adapter : public mac_scheduler_configurator,
                              public mac_scheduler_ce_info_handler,
                              public mac_scheduler_cell_info_handler,
                              public mac_paging_information_handler
{
public:
  virtual ~mac_scheduler_adapter() = default;

  /// \brief Gets the control info handler for a given cell.
  ///
  /// \param cell_index DU-specific index of the cell for which the control info handler is being retrieved.
  /// \return Cell-specific control info handler.
  virtual mac_cell_control_information_handler& get_cell_control_info_handler(du_cell_index_t cell_index) = 0;

  /// \brief Gets the positioning measurement handler.
  /// \return Positioning measurement handler.
  virtual mac_positioning_measurement_handler& get_positioning_handler() = 0;
};

} // namespace ocudu
