// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace ocudu {

/// \brief Connection parameters for one stream direction, parsed from the stream arguments.
///
/// A comma-separated key=value list, "ip=1.2.3.4,port=4991". Tx names the destination, Rx the bind.
struct radio_difi_stream_config {
  /// IP address string (destination for Tx, bind address for Rx).
  std::string ip;
  /// UDP port (destination port for Tx, bind port for Rx).
  uint16_t port;
};

/// \brief Parse a DIFI stream args string into a radio_difi_stream_config.
///
/// \return The parsed configuration, or std::nullopt if \p args is malformed.
std::optional<radio_difi_stream_config> parse_difi_stream_args(const std::string& args,
                                                               const std::string& default_ip   = "127.0.0.1",
                                                               uint16_t           default_port = 4991);

} // namespace ocudu
