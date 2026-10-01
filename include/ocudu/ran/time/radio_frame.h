// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/time/radio_frame_constants.h"
#include <chrono>

namespace ocudu {

/// Time unit for subframes. One subframe is 1 millisecond.
using subframes = std::chrono::milliseconds;

/// Time unit of radio frames. One radio frame is 10 milliseconds.
using radio_frames = std::chrono::duration<std::chrono::milliseconds::rep, std::ratio<1, 100>>;

/// Time unit of hyper system frames. One hyper frame is 10 * 1024 = 10240 milliseconds.
using hyper_frames =
    std::chrono::duration<std::chrono::milliseconds::rep, std::ratio<radio_frame_constants::NOF_SFNS, 100>>;

} // namespace ocudu
