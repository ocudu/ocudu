// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// Helpers to build security keys from hex strings, for tests and fuzz harnesses.

#pragma once

#include "ocudu/security/security.h"
#include <string>

namespace ocudu::test_helpers {

/// \brief Converts a hex string (e.g. 01FA02) to a sec_key.
security::sec_key make_sec_key(const std::string& hex_str);

/// \brief Converts a hex string (e.g. 01FA02) to a sec_128_key.
security::sec_128_key make_sec_128_key(const std::string& hex_str);

} // namespace ocudu::test_helpers
