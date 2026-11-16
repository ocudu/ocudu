// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/cu_cp/o_cu_cp_factory.h"
#include "o_cu_cp_impl.h"
#include "ocudu/cu_cp/o_cu_cp_config.h"
#include "ocudu/e2/e2_cu_cp_factory.h"

using namespace ocudu;
using namespace ocucp;

std::unique_ptr<o_cu_cp> ocucp::create_o_cu_cp(const o_cu_cp_config& config, o_cu_cp_dependencies& dependencies)
{
  return std::make_unique<o_cu_cp_impl>(config, dependencies);
}
