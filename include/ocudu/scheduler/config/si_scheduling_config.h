// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/support/units.h"
#include <optional>

namespace ocudu {

/// \brief Maximum number of SI messages that can be scheduled as per TS 38.331, "maxSI-Message".
constexpr size_t MAX_SI_MESSAGES = 32U;

/// Scheduling parameters of the SI message.
struct si_message_scheduling_config {
  /// SI message payload size in bytes.
  units::bytes msg_len;
  /// Periodicity of the SI-message in radio frames. Values: {8, 16, 32, 64, 128, 256, 512}.
  unsigned period_radio_frames;
  /// SI window position of the associated SI-message. See TS 38.331, \c SchedulingInfo2-r17. Values: {1,...,256}.
  /// \remark This field is only applicable for release 17 \c SI-SchedulingInfo.
  std::optional<unsigned> si_window_position;
  /// \brief Whether this SI-message requires explicit activation before it is actually scheduled.
  ///
  /// An SI-message that requires activation keeps its reserved occasion in schedulingInfoList, but is not actually
  /// scheduled until activated (see \c si_message_scheduler::activate_si_message). Used e.g. for SI-messages that
  /// only carry PWS (ETWS/CMAS) SIBs (SIB6/7/8), which stay dormant until a Write-Replace Warning is received.
  bool requires_activation = false;
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
};

} // namespace ocudu
