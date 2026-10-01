// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "o_cu_cp_unit_impl.h"

using namespace ocudu;
using namespace ocucp;

o_cu_cp_unit_impl::o_cu_cp_unit_impl(std::vector<std::unique_ptr<ocucp::n2_connection_client>> n2_clients_,
                                     std::unique_ptr<e2_cu_metrics_connector_manager>          e2_metric_connector_,
                                     std::unique_ptr<ocucp::o_cu_cp>                           cu_cp_) :
  n2_clients(std::move(n2_clients_)), e2_metric_connector(std::move(e2_metric_connector_)), o_cu(std::move(cu_cp_))
{
  ocudu_assert(o_cu, "Invalid O-CU-CP object");
}

ocucp::cu_cp& o_cu_cp_unit_impl::get_cu_cp()
{
  return o_cu->get_cu_cp();
}

ocucp::cu_cp_operation_controller& o_cu_cp_unit_impl::get_operation_controller()
{
  return o_cu->get_operation_controller();
}
