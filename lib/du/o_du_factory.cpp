// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/du/o_du_factory.h"
#include "o_du_impl.h"
#include "ocudu/ocudulog/ocudulog.h"

using namespace ocudu;
using namespace odu;

std::unique_ptr<o_du> odu::make_o_du(o_du_dependencies&& dependencies)
{
  ocudulog::fetch_basic_logger("DU").info("O-DU created successfully");

  return std::make_unique<o_du_impl>(std::move(dependencies));
}
