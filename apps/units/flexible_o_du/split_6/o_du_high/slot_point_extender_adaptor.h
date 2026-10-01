// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/p7_slot_indication_notifier.h"
#include "ocudu/ran/slot_point_extended.h"
#include <chrono>

namespace ocudu {

/// Hyper-SFN of the first radio frame at or after \c now whose SFN equals \c sfn.
unsigned get_hyper_sfn(uint32_t sfn, std::chrono::system_clock::time_point now);

/// Lifts a FAPI SFN/slot into a continuous slot timeline. The first indication is seeded by \ref get_hyper_sfn;
/// later indications advance from the previous extended slot.
class slot_point_extender_adaptor : public fapi::p7_slot_indication_notifier
{
  fapi::p7_slot_indication_notifier& notifier;
  bool                               has_prev_extended_slot{false};
  slot_point                         prev_raw_slot{};
  slot_point_extended                prev_extended_slot{};

public:
  explicit slot_point_extender_adaptor(fapi::p7_slot_indication_notifier& notifier_) : notifier(notifier_) {}

  void on_slot_indication(const fapi::slot_indication& msg) override;
};

} // namespace ocudu
