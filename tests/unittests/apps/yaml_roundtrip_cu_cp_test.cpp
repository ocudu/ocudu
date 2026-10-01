// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "apps/cu_cp/cu_cp_appconfig.h"
#include "apps/cu_cp/cu_cp_appconfig_cli11_schema.h"
#include "apps/cu_cp/cu_cp_appconfig_yaml_writer.h"
#include "apps/units/o_cu_cp/o_cu_cp_application_unit.h"
#include "yaml_roundtrip_test_helpers.h"
#include "ocudu/adt/format.h"
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

  CLI::App app("cu_cp yaml-roundtrip-test");
  app.config_formatter(create_yaml_config_parser());
  app.allow_config_extras(CLI::config_extras_mode::error);
  std::string cfg_path;
  app.set_config("-c,", cfg_path, "Read config from file", false);

  cu_cp_appconfig cu_cp_cfg;
  configure_cli11_with_cu_cp_appconfig_schema(app, cu_cp_cfg);

  auto o_cu_cp = create_o_cu_cp_application_unit("cucp");
  o_cu_cp->on_parsing_configuration_registration(app);

  app.callback([&]() { o_cu_cp->on_configuration_parameters_autoderivation(app); });

  std::vector<const char*> argv = {"cu_cp", "-c", tmp.path().c_str()};
  app.parse(static_cast<int>(argv.size()), argv.data());

  YAML::Node node;
  fill_cu_cp_appconfig_in_yaml_schema(node, cu_cp_cfg);
  o_cu_cp->dump_config(node);
  return node;
}

YAML::Node emit_defaults()
{
  cu_cp_appconfig cu_cp_cfg;
  auto            o_cu_cp = create_o_cu_cp_application_unit("cucp");
  YAML::Node      node;
  fill_cu_cp_appconfig_in_yaml_schema(node, cu_cp_cfg);
  o_cu_cp->dump_config(node);
  return node;
}

const std::string CONFIGS = CONFIGS_DIR;

class cu_cp_example_config_test : public ::testing::TestWithParam<std::string>
{};

TEST_P(cu_cp_example_config_test, roundtrip)
{
  const std::string& name = GetParam();
  assert_roundtrip(read_file(CONFIGS + "/" + name), &load_and_emit, name);
}

INSTANTIATE_TEST_SUITE_P(, cu_cp_example_config_test, ::testing::Values("cu_cp.yml", "mobility.yml"));

TEST(cu_cp_default_config_test, roundtrip)
{
  YAML::Node a = emit_defaults();
  assert_roundtrip(YAML::Dump(a), &load_and_emit, "cu_cp defaults");
}

/// Mobility configuration exercising the list-valued and optional keys of the mobility writer: several neighbor
/// relations per cell, several report config ids per relation, T312 and the periodic handover offset.
const std::string MOBILITY_LISTS_CONFIG = R"(cu_cp:
  mobility:
    trigger_handover_from_measurements: true
    cells:
      - nr_cell_id: 0x66c000
        periodic_report_cfg_id: 1
        ncells:
          - nr_cell_id: 0x20
            report_configs: [2, 3]
          - nr_cell_id: 0x21
            report_configs: [2]
      - nr_cell_id: 0x20
        ncells:
          - nr_cell_id: 0x66c000
            report_configs: [2]
        gnb_id_bit_length: 32
        pci: 1
        plmn: "00101"
        tac: 7
        ssb_arfcn: 632628
        band: 78
        ssb_scs: 30
        ssb_period: 20
        ssb_offset: 0
        ssb_duration: 5
      - nr_cell_id: 0x21
        gnb_id_bit_length: 32
        pci: 2
        plmn: "00101"
        tac: 7
        ssb_arfcn: 632628
        band: 78
        ssb_scs: 30
        ssb_period: 20
        ssb_offset: 0
        ssb_duration: 5
    report_configs:
      - report_cfg_id: 1
        report_type: periodical
        report_interval_ms: 1024
        periodic_ho_rsrp_offset_db: 5
      - report_cfg_id: 2
        report_type: event_triggered
        event_triggered_report_type: a3
        meas_trigger_quantity: rsrp
        meas_trigger_quantity_offset_db: 3
        hysteresis_db: 0
        time_to_trigger_ms: 100
        report_interval_ms: 1024
        t312: 200
      - report_cfg_id: 3
        report_type: event_triggered
        event_triggered_report_type: a5
        meas_trigger_quantity: rsrp
        meas_trigger_quantity_threshold_db: -110
        meas_trigger_quantity_threshold_2_db: -100
        hysteresis_db: 2
        time_to_trigger_ms: 256
        report_interval_ms: 1024
)";

TEST(cu_cp_mobility_lists_config_test, roundtrip)
{
  // The writer must append list entries (every neighbor of a cell, every report config id of a relation), emit
  // T312 under the key the parser accepts and emit the periodic handover offset when it is set.
  assert_roundtrip(MOBILITY_LISTS_CONFIG, &load_and_emit, "mobility lists");

  YAML::Node       emitted = load_and_emit(MOBILITY_LISTS_CONFIG);
  const YAML::Node cells   = emitted["cu_cp"]["mobility"]["cells"];
  ASSERT_TRUE(cells);
  ASSERT_EQ(cells[0]["ncells"].size(), 2U);
  EXPECT_EQ(cells[0]["ncells"][0]["report_configs"].size(), 2U);
  const YAML::Node report_cfgs = emitted["cu_cp"]["mobility"]["report_configs"];
  ASSERT_EQ(report_cfgs.size(), 3U);
  EXPECT_EQ(report_cfgs[0]["periodic_ho_rsrp_offset_db"].as<int>(), 5);
  EXPECT_EQ(report_cfgs[1]["t312"].as<unsigned>(), 200U);
}

/// Return the example config with its commented-out logical_cells block replaced by a real one carrying the
/// given admin_state for sector 0.
static std::string config_with_logical_cells(const std::string& admin_state)
{
  std::string       text          = read_file(CONFIGS + "/cu_cp.yml");
  const std::string commented_out = "  # logical_cells:\n  #   - sector_id: 0";
  const std::string block         = "  logical_cells:\n"
                                    "    - sector_id: 0\n"
                                    "      admin_state: " +
                            admin_state +
                            "\n"
                            "      cell_barred: true\n"
                            "    - sector_id: 1";
  auto pos = text.find(commented_out);
  EXPECT_NE(pos, std::string::npos) << "example config lost its commented logical_cells block";
  return text.replace(pos, commented_out.size(), block);
}

TEST(cu_cp_logical_cells_config_test, roundtrip)
{
  // A declared logical cell list (locked+barred entry plus an entry keeping the defaults) survives the
  // parse -> emit round trip: the emitted admin_state is the same string form the parser accepts.
  assert_roundtrip(config_with_logical_cells("locked"), &load_and_emit, "cu_cp.yml with logical_cells");
  assert_roundtrip(config_with_logical_cells("unlocked"), &load_and_emit, "cu_cp.yml with logical_cells");

  // Pin the emitted string form: the writer must dump the state name, not the enum's numeric value.
  YAML::Node emitted = load_and_emit(config_with_logical_cells("locked"));
  ASSERT_TRUE(emitted["cu_cp"]["logical_cells"]);
  EXPECT_EQ(emitted["cu_cp"]["logical_cells"][0]["admin_state"].as<std::string>(), "locked");
  EXPECT_EQ(emitted["cu_cp"]["logical_cells"][1]["admin_state"].as<std::string>(), "unlocked");
}

TEST(cu_cp_logical_cells_config_test, invalid_admin_state_is_rejected_at_parse)
{
  EXPECT_THROW(load_and_emit(config_with_logical_cells("bogus")), CLI::ParseError);
}

TEST(cu_cp_logical_cells_config_test, shutting_down_cannot_be_configured)
{
  // shutting_down is a transient the CU-CP holds itself during a graceful stop; the configuration only
  // accepts unlocked or locked.
  EXPECT_THROW(load_and_emit(config_with_logical_cells("shutting_down")), CLI::ParseError);
}

/// Return the example config with a "security:" block declaring the given nea_pref_list/nia_pref_list added
/// under cu_cp.
static std::string config_with_security_pref_lists(const std::string& nea_pref_list, const std::string& nia_pref_list)
{
  std::string       text   = read_file(CONFIGS + "/cu_cp.yml");
  const std::string anchor = "  e1ap:\n";
  const std::string block  = "  security:\n"
                             "    nea_pref_list: " +
                            nea_pref_list + "\n" + "    nia_pref_list: " + nia_pref_list + "\n";
  auto pos = text.find(anchor);
  EXPECT_NE(pos, std::string::npos) << "example config lost its e1ap block";
  return text.insert(pos, block);
}

TEST(cu_cp_security_config_test, short_pref_lists_round_trip)
{
  // Regression test for MR !1498: nea_pref_list/nia_pref_list shorter than their full slot count are padded
  // internally by repeating the highest-priority entry, and the dumped config must reparse without error.
  assert_roundtrip(config_with_security_pref_lists("nea2,nea1,nea3", "nia2,nia1"),
                   &load_and_emit,
                   "cu_cp.yml with short nea/nia pref lists");
}

} // namespace
