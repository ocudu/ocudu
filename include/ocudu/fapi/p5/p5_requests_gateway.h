// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace fapi {

struct config_request;
struct param_request;
struct start_request;
struct stop_request;

/// This interface represents the gateway for sending FAPI request messages to the underlying PHY.
class p5_requests_gateway
{
public:
  virtual ~p5_requests_gateway() = default;

  /// \brief Sends a param request message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void send_param_request(const param_request& msg) = 0;

  /// \brief Sends a configuration request message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void send_config_request(const config_request& msg) = 0;

  /// \brief Sends a start request message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void send_start_request(const start_request& msg) = 0;

  /// \brief Sends a stop request message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void send_stop_request(const stop_request& msg) = 0;
};

} // namespace fapi
} // namespace ocudu
