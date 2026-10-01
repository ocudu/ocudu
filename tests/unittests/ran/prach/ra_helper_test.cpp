// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/adt/format.h"
#include "ocudu/ran/prach/ra_helper.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

/// Parameters of a PRACH occasion whose t_id is under test.
struct prach_occasion_slot_index_params {
  /// Subcarrier spacing the occasion slot is expressed in.
  subcarrier_spacing slot_scs;
  /// PRACH subcarrier spacing, i.e. the numerology t_id is counted in.
  subcarrier_spacing ra_scs;
  /// Preamble format.
  prach_format_type format;
  /// Slot the occasion starts in, within the system frame.
  unsigned occasion_slot_index;
  /// Expected t_id.
  unsigned expected_slot_index;
};

void PrintTo(const prach_occasion_slot_index_params& value, ::std::ostream* os)
{
  *os << fmt::format("slot_scs={} ra_scs={} format={} occasion_slot={}",
                     to_string(value.slot_scs),
                     to_string(value.ra_scs),
                     fmt::underlying(value.format),
                     value.occasion_slot_index);
}

class prach_occasion_slot_index_test : public ::testing::TestWithParam<prach_occasion_slot_index_params>
{};

} // namespace

/// \brief The t_id is the occasion's slot index in the system frame (TS 38.321 Section 5.1.3), counted in slots of the
/// numerology TS 38.211 Section 5.3.2 associates with the PRACH subcarrier spacing.
TEST_P(prach_occasion_slot_index_test, slot_index_is_counted_in_the_prach_numerology)
{
  const prach_occasion_slot_index_params& params = GetParam();
  const slot_point                        occasion_slot(params.slot_scs, 0, params.occasion_slot_index);

  ASSERT_EQ(params.expected_slot_index,
            ra_helper::get_prach_occasion_slot_index(occasion_slot, params.format, params.ra_scs));
}

INSTANTIATE_TEST_SUITE_P(
    prach_occasion_slot_index_test,
    prach_occasion_slot_index_test,
    ::testing::Values(
        // Long format at 15 kHz: t_id is the subframe index, which coincides with the slot index.
        prach_occasion_slot_index_params{subcarrier_spacing::kHz15,
                                         subcarrier_spacing::kHz15,
                                         prach_format_type::one,
                                         7,
                                         7},
        // Long format at 30 kHz: mu = 0, so t_id is the subframe index.
        prach_occasion_slot_index_params{subcarrier_spacing::kHz30,
                                         subcarrier_spacing::kHz30,
                                         prach_format_type::one,
                                         14,
                                         7},
        // Short format whose PRACH SCS matches the slot SCS: t_id is the slot index.
        prach_occasion_slot_index_params{subcarrier_spacing::kHz30,
                                         subcarrier_spacing::kHz30,
                                         prach_format_type::A1,
                                         14,
                                         14},
        // Short format whose PRACH SCS is coarser than the slot SCS: t_id counts the coarser slots.
        prach_occasion_slot_index_params{subcarrier_spacing::kHz30,
                                         subcarrier_spacing::kHz15,
                                         prach_format_type::A1,
                                         14,
                                         7},
        prach_occasion_slot_index_params{subcarrier_spacing::kHz60,
                                         subcarrier_spacing::kHz15,
                                         prach_format_type::A1,
                                         28,
                                         7},
        // Away from a subframe boundary. A coarser PRACH SCS maps every slot of a subframe onto the same t_id.
        prach_occasion_slot_index_params{subcarrier_spacing::kHz30,
                                         subcarrier_spacing::kHz30,
                                         prach_format_type::A1,
                                         15,
                                         15},
        prach_occasion_slot_index_params{subcarrier_spacing::kHz30,
                                         subcarrier_spacing::kHz15,
                                         prach_format_type::A1,
                                         15,
                                         7},
        prach_occasion_slot_index_params{subcarrier_spacing::kHz60,
                                         subcarrier_spacing::kHz15,
                                         prach_format_type::A1,
                                         31,
                                         7},
        prach_occasion_slot_index_params{subcarrier_spacing::kHz60,
                                         subcarrier_spacing::kHz30,
                                         prach_format_type::A1,
                                         31,
                                         15},
        prach_occasion_slot_index_params{subcarrier_spacing::kHz60,
                                         subcarrier_spacing::kHz60,
                                         prach_format_type::A1,
                                         31,
                                         31},
        // A long format always reports the subframe index, whatever the configured PRACH SCS.
        prach_occasion_slot_index_params{subcarrier_spacing::kHz60,
                                         subcarrier_spacing::kHz60,
                                         prach_format_type::one,
                                         31,
                                         7},
        // A long format ignores the PRACH SCS altogether, so it may be left unset.
        prach_occasion_slot_index_params{subcarrier_spacing::kHz30,
                                         subcarrier_spacing::invalid,
                                         prach_format_type::one,
                                         14,
                                         7}));

/// The t_id of the last slot of a system frame stays within the {0,...,79} range the RA-RNTI reserves for it.
TEST(prach_occasion_slot_index, highest_slot_index_of_a_system_frame_fits_in_the_ra_rnti_range)
{
  const slot_point last_slot(subcarrier_spacing::kHz120, 0, 79);

  ASSERT_EQ(79, ra_helper::get_prach_occasion_slot_index(last_slot, prach_format_type::A1, subcarrier_spacing::kHz120));
  ASSERT_TRUE(ra_helper::is_valid_ra_rnti(ra_helper::get_ra_rnti(79, 0, 0)));
}
