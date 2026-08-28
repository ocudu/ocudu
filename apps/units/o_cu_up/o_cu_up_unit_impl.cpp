// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "o_cu_up_unit_impl.h"

using namespace ocudu;

o_cu_up_unit_impl::o_cu_up_unit_impl(std::unique_ptr<e2_cu_metrics_connector_manager> e2_metric_connector_,
                                     std::unique_ptr<ocuup::o_cu_up>                  cu_up_) :
  e2_metric_connector(std::move(e2_metric_connector_)), cu_up(std::move(cu_up_))
{
  ocudu_assert(cu_up, "Invalid CU-UP");
}

ocuup::cu_up_interface& o_cu_up_unit_impl::get_cu_up()
{
  return cu_up->get_cu_up();
}

ocuup::cu_up_operation_controller& o_cu_up_unit_impl::get_operation_controller()
{
  return cu_up->get_operation_controller();
}
