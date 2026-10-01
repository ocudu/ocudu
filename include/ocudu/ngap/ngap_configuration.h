// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/gnb_id.h"
#include "ocudu/ran/supported_tracking_area.h"
#include "ocudu/support/io/transport_layer_address.h"
#include <chrono>
#include <string>
#include <vector>

namespace ocudu::ocucp {

/// \brief NGAP configuration
struct ngap_configuration {
  gnb_id_t                             gnb_id{0, 22};
  std::string                          ran_node_name;
  cu_cp_amf_index_t                    amf_index;
  std::vector<supported_tracking_area> supported_tas;
  transport_layer_address              amf_addr;
  std::chrono::milliseconds            procedure_timeout{5000};
  std::chrono::seconds                 request_pdu_session_timeout; // timeout for requesting a pdu session in seconds
};

} // namespace ocudu::ocucp
