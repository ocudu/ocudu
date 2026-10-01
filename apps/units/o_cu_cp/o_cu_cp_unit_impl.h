// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/e2/e2_metric_connector_manager.h"
#include "ocudu/cu_cp/o_cu_cp.h"
#include "ocudu/e2/e2_cu_metrics_connector.h"
#include "ocudu/ngap/gateways/n2_connection_client.h"

namespace ocudu {

using e2_cu_metrics_connector_manager =
    e2_metric_connector_manager<e2_cu_metrics_connector, e2_cu_metrics_notifier, e2_cu_metrics_interface>;

/// \brief O-RAN CU-CP unit implementation.
///
/// The purpose of this unit is to keep the life cycle of the objects related only to the O-RAN CU-CP.
class o_cu_cp_unit_impl : public ocucp::o_cu_cp
{
public:
  o_cu_cp_unit_impl(std::vector<std::unique_ptr<ocucp::n2_connection_client>> n2_clients_,
                    std::unique_ptr<e2_cu_metrics_connector_manager>          e2_metric_connector_,
                    std::unique_ptr<ocucp::o_cu_cp>                           cu_cp_);

  // See interface for documentation.
  ocucp::cu_cp& get_cu_cp() override;

  // See interface for documentation.
  ocucp::cu_cp_operation_controller& get_operation_controller() override;

private:
  std::vector<std::unique_ptr<ocucp::n2_connection_client>> n2_clients;
  std::unique_ptr<e2_cu_metrics_connector_manager>          e2_metric_connector;
  std::unique_ptr<ocucp::o_cu_cp>                           o_cu;
};

} // namespace ocudu
