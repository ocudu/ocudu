// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/serdes/ofh_uplane_message_decoder_properties.h"

namespace ocudu {
namespace ofh {

/// Open Fronthaul message decoder properties.
struct message_decoder_results {
  /// eAxC.
  unsigned eaxc;
  /// User-Plane decoding results.
  uplane_message_decoder_results uplane_results;
};

} // namespace ofh
} // namespace ocudu
