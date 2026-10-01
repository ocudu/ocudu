// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "metrics_helpers.h"
#include "metrics_config.h"
#include "ocudu/ocudulog/log_channel.h"
#include "ocudu/ocudulog/ocudulog.h"

using namespace ocudu;
using namespace app_helpers;

/// Log channel name for the logger type.
static const std::string logger_channel_name = "METRICS";
/// Log channel name for the JSON type.
static const std::string json_channel_name = "JSON_channel";

ocudulog::log_channel& app_helpers::fetch_logger_metrics_log_channel()
{
  return ocudulog::fetch_log_channel(logger_channel_name);
}

ocudulog::log_channel& app_helpers::fetch_json_metrics_log_channel()
{
  return ocudulog::fetch_log_channel(json_channel_name);
}

static void initialize_logger_channel(unsigned hex_dump_size)
{
  ocudulog::log_channel& channel = ocudulog::fetch_log_channel(logger_channel_name, {logger_channel_name, {}, {}});
  channel.set_hex_dump_max_size(hex_dump_size);
  channel.set_enabled(true);
}

void app_helpers::initialize_metrics_log_channels(const metrics_config& config, unsigned hex_dump_size)
{
  if (config.enable_log_metrics) {
    initialize_logger_channel(hex_dump_size);
  }
}
