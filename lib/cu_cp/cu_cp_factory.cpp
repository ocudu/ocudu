// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/cu_cp/cu_cp_factory.h"
#include "cu_cp_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/support/error_handling.h"

using namespace ocudu;

std::unique_ptr<ocucp::cu_cp> ocudu::create_cu_cp(const ocucp::cu_cp_configuration& cfg)
{
  return std::make_unique<ocucp::cu_cp_impl>(cfg);
}
