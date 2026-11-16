// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cu_cp.h"
#include "ocudu/cu_cp/cu_cp_ng_setup_notifier.h"
#include "ocudu/cu_cp/cu_cp_operation_controller.h"
#include "ocudu/cu_cp/o_cu_cp.h"
#include "ocudu/e2/e2.h"
#include "ocudu/e2/e2_node_component_config_collector.h"

namespace ocudu::ocucp {

struct o_cu_cp_config;
struct o_cu_cp_dependencies;

/// O-RAN CU-CP implementation.
class o_cu_cp_impl : public o_cu_cp, public cu_cp_operation_controller
{
public:
  explicit o_cu_cp_impl(const o_cu_cp_config& config, o_cu_cp_dependencies& dependencies);

  // See interface for documentation.
  cu_cp& get_cu_cp() override { return *cu; }

  // See interface for documentation.
  cu_cp_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  /// Note: Declaration order matters. Members are constructed top-down and destroyed bottom-up:
  ///  - collector must outlive adapter (adapter holds a reference to collector).
  ///  - adapter must outlive cu (cu holds a pointer to adapter as its ng_setup_notifier).
  ///  - cu must outlive e2agent (e2agent holds a reference to cu's cu_configurator).
  /// collector, adapter and e2agent are null when E2 is disabled.
  std::unique_ptr<e2_node_component_config_collector> collector;
  std::unique_ptr<cu_cp_ng_setup_complete_notifier>   adapter;
  std::unique_ptr<cu_cp>                              cu;
  std::unique_ptr<e2_agent>                           e2agent;
};

} // namespace ocudu::ocucp
