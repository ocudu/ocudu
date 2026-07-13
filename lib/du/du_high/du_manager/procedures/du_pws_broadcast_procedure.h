// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/du/du_high/du_manager/du_manager_params.h"
#include "ocudu/f1ap/du/f1ap_du.h"
#include "ocudu/mac/mac_cell_manager.h"
#include "ocudu/ocudulog/logger.h"

namespace ocudu {
namespace odu {

class du_cell_manager;

/// \brief Procedure that pushes a Write-Replace Warning (PWS) SIB6/7/8 update to the MAC/scheduler of each targeted
/// cell, as per TS 38.473, Section 8.5.1.
class du_pws_broadcast_procedure
{
public:
  du_pws_broadcast_procedure(const write_replace_warning_information& req_,
                             const du_manager_params&                 du_params_,
                             du_cell_manager&                         cell_mng_);

  void operator()(coro_context<async_task<std::vector<du_cell_index_t>>>& ctx);

private:
  /// \brief Push the warning SIB content and repeat/count indication to a single cell's MAC.
  /// \return Whether the cell accepted the broadcast (i.e. was statically provisioned with a scheduling slot for
  /// the requested SIB type).
  async_task<mac_cell_reconfig_response> handle_cell_broadcast(du_cell_index_t cell_index);

  const write_replace_warning_information request;
  const du_manager_params&                du_params;
  du_cell_manager&                        cell_mng;
  ocudulog::basic_logger&                 logger;

  unsigned                     next_cell_idx = 0;
  std::vector<du_cell_index_t> accepted_cells;

  /// Storage backing the current cell's \c mac_cell_sys_info_pdu_update::si_messages span. Repopulated for each
  /// cell right before the (sequentially awaited) MAC reconfiguration call.
  std::vector<byte_buffer> si_messages;
};

} // namespace odu
} // namespace ocudu
