// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/ntn_location_mapping.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

/// Two adjoining areas, plus a third mapped to a TAC the cell does not broadcast.
ntn_location_mapping make_mapping()
{
  ntn_location_mapping mapping;
  mapping.location_areas = {{7, 50.0, 52.0, 14.0, 17.0}, {8, 52.0, 54.0, 14.0, 17.0}, {11, 40.0, 42.0, 0.0, 2.0}};
  return mapping;
}

std::optional<tac_t> derive(const reference_location& position)
{
  return derive_tac_from_location(make_mapping(), position);
}

} // namespace

TEST(ntn_location_mapping_test, position_inside_an_area_yields_its_tac)
{
  EXPECT_EQ(derive({51.0, 15.0}), 7);
  EXPECT_EQ(derive({53.0, 15.0}), 8);
}

TEST(ntn_location_mapping_test, position_outside_every_area_yields_no_tac)
{
  // A large footprint over an approximate coverage plan puts UEs outside every area routinely.
  EXPECT_FALSE(derive({10.0, 10.0}).has_value());
}

TEST(ntn_location_mapping_test, tac_the_cell_does_not_broadcast_is_still_derived)
{
  // The UE is in a tracking area the cell is not currently broadcasting, TS 38.300 sec. 16.14.3.1. Reporting it is
  // the point of the IE, TS 23.502 sec. 4.10.
  EXPECT_EQ(derive({41.0, 1.0}), 11);
}

TEST(ntn_location_mapping_test, overlapping_areas_resolve_to_the_first_in_configuration_order)
{
  // The areas share the 52.0 edge, and bounds are inclusive, so both contain this position.
  EXPECT_EQ(derive({52.0, 15.0}), 7);
}

TEST(ntn_location_mapping_test, empty_mapping_yields_no_tac)
{
  EXPECT_FALSE(derive_tac_from_location(ntn_location_mapping{}, {51.0, 15.0}).has_value());
}
