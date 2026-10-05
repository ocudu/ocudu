// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lower_phy_sector_timing_handler.h"
#include "ocudu/phy/lower/lower_phy.h"
#include <memory>

namespace ocudu {

class lower_phy_cfo_controller;
class lower_phy_center_freq_controller;

/// Lower PHY sector implementation.
class lower_phy_sector : public lower_phy
{
public:
  explicit lower_phy_sector(lower_phy_timing_notifier* timing_notifier) : timing_handler(timing_notifier) {}

  // See interface for documentation.
  lower_phy_uplink_request_handler& get_uplink_request_handler() override;

  // See interface for documentation.
  lower_phy_downlink_handler& get_downlink_handler() override;

  // See interface for documentation.
  lower_phy_controller& get_controller() override;

  // See interface for documentation.
  lower_phy_cfo_controller& get_tx_cfo_control() override;

  // See interface for documentation.
  lower_phy_cfo_controller& get_rx_cfo_control() override;

  // See interface for documentation.
  lower_phy_center_freq_controller& get_tx_center_freq_control() override;

  // See interface for documentation.
  lower_phy_center_freq_controller& get_rx_center_freq_control() override;

  // See interface for documentation.
  lower_phy_tx_time_offset_controller& get_tx_time_offset_control() override;

  /// Sets the lower PHY of this sector to the given one.
  void set_lower_phy(std::unique_ptr<lower_phy> phy)
  {
    ocudu_assert(phy, "Invalid lower PHY");
    lower_phy = std::move(phy);
  }

  /// Returns the timing notifier of this lower PHY sector.
  lower_phy_timing_notifier& get_timing_notifier() { return timing_handler; }

private:
  lower_phy_sector_timing_handler timing_handler;
  std::unique_ptr<lower_phy>      lower_phy;
};

} // namespace ocudu
