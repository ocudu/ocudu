// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "split6_o_du_low_unit_config_validator.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config_validator.h"

using namespace ocudu;

bool ocudu::validate_split6_o_du_low_unit_config(const split6_o_du_low_unit_config& config)
{
  if (auto* ru = std::get_if<ru_sdr_unit_config>(&config.ru_cfg)) {
    if (ru->device_driver == "zmq") {
      fmt::println("Split 6 O-DU low does not support 'zmq' driver");
      return false;
    }
  }

  return true;
}
