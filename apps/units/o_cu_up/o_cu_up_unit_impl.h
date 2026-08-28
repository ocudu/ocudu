// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/e2/e2_metric_connector_manager.h"
#include "ocudu/cu_up/o_cu_up.h"
#include "ocudu/e2/e2_cu_metrics_connector.h"

namespace ocudu {

using e2_cu_metrics_connector_manager =
    e2_metric_connector_manager<e2_cu_metrics_connector, e2_cu_metrics_notifier, e2_cu_metrics_interface>;

/// \brief O-RAN CU-UP unit implementation.
///
/// This implementation purpose is to manage the life of the given NGU gateway and the CU-UP interface.
class o_cu_up_unit_impl : public ocuup::o_cu_up
{
public:
  o_cu_up_unit_impl(std::unique_ptr<e2_cu_metrics_connector_manager> e2_metric_connector_,
                    std::unique_ptr<o_cu_up>                         cu_up_);

  // See interface for documentation.
  ocuup::cu_up_interface& get_cu_up() override;

  // See interface for documentation.
  ocuup::cu_up_operation_controller& get_operation_controller() override;

private:
  std::unique_ptr<e2_cu_metrics_connector_manager> e2_metric_connector;
  std::unique_ptr<o_cu_up>                         cu_up;
};

} // namespace ocudu
