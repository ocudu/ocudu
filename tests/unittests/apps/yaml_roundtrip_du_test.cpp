// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "apps/du/du_appconfig.h"
#include "apps/du/du_appconfig_cli11_schema.h"
#include "apps/du/du_appconfig_yaml_writer.h"
#include "apps/helpers/hal/hal_appconfig.h"
#include "apps/helpers/hal/hal_cli11_schema.h"
#include "apps/units/flexible_o_du/flexible_o_du_application_unit.h"
#include "yaml_roundtrip_test_helpers.h"
#include "ocudu/support/config_parsers.h"
#include "CLI/CLI11.hpp"

#ifndef CONFIGS_DIR
#error "CONFIGS_DIR must be defined"
#endif

using namespace ocudu;
using namespace ocudu::yaml_roundtrip_test;

namespace {

YAML::Node load_and_emit(const std::string& yaml_text)
{
  temp_yaml_file tmp(yaml_text);

  CLI::App app("du yaml-roundtrip-test");
  app.config_formatter(create_yaml_config_parser());
  app.allow_config_extras(CLI::config_extras_mode::error);
  std::string cfg_path;
  app.set_config("-c,", cfg_path, "Read config from file", false);

  du_appconfig du_cfg;
  configure_cli11_with_du_appconfig_schema(app, du_cfg);

  auto o_du = create_flexible_o_du_application_unit("du");
  o_du->on_parsing_configuration_registration(app);

  app.callback([&]() { o_du->on_configuration_parameters_autoderivation(app); });

  std::vector<const char*> argv = {"du", "-c", tmp.path().c_str()};
  app.parse(static_cast<int>(argv.size()), argv.data());

  YAML::Node node;
  fill_du_appconfig_in_yaml_schema(node, du_cfg);
  o_du->dump_config(node);
  return node;
}

YAML::Node emit_defaults()
{
  du_appconfig du_cfg;
  auto         o_du = create_flexible_o_du_application_unit("du");
  YAML::Node   node;
  fill_du_appconfig_in_yaml_schema(node, du_cfg);
  o_du->dump_config(node);
  return node;
}

const std::string CONFIGS = CONFIGS_DIR;

class du_example_config_test : public ::testing::TestWithParam<std::string>
{};

TEST_P(du_example_config_test, roundtrip)
{
  const std::string& name = GetParam();
  assert_roundtrip(read_file(CONFIGS + "/" + name), &load_and_emit, name);
}

INSTANTIATE_TEST_SUITE_P(,
                         du_example_config_test,
                         ::testing::Values("du_rf_b200_tdd_n78_20mhz.yml", "du_f1u_multiple_sockets.yml"));

TEST(du_default_config_test, roundtrip)
{
  YAML::Node a = emit_defaults();
  assert_roundtrip(YAML::Dump(a), &load_and_emit, "du defaults");
}

TEST(du_hal_config_test, enable_pdump_init_defaults_to_false)
{
  hal_appconfig cfg;
  EXPECT_FALSE(cfg.enable_pdump_init);
}

TEST(du_hal_config_test, enable_pdump_init_can_be_enabled_from_cli)
{
  CLI::App      app("hal pdump-cli-test");
  hal_appconfig cfg;
  configure_cli11_with_hal_appconfig_schema(app, cfg);

  std::vector<const char*> argv = {"pdump-test", "hal", "--enable_pdump_init=true"};
  app.parse(static_cast<int>(argv.size()), argv.data());

  EXPECT_TRUE(cfg.enable_pdump_init);
}

TEST(du_hal_config_test, enable_pdump_init_can_be_enabled_from_config_file)
{
  temp_yaml_file tmp("hal:\n  enable_pdump_init: true\n");

  CLI::App app("hal pdump-config-file-test");
  app.config_formatter(create_yaml_config_parser());
  app.allow_config_extras(CLI::config_extras_mode::error);
  std::string cfg_path;
  app.set_config("-c,", cfg_path, "Read config from file", false);

  hal_appconfig hal_cfg;
  configure_cli11_with_hal_appconfig_schema(app, hal_cfg);

  std::vector<const char*> argv = {"pdump-test", "-c", tmp.path().c_str()};
  app.parse(static_cast<int>(argv.size()), argv.data());

  EXPECT_TRUE(hal_cfg.enable_pdump_init);
}

/// The ETWS and CMAS blocks are the only way to provision a cell for a warning, so a dumped configuration that leaves
/// them out cannot be fed back to the application.
TEST(du_pws_config_test, roundtrip)
{
  YAML::Node cfg = YAML::Load(read_file(CONFIGS + "/du_rf_b200_tdd_n78_20mhz.yml"));

  YAML::Node sib                            = cfg["cell_cfg"]["sib"];
  sib["etws"]["si_period"]                  = 32;
  sib["etws"]["test"]["message_id"]         = 4353;
  sib["etws"]["test"]["serial_num"]         = 12288;
  sib["etws"]["test"]["warning_type"]       = 2432;
  sib["etws"]["test"]["data_coding_scheme"] = 1;
  sib["etws"]["test"]["warning_message"]    = "An ETWS warning";
  sib["cmas"]["si_period"]                  = 128;

  assert_roundtrip(YAML::Dump(cfg), &load_and_emit, "du pws");
}

TEST(du_multiple_ssb_beams_config_test, roundtrip)
{
  // Band n78 with 30kHz SSB SCS gives L_max 8, so SSB indexes 0 to 7 are valid.
  const std::string yaml_text = read_file(CONFIGS + "/du_rf_b200_tdd_n78_20mhz.yml");

  YAML::Node node = YAML::Load(yaml_text);
  YAML::Node ref_beams;
  YAML::Node ssb_beams;
  for (unsigned i_beam_dim1 : {0U, 3U, 7U}) {
    YAML::Node cell_beam_node;
    cell_beam_node["ref_beam_id"] = i_beam_dim1;
    cell_beam_node["i_pol"]       = 1;
    cell_beam_node["i_beam_dim1"] = i_beam_dim1;
    cell_beam_node["i_beam_dim2"] = 0;
    ref_beams.push_back(cell_beam_node);

    YAML::Node ssb_beam_node;
    ssb_beam_node["ssb_index"]   = i_beam_dim1;
    ssb_beam_node["ref_beam_id"] = i_beam_dim1;
    ssb_beams.push_back(ssb_beam_node);
  }
  node["cell_cfg"]["ssb"]["beams"]    = ssb_beams;
  node["cell_cfg"]["ref_beams"]       = ref_beams;
  node["cell_cfg"]["nof_antennas_dl"] = 4;

  assert_roundtrip(YAML::Dump(node), &load_and_emit, "du multiple SSB beams");
}

} // namespace
