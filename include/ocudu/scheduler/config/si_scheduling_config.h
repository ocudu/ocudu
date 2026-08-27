// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/sib/sib_type.h"
#include "ocudu/support/units.h"
#include <optional>

namespace ocudu {

/// \brief Maximum number of SI messages that can be scheduled as per TS 38.331, "maxSI-Message".
constexpr size_t MAX_SI_MESSAGES = 32U;

/// Scheduling parameters of the SI message.
struct si_message_scheduling_config {
  /// SIBs carried by this SI message. Identifies the SI message within the cell.
  sib_type_set sibs;
  /// SI message payload size in bytes.
  units::bytes msg_len;
  /// Periodicity of the SI-message in radio frames. Values: {8, 16, 32, 64, 128, 256, 512}.
  unsigned period_radio_frames;
  /// SI window position of the associated SI-message. See TS 38.331, \c SchedulingInfo2-r17. Values: {1,...,256}.
  /// \remark This field is only applicable for release 17 \c SI-SchedulingInfo.
  std::optional<unsigned> si_window_position;
  /// \brief Whether this SI-message should be activated at cell startup and broadcast indefinitely, rather than
  /// waiting for a Write-Replace Warning.
  ///
  /// Only meaningful for PWS SI-messages. Used for test_mode-configured ETWS/CMAS content.
  bool test_mode_auto_broadcast = false;

  /// Whether this SI-message requires explicit activation before it is actually scheduled.
  bool requires_activation() const { return sibs.is_pws(); }

  /// Whether this SI-message carries the NTN SIB19, whose content is pushed to the PHY immediately.
  bool is_ntn() const { return sibs.is_ntn(); }

  bool operator==(const si_message_scheduling_config& other) const
  {
    return sibs == other.sibs and msg_len == other.msg_len and period_radio_frames == other.period_radio_frames and
           si_window_position == other.si_window_position and
           test_mode_auto_broadcast == other.test_mode_auto_broadcast;
  }
  bool operator!=(const si_message_scheduling_config& other) const { return not(*this == other); }
};

/// \brief Configuration of the SI message scheduling.
///
/// This struct will be handled by the MAC scheduler to determine the required PDCCH and PDSCH grants for SI.
struct si_scheduling_config {
  /// SIB1 payload size in bytes.
  units::bytes sib1_payload_size = units::bytes{0U};
  /// List of SI-messages to schedule.
  static_vector<si_message_scheduling_config, MAX_SI_MESSAGES> si_messages;
  /// \brief The length of the SI scheduling window, in slots.
  ///
  /// It is always shorter or equal to the period of the SI message.
  /// Values: {0, 5, 10, 20, 40, 80, 160, 320, 640, 1280}. The value 0 is reserved for the case no SI messages need to
  /// be scheduled.
  unsigned si_window_len_slots = 0;

  bool operator==(const si_scheduling_config& other) const
  {
    return sib1_payload_size == other.sib1_payload_size and si_messages == other.si_messages and
           si_window_len_slots == other.si_window_len_slots;
  }
  bool operator!=(const si_scheduling_config& other) const { return not(*this == other); }
};

/// \brief Derives the SI scheduling configuration of the SI epoch that broadcasts a given set of warnings.
///
/// It holds the SI messages that the epoch broadcasts, in the order the SIB1 schedulingInfoList lists them in.
/// \param cell_si_sched_cfg SI scheduling configuration of the cell, listing every SI message it can broadcast.
/// \param on_air SI messages carrying a warning. Empty for the epoch of the normal operation.
si_scheduling_config make_si_epoch_config(const si_scheduling_config& cell_si_sched_cfg,
                                          span<const sib_type_set>    on_air);

} // namespace ocudu
