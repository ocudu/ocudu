// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "fbs/common_generated.h"
#include "lib/scheduler/config/cell_configuration.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include <array>
#include <gtest/gtest.h>

namespace ocudu::schedtrace::test_helper {

inline void check_bwp_cfg(const fbs::BwpConfiguration* bwp_ev, const ocudu::bwp_configuration& bwp)
{
  ASSERT_NE(bwp_ev, nullptr);
  EXPECT_EQ(bwp_ev->cp(), static_cast<fbs::CyclicPrefix>(bwp.cp.value));
  EXPECT_EQ(bwp_ev->scs(), static_cast<fbs::SubcarrierSpacing>(bwp.scs));
  EXPECT_EQ(bwp_ev->crb_start(), bwp.crbs.start());
  EXPECT_EQ(bwp_ev->crb_length(), bwp.crbs.length());
}

/// Returns a real scheduler cell configuration for the given cell index, suitable for constructing a cell event tracer.
/// The configurations are owned by a process-wide manager and are stable for the lifetime of the test program.
inline const ocudu::cell_configuration& make_test_cell_cfg(du_cell_index_t cell_idx = to_du_cell_index(0))
{
  static test_helpers::test_sched_config_manager                        mgr{cell_config_builder_params{}};
  static std::array<const ocudu::cell_configuration*, MAX_NOF_DU_CELLS> cells{};
  if (cells[static_cast<size_t>(cell_idx)] == nullptr) {
    sched_cell_configuration_request_message req = mgr.get_default_cell_config_request();
    req.cell_index                               = cell_idx;
    cells[static_cast<size_t>(cell_idx)]         = mgr.add_cell(req);
  }
  return *cells[static_cast<size_t>(cell_idx)];
}

} // namespace ocudu::schedtrace::test_helper
