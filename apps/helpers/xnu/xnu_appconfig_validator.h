// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

struct xnu_sockets_appconfig;

/// \brief Validates the Xn-U sockets appconfig.
bool validate_xnu_sockets_appconfig(const xnu_sockets_appconfig& config);

} // namespace ocudu
