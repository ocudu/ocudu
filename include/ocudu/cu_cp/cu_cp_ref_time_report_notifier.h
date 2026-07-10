// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/slot_point.h"
#include <chrono>
#include <optional>

namespace ocudu::ocucp {

/// Decoded Reference Time Information reported by a DU, mapping a reference slot to a system clock time point.
struct cu_cp_ref_time_report {
  /// Reference slot (kHz15 numerology) the reported time refers to.
  slot_point ref_slot;
  /// System clock time point corresponding to the reference slot.
  std::chrono::system_clock::time_point time;
  /// Uncertainty of the reported time, in units of 25 ns.
  std::optional<uint16_t> uncertainty;
  /// True if the reported time comes from the DU local clock, false if it is GPS-based.
  bool is_local_clock;
};

/// Notifier invoked when a DU reports Reference Time Information (used e.g. to derive time-SFN mappings for NTN).
class cu_cp_ref_time_report_notifier
{
public:
  virtual ~cu_cp_ref_time_report_notifier() = default;

  /// \brief Called when a Reference Time Information Report is received from a DU.
  /// \param served_cells CGIs of the cells served by the reporting DU. All share the reported timeline.
  /// \param report Decoded reference time information.
  virtual void on_ref_time_info_report(span<const nr_cell_global_id_t> served_cells,
                                       const cu_cp_ref_time_report&    report) = 0;
};

} // namespace ocudu::ocucp
