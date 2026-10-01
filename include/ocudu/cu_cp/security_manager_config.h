// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/security/security.h"

namespace ocudu {
namespace ocucp {

struct security_manager_config {
  security::preferred_integrity_algorithms int_algo_pref_list; ///< Integrity protection algorithms preference list
  security::preferred_ciphering_algorithms enc_algo_pref_list; ///< Encryption algorithms preference list
};

} // namespace ocucp
} // namespace ocudu
