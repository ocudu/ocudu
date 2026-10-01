// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/gtpu/gtpu_gateway.h"
#include "ocudu/ran/qos/five_qi.h"
#include "ocudu/ran/s_nssai.h"
#include <map>

namespace ocudu {

using f1u_gw_list        = std::vector<std::unique_ptr<gtpu_gateway>>;
using f1u_five_qi_gw_map = std::map<five_qi_t, f1u_gw_list>;
using f1u_s_nssai_gw_map = std::map<s_nssai_t, f1u_five_qi_gw_map>;

struct gtpu_gateway_maps {
  f1u_gw_list        default_gws;
  f1u_s_nssai_gw_map gw_maps;

  void add_gtpu_gateway(std::optional<uint8_t>        sst,
                        std::optional<uint32_t>       sd,
                        std::optional<five_qi_t>      five_qi,
                        std::unique_ptr<gtpu_gateway> gtpu_gw)
  {
    if (not five_qi.has_value() || not sst.has_value()) {
      default_gws.push_back(std::move(gtpu_gw));
    } else {
      s_nssai_t s_nssai{slice_service_type{*sst}, {}};
      if (sd.has_value()) {
        expected<slice_differentiator> exp = slice_differentiator::create(*sd);
        if (not exp.has_value()) {
          report_error("Invalid F1U socket configuration. SD is invalid");
        }
        s_nssai.sd = *exp;
      }
      gw_maps[s_nssai][five_qi.value()].push_back(std::move(gtpu_gw));
    }
  }
};

} // namespace ocudu
