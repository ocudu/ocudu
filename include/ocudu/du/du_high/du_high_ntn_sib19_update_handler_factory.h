// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ntn/ntn_sib19_update_handler.h"
#include <memory>

namespace ocudu {

namespace odu {
class du_configurator;
}

/// \brief Create NTN SIB19 msg update handler.
///
/// \param du_cfgtr DU configurator interface.
/// \return NTN SIB19 msg update handler.
std::unique_ptr<ocudu_ntn::ntn_sib19_update_handler> create_ntn_sib19_update_handler(odu::du_configurator& du_cfgtr);

} // namespace ocudu
