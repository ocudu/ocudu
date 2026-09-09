// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/gateways/sctp_dtls_mode.h"
#include "ocudu/support/io/transport_layer_address.h"
#include <map>
#include <string>

namespace ocudu {

struct dtls_context_config {
  // Default mode of the node.
  dtls_mode   mode;
  std::string session_id;
  std::string cert_filename;
  std::string key_filename;
  // Mode for specific connections. Useful for peer to peer connections,
  // where the server client model between nodes is not clearly defined.
  std::map<transport_layer_address, dtls_mode> mode_map;
};

} // namespace ocudu
