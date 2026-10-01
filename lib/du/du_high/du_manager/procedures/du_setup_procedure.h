// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../metrics/du_metrics_aggregator_impl.h"
#include "du_proc_context_view.h"
#include "procedure_logger.h"
#include "ocudu/f1ap/du/f1ap_du_connection_manager.h"
#include <limits>

namespace ocudu {
namespace odu {

class du_cell_manager;
struct du_manager_params;

/// Request to transition the DU to operational mode.
struct du_start_request {
  /// Value of \c max_f1c_tnl_connection_retries that requests an unlimited number of setup attempts.
  static constexpr unsigned unlimited_retries = std::numeric_limits<unsigned>::max();

  /// Maximum number of F1-C TNL connection attempts. A rejected F1 Setup is not retried and is not counted here.
  unsigned                  max_f1c_tnl_connection_retries = 1;
  std::chrono::milliseconds f1c_tnl_connection_retry_wait{1000};
};

/// \brief Configures the DU cells and creates them in the MAC, without activating them.
///
/// It is run when the DU is started, before the F1 interface is set up, so that the layers below are ready to run as
/// soon as the DU is started. The cells are only activated once the CU-CP accepted the F1 Setup.
void configure_du_cells(const du_proc_context_view& ctxt);

/// Procedure to transition the DU state to operation mode.
class du_setup_procedure
{
public:
  du_setup_procedure(const du_proc_context_view& ctxt, const du_start_request& request = {});

  void operator()(coro_context<async_task<void>>& ctx);

private:
  async_task<f1_setup_result> start_f1_setup_request();

  // Handle F1 setup response with list of cells to activate.
  async_task<void> handle_f1_setup_response(const f1_setup_result& resp);

  const du_proc_context_view& ctxt;
  const du_start_request      request;
  du_procedure_logger         proc_logger;

  f1_setup_result response_msg = {};

  unique_timer timer;

  unsigned    count = 0;
  std::string failure_cause;
};

} // namespace odu
} // namespace ocudu
