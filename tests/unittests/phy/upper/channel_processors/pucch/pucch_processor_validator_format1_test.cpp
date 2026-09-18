// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pucch_processor_test_fixture.h"
#include "ocudu/adt/format.h"
#include "ocudu/phy/upper/channel_processors/prach/formatters.h"
#include "ocudu/phy/upper/channel_processors/pucch/formatters.h"
#include "ocudu/ran/pucch/pucch_constants.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>
#include <regex>

using namespace ocudu;

namespace {

// Valid PUCCH Format 1 configuration.
const pucch_processor::format1_configuration base_format_1_config = {
    // Context.
    std::nullopt,
    // Slot.
    {0, 9},
    // Slot offset.
    {},
    // BWP size.
    50,
    // BWP start.
    10,
    // CP.
    cyclic_prefix::NORMAL,
    // Starting PRB.
    1,
    // Second hop PRB.
    {},
    // N_ID
    0,
    // Number of HARQ-ACK bits.
    1,
    // Rx Ports.
    {0},
    // Initial cyclic shift.
    0,
    // Number of OFDM symbols.
    1,
    // Start symbol index.
    3,
    // Time domain OCC.
    0};

// Test case parameters structure.
struct test_params {
  pucch_processor::format1_configuration config;
  std::string                            assert_message;
};

struct test_case_t {
  std::function<test_params()> get_test_params;
};

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os, "{}", test_case.get_test_params().config);
  return os;
}

} // namespace

using PucchProcessorFormat1Param = test_case_t;
using PucchProcessorFormat1Fixture =
    PucchProcessorTestFixture<PucchProcessorFormat1Param, pucch_constants::f1::MIN_NOF_SYMS, 1>;
template <>
std::unique_ptr<pucch_processor> PucchProcessorFormat1Fixture::processor = nullptr;
template <>
std::unique_ptr<pucch_pdu_validator> PucchProcessorFormat1Fixture::validator = nullptr;

// Maximum channel dimensions used to construct the PUCCH processor.
static constexpr channel_estimate::channel_estimate_dimensions max_dimensions =
    PucchProcessorFormat1Fixture::channel_estimate_dimensions;

// Test cases are implemented as lambda functions that generate and return an invalid PUCCH Format 1 configuration,
// along with the expected assert message.
const std::vector<test_case_t> pucch_processor_validator_test_data = {
    {
        [] {
          test_params entry         = {};
          entry.config              = base_format_1_config;
          entry.config.bwp_start_rb = 10;
          entry.config.bwp_size_rb  = MAX_NOF_PRBS - entry.config.bwp_start_rb + 1;
          entry.assert_message      = fmt::format(
              R"(BWP allocation goes up to PRB {}\, exceeding the configured maximum grid RB size\, i\.e\.\, {}\.)",
              entry.config.bwp_start_rb + entry.config.bwp_size_rb,
              MAX_NOF_PRBS);
          return entry;
        },
    },
    {
        [] {
          test_params entry         = {};
          entry.config              = base_format_1_config;
          entry.config.starting_prb = entry.config.bwp_size_rb;
          entry.assert_message =
              fmt::format(R"(PRB allocation within the BWP goes up to PRB {}\, exceeding BWP size\, i\.e\.\, {}\.)",
                          entry.config.starting_prb + 1,
                          entry.config.bwp_size_rb);
          return entry;
        },
    },
    {
        [] {
          test_params entry           = {};
          entry.config                = base_format_1_config;
          entry.config.second_hop_prb = entry.config.bwp_size_rb;
          entry.assert_message        = fmt::format(
              R"(Second hop PRB allocation within the BWP goes up to PRB {}\, exceeding BWP size\, i\.e\.\, {}\.)",
              entry.config.second_hop_prb.value() + 1,
              entry.config.bwp_size_rb);
          return entry;
        },
    },
    {
        [] {
          test_params entry               = {};
          entry.config                    = base_format_1_config;
          entry.config.slot               = slot_point(2, 0);
          entry.config.cp                 = cyclic_prefix::EXTENDED;
          entry.config.nof_symbols        = pucch_constants::f1::MIN_NOF_SYMS;
          entry.config.start_symbol_index = get_nsymb_per_slot(entry.config.cp) - entry.config.nof_symbols + 1;
          entry.assert_message            = fmt::format(
              R"(OFDM symbol allocation goes up to symbol {}\, exceeding the number of symbols in the given slot with {} CP\, i\.e\.\, {}\.)",
              entry.config.start_symbol_index + entry.config.nof_symbols,
              entry.config.cp.to_string(),
              get_nsymb_per_slot(entry.config.cp));
          return entry;
        },
    },
    {
        [] {
          test_params entry               = {};
          entry.config                    = base_format_1_config;
          entry.config.cp                 = cyclic_prefix::NORMAL;
          entry.config.nof_symbols        = pucch_constants::f1::MIN_NOF_SYMS;
          entry.config.start_symbol_index = max_dimensions.nof_symbols - pucch_constants::f1::MIN_NOF_SYMS + 1;
          entry.assert_message            = fmt::format(
              R"(OFDM symbol allocation goes up to symbol {}\, exceeding the configured maximum number of slot symbols\, i\.e\.\, {}\.)",
              entry.config.start_symbol_index + entry.config.nof_symbols,
              max_dimensions.nof_symbols);
          return entry;
        },
    },
    {
        [] {
          test_params entry    = {};
          entry.config         = base_format_1_config;
          entry.config.ports   = {};
          entry.assert_message = fmt::format(R"(The number of receive ports cannot be zero\.)");
          return entry;
        },
    },
    {
        [] {
          test_params entry    = {};
          entry.config         = base_format_1_config;
          entry.config.ports   = {0, 1};
          entry.assert_message = fmt::format(
              R"(The number of receive ports\, i\.e\. {}\, exceeds the configured maximum number of receive ports\, i\.e\.\, {}\.)",
              entry.config.ports.size(),
              max_dimensions.nof_rx_ports);
          return entry;
        },
    },
};

TEST_P(PucchProcessorFormat1Fixture, PucchProcessorValidatortest)
{
  ASSERT_NE(processor, nullptr) << "PUCCH processor not created.";
  ASSERT_NE(validator, nullptr) << "PUCCH validator not created.";

  const test_case_t& param = GetParam();

  // Make sure the configuration is invalid.
  error_type<std::string> validator_out = validator->is_valid(param.get_test_params().config);
  ASSERT_FALSE(validator_out.has_value()) << "Validation should fail.";
  ASSERT_TRUE(std::regex_match(validator_out.error(), std::regex(param.get_test_params().assert_message)))
      << "The assertion message doesn't match the expected pattern.";
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PucchProcessorValidatortest,
                         PucchProcessorFormat1Fixture,
                         ::testing::ValuesIn(pucch_processor_validator_test_data));
