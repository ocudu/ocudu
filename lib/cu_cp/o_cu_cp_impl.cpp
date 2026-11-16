// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "o_cu_cp_impl.h"
#include "adapters/ng_setup_e2_adapter.h"
#include "ocudu/cu_cp/cu_cp_factory.h"
#include "ocudu/cu_cp/o_cu_cp_config.h"
#include "ocudu/e2/e2_cu_cp_factory.h"

using namespace ocudu;
using namespace ocucp;

namespace {

/// Non-owning forwarder so the E2 agent can use a collector owned by o_cu_cp_impl.
class e2_node_component_config_provider_proxy final : public e2_node_component_config_provider
{
public:
  explicit e2_node_component_config_provider_proxy(e2_node_component_config_provider& provider_) : provider(provider_)
  {
  }

  // See interface for documentation.
  async_task<std::vector<e2_node_component_config>> get_configs() override { return provider.get_configs(); }

  // See interface for documentation.
  void on_timeout() override { provider.on_timeout(); }

private:
  e2_node_component_config_provider& provider;
};

} // namespace

/// Creates the E2 node component config collector, or returns null when E2 is disabled.
static std::unique_ptr<e2_node_component_config_collector> create_e2_collector(const o_cu_cp_dependencies& dependencies)
{
  if (!dependencies.e2_client) {
    return nullptr;
  }

  return std::make_unique<e2_node_component_config_collector>(dependencies.cu_cp_deps.cu_cp_executor, 1);
}

/// Creates the NG setup to E2 adapter, or returns null when E2 is disabled.
static std::unique_ptr<cu_cp_ng_setup_complete_notifier>
create_ng_setup_e2_adapter(e2_node_component_config_collector* collector)
{
  if (!collector) {
    return nullptr;
  }

  return std::make_unique<ng_setup_e2_adapter>(*collector);
}

/// Creates the CU-CP, injecting the NG setup complete notifier when present.
static std::unique_ptr<cu_cp> create_cu_cp_with_ng_notifier(const o_cu_cp_config&             config,
                                                            cu_cp_dependencies&               dependencies,
                                                            cu_cp_ng_setup_complete_notifier* notifier)
{
  if (notifier) {
    dependencies.ng_setup_notifier = notifier;
  }

  return create_cu_cp(config.cu_cp_config, dependencies);
}

/// Creates the E2 agent, or returns null when E2 is disabled.
static std::unique_ptr<e2_agent> create_e2_agent(const o_cu_cp_config&               config,
                                                 const o_cu_cp_dependencies&         dependencies,
                                                 cu_cp&                              cu,
                                                 e2_node_component_config_collector* collector)
{
  if (!dependencies.e2_client) {
    return nullptr;
  }

  ocudu_assert(collector, "Invalid E2 node component config collector");

  return create_e2_cu_cp_agent(config.e2ap_cfg,
                               *dependencies.e2_client,
                               dependencies.e2_cu_metric_iface,
                               cu.get_cu_configurator(),
                               timer_factory{dependencies.cu_cp_deps.timers, dependencies.cu_cp_deps.cu_cp_executor},
                               dependencies.cu_cp_deps.cu_cp_executor,
                               std::make_unique<e2_node_component_config_provider_proxy>(*collector));
}

o_cu_cp_impl::o_cu_cp_impl(const o_cu_cp_config& config, o_cu_cp_dependencies& dependencies) :
  collector(create_e2_collector(dependencies)),
  adapter(create_ng_setup_e2_adapter(collector.get())),
  cu(create_cu_cp_with_ng_notifier(config, dependencies.cu_cp_deps, adapter.get())),
  e2agent(create_e2_agent(config, dependencies, *cu, collector.get()))
{
}

void o_cu_cp_impl::start()
{
  cu->start();

  if (e2agent) {
    e2agent->start();
  }
}

void o_cu_cp_impl::stop()
{
  if (e2agent) {
    e2agent->stop();
  }

  cu->stop();
}
