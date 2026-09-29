// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cu_configurator.h"
#include "ocudu/cu_cp/cu_cp_command_handler.h"
#include "ocudu/cu_cp/cu_cp_metrics_handler.h"
#include "ocudu/cu_cp/cu_cp_ng_handler.h"
#include "ocudu/e1ap/cu_cp/cu_cp_e1_handler.h"
#include "ocudu/f1ap/cu_cp/cu_cp_f1c_handler.h"
#include "ocudu/xnap/cu_cp_xnc_handler.h"

namespace ocudu::ocucp {

/// \brief Public interface for a CU-CP instance.
class cu_cp
{
public:
  virtual ~cu_cp() = default;

  /// \brief Get handler of the F1-C interface of the CU-CP.
  virtual cu_cp_f1c_handler& get_f1c_handler() = 0;

  /// \brief Get handler of the E1 interface of the CU-CP.
  virtual cu_cp_e1_handler& get_e1_handler() = 0;

  /// \brief Get handler of the NG interface of the CU-CP.
  virtual cu_cp_ng_handler& get_ng_handler() = 0;

  /// \brief Get handler of the Xn-C interface of the CU-CP.
  virtual cu_cp_xnc_handler& get_xnc_handler() = 0;

  /// \brief Get the handler of external commands/events to the CU-CP.
  virtual cu_cp_command_handler& get_command_handler() = 0;

  /// \brief Get the metrics handler interface of the CU-CP.
  virtual metrics_handler& get_metrics_handler() = 0;

  /// \brief Get cu configutator interface of the CU-CP.
  virtual cu_configurator& get_cu_configurator() = 0;

  /// \brief Initiate AMF TNL connection and run NG Setup Procedure.
  ///
  /// This function blocks until the first attempt is complete. An AMF that is not connected by then is reconnected to
  /// in the background.
  virtual void start() = 0;

  /// \brief Stop the CU-CP operation.
  virtual void stop() = 0;
};

} // namespace ocudu::ocucp
