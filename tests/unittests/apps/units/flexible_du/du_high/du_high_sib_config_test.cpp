// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_cli11_schema.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_translators.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_validator.h"
#include "ocudu/support/config_parsers.h"
#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>
#include <unistd.h>

using namespace ocudu;

namespace {

/// \brief Single-cell configuration, with the parameters a parsed one has auto-derived.
///
/// It goes through the CLI11 schema, as the application does, so that the defaults under test are the real ones.
class du_high_config_bench
{
public:
  du_high_config_bench()
  {
    configure_cli11_with_du_high_config_schema(app, parsed_cfg);
    app.parse(std::vector<std::string>{});
    parsed_cfg.config.cells_cfg.resize(1);
  }

  du_high_unit_sib_config& sib_cfg() { return parsed_cfg.config.cells_cfg[0].cell.sib_cfg; }

  /// Configuration with its auto-derived parameters filled in.
  const du_high_unit_config& derived_config()
  {
    unit_cfg = parsed_cfg.config;
    autoderive_du_high_parameters_after_parsing(app, unit_cfg);
    return unit_cfg;
  }

private:
  CLI::App              app{"du_high_sib_config_test"};
  du_high_parsed_config parsed_cfg;
  du_high_unit_config   unit_cfg;
};

/// SI scheduling information of the only cell of a configuration.
si_scheduling_info_config si_config_of(const du_high_unit_config& cfg)
{
  const std::vector<odu::du_cell_config> cells = generate_du_cell_config(cfg);
  report_fatal_error_if_not(cells.size() == 1 and cells[0].si.si_config.has_value(), "Cell has no SI configuration");
  return cells[0].si.si_config.value();
}

/// Whether the SI configuration holds content for a given SIB.
bool has_content_for(const si_scheduling_info_config& si_config, sib_type sib)
{
  return std::any_of(si_config.sibs.begin(), si_config.sibs.end(), [sib](const sib_type_info& entry) {
    return get_sib_info_type(entry.content) == sib;
  });
}

/// SIBs carried by each SI message that carries a warning, in the order the configuration holds them.
std::vector<sib_type> warning_sibs_of(const si_scheduling_info_config& si_config)
{
  std::vector<sib_type> sibs;
  for (const pws_si_message_config& pws_si_msg : si_config.pws_si_messages) {
    sibs.push_back(pws_si_msg.sib);
  }
  return sibs;
}

} // namespace

TEST(du_high_sib_config_test, etws_block_alone_provisions_the_cell_for_a_warning)
{
  du_high_config_bench bench;
  bench.sib_cfg().etws_cfg.emplace();
  bench.sib_cfg().etws_cfg->si_period_rf = 128;

  const si_scheduling_info_config si_config = si_config_of(bench.derived_config());

  // A warning SIB is never mapped together with another SIB, so ETWS takes one SI message for its primary notification
  // and another for its secondary one.
  ASSERT_EQ(warning_sibs_of(si_config), (std::vector<sib_type>{sib_type::sib6, sib_type::sib7}));
  EXPECT_EQ(si_config.pws_si_messages[0].si_period_radio_frames, 128);
  EXPECT_FALSE(si_config.pws_si_messages[0].auto_broadcast);
  EXPECT_FALSE(has_content_for(si_config, sib_type::sib6))
      << "A warning with no test content waits for a Write-Replace Warning to provide it";
}

TEST(du_high_sib_config_test, etws_test_content_is_created_without_any_sib_mapping)
{
  du_high_config_bench bench;
  bench.sib_cfg().etws_cfg.emplace();
  bench.sib_cfg().etws_cfg->test.emplace();
  bench.sib_cfg().cmas_cfg.emplace();
  bench.sib_cfg().cmas_cfg->test.emplace();

  const si_scheduling_info_config si_config = si_config_of(bench.derived_config());

  ASSERT_EQ(warning_sibs_of(si_config), (std::vector<sib_type>{sib_type::sib6, sib_type::sib7, sib_type::sib8}));
  for (const pws_si_message_config& pws_si_msg : si_config.pws_si_messages) {
    EXPECT_TRUE(pws_si_msg.auto_broadcast) << "Configured test content is broadcast from the cell start";
    EXPECT_TRUE(has_content_for(si_config, pws_si_msg.sib));
  }
  EXPECT_TRUE(si_config.si_sched_info.empty()) << "A warning takes no entry of the SI scheduling info";
}

TEST(du_high_sib_config_test, sib_mapping_of_a_warning_sib_is_rejected)
{
  for (uint8_t warning_sib : {6, 7, 8}) {
    du_high_config_bench bench;
    bench.sib_cfg().si_sched_info.resize(1);
    bench.sib_cfg().si_sched_info[0].sib_mapping_info = {warning_sib};

    EXPECT_FALSE(validate_du_high_config(bench.derived_config()))
        << "SIB" << static_cast<unsigned>(warning_sib) << " was accepted";
  }
}

TEST(du_high_sib_config_test, si_window_budget_reserves_room_for_the_warnings)
{
  du_high_config_bench bench;
  // Two SI messages of the normal operation, with a period that fits their windows, but not the three more a cell
  // provisioned for both ETWS and CMAS needs.
  bench.sib_cfg().si_window_len_slots = 80;
  bench.sib_cfg().si_sched_info.resize(2);
  bench.sib_cfg().si_sched_info[0].sib_mapping_info = {2};
  bench.sib_cfg().si_sched_info[0].si_period_rf     = 32;
  bench.sib_cfg().si_sched_info[1].sib_mapping_info = {3};
  bench.sib_cfg().si_sched_info[1].si_period_rf     = 32;
  bench.sib_cfg().sib2_cfg.emplace();
  bench.sib_cfg().sib3_cfg.emplace();
  ASSERT_TRUE(validate_du_high_config(bench.derived_config()));

  // The warnings need an SI window of their own each, even while none of them is on air, so that the SI messages of
  // the normal operation keep theirs as warnings come and go.
  bench.sib_cfg().etws_cfg.emplace();
  bench.sib_cfg().etws_cfg->si_period_rf = 32;
  bench.sib_cfg().cmas_cfg.emplace();
  bench.sib_cfg().cmas_cfg->si_period_rf = 32;
  EXPECT_FALSE(validate_du_high_config(bench.derived_config()));
}

namespace {

/// Config file that lives only for the duration of a test.
class temp_config_file
{
public:
  explicit temp_config_file(const std::string& contents) : file_path("/tmp/du_high_sib_config_test_XXXXXX")
  {
    const int fd = ::mkstemp(file_path.data());
    report_fatal_error_if_not(fd != -1, "Failed to create the temporary config file");
    ::close(fd);
    std::ofstream{file_path} << contents;
  }
  ~temp_config_file() { ::remove(file_path.c_str()); }

  const std::string& path() const { return file_path; }

private:
  std::string file_path;
};

/// Parses a DU high configuration out of a YAML text, as the application does.
du_high_unit_config parse_config(const std::string& yaml_text)
{
  const temp_config_file cfg_file(yaml_text);

  CLI::App app{"du_high_sib_config_test"};
  app.config_formatter(create_yaml_config_parser());
  app.allow_config_extras(CLI::config_extras_mode::error);
  std::string cfg_path;
  app.set_config("-c,", cfg_path, "Read config from file", false);

  du_high_parsed_config parsed_cfg;
  configure_cli11_with_du_high_config_schema(app, parsed_cfg);

  const std::vector<const char*> argv = {"du_high_sib_config_test", "-c", cfg_file.path().c_str()};
  app.parse(static_cast<int>(argv.size()), argv.data());
  autoderive_du_high_parameters_after_parsing(app, parsed_cfg.config);

  return parsed_cfg.config;
}

} // namespace

/// \brief Regression test: the ETWS configuration of one cell must not leak into the next one.
///
/// The CLI11 schema fills a scratch object per subcommand before moving it into the cell it belongs to. Holding that
/// object in a static would share it across every cell of the configuration, so a cell that leaves a field out would
/// silently inherit the value of the cell parsed before it rather than the default.
TEST(du_high_sib_config_test, warning_configuration_of_one_cell_does_not_leak_into_the_next)
{
  const du_high_unit_config cfg = parse_config(R"(
cells:
  - pci: 1
    sib:
      etws:
        si_period: 32
        test:
          message_id: 4353
          warning_message: First cell warning
  - pci: 2
    sib:
      etws:
        test:
          serial_num: 12288
)");

  ASSERT_EQ(cfg.cells_cfg.size(), 2);
  const auto& first  = cfg.cells_cfg[0].cell.sib_cfg.etws_cfg;
  const auto& second = cfg.cells_cfg[1].cell.sib_cfg.etws_cfg;
  ASSERT_TRUE(first.has_value() and second.has_value());

  EXPECT_EQ(first->si_period_rf, 32);
  EXPECT_EQ(second->si_period_rf, du_high_unit_sib_config::etws_config{}.si_period_rf)
      << "The second cell must keep the default SI period, not the one the first cell set";

  ASSERT_TRUE(first->test.has_value() and second->test.has_value());
  EXPECT_EQ(second->test->message_id, du_high_unit_sib_config::etws_config::test_config{}.message_id)
      << "The second cell must keep the default message ID, not the one the first cell set";
  EXPECT_EQ(second->test->warning_message, du_high_unit_sib_config::etws_config::test_config{}.warning_message)
      << "The second cell must keep the default warning message, not the one the first cell set";
}
