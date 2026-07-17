// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_processor_factory.h"
#include "cu_up_processor_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace ocudu;
using namespace ocucp;

std::unique_ptr<cu_up_processor> ocudu::ocucp::create_cu_up_processor(const cu_up_processor_config&       cfg,
                                                                      const cu_up_processor_dependencies& dependencies)
{
  return std::make_unique<cu_up_processor_impl>(cfg, dependencies);
}
