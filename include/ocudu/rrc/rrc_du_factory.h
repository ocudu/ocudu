// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/rrc/rrc_config.h"
#include "ocudu/rrc/rrc_du.h"
#include "ocudu/rrc/rrc_ue.h"

namespace ocudu {

namespace ocucp {

struct ue_context;

/// Create an instance of an RRC entity
std::unique_ptr<rrc_du> create_rrc_du(const rrc_cfg_t& cfg);

} // namespace ocucp

} // namespace ocudu
