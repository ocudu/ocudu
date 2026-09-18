// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pucch_processor_test_fixture.h"
#include "ocudu/adt/format.h"
#include "ocudu/adt/to_array.h"
#include "ocudu/phy/upper/channel_processors/prach/formatters.h"
#include "ocudu/phy/upper/channel_processors/pucch/formatters.h"
#include "ocudu/ran/pucch/pucch_constants.h"
#include "fmt/ostream.h"
#include "gtest/gtest.h"
#include <regex>

using namespace ocudu;

namespace {

// Valid PUCCH Format 4 configuration.
const pucch_processor::format4_configuration base_format_4_config = {
    // Context.
    std::nullopt,
    // Slot.
    {0, 9},
    // Slot offset.
    {},
    // CP.
    cyclic_prefix::NORMAL,
    // Rx Ports.
    {0},
    // BWP size.
    50,
    // BWP start.
    10,
    // Starting PRB.
    1,
    // Second hop PRB.
    {},
    // Start symbol index.
    8,
    // Number of OFDM symbols.
    4,
    // RNTI.
    65535,
    // N_ID for scrambling.
    0,
    // N_ID for sequence hopping.
    0,
    // Number of HARQ-ACK bits.
    pucch_constants::f4::MIN_NOF_DATA_BITS,
    // Number of SR bits.
    0,
    // Number of CSI Part 1 bits.
    0,
    // CSI Part 2 size.
    {},
    // Additional DM-RS.
    false,
    // pi/2-BPSK.
    false,
    // OCC index.
    0,
    // OCC length.
    2,
    // Max code rate.
    pucch_constants::f4::MAX_CODE_RATE};

// Test case parameters structure.
struct test_params {
  pucch_processor::format4_configuration config;
  std::string                            assert_message;
  bool                                   is_valid = false;
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

using PucchProcessorFormat4Param   = test_case_t;
using PucchProcessorFormat4Fixture = PucchProcessorTestFixture<PucchProcessorFormat4Param, MAX_NSYMB_PER_SLOT - 1, 1>;
template <>
std::unique_ptr<pucch_processor> PucchProcessorFormat4Fixture::processor = nullptr;
template <>
std::unique_ptr<pucch_pdu_validator> PucchProcessorFormat4Fixture::validator = nullptr;

// Maximum channel dimensions used to construct the PUCCH processor.
static constexpr channel_estimate::channel_estimate_dimensions max_dimensions =
    PucchProcessorFormat4Fixture::channel_estimate_dimensions;

// Test cases are implemented as lambda functions that generate and return an invalid PUCCH Format 4 configuration,
// along with the expected assert message.
static const auto pucch_processor_validator_test_data = to_array<test_case_t>(
    {{
         [] {
           test_params entry         = {};
           entry.config              = base_format_4_config;
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
           entry.config              = base_format_4_config;
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
           test_params entry               = {};
           entry.config                    = base_format_4_config;
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
           entry.config                    = base_format_4_config;
           entry.config.cp                 = cyclic_prefix::NORMAL;
           entry.config.nof_symbols        = 1;
           entry.config.start_symbol_index = max_dimensions.nof_symbols;
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
           entry.config         = base_format_4_config;
           entry.config.ports   = {};
           entry.assert_message = R"(The number of receive ports cannot be zero\.)";
           return entry;
         },
     },
     {
         [] {
           test_params entry    = {};
           entry.config         = base_format_4_config;
           entry.config.ports   = {0, 1};
           entry.assert_message = fmt::format(
               R"(The number of receive ports\, i\.e\. {}\, exceeds the configured maximum number of receive ports\, i\.e\.\, {}\.)",
               entry.config.ports.size(),
               max_dimensions.nof_rx_ports);
           return entry;
         },
     },
     {
         [] {
           test_params entry          = {};
           entry.config               = base_format_4_config;
           entry.config.nof_harq_ack  = 0;
           entry.config.nof_sr        = 0;
           entry.config.nof_csi_part1 = 0;
           entry.assert_message       = R"(The UCI Part 1 payload must not be empty\.)";
           return entry;
         },
     },
     {
         [] {
           test_params entry           = {};
           entry.config                = base_format_4_config;
           entry.config.nof_csi_part1  = 0;
           entry.config.csi_part2_size = uci_part2_size_description(4);
           entry.assert_message        = R"(CSI Part 2 size description does not match CSI Part 1 payload size\.)";
           return entry;
         },
     },
     {
         [] {
           test_params entry          = {};
           entry.config               = base_format_4_config;
           entry.config.nof_harq_ack  = pucch_constants::f4::MIN_NOF_DATA_BITS - 1;
           entry.config.nof_sr        = 0;
           entry.config.nof_csi_part1 = 0;
           entry.assert_message       = fmt::format(
               R"(UCI Payload length \(i\.e\.\, {}\) is outside the supported range \(i\.e\.\, \[3\.\.1706\]\)\.)",
               entry.config.nof_harq_ack + entry.config.nof_sr + entry.config.nof_csi_part1);
           return entry;
         },
     },
     {
         [] {
           test_params entry          = {};
           entry.config               = base_format_4_config;
           entry.config.max_code_rate = pucch_constants::f4::MAX_CODE_RATE + 0.1F;
           entry.assert_message = R"(The maximum code rate \(i\.e\., .+\) exceeds the format maximum \(i\.e\., .+\)\.)";
           return entry;
         },
     },
     {
         [] {
           test_params entry         = {};
           entry.config              = base_format_4_config;
           entry.config.nof_harq_ack = uci_constants::MAX_NOF_HARQ_BITS;
           entry.assert_message =
               R"(The effective code rate \(i\.e\., [0-9]+\.*[0-9]*\) exceeds the maximum allowed 0\.8\.)";
           return entry;
         },
     },
     {
         [] {
           test_params entry       = {};
           entry.config            = base_format_4_config;
           entry.config.occ_length = 3;
           entry.assert_message    = R"(Invalid OCC length value \(i\.e\.\, 3\)\. Valid values are 2 and 4\.)";
           return entry;
         },
     },
     {
         [] {
           test_params entry               = {};
           entry.config                    = base_format_4_config;
           entry.config.start_symbol_index = 0;
           entry.config.nof_symbols        = 13;
           entry.config.nof_csi_part1      = 71;
           entry.config.csi_part2_size     = uci_part2_size_description(1);
           entry.config.additional_dmrs    = true;
           entry.assert_message            = R"(There are no rate matching output bits remaining for UCI Part 2\.)";
           return entry;
         },
     },
     {
         [] {
           test_params entry           = {};
           entry.config                = base_format_4_config;
           entry.config.nof_csi_part1  = 10;
           entry.config.csi_part2_size = uci_part2_size_description(4);
           entry.is_valid              = true;
           return entry;
         },
     }});

TEST_P(PucchProcessorFormat4Fixture, PucchProcessorValidatortest)
{
  ASSERT_NE(processor, nullptr) << "PUCCH processor not created.";
  ASSERT_NE(validator, nullptr) << "PUCCH validator not created.";

  const test_case_t& param = GetParam();

  error_type<std::string> validator_out = validator->is_valid(param.get_test_params().config);
  if (param.get_test_params().is_valid) {
    ASSERT_TRUE(validator_out.has_value()) << "Validation should pass.";
    return;
  }

  // Make sure the configuration is invalid.
  ASSERT_FALSE(validator_out.has_value()) << "Validation should fail.";
  ASSERT_TRUE(std::regex_match(validator_out.error(), std::regex(param.get_test_params().assert_message)))
      << "The assertion message doesn't match the expected pattern.";
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PucchProcessorValidatortest,
                         PucchProcessorFormat4Fixture,
                         ::testing::ValuesIn(pucch_processor_validator_test_data));
