// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/messages/slot_indication.h"
#include "ocudu/ran/slot_point_extended.h"

namespace ocudu {
namespace fapi {

/// Builds and returns a slot.indication message with the given parameters, as per SCF-222 v4.0 section 3.4.1 in table
/// Slot indication message body.
inline slot_indication build_slot_indication(slot_point_extended                                slot,
                                             std::chrono::time_point<std::chrono::system_clock> time_point)
{
  slot_indication msg;

  msg.slot       = slot;
  msg.time_point = time_point;

  return msg;
}

} // namespace fapi
} // namespace ocudu
