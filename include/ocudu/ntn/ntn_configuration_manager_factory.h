// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <memory>

namespace ocudu {

struct ntn_config;

namespace ocudu_ntn {

class ntn_configuration_manager;
struct ntn_configuration_manager_config;
struct ntn_configuration_manager_dependencies;

/// NTN Configuration Manager factory interface with the given configuration and dependencies.
std::unique_ptr<ntn_configuration_manager>
create_ntn_configuration_manager(const ntn_configuration_manager_config& ntn_config,
                                 ntn_configuration_manager_dependencies  dependencies);

} // namespace ocudu_ntn
} // namespace ocudu
