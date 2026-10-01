// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu::radio_frame_constants {

/// Number of subframes per system frame.
constexpr uint32_t NOF_SUBFRAMES_PER_FRAME = 10;

/// Number of System Frame Number (SFN) values.
constexpr size_t NOF_SFNS = 1024;

/// Number of Hyper System Frame Number (HyperSFN) values. As per TS 38.331, a HyperSFN ranges from 0 to 1023.
constexpr size_t NOF_HYPER_SFNS = 1024;

/// Duration of a subframe in milliseconds.
constexpr uint32_t SUBFRAME_DURATION_MSEC = 1;

} // namespace ocudu::radio_frame_constants
