// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lower_phy_sector.h"

using namespace ocudu;

lower_phy_uplink_request_handler& lower_phy_sector::get_uplink_request_handler()
{
  return lower_phy->get_uplink_request_handler();
}

lower_phy_downlink_handler& lower_phy_sector::get_downlink_handler()
{
  return lower_phy->get_downlink_handler();
}

lower_phy_controller& lower_phy_sector::get_controller()
{
  return lower_phy->get_controller();
}

lower_phy_cfo_controller& lower_phy_sector::get_tx_cfo_control()
{
  return lower_phy->get_tx_cfo_control();
}

lower_phy_cfo_controller& lower_phy_sector::get_rx_cfo_control()
{
  return lower_phy->get_rx_cfo_control();
}

lower_phy_center_freq_controller& lower_phy_sector::get_tx_center_freq_control()
{
  return lower_phy->get_tx_center_freq_control();
}

lower_phy_center_freq_controller& lower_phy_sector::get_rx_center_freq_control()
{
  return lower_phy->get_rx_center_freq_control();
}

lower_phy_tx_time_offset_controller& lower_phy_sector::get_tx_time_offset_control()
{
  return lower_phy->get_tx_time_offset_control();
}
