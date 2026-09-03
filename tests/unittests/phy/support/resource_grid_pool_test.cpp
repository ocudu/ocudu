// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "resource_grid_test_doubles.h"
#include "ocudu/adt/format.h"
#include "ocudu/phy/support/shared_resource_grid.h"
#include "ocudu/phy/support/support_factories.h"
#include <gtest/gtest.h>
#include <vector>

using namespace ocudu;

static void test_allocate_grids(unsigned nof_slots)
{
  // Create grids.
  std::vector<std::unique_ptr<resource_grid>> grid_pool;
  std::vector<resource_grid*>                 expected_grids;

  // Generate resource grids
  for (unsigned slot = 0; slot != nof_slots; ++slot) {
    grid_pool.emplace_back(std::make_unique<resource_grid_dummy>());
    expected_grids.emplace_back(grid_pool.back().get());
  }

  // Create resource grid pool
  std::unique_ptr<resource_grid_pool> pool = create_generic_resource_grid_pool(std::move(grid_pool));

  // Iterate all parameters and assert grid reference
  std::vector<shared_resource_grid> reserved_grids;
  for (unsigned slot = 0; slot != nof_slots; ++slot) {
    // Get grid.
    shared_resource_grid grid = pool->allocate_resource_grid({0, slot});
    report_fatal_error_if_not(grid.is_valid(), "grid.is_valid()");

    // Verify grid reference match
    report_fatal_error_if_not(
        (reinterpret_cast<const void*>(&grid.get())) == (reinterpret_cast<const void*>(expected_grids[slot])),
        "reinterpret_cast<const void*>(&grid.get()) != reinterpret_cast<const void*>(expected_grids[slot])");

    // Move grid to the reserved list to avoid being released.
    reserved_grids.emplace_back(std::move(grid));
  }
}

TEST(resource_grid_pool_test, allocate_grids)
{
  for (unsigned nof_slots : {40}) {
    test_allocate_grids(nof_slots);
  }
}
