// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "prach_configuration_test_data.h"
#include "ocudu/ran/prach/prach_configuration.h"
#include "ocudu/ran/prach/prach_helper.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace ocudu {

std::ostream& operator<<(std::ostream& os, test_case_t tc)
{
  fmt::print(os, "{} {} - index {}", to_string(tc.fr), (tc.dm == duplex_mode::FDD) ? "paired" : "unpaired", tc.index);
  return os;
}

} // namespace ocudu

class PrachConfiguration : public testing::TestWithParam<test_case_t>
{};

TEST_P(PrachConfiguration, PrachConfigTest)
{
  const test_case_t&  ref_conf       = GetParam();
  prach_configuration generated_conf = prach_configuration_get(ref_conf.fr, ref_conf.dm, ref_conf.index);

  prach_format_type ref_format = ref_conf.config.format;

  if (ref_format == prach_format_type::invalid) {
    // We should never get here.
    FAIL() << "Invalid reference PRACH configuration.";
  }

  if ((ref_format == prach_format_type::A1_B1) || (ref_format == prach_format_type::A2_B2) ||
      (ref_format == prach_format_type::A3_B3) || (ref_format == prach_format_type::B1)) {
    ASSERT_EQ(generated_conf.format, prach_format_type::invalid) << "This PRACH configuration should be unsupported.";
    return;
  }

  ASSERT_EQ(generated_conf.format, ref_format) << "PRACH format does not match.";
  ASSERT_EQ(generated_conf.x, ref_conf.config.x) << "SFN period 'x' does not match.";
  ASSERT_EQ(generated_conf.y, ref_conf.config.y) << "SFN offset 'y' does not match.";
  ASSERT_EQ(generated_conf.slots, ref_conf.config.slots) << "Number of subframes/slots does not match.";
  ASSERT_EQ(generated_conf.starting_symbol, ref_conf.config.starting_symbol) << "Starting symbol does not match.";
  ASSERT_EQ(generated_conf.nof_prach_slots_within_subframe, ref_conf.config.nof_prach_slots_within_subframe)
      << "Number of PRACH slots within a subframe/60-kHz slot does not match.";
  ASSERT_EQ(generated_conf.nof_occasions_within_slot, ref_conf.config.nof_occasions_within_slot)
      << "Number of PRACH occasions within a slot does not match.";
  ASSERT_EQ(generated_conf.duration, ref_conf.config.duration) << "PRACH duration does not match.";
}

INSTANTIATE_TEST_SUITE_P(PrachConfigCompare, PrachConfiguration, testing::ValuesIn(prach_configuration_test_data));

/// Number of PRACH configuration indexes, as per TS 38.331 (prach-ConfigurationIndex).
static constexpr uint16_t NOF_PRACH_CONFIG_INDEXES = 256;

// FR2-NTN bands are the only FR2 bands with paired spectrum. Table 6.3.3.2-4, TS 38.211, applies to them as it does to
// unpaired FR2 spectrum.
TEST(PrachConfigurationFr2, PairedSpectrumUsesTheUnpairedTable)
{
  for (uint16_t index = 0; index != NOF_PRACH_CONFIG_INDEXES; ++index) {
    const prach_configuration paired   = prach_configuration_get(frequency_range::FR2, duplex_mode::FDD, index);
    const prach_configuration unpaired = prach_configuration_get(frequency_range::FR2, duplex_mode::TDD, index);
    ASSERT_EQ(paired.format, unpaired.format) << "PRACH format does not match for index " << index;
    ASSERT_EQ(paired.x, unpaired.x) << "SFN period 'x' does not match for index " << index;
    ASSERT_EQ(paired.y, unpaired.y) << "SFN offset 'y' does not match for index " << index;
    ASSERT_EQ(paired.slots, unpaired.slots) << "Slots do not match for index " << index;
    ASSERT_EQ(paired.starting_symbol, unpaired.starting_symbol) << "Starting symbol does not match for index " << index;
    ASSERT_EQ(paired.nof_prach_slots_within_subframe, unpaired.nof_prach_slots_within_subframe)
        << "Number of PRACH slots does not match for index " << index;
    ASSERT_EQ(paired.nof_occasions_within_slot, unpaired.nof_occasions_within_slot)
        << "Number of PRACH occasions does not match for index " << index;
    ASSERT_EQ(paired.duration, unpaired.duration) << "PRACH duration does not match for index " << index;
  }
}

TEST(PrachConfigurationFr2, ValidatorAcceptsTheSameIndexesForPairedAndUnpairedSpectrum)
{
  for (uint16_t index = 0; index != NOF_PRACH_CONFIG_INDEXES; ++index) {
    const bool paired_valid =
        prach_helper::prach_config_index_is_valid(index, frequency_range::FR2, duplex_mode::FDD).has_value();
    const bool unpaired_valid =
        prach_helper::prach_config_index_is_valid(index, frequency_range::FR2, duplex_mode::TDD).has_value();
    ASSERT_EQ(paired_valid, unpaired_valid) << "Validity differs for index " << index;
  }
}
