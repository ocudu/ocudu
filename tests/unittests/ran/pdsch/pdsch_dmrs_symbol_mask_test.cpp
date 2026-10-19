// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/adt/format.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/pdsch/pdsch_dmrs_symbol_mask.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>
#include <map>

using namespace ocudu;

namespace ocudu {

std::ostream& operator<<(std::ostream& os, dmrs_typeA_position dmrs_pos)
{
  switch (dmrs_pos) {
    case dmrs_typeA_position::pos2:
      return os << "pos2";
    case dmrs_typeA_position::pos3:
    default:
      return os << "pos3";
  }
}

std::ostream& operator<<(std::ostream& os, dmrs_additional_positions additional_pos)
{
  switch (additional_pos) {
    case dmrs_additional_positions::pos0:
      return os << "addpos0";
    case dmrs_additional_positions::pos1:
      return os << "addpos1";
    case dmrs_additional_positions::pos2:
      return os << "addpos2";
    case dmrs_additional_positions::pos3:
    default:
      return os << "addpos3";
  }
}

std::ostream& operator<<(std::ostream& os, dmrs_symbol_mask mask)
{
  fmt::print(os, "{}", mask);
  return os;
}

} // namespace ocudu

namespace {
#define L0 (UINT8_MAX)
#define L1 (UINT8_MAX + 1)

using PdschDmrsSymbolMaskParams = std::tuple<dmrs_typeA_position, unsigned, dmrs_additional_positions, bool, bool>;

class PdschDmrsSymbolMaskFixture : public ::testing::TestWithParam<PdschDmrsSymbolMaskParams>
{};

const std::map<std::pair<unsigned, dmrs_additional_positions>, static_vector<unsigned, 4>>
    pdsch_dmrs_symbol_mask_mapping_type_A_single_table = {
        {{3, dmrs_additional_positions::pos0}, {L0}},         {{3, dmrs_additional_positions::pos1}, {L0}},
        {{3, dmrs_additional_positions::pos2}, {L0}},         {{3, dmrs_additional_positions::pos3}, {L0}},
        {{4, dmrs_additional_positions::pos0}, {L0}},         {{4, dmrs_additional_positions::pos1}, {L0}},
        {{4, dmrs_additional_positions::pos2}, {L0}},         {{4, dmrs_additional_positions::pos3}, {L0}},
        {{5, dmrs_additional_positions::pos0}, {L0}},         {{5, dmrs_additional_positions::pos1}, {L0}},
        {{5, dmrs_additional_positions::pos2}, {L0}},         {{5, dmrs_additional_positions::pos3}, {L0}},
        {{6, dmrs_additional_positions::pos0}, {L0}},         {{6, dmrs_additional_positions::pos1}, {L0}},
        {{6, dmrs_additional_positions::pos2}, {L0}},         {{6, dmrs_additional_positions::pos3}, {L0}},
        {{7, dmrs_additional_positions::pos0}, {L0}},         {{7, dmrs_additional_positions::pos1}, {L0}},
        {{7, dmrs_additional_positions::pos2}, {L0}},         {{7, dmrs_additional_positions::pos3}, {L0}},
        {{8, dmrs_additional_positions::pos0}, {L0}},         {{8, dmrs_additional_positions::pos1}, {L0, 7}},
        {{8, dmrs_additional_positions::pos2}, {L0, 7}},      {{8, dmrs_additional_positions::pos3}, {L0, 7}},
        {{9, dmrs_additional_positions::pos0}, {L0}},         {{9, dmrs_additional_positions::pos1}, {L0, 7}},
        {{9, dmrs_additional_positions::pos2}, {L0, 7}},      {{9, dmrs_additional_positions::pos3}, {L0, 7}},
        {{10, dmrs_additional_positions::pos0}, {L0}},        {{10, dmrs_additional_positions::pos1}, {L0, 9}},
        {{10, dmrs_additional_positions::pos2}, {L0, 6, 9}},  {{10, dmrs_additional_positions::pos3}, {L0, 6, 9}},
        {{11, dmrs_additional_positions::pos0}, {L0}},        {{11, dmrs_additional_positions::pos1}, {L0, 9}},
        {{11, dmrs_additional_positions::pos2}, {L0, 6, 9}},  {{11, dmrs_additional_positions::pos3}, {L0, 6, 9}},
        {{12, dmrs_additional_positions::pos0}, {L0}},        {{12, dmrs_additional_positions::pos1}, {L0, 9}},
        {{12, dmrs_additional_positions::pos2}, {L0, 6, 9}},  {{12, dmrs_additional_positions::pos3}, {L0, 5, 8, 11}},
        {{13, dmrs_additional_positions::pos0}, {L0}},        {{13, dmrs_additional_positions::pos1}, {L0, L1}},
        {{13, dmrs_additional_positions::pos2}, {L0, 7, 11}}, {{13, dmrs_additional_positions::pos3}, {L0, 5, 8, 11}},
        {{14, dmrs_additional_positions::pos0}, {L0}},        {{14, dmrs_additional_positions::pos1}, {L0, L1}},
        {{14, dmrs_additional_positions::pos2}, {L0, 7, 11}}, {{14, dmrs_additional_positions::pos3}, {L0, 5, 8, 11}},
};

TEST_P(PdschDmrsSymbolMaskFixture, ANormal)
{
  // Extract parameters.
  dmrs_typeA_position             typeA_pos                         = std::get<0>(GetParam());
  bounded_integer<uint8_t, 3, 14> last_symbol                       = std::get<1>(GetParam());
  dmrs_additional_positions       additional_position               = std::get<2>(GetParam());
  bool                            lte_crs_match_around              = std::get<3>(GetParam());
  bool                            ue_capable_additional_dmrs_dl_alt = std::get<4>(GetParam());

  // Prepare configuration.
  pdsch_dmrs_symbol_mask_mapping_type_A_single_configuration config;
  config.typeA_pos                         = typeA_pos;
  config.additional_position               = additional_position;
  config.last_symbol                       = last_symbol;
  config.lte_crs_match_around              = lte_crs_match_around;
  config.ue_capable_additional_dmrs_dl_alt = ue_capable_additional_dmrs_dl_alt;

  // Get mask.
  dmrs_symbol_mask mask = pdsch_dmrs_symbol_mask_mapping_type_A_single_get(config);

  // Calculate expected mask.
  dmrs_typeA_position l0 = typeA_pos;
  unsigned            l1 =
      (config.lte_crs_match_around && config.ue_capable_additional_dmrs_dl_alt &&
       config.additional_position == ocudu::dmrs_additional_positions::pos1 && l0 == ocudu::dmrs_typeA_position::pos3)
                     ? 12
                     : 11;

  // Get expected symbol position list from table.
  ASSERT_TRUE(pdsch_dmrs_symbol_mask_mapping_type_A_single_table.count({last_symbol.value(), additional_position}));
  static_vector<unsigned, 4> symbol_pos_list =
      pdsch_dmrs_symbol_mask_mapping_type_A_single_table.at({last_symbol.value(), additional_position});

  // Convert list to mask.
  dmrs_symbol_mask expected(14);
  for (unsigned symbol_index : symbol_pos_list) {
    if (symbol_index == L0) {
      symbol_index = to_symbol_index(l0);
    }
    if (symbol_index == L1) {
      symbol_index = l1;
    }
    expected.set(symbol_index);
  }

  // Assert each mask with the expected.
  ASSERT_EQ(mask, expected);
}

INSTANTIATE_TEST_SUITE_P(PdschDmrsSymbolMaskTypeASingle,
                         PdschDmrsSymbolMaskFixture,
                         ::testing::Combine(::testing::Values(dmrs_typeA_position::pos2, dmrs_typeA_position::pos3),
                                            ::testing::Range(3U, 15U),
                                            ::testing::Values(dmrs_additional_positions::pos0,
                                                              dmrs_additional_positions::pos1,
                                                              dmrs_additional_positions::pos2,
                                                              dmrs_additional_positions::pos3),
                                            ::testing::Values(true, false),
                                            ::testing::Values(true, false)));

} // namespace

// Mapping type B, single-symbol DMRS, as per TS38.211 Table 7.4.1.1.2-3. The positions are relative to the first
// symbol of the allocation, which is what distinguishes this mapping from type A.
namespace {

struct type_B_expectation {
  unsigned                  duration;
  dmrs_additional_positions additional_position;
  std::vector<unsigned>     expected_relative_positions;
};

class pdsch_dmrs_symbol_mask_type_B_test : public ::testing::TestWithParam<type_B_expectation>
{};

} // namespace

TEST_P(pdsch_dmrs_symbol_mask_type_B_test, positions_are_relative_to_the_allocation_start)
{
  const type_B_expectation& param = GetParam();

  // The same duration is checked at several allocation starts: an implementation that anchored the positions to the
  // slot instead of to the allocation would pass at start 0 alone.
  for (unsigned start_symbol : {0U, 1U, 4U, 5U}) {
    if (start_symbol + param.duration > 14) {
      continue;
    }
    pdsch_dmrs_symbol_mask_mapping_type_B_single_configuration config{};
    config.duration            = param.duration;
    config.start_symbol        = start_symbol;
    config.additional_position = param.additional_position;

    dmrs_symbol_mask expected(14);
    for (unsigned pos : param.expected_relative_positions) {
      expected.set(start_symbol + pos);
    }

    ASSERT_EQ(expected, pdsch_dmrs_symbol_mask_mapping_type_B_single_get(config))
        << "duration=" << param.duration << " start=" << start_symbol;
  }
}

INSTANTIATE_TEST_SUITE_P(
    pdsch_dmrs_symbol_mask,
    pdsch_dmrs_symbol_mask_type_B_test,
    // Every cell of the mapping type B columns of TS38.211 Table 7.4.1.1.2-3, durations 2 to 13 symbols
    // (TS38.214 Table 5.1.2.1-1, normal cyclic prefix), dmrs-AdditionalPosition pos0 to pos3.
    ::testing::Values(type_B_expectation{2, dmrs_additional_positions::pos0, {0}},
                      type_B_expectation{2, dmrs_additional_positions::pos1, {0}},
                      type_B_expectation{2, dmrs_additional_positions::pos2, {0}},
                      type_B_expectation{2, dmrs_additional_positions::pos3, {0}},
                      type_B_expectation{3, dmrs_additional_positions::pos0, {0}},
                      type_B_expectation{3, dmrs_additional_positions::pos1, {0}},
                      type_B_expectation{3, dmrs_additional_positions::pos2, {0}},
                      type_B_expectation{3, dmrs_additional_positions::pos3, {0}},
                      type_B_expectation{4, dmrs_additional_positions::pos0, {0}},
                      type_B_expectation{4, dmrs_additional_positions::pos1, {0}},
                      type_B_expectation{4, dmrs_additional_positions::pos2, {0}},
                      type_B_expectation{4, dmrs_additional_positions::pos3, {0}},
                      type_B_expectation{5, dmrs_additional_positions::pos0, {0}},
                      type_B_expectation{5, dmrs_additional_positions::pos1, {0, 4}},
                      type_B_expectation{5, dmrs_additional_positions::pos2, {0, 4}},
                      type_B_expectation{5, dmrs_additional_positions::pos3, {0, 4}},
                      type_B_expectation{6, dmrs_additional_positions::pos0, {0}},
                      type_B_expectation{6, dmrs_additional_positions::pos1, {0, 4}},
                      type_B_expectation{6, dmrs_additional_positions::pos2, {0, 4}},
                      type_B_expectation{6, dmrs_additional_positions::pos3, {0, 4}},
                      type_B_expectation{7, dmrs_additional_positions::pos0, {0}},
                      type_B_expectation{7, dmrs_additional_positions::pos1, {0, 4}},
                      type_B_expectation{7, dmrs_additional_positions::pos2, {0, 4}},
                      type_B_expectation{7, dmrs_additional_positions::pos3, {0, 4}},
                      type_B_expectation{8, dmrs_additional_positions::pos0, {0}},
                      type_B_expectation{8, dmrs_additional_positions::pos1, {0, 6}},
                      type_B_expectation{8, dmrs_additional_positions::pos2, {0, 3, 6}},
                      type_B_expectation{8, dmrs_additional_positions::pos3, {0, 3, 6}},
                      type_B_expectation{9, dmrs_additional_positions::pos0, {0}},
                      type_B_expectation{9, dmrs_additional_positions::pos1, {0, 7}},
                      type_B_expectation{9, dmrs_additional_positions::pos2, {0, 4, 7}},
                      type_B_expectation{9, dmrs_additional_positions::pos3, {0, 4, 7}},
                      type_B_expectation{10, dmrs_additional_positions::pos0, {0}},
                      type_B_expectation{10, dmrs_additional_positions::pos1, {0, 7}},
                      type_B_expectation{10, dmrs_additional_positions::pos2, {0, 4, 7}},
                      type_B_expectation{10, dmrs_additional_positions::pos3, {0, 4, 7}},
                      type_B_expectation{11, dmrs_additional_positions::pos0, {0}},
                      type_B_expectation{11, dmrs_additional_positions::pos1, {0, 8}},
                      type_B_expectation{11, dmrs_additional_positions::pos2, {0, 4, 8}},
                      type_B_expectation{11, dmrs_additional_positions::pos3, {0, 3, 6, 9}},
                      type_B_expectation{12, dmrs_additional_positions::pos0, {0}},
                      type_B_expectation{12, dmrs_additional_positions::pos1, {0, 9}},
                      type_B_expectation{12, dmrs_additional_positions::pos2, {0, 5, 9}},
                      type_B_expectation{12, dmrs_additional_positions::pos3, {0, 3, 6, 9}},
                      type_B_expectation{13, dmrs_additional_positions::pos0, {0}},
                      type_B_expectation{13, dmrs_additional_positions::pos1, {0, 9}},
                      type_B_expectation{13, dmrs_additional_positions::pos2, {0, 5, 9}},
                      type_B_expectation{13, dmrs_additional_positions::pos3, {0, 3, 6, 9}}));
