// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/cu_up/cu_up_factory.h"
#include "cu_up_impl.h"

using namespace ocudu;
using namespace ocuup;

std::unique_ptr<cu_up_interface> ocudu::ocuup::create_cu_up(const cu_up_config& cfg, cu_up_dependencies dependencies)
{
  return std::make_unique<cu_up>(cfg, std::move(dependencies));
}
