// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

struct cell_resource_allocator;

/// UCI scheduling interface, which handles the scheduling of SR and CSI opportunities.
class uci_scheduler
{
public:
  virtual ~uci_scheduler() = default;

  /// Schedules the SR opportunities and CSI occasions.
  /// \param[out,in] res_alloc struct with scheduling results.
  virtual void run_slot(cell_resource_allocator& res_alloc) = 0;
};

} // namespace ocudu
