// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ofh_integration_test_config.h"
#include "ocudu/ofh/compression/compression_validator.h"
#include "ocudu/ofh/ofh_constants.h"
#include "ocudu/support/cli11_utils.h"
#include "ocudu/support/config_parsers.h"
#include "CLI/CLI11.hpp"

using namespace ocudu;
using namespace ofh;
using namespace test;

static void configure_cli11_logger_args(CLI::App& app, logger_config& config)
{
  add_option(app, "--filename", config.filename, "Logger sink filename. Set to 'stdout' for using the console output")
      ->capture_default_str();

  add_option_function<std::string>(
      app,
      "--level",
      [&config](const std::string& value) {
        auto level   = ocudulog::str_to_basic_level(value);
        config.level = level.has_value() ? *level : ocudulog::basic_levels::none;
      },
      "Integration test log level")
      ->default_str(ocudulog::basic_level_to_string(config.level))
      ->check([](const std::string& value) -> std::string {
        if (ocudulog::str_to_basic_level(value).has_value()) {
          return {};
        }
        return fmt::format("Log level '{}' not supported. Accepted values [none,info,debug,warning,error]", value);
      });
}

/// Adds the compression method and bit width options named with the given suffix.
static void configure_cli11_compression_args(CLI::App&              app,
                                             ru_compression_params& params,
                                             const std::string&     suffix,
                                             const std::string&     desc)
{
  add_option_function<std::string>(
      app,
      "--compr_method_" + suffix,
      [&params](const std::string& value) { params.type = to_compression_type(value); },
      desc + " compression method")
      ->default_str(to_string(params.type))
      ->check([](const std::string& value) -> std::string {
        compression_type type = to_compression_type(value);
        if (type == compression_type::none || type == compression_type::BFP) {
          return {};
        }
        return "Compression method not supported. Accepted values [none, bfp]";
      });
  add_option(app, "--compr_bitwidth_" + suffix, params.data_width, desc + " compression bit width")
      ->capture_default_str()
      ->range(1, 16);
}

static void configure_cli11_beamforming_args(CLI::App& app, beamforming_config& config)
{
  add_option(app, "--enable", config.enable, "Enables downlink beamforming (Category B O-RU)")->capture_default_str();
  configure_cli11_compression_args(app, config.bfw_compr_params, "bfw", "Beamforming weights");
}

static void configure_cli11_test_args(CLI::App& app, test_parameters& config)
{
  CLI::App* log_subcmd = add_subcommand(app, "log", "Logging configuration");
  configure_cli11_logger_args(*log_subcmd, config.logger_cfg);

  CLI::App* bf_subcmd = add_subcommand(app, "beamforming", "Downlink beamforming configuration");
  configure_cli11_beamforming_args(*bf_subcmd, config.beamforming_cfg);

  add_option(app, "--silent", config.silent, "Silent operation")->capture_default_str();

  add_option_enum<bs_channel_bandwidth>(
      app,
      "--channel_bandwidth_MHz",
      config.channel_bw_mhz,
      {5, 10, 15, 20, 25, 30, 40, 50, 60, 70, 80, 90, 100},
      [](int value) { return static_cast<bs_channel_bandwidth>(value); },
      "Channel bandwidth in MHz")
      ->default_val(static_cast<unsigned>(config.channel_bw_mhz));

  add_option_enum<subcarrier_spacing>(
      app,
      "--scs",
      config.scs,
      {15, 30},
      [](int value) { return to_subcarrier_spacing(std::to_string(value)); },
      "Cell common subcarrier spacing")
      ->default_val(scs_to_khz(config.scs));

  add_option(app, "--tdd_pattern", config.tdd_pattern_str, "TDD pattern")
      ->capture_default_str()
      ->check(CLI::IsMember({"7d2u", "6d3u"}));

  add_option(app, "--prach_port_id", config.prach_port_id, "RU PRACH port identifier")->capture_default_str();
  add_option(app, "--dl_port_id", config.dl_port_id, "RU downlink port identifier")->capture_default_str();
  add_option(app, "--ul_port_id", config.ul_port_id, "RU uplink port identifier")->capture_default_str();

  configure_cli11_compression_args(app, config.data_compr_params, "data", "Downlink and uplink");
  configure_cli11_compression_args(app, config.prach_compr_params, "prach", "PRACH");
  add_option(app,
             "--enable_dl_static_compr_hdr",
             config.is_downlink_static_comp_hdr_enabled,
             "Downlink static compression header enabled flag")
      ->capture_default_str();
  add_option(app,
             "--enable_ul_static_compr_hdr",
             config.is_uplink_static_comp_hdr_enabled,
             "Uplink static compression header enabled flag")
      ->capture_default_str();

  add_option(app,
             "--enable_prach_cp",
             config.is_prach_control_plane_enabled,
             "Enables the Control-Plane PRACH message signalling")
      ->capture_default_str();
  add_option(app,
             "--ignore_ecpri_payload_size",
             config.ignore_ecpri_payload_size_field,
             "Ignores the payload size encoded in the eCPRI header")
      ->capture_default_str();
  add_option(
      app, "--enable_dl_parallelization", config.is_downlink_parallelized, "Processes downlink with a pool of workers")
      ->capture_default_str();

  add_option_function<unsigned>(
      app, "--mtu", [&config](unsigned value) { config.mtu = units::bytes(value); }, "Ethernet frame size")
      ->default_val(config.mtu.value())
      ->range(1500, 9600);
  add_option(app,
             "--use_loopback_receiver",
             config.use_loopback_receiver,
             "Uses the loopback Ethernet interface (requires root permissions)")
      ->capture_default_str();
  add_option(app, "--nof_test_slots", config.nof_test_slots, "Number of slots processed in the test")
      ->capture_default_str();
  add_option(app,
             "--non_realtime",
             config.is_non_realtime,
             "Runs the test in non-realtime mode, where the OTA time advances one symbol duration sleep at a time")
      ->capture_default_str();
}

/// Validates the cross-parameter constraints of the test configuration.
static error_type<std::string> validate_test_configuration(const test_parameters& config)
{
  for (const auto* ports : {&config.dl_port_id, &config.ul_port_id, &config.prach_port_id}) {
    if (ports->empty() || ports->size() > MAX_NOF_SUPPORTED_EAXC) {
      return make_unexpected(
          fmt::format("the number of ports must be in the range [1, {}]", unsigned(MAX_NOF_SUPPORTED_EAXC)));
    }
  }

  if (auto result = validate_compression_params(config.data_compr_params); !result.has_value()) {
    return make_unexpected(fmt::format("Data {}", result.error()));
  }
  if (auto result = validate_compression_params(config.prach_compr_params); !result.has_value()) {
    return make_unexpected(fmt::format("PRACH {}", result.error()));
  }

  const beamforming_config& bf_cfg = config.beamforming_cfg;
  if (!bf_cfg.enable) {
    return default_success_t();
  }

  if (!get_dl_antenna_topology(config.dl_port_id.size()).has_value()) {
    return make_unexpected(fmt::format("beamforming does not support {} downlink ports. Valid values are [1,2,4,8]",
                                       config.dl_port_id.size()));
  }
  if (auto result = validate_compression_params(bf_cfg.bfw_compr_params); !result.has_value()) {
    return make_unexpected(fmt::format("Beamforming weights {}", result.error()));
  }

  return default_success_t();
}

std::optional<antenna_topology> test::get_dl_antenna_topology(unsigned nof_dl_ports)
{
  switch (nof_dl_ports) {
    case 1:
      return antenna_topology::one_port;
    case 2:
      return antenna_topology::two_port;
    case 4:
      return antenna_topology::single_panel_two_one;
    case 8:
      return antenna_topology::single_panel_four_one;
    default:
      return std::nullopt;
  }
}

expected<test_parameters, int> test::parse_test_configuration(int argc, char** argv)
{
  test_parameters config;

  CLI::App app("OFH integration test");
  app.config_formatter(create_yaml_config_parser());
  app.allow_config_extras(CLI::config_extras_mode::error);
  app.set_config("-c,--config_file", "", "Read configuration from a YAML file", false);
  configure_cli11_test_args(app, config);

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    return make_unexpected(app.exit(e));
  }

  if (auto result = validate_test_configuration(config); !result.has_value()) {
    fmt::println("Invalid test configuration: {}", result.error());
    return make_unexpected(EXIT_FAILURE);
  }

  return config;
}
