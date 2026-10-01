// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace fapi {

struct dl_tti_request;
struct tx_data_request;
struct ul_dci_request;
struct ul_tti_request;

/// This interface represents the gateway for sending P7 requests to the underlying PHY.
class p7_requests_gateway
{
public:
  virtual ~p7_requests_gateway() = default;

  /// \brief Sends a downlink TTI request message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void send_dl_tti_request(const dl_tti_request& msg) = 0;

  /// \brief Sends an uplink TTI request message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void send_ul_tti_request(const ul_tti_request& msg) = 0;

  /// \brief Sends an uplink DCI request message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void send_ul_dci_request(const ul_dci_request& msg) = 0;

  /// \brief Sends a TX data request message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void send_tx_data_request(const tx_data_request& msg) = 0;
};

} // namespace fapi
} // namespace ocudu
