// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lower_phy_impl.h"
#include "ocudu/phy/lower/lower_phy_rx_symbol_context.h"

using namespace ocudu;

lower_phy_impl::lower_phy_impl(dependencies deps) :
  notification_adaptor(deps.error_notifier, deps.rx_symbol_notifier, deps.timing_notifier),
  downlink_proc(std::move(deps.downlink_proc)),
  uplink_proc(std::move(deps.uplink_proc)),
  handler_adaptor(downlink_proc->get_downlink_request_handler(),
                  uplink_proc->get_prach_request_handler(),
                  uplink_proc->get_puxch_request_handler()),
  controller(std::move(deps.controller))
{
  ocudu_assert(downlink_proc != nullptr, "Invalid downlink processor.");
  ocudu_assert(uplink_proc != nullptr, "Invalid uplink processor.");
  ocudu_assert(controller != nullptr, "Invalid controller.");

  // Connect internal adaptors.
  downlink_proc->connect(notification_adaptor.get_downlink_notifier(), notification_adaptor.get_pdxch_notifier());
  uplink_proc->connect(notification_adaptor.get_uplink_notifier(),
                       notification_adaptor.get_prach_notifier(),
                       notification_adaptor.get_puxch_notifier());
}

lower_phy_cfo_controller& lower_phy_impl::get_tx_cfo_control()
{
  return downlink_proc->get_cfo_control();
}

lower_phy_cfo_controller& lower_phy_impl::get_rx_cfo_control()
{
  return uplink_proc->get_cfo_control();
}

lower_phy_center_freq_controller& lower_phy_impl::get_tx_center_freq_control()
{
  return downlink_proc->get_carrier_center_frequency_control();
}

lower_phy_center_freq_controller& lower_phy_impl::get_rx_center_freq_control()
{
  return uplink_proc->get_carrier_center_frequency_control();
}

lower_phy_tx_time_offset_controller& lower_phy_impl::get_tx_time_offset_control()
{
  return downlink_proc->get_tx_time_offset_control();
}
