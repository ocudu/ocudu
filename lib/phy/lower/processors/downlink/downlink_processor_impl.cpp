// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "downlink_processor_impl.h"
#include "ocudu/phy/lower/lower_phy_timing_context.h"
#include "ocudu/phy/lower/processors/downlink/pdxch/pdxch_processor_baseband.h"
#include "ocudu/phy/support/resource_grid_context.h"

using namespace ocudu;

downlink_processor_impl::downlink_processor_impl(std::unique_ptr<pdxch_processor>                 pdxch_proc_,
                                                 const downlink_processor_baseband_configuration& config) :
  pdxch_proc(std::move(pdxch_proc_)), downlink_proc_baseband(pdxch_proc->get_baseband(), config)
{
  ocudu_assert(pdxch_proc, "Invalid PDxCH processor.");
}

void downlink_processor_impl::connect(downlink_processor_notifier& notifier, pdxch_processor_notifier& pdxch_notifier)
{
  downlink_proc_baseband.connect(notifier);
  pdxch_proc->connect(pdxch_notifier);
}

pdxch_processor_request_handler& downlink_processor_impl::get_downlink_request_handler()
{
  return pdxch_proc->get_request_handler();
}

downlink_processor_baseband& downlink_processor_impl::get_baseband()
{
  return downlink_proc_baseband;
}

baseband_cfo_processor& downlink_processor_impl::get_cfo_control()
{
  return downlink_proc_baseband.get_cfo_control();
}

lower_phy_center_freq_controller& downlink_processor_impl::get_carrier_center_frequency_control()
{
  return pdxch_proc->get_center_freq_control();
}

lower_phy_tx_time_offset_controller& downlink_processor_impl::get_tx_time_offset_control()
{
  return downlink_proc_baseband;
}
