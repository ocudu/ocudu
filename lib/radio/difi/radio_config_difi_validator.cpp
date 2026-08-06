// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_config_difi_validator.h"
#include "radio_difi_stream_config.h"
#include "fmt/base.h"
#include <cmath>

using namespace ocudu;

static bool validate_sampling_rate(double sampling_rate)
{
  if (!std::isnormal(sampling_rate)) {
    fmt::print("The sampling rate must be non-zero, NAN nor infinite.\n");
    return false;
  }

  if (sampling_rate < 0.0) {
    fmt::print("The sampling rate must be greater than zero.\n");
    return false;
  }

  return true;
}

static bool validate_otw_format(radio_configuration::over_the_wire_format otw_format)
{
  if (otw_format != radio_configuration::over_the_wire_format::DEFAULT &&
      otw_format != radio_configuration::over_the_wire_format::SC8 &&
      otw_format != radio_configuration::over_the_wire_format::SC16) {
    fmt::print("Only DEFAULT, SC8 and SC16 OTW formats are supported by the DIFI radio.\n");
    return false;
  }

  return true;
}

static bool validate_tx_stream_args(const std::string& args)
{
  // Default Tx: send to localhost:4991.
  return parse_difi_stream_args(args, "127.0.0.1", 4991).has_value();
}

static bool validate_rx_stream_args(const std::string& args)
{
  // Default Rx: bind on all interfaces, port 4992.
  return parse_difi_stream_args(args, "0.0.0.0", 4992).has_value();
}

bool radio_config_difi_validator::is_configuration_valid(const radio_configuration::radio& config) const
{
  if (config.tx_streams.size() != config.rx_streams.size()) {
    fmt::print("Transmit and receive number of streams must be equal.\n");
    return false;
  }

  if (config.tx_streams.empty()) {
    fmt::print("At least one transmit and one receive stream must be configured.\n");
    return false;
  }

  for (const radio_configuration::stream& tx_stream : config.tx_streams) {
    if (!validate_tx_stream_args(tx_stream.args)) {
      return false;
    }
  }

  for (const radio_configuration::stream& rx_stream : config.rx_streams) {
    if (!validate_rx_stream_args(rx_stream.args)) {
      return false;
    }
  }

  if (!validate_sampling_rate(config.sampling_rate_Hz)) {
    return false;
  }

  if (!validate_otw_format(config.otw_format)) {
    return false;
  }

  return true;
}
