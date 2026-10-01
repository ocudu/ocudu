// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {

/// Returns a formatted string containing the current commit hash.
const char* get_build_hash();

/// Returns a formatted string containing the current commit hash and branch name (if available).
const char* get_build_info();

/// Returns the selected (CMake) build mode as string.
const char* get_build_mode();

} // namespace ocudu
