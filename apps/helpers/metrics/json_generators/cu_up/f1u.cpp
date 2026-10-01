// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1u.h"
#include "helpers.h"

using namespace ocudu;
using namespace app_helpers;
using namespace json_generators;

static nlohmann::json generate_f1u_tx(const ocuup::f1u_tx_metrics_container& metrics, unsigned period)
{
  nlohmann::json json;

  // TODO: fill json fields

  return json;
}

static nlohmann::json generate_f1u_rx(const ocuup::f1u_rx_metrics_container& metrics, unsigned period)
{
  nlohmann::json json;

  // TODO: fill json fields

  return json;
}

nlohmann::json ocudu::app_helpers::json_generators::generate(const ocuup::f1u_tx_metrics_container& tx,
                                                             const ocuup::f1u_rx_metrics_container& rx,
                                                             timer_duration                         metrics_period)
{
  nlohmann::json json;

  json["timestamp"]          = get_time_stamp();
  nlohmann::json& cu_up_json = json["cu-up"];
  nlohmann::json& f1u_json   = cu_up_json["nrup"];

  f1u_json["dl"] = generate_f1u_tx(tx, metrics_period.count());
  f1u_json["ul"] = generate_f1u_rx(rx, metrics_period.count());

  return json;
}

std::string ocudu::app_helpers::json_generators::generate_string(const ocuup::f1u_tx_metrics_container& tx,
                                                                 const ocuup::f1u_rx_metrics_container& rx,
                                                                 timer_duration                         metrics_period,
                                                                 int                                    indent)
{
  return generate(tx, rx, metrics_period).dump(indent);
}
