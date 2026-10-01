// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ofh_timing_manager_impl.h"

using namespace ocudu;
using namespace ofh;

ota_symbol_boundary_notifier_manager& timing_manager_impl::get_ota_symbol_boundary_notifier_manager()
{
  return worker;
}

timing_metrics_collector& timing_manager_impl::get_metrics_collector()
{
  return worker.get_metrics_collector();
}

operation_controller& timing_manager_impl::get_controller()
{
  return worker;
}
