// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"

namespace ocudu {
namespace ether {

class transmitter_metrics_collector;

/// Describes an Ethernet transmitter.
class transmitter
{
public:
  /// Default destructor.
  virtual ~transmitter() = default;

  /// Sends the given list of frames through the underlying Ethernet link.
  virtual void send(span<span<const uint8_t>> frames) = 0;

  /// Returns the metrics collector of this Ethernet transmitter or nullptr if metrics are disabled.
  virtual transmitter_metrics_collector* get_metrics_collector() = 0;
};

} // namespace ether
} // namespace ocudu
