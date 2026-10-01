// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/du/du_high/du_manager/du_manager_factory.h"
#include "du_manager_impl.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace odu;

std::unique_ptr<du_manager> ocudu::odu::create_du_manager(const du_manager_params& params)
{
  auto du_manager = std::make_unique<du_manager_impl>(params);
  return du_manager;
}
