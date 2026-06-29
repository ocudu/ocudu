// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include <chrono>

namespace ocudu {
namespace odu {

/// \brief PER-encodes a UTC time point into a ReferenceTime-r16 octet string (TS 38.331 section 6.3.2), as
/// carried opaquely in the ref_time field of the F1AP TimeReferenceInformation IE (TS 38.473 section 9.3.1.148).
///
/// \param time_point Time to encode.
/// \param is_local_clock When true, the counter is relative to the Unix epoch (TS 38.331 leaves the localClock
///        epoch unspecified); otherwise it is relative to the GPS epoch (00:00:00 UTC 6 January 1980).
byte_buffer pack_ref_time_r16(std::chrono::system_clock::time_point time_point, bool is_local_clock);

} // namespace odu
} // namespace ocudu
