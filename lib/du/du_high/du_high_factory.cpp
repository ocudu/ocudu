// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/du/du_high/du_high_factory.h"
#include "du_high_impl.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace odu;

std::unique_ptr<du_high> ocudu::odu::make_du_high(const du_high_configuration& du_hi_cfg,
                                                  const du_high_dependencies&  dependencies)
{
  return std::make_unique<du_high_impl>(du_hi_cfg, dependencies);
}
