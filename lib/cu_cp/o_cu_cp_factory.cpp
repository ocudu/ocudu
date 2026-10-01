// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/cu_cp/o_cu_cp_factory.h"
#include "adapters/ng_setup_e2_adapter.h"
#include "o_cu_cp_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/cu_cp/cu_cp_factory.h"
#include "ocudu/cu_cp/o_cu_cp_config.h"
#include "ocudu/e2/e2_cu_cp_factory.h"

using namespace ocudu;
using namespace ocucp;

std::unique_ptr<o_cu_cp> ocucp::create_o_cu_cp(const o_cu_cp_config& config, const o_cu_cp_dependencies& dependencies)
{
  if (dependencies.e2_client == nullptr) {
    auto cu = create_cu_cp(config.cu_cp_config);
    return std::make_unique<o_cu_cp_impl>(std::move(cu));
  }

  auto collector =
      std::make_unique<e2_node_component_config_collector>(*config.cu_cp_config.services.cu_cp_executor, 1);
  auto adapter = std::make_unique<ng_setup_e2_adapter>(*collector);

  // Take a local copy of cu_cp_config to inject the ng_setup_notifier pointer before passing it
  // to create_cu_cp.
  cu_cp_configuration cu_cp_cfg    = config.cu_cp_config;
  cu_cp_cfg.ngap.ng_setup_notifier = adapter.get();

  auto cu = create_cu_cp(cu_cp_cfg);

  auto e2agent = create_e2_cu_cp_agent(config.e2ap_cfg,
                                       *dependencies.e2_client,
                                       dependencies.e2_cu_metric_iface,
                                       cu->get_cu_configurator(),
                                       timer_factory{*cu_cp_cfg.services.timers, *cu_cp_cfg.services.cu_cp_executor},
                                       *cu_cp_cfg.services.cu_cp_executor,
                                       std::move(collector));

  return std::make_unique<o_cu_cp_with_e2_impl>(std::move(e2agent), std::move(cu), std::move(adapter));
}
