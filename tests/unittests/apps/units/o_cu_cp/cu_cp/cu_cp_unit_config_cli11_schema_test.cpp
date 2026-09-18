// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/units/o_cu_cp/cu_cp/cu_cp_unit_config.h"
#include "apps/units/o_cu_cp/cu_cp/cu_cp_unit_config_cli11_schema.h"
#include "ocudu/support/config_parsers.h"
#include "CLI/CLI11.hpp"
#include <gtest/gtest.h>
#include <sstream>

using namespace ocudu;

namespace {

/// Parses the given "cu_cp: security: ..." YAML snippet and returns the resulting security configuration.
/// Throws CLI::ParseError if any of the registered ->check() validators reject the input.
cu_cp_unit_security_config parse_security_config(const std::string& yaml_body)
{
  cu_cp_unit_config cfg;

  CLI::App app("cu_cp_unit_config_cli11_schema_test");
  app.config_formatter(create_yaml_config_parser());
  app.allow_config_extras(CLI::config_extras_mode::capture);
  configure_cli11_with_cu_cp_unit_config_schema(app, cfg);

  std::istringstream ss(yaml_body);
  app.parse_from_stream(ss);

  return cfg.security_config;
}

std::string security_yaml(const std::string& key, const std::string& value)
{
  return "cu_cp:\n  security:\n    " + key + ": " + value + "\n";
}

} // namespace

TEST(cu_cp_unit_config_cli11_schema_test, nea_pref_list_pads_short_list_by_repeating_the_first_entry)
{
  // Regression test: a nea_pref_list shorter than 4 entries must not leave trailing slots zero-initialized,
  // since ciphering_algorithm::nea0 == 0 would silently reintroduce null ciphering as a fallback.
  cu_cp_unit_security_config cfg = parse_security_config(security_yaml("nea_pref_list", "nea2,nea1,nea3"));
  EXPECT_EQ(cfg.nea_preference_list[0], security::ciphering_algorithm::nea2);
  EXPECT_EQ(cfg.nea_preference_list[1], security::ciphering_algorithm::nea1);
  EXPECT_EQ(cfg.nea_preference_list[2], security::ciphering_algorithm::nea3);
  EXPECT_EQ(cfg.nea_preference_list[3], security::ciphering_algorithm::nea2);
  for (auto algo : cfg.nea_preference_list) {
    EXPECT_NE(algo, security::ciphering_algorithm::nea0);
  }
}

TEST(cu_cp_unit_config_cli11_schema_test, nea_pref_list_pads_a_single_entry_list)
{
  cu_cp_unit_security_config cfg = parse_security_config(security_yaml("nea_pref_list", "nea2"));
  for (auto algo : cfg.nea_preference_list) {
    EXPECT_EQ(algo, security::ciphering_algorithm::nea2);
  }
}

TEST(cu_cp_unit_config_cli11_schema_test, nea_pref_list_short_list_round_trips_through_to_string)
{
  // Regression test: to_string() must stop at the padded (repeated) slot, or a dumped config with a short
  // nea_pref_list fails to reparse (the padded array {nea2,nea1,nea3,nea2} would otherwise be written back
  // in full and rejected by the duplicate check on the next parse).
  cu_cp_unit_security_config cfg = parse_security_config(security_yaml("nea_pref_list", "nea2,nea1,nea3"));
  EXPECT_EQ(to_string(cfg.nea_preference_list), "nea2,nea1,nea3");
}

TEST(cu_cp_unit_config_cli11_schema_test, nea_pref_list_accepts_a_full_permutation_without_padding)
{
  cu_cp_unit_security_config cfg = parse_security_config(security_yaml("nea_pref_list", "nea1,nea2,nea3,nea0"));
  EXPECT_EQ(cfg.nea_preference_list[0], security::ciphering_algorithm::nea1);
  EXPECT_EQ(cfg.nea_preference_list[1], security::ciphering_algorithm::nea2);
  EXPECT_EQ(cfg.nea_preference_list[2], security::ciphering_algorithm::nea3);
  EXPECT_EQ(cfg.nea_preference_list[3], security::ciphering_algorithm::nea0);
}

TEST(cu_cp_unit_config_cli11_schema_test, nea_pref_list_rejects_an_empty_list)
{
  EXPECT_THROW(parse_security_config(security_yaml("nea_pref_list", "''")), CLI::ParseError);
}

TEST(cu_cp_unit_config_cli11_schema_test, nea_pref_list_rejects_duplicate_algorithms)
{
  try {
    parse_security_config(security_yaml("nea_pref_list", "nea1,nea2,nea1"));
    FAIL() << "Expected a CLI::ParseError";
  } catch (const CLI::ParseError& e) {
    EXPECT_NE(std::string(e.what()).find("Duplicate ciphering algorithm"), std::string::npos);
  }
}

TEST(cu_cp_unit_config_cli11_schema_test, nea_pref_list_rejects_more_than_four_algorithms)
{
  EXPECT_THROW(parse_security_config(security_yaml("nea_pref_list", "nea0,nea1,nea2,nea3,nea0")), CLI::ParseError);
}

TEST(cu_cp_unit_config_cli11_schema_test, nea_pref_list_rejects_an_unknown_algorithm)
{
  EXPECT_THROW(parse_security_config(security_yaml("nea_pref_list", "nea9")), CLI::ParseError);
}

TEST(cu_cp_unit_config_cli11_schema_test, nia_pref_list_pads_short_list_by_repeating_the_first_entry)
{
  // Regression test: a nia_pref_list shorter than 4 entries must not leave the trailing slot zero-initialized,
  // since integrity_algorithm::nia0 == 0 would silently reintroduce NIA0 (which cannot be selected explicitly).
  cu_cp_unit_security_config cfg = parse_security_config(security_yaml("nia_pref_list", "nia1,nia2,nia3"));
  EXPECT_EQ(cfg.nia_preference_list[0], security::integrity_algorithm::nia1);
  EXPECT_EQ(cfg.nia_preference_list[1], security::integrity_algorithm::nia2);
  EXPECT_EQ(cfg.nia_preference_list[2], security::integrity_algorithm::nia3);
  EXPECT_EQ(cfg.nia_preference_list[3], security::integrity_algorithm::nia1);
  for (auto algo : cfg.nia_preference_list) {
    EXPECT_NE(algo, security::integrity_algorithm::nia0);
  }
}

TEST(cu_cp_unit_config_cli11_schema_test, nia_pref_list_short_list_round_trips_through_to_string)
{
  // Regression test: same as the NEA case above, but for the integrity preference list.
  cu_cp_unit_security_config cfg = parse_security_config(security_yaml("nia_pref_list", "nia2,nia1"));
  EXPECT_EQ(to_string(cfg.nia_preference_list), "nia2,nia1");
}

TEST(cu_cp_unit_config_cli11_schema_test, nia_pref_list_pads_a_single_entry_list)
{
  cu_cp_unit_security_config cfg = parse_security_config(security_yaml("nia_pref_list", "nia3"));
  for (auto algo : cfg.nia_preference_list) {
    EXPECT_EQ(algo, security::integrity_algorithm::nia3);
  }
}

TEST(cu_cp_unit_config_cli11_schema_test, nia_pref_list_rejects_an_empty_list)
{
  EXPECT_THROW(parse_security_config(security_yaml("nia_pref_list", "''")), CLI::ParseError);
}

TEST(cu_cp_unit_config_cli11_schema_test, nia_pref_list_rejects_nia0)
{
  EXPECT_THROW(parse_security_config(security_yaml("nia_pref_list", "nia0")), CLI::ParseError);
}

TEST(cu_cp_unit_config_cli11_schema_test, nia_pref_list_rejects_duplicate_algorithms)
{
  try {
    parse_security_config(security_yaml("nia_pref_list", "nia2,nia2"));
    FAIL() << "Expected a CLI::ParseError";
  } catch (const CLI::ParseError& e) {
    EXPECT_NE(std::string(e.what()).find("Duplicate integrity algorithm"), std::string::npos);
  }
}

TEST(cu_cp_unit_config_cli11_schema_test, nia_pref_list_rejects_more_than_three_algorithms)
{
  EXPECT_THROW(parse_security_config(security_yaml("nia_pref_list", "nia1,nia2,nia3,nia1")), CLI::ParseError);
}

TEST(cu_cp_unit_config_cli11_schema_test, nia_pref_list_rejects_an_unknown_algorithm)
{
  EXPECT_THROW(parse_security_config(security_yaml("nia_pref_list", "nia9")), CLI::ParseError);
}
