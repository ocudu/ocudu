// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"

namespace ocudu::rohc {

struct rohc_decromp_result {
  /// The decompressed packet. Empty in case of feedback-only or error.
  byte_buffer decomp_packet;
  /// Feedback packet for transmission to the peer. Empty in case of no feedback available.
  byte_buffer feedback_packet;
};

class rohc_decompressor
{
public:
  virtual ~rohc_decompressor()                               = default;
  virtual rohc_decromp_result decompress(byte_buffer packet) = 0;
};

} // namespace ocudu::rohc
