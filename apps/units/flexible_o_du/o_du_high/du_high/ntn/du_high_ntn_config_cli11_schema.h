// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "CLI/CLI11.hpp"
#include <optional>

namespace ocudu {

struct ntn_config;
struct du_high_unit_cell_ntn_config;

/// Configures shared NTN config CLI11 options.
/// \param app The CLI11 application or subcommand to configure.
/// \param config NTN config to populate.
void configure_cli11_ntn_config_args(CLI::App& app, ntn_config& config);

/// Configures per-cell NTN CLI11 options. Allows overriding NTN parameters on a per-cell basis.
/// \param app The CLI11 application or subcommand to configure.
/// \param cell_ntn_params Optional NTN configuration for the cell; will be populated if NTN options are specified.
void configure_cli11_cell_ntn_args(CLI::App& app, std::optional<du_high_unit_cell_ntn_config>& cell_ntn_params);

} // namespace ocudu
