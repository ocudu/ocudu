// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/ntn_location_mapping.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

/// Mapped Cell ID configured for the first area, TS 38.300 sec. 16.14.5.
constexpr uint64_t mapped_cell_id = 0x66c001;
/// A second Mapped Cell ID, for the areas of one cell naming more than one, TS 38.300 sec. 16.14.5 NOTE 2.
constexpr uint64_t second_mapped_cell_id = 0x66c002;

ntn_location_area make_area(tac_t                   tac,
                            double                  lat_min,
                            double                  lat_max,
                            double                  lon_min,
                            double                  lon_max,
                            std::optional<uint64_t> mapped_nci = std::nullopt)
{
  ntn_location_area area;
  area.tac     = tac;
  area.lat_min = lat_min;
  area.lat_max = lat_max;
  area.lon_min = lon_min;
  area.lon_max = lon_max;
  if (mapped_nci.has_value()) {
    area.mapped_nci = nr_cell_identity::create(mapped_nci.value()).value();
  }
  return area;
}

/// Two adjoining areas, plus a third set well away from them. Only the first configures a Mapped Cell ID, so the
/// second exercises an area that reports the Uu Cell ID instead.
ntn_location_mapping make_mapping()
{
  ntn_location_mapping mapping;
  mapping.location_areas = {make_area(7, 50.0, 52.0, 14.0, 17.0, mapped_cell_id),
                            make_area(8, 52.0, 54.0, 14.0, 17.0),
                            make_area(11, 40.0, 42.0, 0.0, 2.0)};
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

TEST(ntn_location_mapping_test, position_inside_an_area_yields_its_mapped_cell_id)
{
  const std::optional<nr_cell_identity> nci = derive_mapped_cell_id_from_location(make_mapping(), {51.0, 15.0});
  ASSERT_TRUE(nci.has_value());
  EXPECT_EQ(nci->value(), mapped_cell_id);
}

TEST(ntn_location_mapping_test, area_without_a_mapped_cell_id_yields_none)
{
  // The caller then reports the Uu Cell ID of the serving cell.
  EXPECT_FALSE(derive_mapped_cell_id_from_location(make_mapping(), {53.0, 15.0}).has_value());
}

/// TS 38.300 sec. 16.14.5 keeps the derived TAI and the Mapped Cell ID separate, so an area may name only one of them.
TEST(ntn_location_mapping_test, area_without_a_tac_yields_its_mapped_cell_id_and_no_tac)
{
  ntn_location_mapping mapping;
  ntn_location_area    area;
  area.mapped_nci = nr_cell_identity::create(mapped_cell_id).value();
  area.lat_min    = 50.0;
  area.lat_max    = 52.0;
  area.lon_min    = 14.0;
  area.lon_max    = 17.0;
  mapping.location_areas.push_back(area);

  const std::optional<nr_cell_identity> nci = derive_mapped_cell_id_from_location(mapping, {51.0, 15.0});
  ASSERT_TRUE(nci.has_value());
  EXPECT_EQ(nci->value(), mapped_cell_id);
  EXPECT_FALSE(derive_tac_from_location(mapping, {51.0, 15.0}).has_value());
}

TEST(ntn_location_mapping_test, position_outside_every_area_yields_no_mapped_cell_id)
{
  EXPECT_FALSE(derive_mapped_cell_id_from_location(make_mapping(), {10.0, 10.0}).has_value());
}

TEST(ntn_location_mapping_test, overlapping_areas_yield_the_mapped_cell_id_of_the_first_configured)
{
  // TS 38.300 sec. 16.14.5 NOTE 2 allows areas that overlap, each naming its own Mapped Cell ID, so a position may sit
  // in more than one. Configuration order decides, as it does for the derived TAC.
  ntn_location_mapping mapping;
  mapping.location_areas = {make_area(7, 50.0, 52.0, 14.0, 17.0, mapped_cell_id),
                            make_area(8, 51.0, 53.0, 14.0, 17.0, second_mapped_cell_id)};

  const std::optional<nr_cell_identity> nci = derive_mapped_cell_id_from_location(mapping, {51.5, 15.0});
  ASSERT_TRUE(nci.has_value());
  EXPECT_EQ(nci->value(), mapped_cell_id);
  EXPECT_EQ(derive_tac_from_location(mapping, {51.5, 15.0}), 7);
}

TEST(ntn_location_mapping_test, empty_mapping_yields_no_mapped_cell_id)
{
  EXPECT_FALSE(derive_mapped_cell_id_from_location(ntn_location_mapping{}, {51.0, 15.0}).has_value());
}

TEST(ntn_location_mapping_test, a_configured_mapped_cell_id_is_reported_by_the_mapping)
{
  // The reverse question of the derivation: the core names a cell by the identity the gNB reported for it, so the
  // mapping has to recognise its own, TS 38.300 sec. 16.14.5.
  EXPECT_TRUE(make_mapping().reports_mapped_cell_id(nr_cell_identity::create(mapped_cell_id).value()));
}

TEST(ntn_location_mapping_test, every_mapped_cell_id_the_areas_name_is_reported_by_the_mapping)
{
  // TS 38.300 sec. 16.14.5 NOTE 2 lets Mapped Cell IDs name different geographical areas, so the areas of one cell may
  // name several. The core may then address the cell by any of them.
  ntn_location_mapping mapping;
  mapping.location_areas = {make_area(7, 50.0, 52.0, 14.0, 17.0, mapped_cell_id),
                            make_area(8, 52.0, 54.0, 14.0, 17.0, second_mapped_cell_id)};

  EXPECT_TRUE(mapping.reports_mapped_cell_id(nr_cell_identity::create(mapped_cell_id).value()));
  EXPECT_TRUE(mapping.reports_mapped_cell_id(nr_cell_identity::create(second_mapped_cell_id).value()));
}

TEST(ntn_location_mapping_test, an_unconfigured_mapped_cell_id_is_not_reported_by_the_mapping)
{
  EXPECT_FALSE(make_mapping().reports_mapped_cell_id(nr_cell_identity::create(0x66c009).value()));
}

TEST(ntn_location_mapping_test, a_mapping_without_mapped_cell_ids_reports_none)
{
  ntn_location_mapping mapping;
  mapping.location_areas = {make_area(7, 50.0, 52.0, 14.0, 17.0)};

  EXPECT_FALSE(mapping.reports_mapped_cell_id(nr_cell_identity::create(mapped_cell_id).value()));
}

TEST(ntn_location_mapping_test, an_area_yields_both_the_tac_and_the_mapped_cell_id_it_configures)
{
  // A Mapped Cell ID names a geographical area agreed with the core network, so like the derived TAC it is not
  // checked against what the cell serves, TS 38.300 sec. 16.14.5.
  ntn_location_mapping mapping;
  mapping.location_areas = {make_area(11, 40.0, 42.0, 0.0, 2.0, mapped_cell_id)};

  EXPECT_EQ(derive_tac_from_location(mapping, {41.0, 1.0}), 11);
  EXPECT_TRUE(derive_mapped_cell_id_from_location(mapping, {41.0, 1.0}).has_value());
}
