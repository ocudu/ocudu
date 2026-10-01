// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "CLI/CLI11.hpp"

namespace ocudu {

/// Creates an instance of a YAML configuration parser.
std::unique_ptr<CLI::Config> create_yaml_config_parser();

} // namespace ocudu
