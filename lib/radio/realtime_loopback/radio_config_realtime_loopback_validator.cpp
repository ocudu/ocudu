// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_config_realtime_loopback_validator.h"
#include "fmt/base.h"

using namespace ocudu;

static bool validate_clock_sources(const radio_configuration::clock_sources& sources)
{
  if (sources.clock != radio_configuration::clock_sources::source::DEFAULT) {
    fmt::print("Only 'default' clock source is available.\n");
    return false;
  }

  if (sources.sync != radio_configuration::clock_sources::source::DEFAULT) {
    fmt::print("Only 'default' sync source is available.\n");
    return false;
  }

  return true;
}

static bool validate_lo_freq(const radio_configuration::lo_frequency& lo_freq)
{
  if (!std::isnormal(lo_freq.center_frequency_Hz)) {
    fmt::print("The center frequency must be non-zero, NAN nor infinite.\n");
    return false;
  }

  if (lo_freq.lo_frequency_Hz != 0.0) {
    fmt::print("The custom LO frequency is not currently supported.\n");
    return false;
  }

  return true;
}

static bool validate_channel_args(const std::string& channel_args)
{
  if (!channel_args.empty()) {
    fmt::print("Channel arguments are not supported by the loopback realtime radio.\n");
    return false;
  }

  return true;
}

static bool validate_channel(const radio_configuration::channel& channel)
{
  if (!validate_lo_freq(channel.freq)) {
    return false;
  }

  if (std::isnan(channel.gain_dB) || std::isinf(channel.gain_dB)) {
    fmt::print("Channel gain must not be NAN nor infinite.\n");
    return false;
  }

  if (!validate_channel_args(channel.args)) {
    return false;
  }

  return true;
}

static bool validate_stream(const radio_configuration::stream& stream)
{
  if (stream.channels.empty()) {
    fmt::print("Streams must contain at least one channel.\n");
    return false;
  }

  for (const radio_configuration::channel& channel : stream.channels) {
    if (!validate_channel(channel)) {
      return false;
    }
  }

  if (!stream.args.empty()) {
    fmt::print("Stream arguments are not currently supported.\n");
    return false;
  }

  return true;
}

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
  if (otw_format != radio_configuration::over_the_wire_format::DEFAULT) {
    fmt::print("Only default OTW format is currently supported.\n");
    return false;
  }

  return true;
}

bool radio_config_realtime_loopback_validator::is_configuration_valid(const radio_configuration::radio& config) const
{
  if (!validate_clock_sources(config.clock)) {
    return false;
  }

  if (config.tx_streams.size() != config.rx_streams.size()) {
    fmt::print("Transmit and receive number of streams must be equal.\n");
    return false;
  }

  if (config.tx_streams.empty()) {
    fmt::print("At least one transmit and one receive stream must be configured.\n");
    return false;
  }

  if (config.tx_streams.size() != 1) {
    fmt::print("Only a single transmit and receive stream is currently supported.\n");
    return false;
  }

  for (const radio_configuration::stream& tx_stream : config.tx_streams) {
    if (!validate_stream(tx_stream)) {
      return false;
    }
  }

  for (const radio_configuration::stream& rx_stream : config.rx_streams) {
    if (!validate_stream(rx_stream)) {
      return false;
    }
  }

  if (!validate_sampling_rate(config.sampling_rate_Hz)) {
    return false;
  }

  if (!validate_otw_format(config.otw_format)) {
    return false;
  }

  if (config.tx_mode != radio_configuration::transmission_mode::continuous) {
    fmt::print("Discontinuous transmission modes are not supported by the realtime loopback radio.\n");
    return false;
  }

  if (config.power_ramping_us != 0.0F) {
    fmt::print("Power ramping is not supported by the realtime loopback radio.\n");
    return false;
  }

  return true;
}
