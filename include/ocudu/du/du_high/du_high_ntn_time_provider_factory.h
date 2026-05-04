// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ntn/ntn_time_provider.h"
#include <memory>

namespace ocudu {

class mac_subframe_time_mapper;

/// \brief Create NTN time (i.e., time point to slot point mapping) provider.
///
/// \param du_time_mapper MAC subframe time mapper.
/// \return NTN time provider.
std::unique_ptr<ocudu_ntn::ntn_time_provider>
create_du_high_ntn_time_provider(mac_subframe_time_mapper& du_time_mapper);

} // namespace ocudu
