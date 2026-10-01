// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cu_cp_ng_setup_notifier.h"
#include "ocudu/e2/e2_node_component_config_collector.h"

namespace ocudu::ocucp {

/// Adapter that forwards NG setup completion to the E2 node-component-config collector.
class ng_setup_e2_adapter final : public cu_cp_ng_setup_complete_notifier
{
public:
  explicit ng_setup_e2_adapter(e2_node_component_config_collector& collector_) : collector(collector_) {}

  void on_ng_setup_complete(byte_buffer req, byte_buffer resp, std::string amf_name) override
  {
    collector.deliver(e2_node_component_interface_type::ng,
                      std::move(req),
                      std::move(resp),
                      e2_node_component_id{std::move(amf_name)});
  }

private:
  e2_node_component_config_collector& collector;
};

} // namespace ocudu::ocucp
