// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_up/cu_up_e1_setup_notifier.h"
#include "ocudu/e2/e2_node_component_config_collector.h"

namespace ocudu::ocuup {

/// Adapter that forwards E1 setup completion to the E2 node-component-config collector.
class e1_setup_e2_adapter final : public cu_up_e1_setup_complete_notifier
{
public:
  explicit e1_setup_e2_adapter(e2_node_component_config_collector& collector_) : collector(collector_) {}

  void on_e1_setup_complete(byte_buffer req, byte_buffer resp, gnb_cu_up_id_t gnb_cu_up_id) override
  {
    collector.deliver(
        e2_node_component_interface_type::e1, std::move(req), std::move(resp), e2_node_component_id{gnb_cu_up_id});
  }

private:
  e2_node_component_config_collector& collector;
};

} // namespace ocudu::ocuup
