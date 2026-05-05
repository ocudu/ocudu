// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ntn/ntn_configuration_manager_factory.h"
#include "ntn_configuration_manager_impl.h"
#include "ocudu/ntn/ntn_configuration_manager.h"
#include "ocudu/ntn/ntn_configuration_manager_dependencies.h"

using namespace ocudu;
using namespace ocudu_ntn;

std::unique_ptr<ntn_configuration_manager>
ocudu::ocudu_ntn::create_ntn_configuration_manager(const ntn_configuration_manager_config& ntn_config,
                                                   ntn_configuration_manager_dependencies  dependencies)
{
  // NTN Time provider has to be provided.
  if (!dependencies.time_provider) {
    // Note: This requires access to mac_subframe_time_mapper.
    report_error("NTN time provider must be provided.");
  }

  return std::make_unique<ntn_configuration_manager_impl>(ntn_config, std::move(dependencies));
}
