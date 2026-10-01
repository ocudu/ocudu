// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/gnb_cu_up_id.h"
#include "ocudu/ran/gnb_du_id.h"
#include <string>
#include <variant>

namespace ocudu {

/// E2AP interface type for node component config (O-RAN WG3 E2AP spec table 9.2.26).
enum class e2_node_component_interface_type { ng, xn, e1, f1, w1, s1, x2 };

/// Typed component identifier carried in an E2 node-component-config item.
///
/// Exactly one alternative is populated depending on the interface type:
///   - NG  -> std::string      (AMF name)
///   - F1  -> gnb_du_id_t      (gNB-DU-ID)
///   - E1  -> gnb_cu_up_id_t   (gNB-CU-UP-ID)
///   - others -> std::monostate (not set)
using e2_node_component_id = std::variant<std::monostate, std::string, gnb_du_id_t, gnb_cu_up_id_t>;

/// Encoded request/response PDU pair captured from an interface setup exchange (F1, NG, E1, XN, etc.)
/// together with the identity of the interface.
struct e2_node_component_config {
  e2_node_component_interface_type interface_type;
  byte_buffer                      request_part;
  byte_buffer                      response_part;
  e2_node_component_id             component_id;
};

} // namespace ocudu
