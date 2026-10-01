// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "adapters/sdap_adapters.h"
#include "ocudu/e1ap/common/e1ap_types.h"
#include "ocudu/pdcp/pdcp_rx.h"
#include "ocudu/ran/cu_types.h"

namespace ocudu {
namespace ocuup {

struct qos_flow_context {
  qos_flow_context(const e1ap_qos_flow_qos_param_item& flow) : qos_flow_id(flow.qos_flow_id)
  {
    const auto& qos_params = flow.qos_flow_level_qos_params.qos_desc;
    five_qi                = qos_params.get_5qi();
    ocudu_assert(not qos_params.is_dyn_5qi(), "Dynamic 5QI not supported.");
    ocudu_assert(five_qi != five_qi_t::invalid, "FiveQI must be set.");
  }

  qos_flow_id_t qos_flow_id = qos_flow_id_t::invalid; // The QoS flow ID.
  five_qi_t     five_qi     = five_qi_t::invalid;     // The FiveQI assigned to this flow.

  // TODO: add other fields contained in:
  // * qos_flow_level_qos_params_s
  // * ngran_alloc_and_retention_prio_s
  // * etc.

  sdap_pdcp_adapter sdap_to_pdcp_adapter;

  std::unique_ptr<pdcp_rx_upper_data_notifier> sdap_rx_notifier;
};

} // namespace ocuup
} // namespace ocudu
