// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_cli11_schema.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_translators.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_validator.h"
#include "ocudu/du/du_cell_config_validation.h"
#include "ocudu/support/config_parsers.h"
#include <gtest/gtest.h>
#include <sstream>

using namespace ocudu;

namespace {

/// Parses a YAML configuration through the CLI11 schema, as the application does, and auto-derives its parameters.
du_high_unit_config parse_config(const std::string& yaml)
{
  CLI::App app{"du_high_coreset0_config_test"};
  app.config_formatter(create_yaml_config_parser());
  app.allow_config_extras(CLI::config_extras_mode::error);

  du_high_parsed_config parsed_cfg;
  configure_cli11_with_du_high_config_schema(app, parsed_cfg);

  std::istringstream input(yaml);
  app.parse_from_stream(input);
  autoderive_du_high_parameters_after_parsing(parsed_cfg.config);

  return parsed_cfg.config;
}

/// Cell in band n78, 20 MHz, 30 kHz SCS. Every CORESET#0 index of TS 38.213, Table 13-4 fits this carrier.
constexpr const char* n78_cell = R"(
cell_cfg:
  dl_arfcn: 632628
  band: 78
  channel_bandwidth_MHz: 20
  common_scs: 30
  pci: 1
)";

/// Generates the cell configuration of a single-cell DU high configuration.
odu::du_cell_config single_cell_config(const du_high_unit_config& cfg)
{
  const std::vector<odu::du_cell_config> cells = generate_du_cell_config(cfg);
  report_fatal_error_if_not(cells.size() == 1, "Expected a single cell");
  return cells.front();
}

} // namespace

/// Table 13-4 index 5 is a 24 RB, 3-symbol CORESET#0, which requires dmrs-TypeA-Position pos3 as per TS 38.211,
/// Section 7.3.2.2.
TEST(du_high_coreset0_config_test, three_symbol_coreset0_index_is_accepted_and_selects_pos3)
{
  const du_high_unit_config cfg = parse_config(std::string(n78_cell) + R"(
  pdcch:
    common:
      coreset0_index: 5
)");

  ASSERT_TRUE(validate_du_high_config(cfg));

  const odu::du_cell_config cell = single_cell_config(cfg);
  ASSERT_TRUE(cell.ran.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.has_value());
  EXPECT_EQ(cell.ran.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->duration(), 3);
  EXPECT_EQ(cell.ran.dmrs_typeA_pos, dmrs_typeA_position::pos3);
}

/// Table 13-4 index 0 is a 24 RB, 2-symbol CORESET#0.
TEST(du_high_coreset0_config_test, two_symbol_coreset0_index_keeps_pos2)
{
  const du_high_unit_config cfg = parse_config(std::string(n78_cell) + R"(
  pdcch:
    common:
      coreset0_index: 0
)");

  ASSERT_TRUE(validate_du_high_config(cfg));

  const odu::du_cell_config cell = single_cell_config(cfg);
  ASSERT_TRUE(cell.ran.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.has_value());
  EXPECT_EQ(cell.ran.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->duration(), 2);
  EXPECT_EQ(cell.ran.dmrs_typeA_pos, dmrs_typeA_position::pos2);
}

/// The CORESET#0 derivation may consider 3-symbol candidates, and dmrs-TypeA-Position follows the selected duration.
TEST(du_high_coreset0_config_test, max_coreset0_duration_of_three_is_accepted)
{
  const du_high_unit_config cfg = parse_config(std::string(n78_cell) + R"(
  pdcch:
    common:
      max_coreset0_duration: 3
)");

  ASSERT_TRUE(validate_du_high_config(cfg));

  const odu::du_cell_config cell = single_cell_config(cfg);
  ASSERT_TRUE(cell.ran.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.has_value());
  const unsigned duration = cell.ran.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->duration();
  EXPECT_LE(duration, 3U);
  EXPECT_EQ(cell.ran.dmrs_typeA_pos, duration == 3 ? dmrs_typeA_position::pos3 : dmrs_typeA_position::pos2);
}

/// A CORESET has at most 3 symbols, as per TS 38.211, Section 7.3.2.2.
TEST(du_high_coreset0_config_test, max_coreset0_duration_above_three_is_rejected)
{
  EXPECT_THROW(parse_config(std::string(n78_cell) + R"(
  pdcch:
    common:
      max_coreset0_duration: 4
)"),
               CLI::ParseError);
}

/// A 3-symbol CORESET#1 requires dmrs-TypeA-Position pos3 (TS 38.331, ControlResourceSet), whatever the CORESET#0
/// duration, and the UE PDSCH starts after the CORESET.
TEST(du_high_coreset0_config_test, three_symbol_coreset1_selects_pos3_and_starts_pdsch_after_it)
{
  const du_high_unit_config cfg = parse_config(std::string(n78_cell) + R"(
  pdcch:
    common:
      coreset0_index: 0
    dedicated:
      coreset1_duration: 3
)");

  ASSERT_TRUE(validate_du_high_config(cfg));

  const odu::du_cell_config cell = single_cell_config(cfg);
  ASSERT_TRUE(cell.ran.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.has_value());
  EXPECT_EQ(cell.ran.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->duration(), 2);
  ASSERT_TRUE(cell.ran.init_bwp.pdcch_cfg.has_value());
  ASSERT_FALSE(cell.ran.init_bwp.pdcch_cfg->coresets.empty());
  EXPECT_EQ(cell.ran.init_bwp.pdcch_cfg->coresets.front().duration(), 3);
  EXPECT_EQ(cell.ran.dmrs_typeA_pos, dmrs_typeA_position::pos3);
  ASSERT_FALSE(cell.ran.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.empty());
  for (const auto& pdsch_td : cell.ran.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list) {
    EXPECT_EQ(pdsch_td.symbols.start(), 3);
  }
  const validator_result du_check = is_du_cell_config_valid(cell);
  EXPECT_TRUE(du_check.has_value()) << (du_check.has_value() ? std::string{} : du_check.error());
}

/// A 3-symbol CORESET#1 on a cell with a 3-symbol CORESET#0 keeps pos3.
TEST(du_high_coreset0_config_test, three_symbol_coreset1_with_three_symbol_coreset0_keeps_pos3)
{
  const du_high_unit_config cfg = parse_config(std::string(n78_cell) + R"(
  pdcch:
    common:
      coreset0_index: 5
    dedicated:
      coreset1_duration: 3
)");

  ASSERT_TRUE(validate_du_high_config(cfg));

  const odu::du_cell_config cell = single_cell_config(cfg);
  ASSERT_TRUE(cell.ran.init_bwp.pdcch_cfg.has_value());
  ASSERT_FALSE(cell.ran.init_bwp.pdcch_cfg->coresets.empty());
  EXPECT_EQ(cell.ran.init_bwp.pdcch_cfg->coresets.front().duration(), 3);
  EXPECT_EQ(cell.ran.dmrs_typeA_pos, dmrs_typeA_position::pos3);
  const validator_result du_check = is_du_cell_config_valid(cell);
  EXPECT_TRUE(du_check.has_value()) << (du_check.has_value() ? std::string{} : du_check.error());
}

/// A CORESET has at most 3 symbols, as per TS 38.211, Section 7.3.2.2.
TEST(du_high_coreset0_config_test, coreset1_duration_above_three_is_rejected)
{
  EXPECT_THROW(parse_config(std::string(n78_cell) + R"(
  pdcch:
    dedicated:
      coreset1_duration: 4
)"),
               CLI::ParseError);
}
