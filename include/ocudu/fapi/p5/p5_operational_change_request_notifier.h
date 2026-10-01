// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace fapi {

struct cell_configuration;

/// \brief P5 FAPI operational change request notifier.
///
/// Notifies that P5 FAPI START/STOP request messages have been processed and requests a change in the operational state
/// of the cell.
class p5_operational_change_request_notifier
{
public:
  virtual ~p5_operational_change_request_notifier() = default;

  /// \brief Notifies a start request from FAPI.
  ///
  /// This method is synchronous, as such, when this callback returns the start procedure will be completed. Returns
  /// true on start success, otherwise false.
  virtual bool on_start_request(const cell_configuration& config) = 0;

  /// \brief Notifies a stop request from FAPI.
  ///
  /// This method is synchronous, so when this function returns the stop process is already finished.
  virtual void on_stop_request() = 0;
};

} // namespace fapi
} // namespace ocudu
