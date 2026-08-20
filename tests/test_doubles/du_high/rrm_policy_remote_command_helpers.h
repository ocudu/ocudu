// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "nlohmann/json.hpp"
#include "ocudu/adt/expected.h"
#include "ocudu/ran/plmn_identity.h"

namespace ocudu {
struct s_nssai_t;

namespace odu {
class du_configurator;
} // namespace odu

namespace test_helpers {

/// Builds an RRM policy ratio remote-command request and executes it against the given DU configurator.
expected<nlohmann::json, std::string> apply_rrm_policy_reconfiguration(odu::du_configurator& configurator,
                                                                       plmn_identity         plmn_id,
                                                                       s_nssai_t             s_nssai,
                                                                       unsigned              min_rbs,
                                                                       unsigned              max_rbs);

} // namespace test_helpers
} // namespace ocudu
