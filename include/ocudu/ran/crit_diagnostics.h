// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace ocudu {

struct crit_diagnostics_item {
  std::string iecrit;
  uint32_t    ie_id;
  std::string type_of_error;
};

struct crit_diagnostics_t {
  std::vector<crit_diagnostics_item> ies_crit_diagnostics = {};
  std::optional<uint16_t>            proc_code;
  std::optional<std::string>         trigger_msg;
  std::optional<std::string>         proc_crit;
  std::optional<uint16_t>            transaction_id;
};

} // namespace ocudu
