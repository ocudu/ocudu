// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "CLI/CLI11.hpp"

namespace ocudu {

struct gnb_appconfig;
struct cu_cp_unit_config;
struct du_high_unit_config;

/// Configures the given CLI11 application with the gNB application configuration schema.
void configure_cli11_with_gnb_appconfig_schema(CLI::App& app, gnb_appconfig& gnb_parsed_cfg);

/// Auto derive gNB parameters after the parsing.
void autoderive_gnb_parameters_after_parsing(CLI::App& app, gnb_appconfig& parsed_cfg);

/// Auto derive the supported TAs for the CU-CP AMF config from the DU cells config.
void autoderive_supported_tas_for_amf_from_du_cells(const du_high_unit_config& du_hi_cfg, cu_cp_unit_config& cu_cp_cfg);

} // namespace ocudu
