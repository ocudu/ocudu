// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

class lower_phy_controller;
class lower_phy_center_freq_controller;
class lower_phy_cfo_controller;
class lower_phy_error_notifier;
class lower_phy_downlink_handler;
class lower_phy_rx_symbol_notifier;
class lower_phy_timing_notifier;
class lower_phy_tx_time_offset_controller;
class lower_phy_uplink_request_handler;

/// \brief Lower PHY main interface.
///
/// Provides access to all the lower PHY components.
class lower_phy
{
public:
  /// Default destructor.
  virtual ~lower_phy() = default;

  /// Returns a reference to the lower PHY uplink request handler.
  virtual lower_phy_uplink_request_handler& get_uplink_request_handler() = 0;

  /// Returns a reference to the lower PHY resource grid handler.
  virtual lower_phy_downlink_handler& get_downlink_handler() = 0;

  /// Returns a reference to the lower PHY controller.
  virtual lower_phy_controller& get_controller() = 0;

  /// Gets the transmit carrier frequency offset control.
  virtual lower_phy_cfo_controller& get_tx_cfo_control() = 0;

  /// Gets the receive carrier frequency offset control.
  virtual lower_phy_cfo_controller& get_rx_cfo_control() = 0;

  /// Gets the transmit carrier center frequency control.
  virtual lower_phy_center_freq_controller& get_tx_center_freq_control() = 0;

  /// Gets the receive carrier center frequency control.
  virtual lower_phy_center_freq_controller& get_rx_center_freq_control() = 0;

  /// Gets the transmit time offset control.
  virtual lower_phy_tx_time_offset_controller& get_tx_time_offset_control() = 0;
};

} // namespace ocudu
