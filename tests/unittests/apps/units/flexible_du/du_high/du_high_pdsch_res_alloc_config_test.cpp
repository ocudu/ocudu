// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_cli11_schema.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_translators.h"
#include "ocudu/support/config_parsers.h"
#include <gtest/gtest.h>
#include <sstream>

using namespace ocudu;

namespace {

/// Parses a YAML configuration through the CLI11 schema, as the application does, and auto-derives its parameters.
du_high_unit_config parse_config(const std::string& yaml)
{
  CLI::App app{"du_high_pdsch_res_alloc_config_test"};
  app.config_formatter(create_yaml_config_parser());
  app.allow_config_extras(CLI::config_extras_mode::error);

  du_high_parsed_config parsed_cfg;
  configure_cli11_with_du_high_config_schema(app, parsed_cfg);

  std::istringstream input(yaml);
  app.parse_from_stream(input);
  autoderive_du_high_parameters_after_parsing(parsed_cfg.config);

  return parsed_cfg.config;
}

/// Builds the YAML configuration of a cell with the given PDSCH resource allocation and RBG size values.
std::string make_pdsch_config(const std::string& res_alloc, const std::string& rbg_size_value)
{
  return R"(
cell_cfg:
  dl_arfcn: 632628
  band: 78
  channel_bandwidth_MHz: 20
  common_scs: 30
  pci: 1
  pdsch:
    resource_allocation: )" +
         res_alloc + R"(
    rbg_size: )" +
         rbg_size_value + "\n";
}

} // namespace

TEST(du_high_pdsch_res_alloc_config_test, when_options_are_not_set_then_type1_and_config1_are_used)
{
  const du_high_unit_config cfg = parse_config(R"(
cell_cfg:
  dl_arfcn: 632628
  band: 78
  channel_bandwidth_MHz: 20
  common_scs: 30
  pci: 1
)");

  const du_high_unit_pdsch_config& pdsch = cfg.cells_cfg.front().cell.pdsch_cfg;
  ASSERT_EQ(pdsch.res_alloc, resource_allocation::resource_allocation_type_1);
  ASSERT_EQ(pdsch.rbg_sz, rbg_size::config1);
}

TEST(du_high_pdsch_res_alloc_config_test, when_values_differ_in_case_then_they_are_accepted)
{
  du_high_unit_config cfg = parse_config(make_pdsch_config("Type0", "Config2"));
  ASSERT_EQ(cfg.cells_cfg.front().cell.pdsch_cfg.res_alloc, resource_allocation::resource_allocation_type_0);
  ASSERT_EQ(cfg.cells_cfg.front().cell.pdsch_cfg.rbg_sz, rbg_size::config2);

  cfg = parse_config(make_pdsch_config("DYNAMIC_SWITCH", "config1"));
  ASSERT_EQ(cfg.cells_cfg.front().cell.pdsch_cfg.res_alloc, resource_allocation::dynamic_switch);
  ASSERT_EQ(cfg.cells_cfg.front().cell.pdsch_cfg.rbg_sz, rbg_size::config1);
}

TEST(du_high_pdsch_res_alloc_config_test, when_values_are_unknown_then_parsing_fails)
{
  ASSERT_THROW(parse_config(make_pdsch_config("type2", "config1")), CLI::ParseError);
  ASSERT_THROW(parse_config(make_pdsch_config("type0", "config3")), CLI::ParseError);
}

TEST(du_high_pdsch_res_alloc_config_test, configured_values_reach_the_cell_configuration)
{
  const du_high_unit_config              cfg   = parse_config(make_pdsch_config("type0", "config2"));
  const std::vector<odu::du_cell_config> cells = generate_du_cell_config(cfg);
  ASSERT_EQ(cells.size(), 1);
  ASSERT_EQ(cells.front().ran.init_bwp.pdsch.res_alloc, resource_allocation::resource_allocation_type_0);
  ASSERT_EQ(cells.front().ran.init_bwp.pdsch.rbg_sz, rbg_size::config2);
}
