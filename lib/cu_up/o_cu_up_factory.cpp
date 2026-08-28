// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/cu_up/o_cu_up_factory.h"
#include "adapters/e1_setup_e2_adapter.h"
#include "o_cu_up_impl.h"
#include "ocudu/cu_up/cu_up_factory.h"
#include "ocudu/e2/e2_cu_up_factory.h"

using namespace ocudu;
using namespace ocuup;

std::unique_ptr<o_cu_up> ocuup::create_o_cu_up(const o_cu_up_config& config, o_cu_up_dependencies&& dependencies)
{
  // E2 is not enabled.
  if (!dependencies.e2_client) {
    auto cu_up = create_cu_up(config.cu_up_cfg, std::move(dependencies.cu_dependencies));
    return std::make_unique<o_cu_up_impl>(std::move(cu_up));
  }

  auto collector =
      std::make_unique<e2_node_component_config_collector>(dependencies.cu_dependencies.exec_mapper.e2_executor(), 1);
  dependencies.cu_dependencies.e1_setup_notifier = std::make_unique<e1_setup_e2_adapter>(*collector);

  auto cu_up = create_cu_up(config.cu_up_cfg, std::move(dependencies.cu_dependencies));

  auto e2agent = create_e2_cu_up_agent(
      config.e2ap_cfg,
      *dependencies.e2_client,
      dependencies.e2_cu_metric_iface,
      timer_factory{dependencies.cu_dependencies.timers, dependencies.cu_dependencies.exec_mapper.ctrl_executor()},
      dependencies.cu_dependencies.exec_mapper.e2_executor(),
      std::move(collector));

  return std::make_unique<o_cu_up_with_e2_impl>(std::move(e2agent), std::move(cu_up));
}
