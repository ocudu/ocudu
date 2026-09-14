// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_cli11_schema.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_translators.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config_validator.h"
#include "ocudu/du/du_high/du_high_configuration.h"
#include "ocudu/support/config_parsers.h"
#include <gtest/gtest.h>
#include <sstream>

using namespace ocudu;

namespace {

/// \brief Single-cell configuration, with the parameters a parsed one has auto-derived.
///
/// The bearers are written as configuration text and parsed by the CLI11 schema, as the application does. A bearer
/// built by hand would carry an RLC configuration of zeroes, which the validation rejects before it reaches the
/// allowed HARQ mode.
class du_high_config_bench
{
public:
  /// Adds a 5QI carrying the given allowed HARQ mode, or none when the mode is empty.
  void add_qos(five_qi_t five_qi, const std::string& allowed_harq_mode)
  {
    qos_text += "\n  - five_qi: " + std::to_string(static_cast<unsigned>(five_qi)) + R"(
    rlc:
      mode: am
      am:
        tx: {sn: 18, t-poll-retransmit: 100, max-retx-threshold: 32, poll-pdu: 16, poll-byte: -1}
        rx: {sn: 18, t-reassembly: 20, t-status-prohibit: 10}
    f1u_du:
      backoff_timer: 5)";
    qos_text += mac_text(allowed_harq_mode);
  }

  /// \brief Puts the second half of the UL HARQ processes of the cell in mode B, leaving both modes available.
  ///
  /// Mode B is only accepted on an NTN cell, so the cell moves to an NTN band. The mask is written from the left, a
  /// set bit meaning mode B, and only the first \c nof_harqs processes of the cell exist.
  void add_mode_b_processes(const std::string& mask = "0x00ff0000", unsigned nof_harqs = 16)
  {
    cell_text = "\n  - band: 256\n    dl_arfcn: 437000\n    common_scs: 15\n    channel_bandwidth_MHz: 5"
                "\n    pusch:\n      nof_harqs: " +
                std::to_string(nof_harqs) + "\n      harq_mode_b: \"" + mask + "\"";
  }

  /// Configuration with its auto-derived parameters filled in.
  const du_high_unit_config& derived_config()
  {
    CLI::App app{"du_high_allowed_harq_mode_test"};
    app.config_formatter(create_yaml_config_parser());
    app.allow_config_extras(CLI::config_extras_mode::error);

    du_high_parsed_config parsed_cfg;
    configure_cli11_with_du_high_config_schema(app, parsed_cfg);

    // The schema parses any stream, so the configuration needs no file on disk.
    std::istringstream input(config_text());
    app.parse_from_stream(input);

    unit_cfg = parsed_cfg.config;
    autoderive_du_high_parameters_after_parsing(unit_cfg);
    return unit_cfg;
  }

  bool validate() { return validate_du_high_config(derived_config()); }

private:
  static std::string mac_text(const std::string& allowed_harq_mode)
  {
    return allowed_harq_mode.empty() ? std::string{} : "\n    mac:\n      allowed_harq_mode: " + allowed_harq_mode;
  }

  std::string config_text() const
  {
    std::string text = "cells:" + (cell_text.empty() ? std::string{"\n  - pci: 1"} : cell_text);
    if (not qos_text.empty()) {
      text += "\nqos:" + qos_text;
    }
    return text + "\n";
  }

  std::string         cell_text;
  std::string         qos_text;
  du_high_unit_config unit_cfg;
};

} // namespace

/// TS 38.331 makes allowedHARQ-mode optional, and an absent one leaves the mapping unrestricted.
TEST(du_high_allowed_harq_mode_test, an_unset_allowed_harq_mode_is_accepted)
{
  du_high_config_bench bench;
  EXPECT_TRUE(bench.validate());
}

/// Every HARQ process of a cell is in mode A unless harq_mode_b says otherwise, so mode A is always available.
TEST(du_high_allowed_harq_mode_test, mode_a_is_accepted_on_a_cell_whose_processes_are_all_mode_a)
{
  du_high_config_bench bench;
  bench.add_qos(uint_to_five_qi(9), "mode_a");
  EXPECT_TRUE(bench.validate());
}

/// A logical channel restricted to a mode no HARQ process carries would never be multiplexed into a grant.
TEST(du_high_allowed_harq_mode_test, mode_b_is_rejected_when_no_process_is_in_mode_b)
{
  du_high_config_bench bench;
  bench.add_qos(uint_to_five_qi(9), "mode_b");
  EXPECT_FALSE(bench.validate());
}

/// The SRBs are served in mode A, so the mask must leave a process out even when no 5QI asks for a mode.
TEST(du_high_allowed_harq_mode_test, a_mask_without_a_mode_a_process_is_rejected)
{
  du_high_config_bench bench;
  bench.add_mode_b_processes("0xffff0000");
  EXPECT_FALSE(bench.validate());
}

/// A mask that keeps a process in mode A serves the SRBs, so it is accepted with no 5QI configured.
TEST(du_high_allowed_harq_mode_test, a_mask_keeping_a_mode_a_process_is_accepted)
{
  du_high_config_bench bench;
  bench.add_mode_b_processes();
  EXPECT_TRUE(bench.validate());
}

/// A UE is given the first 16 UL HARQ processes at most, so a mode A process beyond them does not serve the SRBs.
TEST(du_high_allowed_harq_mode_test, a_mode_a_process_beyond_the_ue_process_cap_is_rejected)
{
  du_high_config_bench bench;
  // Processes 0 to 15 in mode B and 16 to 31 in mode A, of which a UE only ever gets the first 16.
  bench.add_mode_b_processes("0xffff0000", 32);
  EXPECT_FALSE(bench.validate());
}

/// The process cap binds the mode a 5QI asks for just as much.
TEST(du_high_allowed_harq_mode_test, a_five_qi_mode_beyond_the_ue_process_cap_is_rejected)
{
  du_high_config_bench bench;
  // Processes 0 to 15 in mode A and 16 to 31 in mode B, so no process a UE gets carries mode B.
  bench.add_mode_b_processes("0x0000ffff", 32);
  bench.add_qos(uint_to_five_qi(9), "mode_b");
  EXPECT_FALSE(bench.validate());
}

/// The cap only bars the processes beyond it, so a cell offering both modes within reach of every UE is accepted.
TEST(du_high_allowed_harq_mode_test, both_modes_within_the_ue_process_cap_are_accepted)
{
  du_high_config_bench bench;
  // Processes 8 to 15 in mode B and the rest in mode A, so both modes sit among the first 16.
  bench.add_mode_b_processes("0x00ff0000", 32);
  bench.add_qos(uint_to_five_qi(9), "mode_b");
  EXPECT_TRUE(bench.validate());
}

/// A DRB restricted to mode B takes a logical channel group of its own, so the 5QIs are free to differ.
TEST(du_high_allowed_harq_mode_test, five_qis_may_ask_for_different_allowed_harq_modes)
{
  du_high_config_bench bench;
  bench.add_mode_b_processes();
  bench.add_qos(uint_to_five_qi(9), "mode_a");
  bench.add_qos(uint_to_five_qi(7), "mode_b");
  EXPECT_TRUE(bench.validate());
}

/// The configured mode reaches the QoS configuration the DU hands to the bearer manager.
TEST(du_high_allowed_harq_mode_test, the_configured_mode_reaches_the_qos_configuration)
{
  du_high_config_bench bench;
  bench.add_qos(uint_to_five_qi(9), "mode_a");

  odu::du_high_configuration du_hi_cfg;
  generate_du_high_config(du_hi_cfg, bench.derived_config());

  ASSERT_TRUE(du_hi_cfg.ran.qos.count(uint_to_five_qi(9)) == 1);
  const auto& qos = du_hi_cfg.ran.qos.at(uint_to_five_qi(9));
  ASSERT_TRUE(qos.allowed_harq_mode.has_value());
  EXPECT_EQ(*qos.allowed_harq_mode, ul_harq_mode::mode_a);
}

/// An unconfigured 5QI leaves the mapping unrestricted.
TEST(du_high_allowed_harq_mode_test, an_unset_mode_leaves_the_qos_configuration_unrestricted)
{
  du_high_config_bench bench;
  bench.add_qos(uint_to_five_qi(9), "");

  odu::du_high_configuration du_hi_cfg;
  generate_du_high_config(du_hi_cfg, bench.derived_config());

  EXPECT_FALSE(du_hi_cfg.ran.qos.at(uint_to_five_qi(9)).allowed_harq_mode.has_value());
}
