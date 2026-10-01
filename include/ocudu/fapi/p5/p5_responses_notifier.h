// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace fapi {

struct config_response;
struct param_response;
struct stop_indication;

/// \brief Configuration message notifier interface.
///
/// This interface notifies the reception of FAPI configuration P5 response messages from the underlying PHY.
class p5_responses_notifier
{
public:
  virtual ~p5_responses_notifier() = default;

  /// \brief Notifies the reception of a parameter response message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_param_response(const param_response& msg) = 0;

  /// \brief Notifies the reception of a configuration response message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_config_response(const config_response& msg) = 0;

  /// \brief Notifies the reception of a stop response message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_stop_indication(const stop_indication& msg) = 0;
};

} // namespace fapi
} // namespace ocudu
