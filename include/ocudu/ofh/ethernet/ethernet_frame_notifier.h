// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ofh/ethernet/ethernet_unique_buffer.h"

namespace ocudu {
namespace ether {

/// Describes an Ethernet frame notifier.
class frame_notifier
{
public:
  /// Default destructor.
  virtual ~frame_notifier() = default;

  /// Notifies the reception of an Ethernet frame coming from the underlying Ethernet link.
  virtual void on_new_frame(unique_rx_buffer buffer) = 0;
};

} // namespace ether
} // namespace ocudu
