// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/pusch/pusch_antenna_ports_mapping.h"
#include "ocudu/ran/pusch/pusch_constants.h"
#include "fmt/format.h"
#include <array>
#include <gtest/gtest.h>

using namespace ocudu;

static dmrs_port_mask make_ports(std::initializer_list<unsigned> ports)
{
  dmrs_port_mask mask(DMRS_MAX_NPORTS);
  for (unsigned port : ports) {
    mask.set(port);
  }
  return mask;
}

TEST(pusch_antenna_port_mapping_test, every_row_is_found_at_its_index)
{
  for (dmrs_max_length max_length : {dmrs_max_length::len1, dmrs_max_length::len2}) {
    for (bool transform_precoder : {false, true}) {
      for (unsigned nof_layers = 1; nof_layers <= pusch_constants::MAX_NOF_LAYERS; ++nof_layers) {
        const auto table =
            get_pusch_antenna_port_mapping_table(transform_precoder, dmrs_config_type::type1, max_length, nof_layers);
        for (unsigned i_row = 0; i_row != table.size(); ++i_row) {
          const pusch_antenna_ports_mapping& row = table[i_row];
          // The number of DM-RS ports must select the table of the row.
          ASSERT_EQ(row.dmrs_ports.count(), nof_layers);
          // The CDM groups without data must include the CDM groups of the DM-RS ports.
          ASSERT_GE(row.nof_dmrs_cdm_groups_without_data,
                    get_min_nof_cdm_groups_without_data(dmrs_config_type::type1, row.dmrs_ports));
          // Only a double-symbol DM-RS has ports 4 to 7.
          ASSERT_TRUE(row.nof_front_load_symbols == 2 or row.dmrs_ports.find_highest() < 4);
          // The row is unique, hence the search returns its index.
          ASSERT_EQ(
              find_pusch_antenna_port_mapping_row_index(transform_precoder, dmrs_config_type::type1, max_length, row),
              i_row)
              << fmt::format("max_length={} transform_precoder={} nof_layers={} row={}",
                             fmt::underlying(max_length),
                             transform_precoder,
                             nof_layers,
                             i_row);
        }
      }
    }
  }
}

TEST(pusch_antenna_port_mapping_test, single_user_ports_without_data_in_any_cdm_group_select_ts38212_rows)
{
  // TS 38.212 Tables 7.3.1.1.2-8 to 7.3.1.1.2-11 for maximum length 1 and Tables 7.3.1.1.2-12 to 7.3.1.1.2-15 for
  // maximum length 2, ports 0 to nof_layers - 1 with two CDM groups without data and a single front-load symbol.
  const std::array<unsigned, pusch_constants::MAX_NOF_LAYERS> expected_rows = {2, 1, 0, 0};
  for (dmrs_max_length max_length : {dmrs_max_length::len1, dmrs_max_length::len2}) {
    for (unsigned nof_layers = 1; nof_layers <= pusch_constants::MAX_NOF_LAYERS; ++nof_layers) {
      dmrs_port_mask ports(DMRS_MAX_NPORTS);
      ports.fill(0, nof_layers);
      ASSERT_EQ(find_pusch_antenna_port_mapping_row_index(false, dmrs_config_type::type1, max_length, {2, ports, 1}),
                expected_rows[nof_layers - 1])
          << fmt::format("max_length={} nof_layers={}", fmt::underlying(max_length), nof_layers);
    }

    // TS 38.212 Tables 7.3.1.1.2-6 and 7.3.1.1.2-7, port 0.
    ASSERT_EQ(
        find_pusch_antenna_port_mapping_row_index(true, dmrs_config_type::type1, max_length, {2, make_ports({0}), 1}),
        0);
  }
}

TEST(pusch_antenna_port_mapping_test, multi_user_ports_select_rows_of_other_ports)
{
  // Two UEs with two layers each share the DM-RS symbols on CDM groups 0 and 1.
  ASSERT_EQ(find_pusch_antenna_port_mapping_row_index(
                false, dmrs_config_type::type1, dmrs_max_length::len1, {2, make_ports({2, 3}), 1}),
            2);
  // Two UEs with one layer each.
  ASSERT_EQ(find_pusch_antenna_port_mapping_row_index(
                false, dmrs_config_type::type1, dmrs_max_length::len1, {2, make_ports({1}), 1}),
            3);
  // Ports in CDM group 1 cannot have data in CDM group 1.
  ASSERT_FALSE(find_pusch_antenna_port_mapping_row_index(
                   false, dmrs_config_type::type1, dmrs_max_length::len1, {1, make_ports({2, 3}), 1})
                   .has_value());

  // With a double-symbol DM-RS, four UEs with two layers each, TS 38.212 Table 7.3.1.1.2-13, value 7.
  ASSERT_EQ(find_pusch_antenna_port_mapping_row_index(
                false, dmrs_config_type::type1, dmrs_max_length::len2, {2, make_ports({6, 7}), 2}),
            7);
  // Ports 4 to 7 require a double-symbol DM-RS.
  ASSERT_FALSE(find_pusch_antenna_port_mapping_row_index(
                   false, dmrs_config_type::type1, dmrs_max_length::len2, {2, make_ports({6, 7}), 1})
                   .has_value());
  // A double-symbol DM-RS requires a maximum length of 2.
  ASSERT_FALSE(find_pusch_antenna_port_mapping_row_index(
                   false, dmrs_config_type::type1, dmrs_max_length::len1, {2, make_ports({0, 1}), 2})
                   .has_value());
}

TEST(pusch_antenna_port_mapping_test, unsupported_mappings_are_not_found)
{
  // Transform precoding is only possible with one layer.
  ASSERT_TRUE(get_pusch_antenna_port_mapping_table(true, dmrs_config_type::type1, dmrs_max_length::len1, 2).empty());
  ASSERT_TRUE(get_pusch_antenna_port_mapping_table(true, dmrs_config_type::type1, dmrs_max_length::len2, 2).empty());
  // The tables for DM-RS type 2 are not supported.
  ASSERT_TRUE(get_pusch_antenna_port_mapping_table(false, dmrs_config_type::type2, dmrs_max_length::len1, 1).empty());
  ASSERT_TRUE(get_pusch_antenna_port_mapping_table(false, dmrs_config_type::type2, dmrs_max_length::len2, 1).empty());
  // A PUSCH carries at most four layers.
  ASSERT_FALSE(find_pusch_antenna_port_mapping_row_index(
                   false, dmrs_config_type::type1, dmrs_max_length::len1, {2, make_ports({0, 1, 2, 3, 4}), 1})
                   .has_value());
}
