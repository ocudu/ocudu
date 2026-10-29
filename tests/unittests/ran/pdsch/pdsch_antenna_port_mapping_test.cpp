// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/pdsch/pdsch_antenna_ports_mapping.h"
#include "ocudu/ran/pdsch/pdsch_constants.h"
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

static const std::array<dmrs_config_type, 2> dmrs_types   = {dmrs_config_type::type1, dmrs_config_type::type2};
static const std::array<dmrs_max_length, 2>  dmrs_lengths = {dmrs_max_length::len1, dmrs_max_length::len2};

TEST(pdsch_antenna_port_mapping_test, dmrs_cdm_group_follows_ts38211_tables)
{
  // TS 38.211 Table 7.4.1.1.2-1.
  const std::array<unsigned, 8> type1_groups = {0, 0, 1, 1, 0, 0, 1, 1};
  for (unsigned port = 0; port != type1_groups.size(); ++port) {
    ASSERT_EQ(get_dmrs_cdm_group_index(dmrs_config_type::type1, port), type1_groups[port]) << "port=" << port;
  }
  // TS 38.211 Table 7.4.1.1.2-2.
  const std::array<unsigned, 12> type2_groups = {0, 0, 1, 1, 2, 2, 0, 0, 1, 1, 2, 2};
  for (unsigned port = 0; port != type2_groups.size(); ++port) {
    ASSERT_EQ(get_dmrs_cdm_group_index(dmrs_config_type::type2, port), type2_groups[port]) << "port=" << port;
  }

  ASSERT_EQ(get_min_nof_cdm_groups_without_data(dmrs_config_type::type1, make_ports({0, 1})), 1);
  ASSERT_EQ(get_min_nof_cdm_groups_without_data(dmrs_config_type::type1, make_ports({2})), 2);
  ASSERT_EQ(get_min_nof_cdm_groups_without_data(dmrs_config_type::type2, make_ports({4, 5})), 3);
}

TEST(pdsch_antenna_port_mapping_test, field_size_follows_ts38212)
{
  ASSERT_EQ(get_pdsch_antenna_ports_field_size(dmrs_config_type::type1, dmrs_max_length::len1), 4);
  ASSERT_EQ(get_pdsch_antenna_ports_field_size(dmrs_config_type::type1, dmrs_max_length::len2), 5);
  ASSERT_EQ(get_pdsch_antenna_ports_field_size(dmrs_config_type::type2, dmrs_max_length::len1), 5);
  ASSERT_EQ(get_pdsch_antenna_ports_field_size(dmrs_config_type::type2, dmrs_max_length::len2), 6);
}

TEST(pdsch_antenna_port_mapping_test, every_row_is_found_at_its_index)
{
  for (dmrs_config_type type : dmrs_types) {
    for (dmrs_max_length max_len : dmrs_lengths) {
      for (bool are_both_cws_enabled : {false, true}) {
        const auto table = get_pdsch_antenna_port_mapping_table(type, max_len, are_both_cws_enabled);
        for (unsigned i_row = 0; i_row != table.size(); ++i_row) {
          const pdsch_antenna_ports_mapping& row = table[i_row];
          // The number of DM-RS ports must select the table of the row.
          ASSERT_EQ(row.dmrs_ports.count() > pdsch_constants::MAX_NOF_LAYERS_PER_CODEWORD, are_both_cws_enabled);
          // The CDM groups without data must include the CDM groups of the DM-RS ports.
          ASSERT_GE(row.nof_dmrs_cdm_groups_without_data, get_min_nof_cdm_groups_without_data(type, row.dmrs_ports));
          // The row is unique, hence the search returns its index.
          ASSERT_EQ(find_pdsch_antenna_port_mapping_row_index(type, max_len, row), i_row)
              << fmt::format("type={} len={} two_cws={} row={}",
                             fmt::underlying(type),
                             fmt::underlying(max_len),
                             are_both_cws_enabled,
                             i_row);
        }
      }
    }
  }
}

TEST(pdsch_antenna_port_mapping_test, single_user_ports_select_the_first_row_with_as_many_ports)
{
  // Ports 1000 to 999 + nof_layers with the minimum number of CDM groups without data.
  const std::array<unsigned, 4> expected_rows = {0, 2, 9, 10};
  for (dmrs_config_type type : dmrs_types) {
    for (dmrs_max_length max_len : dmrs_lengths) {
      for (unsigned nof_layers = 1; nof_layers <= expected_rows.size(); ++nof_layers) {
        dmrs_port_mask ports(DMRS_MAX_NPORTS);
        ports.fill(0, nof_layers);
        const pdsch_antenna_ports_mapping mapping{get_min_nof_cdm_groups_without_data(type, ports), ports, 1};
        ASSERT_EQ(find_pdsch_antenna_port_mapping_row_index(type, max_len, mapping), expected_rows[nof_layers - 1])
            << fmt::format("type={} len={} nof_layers={}", fmt::underlying(type), fmt::underlying(max_len), nof_layers);
      }
    }
  }
}

TEST(pdsch_antenna_port_mapping_test, multi_user_ports_select_rows_without_data_in_partner_cdm_group)
{
  const dmrs_config_type type    = dmrs_config_type::type1;
  const dmrs_max_length  max_len = dmrs_max_length::len1;

  // Two UEs with two layers each share the DM-RS symbols, thus no UE has data in CDM groups 0 and 1.
  ASSERT_EQ(find_pdsch_antenna_port_mapping_row_index(type, max_len, {2, make_ports({0, 1}), 1}), 7);
  ASSERT_EQ(find_pdsch_antenna_port_mapping_row_index(type, max_len, {2, make_ports({2, 3}), 1}), 8);
  // Ports in CDM group 1 cannot have data in CDM group 1.
  ASSERT_FALSE(find_pdsch_antenna_port_mapping_row_index(type, max_len, {1, make_ports({2, 3}), 1}).has_value());
}

TEST(pdsch_antenna_port_mapping_test, unsupported_mappings_are_not_found)
{
  // DM-RS type 1 with a single front-load symbol carries at most four layers.
  ASSERT_TRUE(get_pdsch_antenna_port_mapping_table(dmrs_config_type::type1, dmrs_max_length::len1, true).empty());
  ASSERT_FALSE(find_pdsch_antenna_port_mapping_row_index(
                   dmrs_config_type::type1, dmrs_max_length::len1, {2, make_ports({0, 1, 2, 3, 4}), 1})
                   .has_value());
  // Ports 1004 to 1007 require two front-load symbols.
  ASSERT_FALSE(
      find_pdsch_antenna_port_mapping_row_index(dmrs_config_type::type1, dmrs_max_length::len2, {2, make_ports({4}), 1})
          .has_value());
  ASSERT_EQ(find_pdsch_antenna_port_mapping_row_index(
                dmrs_config_type::type1, dmrs_max_length::len2, {2, make_ports({4}), 2}),
            16);
}
