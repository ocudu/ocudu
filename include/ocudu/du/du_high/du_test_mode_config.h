// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/rnti.h"
#include <chrono>
#include <optional>

namespace ocudu {
namespace odu {

/// Configuration that enable DU test mode operation.
struct du_test_mode_config {
  struct test_mode_ue_config {
    rnti_t   rnti    = to_rnti(0x4444);
    uint16_t nof_ues = 1;
    /// Number of slots between consecutive test mode UE creations for a given cell.
    unsigned                ue_creation_stagger_slots = 10;
    std::optional<unsigned> auto_ack_indication_delay;
    bool                    pdsch_active;
    bool                    pusch_active;
    unsigned                cqi;
    unsigned                ri;
    unsigned                pmi;
    unsigned                i_1_1;
    std::optional<unsigned> i_1_3;
    unsigned                i_2;
    /// \brief Number of milliseconds of active traffic before all UEs in a cell are released. When set,
    /// UEs cycle through attach, traffic, detach, and attach again indefinitely. Unset disables cycling.
    std::optional<std::chrono::milliseconds> attach_detach_duration;
    /// \brief Guard period duration between a release cycle and the next creation cycle.
    std::chrono::milliseconds attach_detach_guard_duration{1000};
  };

  /// Creation of a phantom UE for test mode operation.
  std::optional<test_mode_ue_config> test_ue;
};

} // namespace odu
} // namespace ocudu
