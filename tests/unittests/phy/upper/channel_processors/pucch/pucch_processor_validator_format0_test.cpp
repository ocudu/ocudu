// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pucch_processor_test_fixture.h"
#include "ocudu/adt/expected.h"
#include "ocudu/adt/format.h"
#include "ocudu/phy/upper/channel_processors/prach/formatters.h"
#include "ocudu/phy/upper/channel_processors/pucch/formatters.h"
#include "ocudu/ran/pucch/pucch_constants.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>
#include <regex>

using namespace ocudu;

namespace {

// Test case parameters structure.
struct test_params {
  pucch_processor::format0_configuration config;
  std::string                            assert_message;
};

struct test_case_t {
  std::function<test_params()> get_test_params;
};

// Valid PUCCH Format 0 configuration.
const pucch_processor::format0_configuration base_format_0_config = {
    // Context.
    std::nullopt,
    // Slot.
    {0, 9},
    // Slot offset.
    {},
    // CP.
    cyclic_prefix::NORMAL,
    // BWP size.
    50,
    // BWP start.
    10,
    // Starting PRB.
    1,
    // Second hop PRB.
    {},
    // Start symbol index.
    3,
    // Number of OFDM symbols.
    1,
    // Initial cyclic shift.
    0,
    // N_ID
    0,
    // Number of HARQ-ACK bits.
    1,
    // SR opportunity.
    false,
    // Rx Ports.
    {0},
};

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os, "{}", test_case.get_test_params().config);
  return os;
}

// Test cases are implemented as lambda functions that generate and return an invalid PUCCH Format 0 configuration,
// along with the expected assert message.
const std::vector<test_case_t> pucch_processor_validator_test_data = {
    {
        [] {
          test_params entry         = {};
          entry.config              = base_format_0_config;
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
          entry.config              = base_format_0_config;
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
          entry.config                = base_format_0_config;
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
          entry.config                    = base_format_0_config;
          entry.config.slot               = slot_point(2, 0);
          entry.config.cp                 = cyclic_prefix::EXTENDED;
          entry.config.nof_symbols        = pucch_constants::f0::MIN_NOF_SYMS;
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
          test_params entry        = {};
          entry.config             = base_format_0_config;
          entry.config.nof_symbols = pucch_constants::f0::MAX_NOF_SYMS + 1;
          entry.assert_message     = R"(Number of symbols \(i\.e\., 3\) is out of the range \[1\.\.2\]\.)";
          return entry;
        },
    },
    {
        [] {
          test_params entry                 = {};
          entry.config                      = base_format_0_config;
          entry.config.initial_cyclic_shift = pucch_constants::f0::NOF_ICS + 1;
          entry.assert_message = R"(The initial cyclic shift \(i\.e\., 13\) is out of the range \[0\.\.12\)\.)";
          return entry;
        },
    },
    {
        [] {
          test_params entry = {};
          entry.config      = base_format_0_config;
          entry.config.n_id = pucch_constants::N_ID.stop() + 1;
          entry.assert_message =
              R"(The sequence hopping identifier \(i\.e\., 1025\) is out of the range \[0\.\.1024\)\.)";
          return entry;
        },
    },
    {
        [] {
          test_params entry         = {};
          entry.config              = base_format_0_config;
          entry.config.nof_harq_ack = pucch_constants::f0::MAX_NOF_HARQ_ACK_BITS + 1;
          entry.assert_message      = R"(The number of HARQ-ACK bits \(i\.e\., 3\) is out of the range \[0\.\.2\]\.)";
          return entry;
        },
    },
    {
        [] {
          test_params entry           = {};
          entry.config                = base_format_0_config;
          entry.config.nof_harq_ack   = 0;
          entry.config.sr_opportunity = false;
          entry.assert_message        = R"(No payload\.)";
          return entry;
        },
    },
    {
        [] {
          test_params entry    = {};
          entry.config         = base_format_0_config;
          entry.config.ports   = {};
          entry.assert_message = R"(The number of receive ports cannot be zero\.)";
          return entry;
        },
    },
};

} // namespace

using PucchProcessorFormat0Param   = test_case_t;
using PucchProcessorFormat0Fixture = PucchProcessorTestFixture<PucchProcessorFormat0Param>;
template <>
std::unique_ptr<pucch_processor> PucchProcessorFormat0Fixture::processor = nullptr;
template <>
std::unique_ptr<pucch_pdu_validator> PucchProcessorFormat0Fixture::validator = nullptr;

TEST_P(PucchProcessorFormat0Fixture, PucchProcessorValidatortest)
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
                         PucchProcessorFormat0Fixture,
                         ::testing::ValuesIn(pucch_processor_validator_test_data));
