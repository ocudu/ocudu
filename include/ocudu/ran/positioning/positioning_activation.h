// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/positioning/common.h"
#include <cstdint>
#include <variant>

namespace ocudu {

struct semipersistent_srs_t {
  uint8_t srs_res_set_id;
  // The IE exts is optional in the NRPPa but mandatory in the F1AP.
  std::optional<spatial_relation_info_t> srs_spatial_relation;
  std::optional<spatial_relation_info_t> srs_spatial_relation_per_srs_res;
};

struct srs_res_trigger_t {
  std::vector<uint8_t> aperiodic_srs_res_trigger_list;
};

struct aperiodic_srs_t {
  bool                             aperiodic;
  std::optional<srs_res_trigger_t> srs_res_trigger;
};

using srs_type_t = std::variant<semipersistent_srs_t, aperiodic_srs_t>;

} // namespace ocudu
