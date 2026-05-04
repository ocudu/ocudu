// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <cstddef>

namespace ocudu {
namespace ofh {

/// Open Fronthaul message type.
enum class message_type { control_plane, user_plane, num_ofh_types };

/// Maximum number of supported eAxC. Implementation defined.
constexpr unsigned MAX_NOF_SUPPORTED_EAXC = 4;

/// Maximum allowed value for eAxC ID.
constexpr size_t MAX_SUPPORTED_EAXC_ID_VALUE = 64;

} // namespace ofh
} // namespace ocudu
