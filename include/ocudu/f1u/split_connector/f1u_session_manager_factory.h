// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1u/split_connector/f1u_session_manager.h"

namespace ocudu {

/// \brief Creates an F1-U bearer for the CU-UP.
std::unique_ptr<f1u_session_manager> create_f1u_cu_up_session_manager(const f1u_session_maps& f1u_sessions);

} // namespace ocudu
