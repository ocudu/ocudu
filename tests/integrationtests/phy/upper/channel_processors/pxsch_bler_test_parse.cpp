// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pxsch_bler_test_parse.h"
#include "ocudu/adt/format.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/phy/antenna_ports.h"
#include "ocudu/ran/precoding/precoding_constants.h"
#include "ocudu/ran/resource_block.h"
#include "ocudu/support/cli11_utils.h"
#include "ocudu/support/string_parsing_utils.h"
#include "CLI/CLI11.hpp"

namespace ocudu {

namespace {

std::optional<pusch_mcs_table> to_mcs_table_impl(const char* str)
{
  for (unsigned table_idx = 0; table_idx != 5; ++table_idx) {
    pusch_mcs_table cur_table = static_cast<pusch_mcs_table>(table_idx);
    if (strcmp(str, pusch_mcs_table_to_string(cur_table)) == 0) {
      return cur_table;
    }
  }

  return std::nullopt;
}

} // namespace

static std::optional<port_channel_estimator_td_interpolation_strategy>
parse_td_interpolation_strategy(const std::string& str)
{
  if (str == "interpolate") {
    return port_channel_estimator_td_interpolation_strategy::interpolate;
  }
  if (str == "average") {
    return port_channel_estimator_td_interpolation_strategy::average;
  }
  return std::nullopt;
}

std::optional<pxsch_bler_test_configuration> parse_configuration(int argc, char** argv)
{
  CLI::App                      app{"OCUDU PxSCH BLER test"};
  pxsch_bler_test_configuration cfg;

  app.add_option(
         "-C,--channel-delay-profile", cfg.channel_delay_profile, "Channel delay profile: single-tap, TDLA, TDLB, TDLC")
      ->check(CLI::IsMember({"single-tap", "TDLA", "TDLB", "TDLC"}));

  app.add_option("-F,--channel-fading-distribution",
                 cfg.channel_fading_distribution,
                 "Channel fading distribution: uniform-phase, rayleigh, butler")
      ->check(CLI::IsMember({"uniform-phase", "rayleigh", "butler"}));

  app.add_flag("-D,--enable-dc-position", cfg.enable_dc_position, "Enable DC position");

  app.add_option("-S,--sinr-db", cfg.sinr_dB, "SINR in dB")->check(CLI::Range(0.0F, 100.0F));

  app.add_option("-N,--corrupted-re", cfg.nof_corrupted_re_per_ofdm_symbol, "Number of corrupted RE per OFDM symbol")
      ->check(CLI::Range(0u, static_cast<unsigned>(MAX_NOF_SUBCARRIERS)));

  app.add_option("-P,--rx-ports", cfg.nof_rx_ports, "Number of receive ports")
      ->check(CLI::Range(1u, std::min(precoding_constants::MAX_NOF_LAYERS, MAX_PORTS)));

  app.add_option("-L,--layers", cfg.nof_layers, "Number of transmit layers (must not exceed ports)")
      ->check(CLI::Range(1u, MAX_PORTS));

  app.add_option("-B,--bwp-size-rb", cfg.bwp_size_rb, "Number of allocated PRBs (BWP size)")
      ->check(CLI::Range(1u, static_cast<unsigned>(MAX_NOF_PRBS)));

  app.add_option_function<std::string>(
      "-M,--mcs-table",
      [&cfg](const std::string& value) {
        std::optional<pusch_mcs_table> table = to_mcs_table_impl(value.c_str());
        report_error_if_not(table, "Invalid MCS table {}", value);
        cfg.mcs_table = table.value();
      },
      "MCS table");

  app.add_option("-m,--mcs-index", cfg.mcs_index, "MCS index")->check(CLI::Range(0u, 31u));

  app.add_option("-R,--repetitions", cfg.nof_repetitions, "Number of slots to process")
      ->check(CLI::Range(1u, static_cast<unsigned>(std::numeric_limits<unsigned>::max())));

  app.add_option("-T,--pxsch-type", cfg.pxsch_type, "PxSCH implementation type: auto, acc100")
      ->check(CLI::IsMember({"auto", "neon", "avx", "avx512", "acc100"}));

  app.add_option_function<std::string>(
      "-E,--td-interpolation",
      [&cfg](const std::string& value) {
        auto strat = parse_td_interpolation_strategy(value);
        report_error_if_not(strat, "Invalid TD interpolation strategy {}", value);
        cfg.td_interpolation_strategy = strat.value();
      },
      "TD interpolation strategy: interpolate, average");

  app.add_option_function<std::string>(
      "-A,--dmrs-additional-positions",
      [&cfg](const std::string& value) {
        auto raw = parse_int<unsigned>(value);
        report_error_if_not(raw, "Invalid DM-RS additional positions {}", value);
        auto num = raw.value();
        report_error_if_not(num <= 3, "Invalid DM-RS additional positions {}", value);
        cfg.dmrs_additional_pos = static_cast<dmrs_additional_positions>(num);
      },
      "DM-RS additional positions (0-3)");

  app.add_option("-j,--max-threads", cfg.max_nof_threads, "Maximum number of threads")
      ->check(CLI::Range(1u, std::numeric_limits<unsigned>::max()));

  app.add_option("-i,--ldpc-iterations", cfg.nof_ldpc_iterations, "Max number of LDPC decoder iterations")
      ->check(CLI::Range(1u, 100u));

  add_option(app, "-V,--rv-sequence", cfg.rep_rv_sequence, "Retransmission RV sequence (e.g. [0 4 2 3])")
      ->check(CLI::IsMember({0, 1, 2, 3, 4}))
      ->default_val(0);

  app.add_flag("-v,--stats", cfg.show_stats, "Show preliminary stats");

  app.add_option("-l,--log-level", cfg.log_level, "Log level: none, error, warning, info, debug")
      ->default_str("warning")
      ->check([](const std::string& value) {
        return ocudulog::str_to_basic_level(value).has_value() ? "" : fmt::format("Invalid log level '{}'", value);
      });

  // Actual parsing.
  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    app.exit(e);
    return std::nullopt;
  }

  return cfg;
}

} // namespace ocudu
