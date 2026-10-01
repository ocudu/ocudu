// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/gateways/f1c_connection_client.h"

namespace ocudu {

/// Generate a stub F1-C connection client for when the O-DU application is in test mode.
std::unique_ptr<odu::f1c_connection_client> make_test_mode_f1c_connection_client();

} // namespace ocudu
