// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "slot_point_extender_adaptor.h"
#include "ocudu/fapi/p7/messages/slot_indication.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/ran/slot_point_extended.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;

unsigned ocudu::get_hyper_sfn(uint32_t sfn, std::chrono::system_clock::time_point now)
{
  ocudu_assert(sfn < NOF_SFNS, "Invalid SFN={}", sfn);

  static constexpr std::chrono::milliseconds frame_duration{NOF_SUBFRAMES_PER_FRAME * SUBFRAME_DURATION_MSEC};

  const auto     time_since_epoch  = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
  const uint64_t clock_frame_count = static_cast<uint64_t>(time_since_epoch / frame_duration);

  const uint64_t offset = (NOF_SFNS + sfn - clock_frame_count % NOF_SFNS) % NOF_SFNS;

  return static_cast<unsigned>(((clock_frame_count + offset) / NOF_SFNS) % NOF_HYPER_SFNS);
}

void slot_point_extender_adaptor::on_slot_indication(const fapi::slot_indication& msg)
{
  slot_point          raw_slot = msg.slot.without_hyper_sfn();
  slot_point_extended extended_slot;
  if (has_prev_extended_slot) {
    uint32_t slots_per_hyper = raw_slot.nof_slots_per_hyper_system_frame();
    uint32_t raw_delta       = (raw_slot.count() + slots_per_hyper - prev_raw_slot.count()) % slots_per_hyper;
    extended_slot            = prev_extended_slot + raw_delta;
  } else {
    unsigned hfn  = get_hyper_sfn(raw_slot.sfn(), std::chrono::system_clock::now());
    extended_slot = slot_point_extended(raw_slot, hfn);
  }

  has_prev_extended_slot = true;
  prev_raw_slot          = raw_slot;
  prev_extended_slot     = extended_slot;

  // Generate and notify a new SLOT.indication with the hyper frame index.
  notifier.on_slot_indication(fapi::slot_indication{.slot = extended_slot, .time_point = msg.time_point});
}
