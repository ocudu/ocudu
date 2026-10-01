// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/du_manager/du_f1_setup_notifier.h"
#include "ocudu/e2/e2_node_component_config_collector.h"

namespace ocudu::odu {

/// Adapter that forwards F1 setup completion to the E2 node-component-config collector.
class f1_setup_e2_adapter final : public du_f1_setup_complete_notifier
{
public:
  explicit f1_setup_e2_adapter(e2_node_component_config_collector& collector_) : collector(collector_) {}

  void on_f1_setup_complete(byte_buffer req, byte_buffer resp, gnb_du_id_t gnb_du_id) override
  {
    collector.deliver(
        e2_node_component_interface_type::f1, std::move(req), std::move(resp), e2_node_component_id{gnb_du_id});
  }

private:
  e2_node_component_config_collector& collector;
};

} // namespace ocudu::odu
