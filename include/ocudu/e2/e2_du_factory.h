// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e2/e2_du.h"
#include "ocudu/e2/e2ap_config.h"
#include "ocudu/e2/gateways/e2_connection_client.h"
#include <memory>

namespace ocudu {

/// Creates an instance of an E2 interface (with subscription manager).
std::unique_ptr<e2_agent> create_e2_du_agent(const e2ap_config& e2ap_cfg_, e2ap_dependencies dependencies);

} // namespace ocudu
