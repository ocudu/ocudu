// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/config/si_scheduling_config.h"
#include "ocudu/support/ocudu_assert.h"
#include <algorithm>

using namespace ocudu;

si_scheduling_config ocudu::make_si_epoch_config(const si_scheduling_config& cell_si_sched_cfg,
                                                 span<const sib_type_set>    on_air)
{
  si_scheduling_config epoch_cfg;
  epoch_cfg.sib1_payload_size   = cell_si_sched_cfg.sib1_payload_size;
  epoch_cfg.si_window_len_slots = cell_si_sched_cfg.si_window_len_slots;

  const span<const si_message_scheduling_config> si_msgs = cell_si_sched_cfg.si_messages;

  auto is_on_air = [on_air](sib_type_set sibs) {
    return std::find(on_air.begin(), on_air.end(), sibs) != on_air.end();
  };

  // The SI window of an SI message listed in the SIB1 schedulingInfoList derives from its position in that list, so the
  // epoch holds exactly the SI messages the cell broadcasts, in the order SIB1 lists them. A dormant warning is left
  // out altogether, and the ones on air come after every SI message that is always broadcast, so that their SI windows
  // stay in place as warnings come and go.
  for (const si_message_scheduling_config& si_msg : si_msgs) {
    if (not si_msg.si_window_position.has_value()) {
      epoch_cfg.si_messages.push_back(si_msg);
    }
  }
  for (const si_message_scheduling_config& si_msg : cell_si_sched_cfg.pws_si_messages) {
    if (is_on_air(si_msg.sibs)) {
      epoch_cfg.si_messages.push_back(si_msg);
    }
  }
  // Entries of the schedulingInfoList2 state their SI window position explicitly, so their position in the epoch does
  // not matter.
  for (const si_message_scheduling_config& si_msg : si_msgs) {
    if (si_msg.si_window_position.has_value()) {
      epoch_cfg.si_messages.push_back(si_msg);
    }
  }

  ocudu_sanity_check(std::all_of(epoch_cfg.si_messages.begin(),
                                 epoch_cfg.si_messages.end(),
                                 [&epoch_cfg](const si_message_scheduling_config& si_msg) {
                                   return std::count_if(epoch_cfg.si_messages.begin(),
                                                        epoch_cfg.si_messages.end(),
                                                        [&si_msg](const si_message_scheduling_config& other) {
                                                          return other.sibs == si_msg.sibs;
                                                        }) == 1;
                                 }),
                     "An SI message cannot be listed twice in an SI epoch");

  return epoch_cfg;
}
