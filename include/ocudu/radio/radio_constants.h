// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {

/// Maximum number of channels per stream in a radio baseband unit.
constexpr unsigned RADIO_MAX_NOF_CHANNELS = 8;

/// Maximum number of streams that a radio baseband unit can support.
constexpr unsigned RADIO_MAX_NOF_STREAMS = 8;

/// Total maximum number of ports that a radio baseband unit can support.
constexpr unsigned RADIO_MAX_NOF_PORTS = RADIO_MAX_NOF_CHANNELS * RADIO_MAX_NOF_STREAMS;

} // namespace ocudu
