// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ofh_sector_impl.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace ofh;

receiver& sector_impl::get_receiver()
{
  return *ofh_receiver;
}

transmitter& sector_impl::get_transmitter()
{
  return *ofh_transmitter;
}

operation_controller& sector_impl::get_operation_controller()
{
  return ofh_sector_controller;
}

metrics_collector* sector_impl::get_metrics_collector()
{
  return ofh_metrics_collector.disabled() ? nullptr : &ofh_metrics_collector;
}
