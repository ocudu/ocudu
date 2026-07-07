// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu {

/// \brief Offset between the Unix epoch and the GPS epoch, in nanoseconds.
///
/// The Unix epoch is 00:00:00 UTC on 1 January 1970. The GPS epoch is 00:00:00 UTC on 6 January 1980, i.e. 3657
/// days (315 964 800 seconds) later. This offset does not account for leap seconds inserted after the GPS epoch.
///
/// Used to convert between GPS-relative and Unix-relative time points, e.g. when encoding/decoding the
/// ReferenceTime-r16 IE (TS 38.331 section 6.3.2).
constexpr int64_t GPS_EPOCH_OFFSET_NS = 315964800LL * 1'000'000'000LL;

} // namespace ocudu
