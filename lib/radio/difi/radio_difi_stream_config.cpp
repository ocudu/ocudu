// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_difi_stream_config.h"
#include "ocudu/support/string_parsing_utils.h"
#include "fmt/base.h"
#include <arpa/inet.h>
#include <sstream>

using namespace ocudu;

/// Highest valid UDP port number.
static constexpr int MAX_UDP_PORT = 65535;

std::optional<radio_difi_stream_config>
ocudu::parse_difi_stream_args(const std::string& args, const std::string& default_ip, uint16_t default_port)
{
  radio_difi_stream_config config = {default_ip, default_port};

  if (args.empty()) {
    return config;
  }

  std::istringstream ss(args);
  std::string        token;

  while (std::getline(ss, token, ',')) {
    // Skip empty tokens caused by trailing commas.
    if (token.empty()) {
      continue;
    }

    const auto eq = token.find('=');
    if (eq == std::string::npos) {
      fmt::print("Stream argument '{}' is malformed, expected key=value.\n", token);
      return std::nullopt;
    }

    const std::string key = token.substr(0, eq);
    const std::string val = token.substr(eq + 1);

    if (key == "ip") {
      // Validate as a dotted-decimal IPv4 address.
      struct in_addr addr = {};
      if (::inet_pton(AF_INET, val.c_str(), &addr) != 1) {
        fmt::print("Invalid IP address '{}' in stream arguments.\n", val);
        return std::nullopt;
      }
      config.ip = val;

    } else if (key == "port") {
      // parse_int() rejects trailing characters, so no separate check is needed.
      const expected<int, std::string> port = parse_int<int>(val);
      if (!port.has_value()) {
        fmt::print("Stream port '{}' is not a valid integer.\n", val);
        return std::nullopt;
      }
      if ((port.value() < 1) || (port.value() > MAX_UDP_PORT)) {
        fmt::print("Stream port {} is out of valid range [1, {}].\n", port.value(), MAX_UDP_PORT);
        return std::nullopt;
      }
      config.port = static_cast<uint16_t>(port.value());

    } else {
      fmt::print("Unknown stream argument key '{}'.\n", key);
      return std::nullopt;
    }
  }

  return config;
}
