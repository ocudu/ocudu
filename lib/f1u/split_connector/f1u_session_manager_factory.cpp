// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/f1u/split_connector/f1u_session_manager_factory.h"
#include "f1u_session_manager_impl.h"

using namespace ocudu;

std::unique_ptr<f1u_session_manager> ocudu::create_f1u_cu_up_session_manager(const f1u_session_maps& f1u_sessions)
{
  return std::make_unique<f1u_session_manager_impl>(f1u_sessions);
}
