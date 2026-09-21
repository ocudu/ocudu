// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <map>
#include <string>
namespace ocudu {

enum class dtls_appconfig_mode { client, server };

/// Common DTLS option parameters shared across application configurations.
struct dtls_appconfig {
  bool enabled = false;
  // Default mode of the node.
  dtls_appconfig_mode mode;
  std::string         cert_filename;
  std::string         key_filename;
  std::string         ca_cert_filename;
  // Mode for specific connections. Useful for peer to peer connections,
  // where the server client model between nodes is not clearly defined.
  std::map<std::string, dtls_appconfig_mode> mode_map;
};

} // namespace ocudu
