// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_processor_factory.h"
#include "du_processor_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace ocudu;
using namespace ocucp;

std::unique_ptr<du_processor> ocudu::ocucp::create_du_processor(const du_processor_config& cfg,
                                                                du_processor_dependencies  dependencies)
{
  return std::make_unique<du_processor_impl>(cfg, std::move(dependencies));
}
