// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "tests/ocudu_test_requirements.h"
#include "ocudu/ran/meas_gap_config.h"
#include <gtest/gtest.h>

using namespace ocudu;

TEST(supported_meas_gap_patterns_test, default_only_supports_mandatory_patterns_0_and_1)
{
  OCUDU_TEST_REQUIREMENTS("DU-GEN-9");

  const supported_meas_gap_patterns default_patterns;

  // Gap patterns 0 and 1 are mandatory and always supported.
  EXPECT_TRUE(default_patterns.is_supported(0));
  EXPECT_TRUE(default_patterns.is_supported(1));
  // No other gap pattern is supported when no UE capabilities are available.
  for (unsigned pattern_id = 2; pattern_id != nof_meas_gap_patterns; ++pattern_id) {
    EXPECT_FALSE(default_patterns.is_supported(pattern_id)) << "pattern " << pattern_id;
  }
}

TEST(supported_meas_gap_patterns_test, mandatory_patterns_are_supported_even_if_not_marked)
{
  OCUDU_TEST_REQUIREMENTS("DU-GEN-9");

  // Even if patterns 0 and 1 are never explicitly marked, they must remain supported.
  supported_meas_gap_patterns patterns;
  patterns.mark_supported(4);

  EXPECT_TRUE(patterns.is_supported(0));
  EXPECT_TRUE(patterns.is_supported(1));
  EXPECT_TRUE(patterns.is_supported(4));
}

namespace {

/// Gap configuration used in the tests below: 6ms gap every 80ms, starting at subframe 10.
constexpr meas_gap_config test_gap{10, meas_gap_length::ms6, meas_gap_repetition_period::ms80};

/// Builds a 15kHz slot (1 slot == 1 subframe) whose position within the gap period is \c phase subframes after the
/// gap offset.
slot_point slot_at_gap_phase(unsigned phase, unsigned nof_periods = 70)
{
  const unsigned mgrp_ms  = static_cast<unsigned>(test_gap.mgrp);
  const unsigned slot_idx = nof_periods * mgrp_ms + test_gap.offset + phase;
  return slot_point{subcarrier_spacing::kHz15, slot_idx / 10, slot_idx % 10};
}

} // namespace

TEST(is_inside_meas_gap_test, gap_spans_exactly_mgl_slots)
{
  OCUDU_TEST_REQUIREMENTS("DU-GEN-9");

  // MGL is 6ms and there is one slot per subframe at 15kHz, so the gap covers the 6 slots at phases 0..5. Phase 6 is
  // the first slot after the gap.
  for (unsigned phase = 0; phase != 6; ++phase) {
    EXPECT_TRUE(is_inside_meas_gap(test_gap, slot_at_gap_phase(phase))) << "phase " << phase;
  }
  EXPECT_FALSE(is_inside_meas_gap(test_gap, slot_at_gap_phase(6)));
}

TEST(is_inside_ul_meas_gap_test, untracked_timing_advance_still_covers_the_trailing_slot)
{
  OCUDU_TEST_REQUIREMENTS("DU-GEN-9");

  // An untracked T_TA gets no leading guard, but keeps the trailing slot: a terrestrial UE is advanced by its own round
  // trip, which the caller does not report. So the window is MGL + 1 slots at phases 0..6, unshifted.
  constexpr std::optional<std::chrono::microseconds> no_ul_ta = std::nullopt;

  EXPECT_FALSE(is_inside_ul_meas_gap(test_gap, slot_at_gap_phase(79), no_ul_ta));
  for (unsigned phase = 0; phase != 7; ++phase) {
    EXPECT_TRUE(is_inside_ul_meas_gap(test_gap, slot_at_gap_phase(phase), no_ul_ta)) << "phase " << phase;
  }
  EXPECT_FALSE(is_inside_ul_meas_gap(test_gap, slot_at_gap_phase(7), no_ul_ta));
  EXPECT_FALSE(is_inside_ul_meas_gap(test_gap, slot_at_gap_phase(14), no_ul_ta));
}

TEST(is_inside_ul_meas_gap_test, ul_gap_is_shifted_by_the_ue_timing_advance)
{
  OCUDU_TEST_REQUIREMENTS("DU-GEN-9");

  // NTN cell with a feeder link: T_TA is ta-Common (7.3ms, feeder link round trip) plus the service link round trip
  // (7.3ms). The UE transmits uplink slot S at position S - T_TA on its downlink grid (TS 38.211, Section 4.3.1), so
  // the slots it cannot transmit in are those whose phase falls at T_TA past the gap offset, not at the gap offset.
  constexpr std::chrono::microseconds ul_ta{14600};

  // Phase 14 is where the UE actually drops the transmission.
  EXPECT_TRUE(is_inside_ul_meas_gap(test_gap, slot_at_gap_phase(14), ul_ta));
  // Phase 0 is where an unshifted check would wrongly withhold the grant: the UE can transmit there.
  EXPECT_FALSE(is_inside_ul_meas_gap(test_gap, slot_at_gap_phase(0), ul_ta));
}

TEST(is_inside_ul_meas_gap_test, ul_gap_is_guarded_at_each_edge)
{
  OCUDU_TEST_REQUIREMENTS("DU-GEN-9");

  // A guard slot at each edge on top of the slot the trailing edge gets for the truncation of T_TA, so the window spans
  // MGL + 3 slots: phases 13..21 for a T_TA of 14 slots.
  constexpr std::chrono::microseconds ul_ta{14600};

  EXPECT_FALSE(is_inside_ul_meas_gap(test_gap, slot_at_gap_phase(12), ul_ta));
  for (unsigned phase = 13; phase != 22; ++phase) {
    EXPECT_TRUE(is_inside_ul_meas_gap(test_gap, slot_at_gap_phase(phase), ul_ta)) << "phase " << phase;
  }
  EXPECT_FALSE(is_inside_ul_meas_gap(test_gap, slot_at_gap_phase(22), ul_ta));
}

TEST(is_inside_ul_meas_gap_test, the_guard_keeps_its_duration_when_the_cell_scs_grows)
{
  OCUDU_TEST_REQUIREMENTS("DU-GEN-9");

  // The 1ms guard spans 2 slots at 30kHz, not 1. T_TA is a whole number of milliseconds here, so that it truncates to
  // the same physical time on either grid.
  constexpr std::chrono::microseconds ul_ta{14000};
  constexpr unsigned                  slots_per_sf = 2;

  // The first slot of the subframe sitting \c phase_ms after the gap offset, on a 30kHz grid.
  auto slot_at_30khz_gap_phase_ms = [](unsigned phase_ms) {
    const unsigned slot_idx = (70 * static_cast<unsigned>(test_gap.mgrp) + test_gap.offset + phase_ms) * slots_per_sf;
    return slot_point{subcarrier_spacing::kHz30, slot_idx / (10 * slots_per_sf), slot_idx % (10 * slots_per_sf)};
  };

  // The same subframes are protected as at 15kHz. A guard of one 30kHz slot would leave phases 13 and 21 exposed.
  EXPECT_TRUE(is_inside_ul_meas_gap(test_gap, slot_at_30khz_gap_phase_ms(13), ul_ta)) << "leading guard";
  EXPECT_TRUE(is_inside_ul_meas_gap(test_gap, slot_at_30khz_gap_phase_ms(21), ul_ta)) << "trailing guard";
  // One subframe further out on each side is free again.
  EXPECT_FALSE(is_inside_ul_meas_gap(test_gap, slot_at_30khz_gap_phase_ms(12), ul_ta));
  EXPECT_FALSE(is_inside_ul_meas_gap(test_gap, slot_at_30khz_gap_phase_ms(22), ul_ta));
}

TEST(is_inside_ul_meas_gap_test, timing_advance_longer_than_the_gap_period_wraps)
{
  OCUDU_TEST_REQUIREMENTS("DU-GEN-9");

  // A GEO cell has a T_TA of several hundred milliseconds, well beyond one MGRP. Only T_TA modulo MGRP determines the
  // position of the window, so a large T_TA does not widen it beyond the usual MGL + 3 slots.
  constexpr std::chrono::microseconds geo_ul_ta{480000};

  // 480ms modulo 80ms is 0, so the uplink window lands back on the gap offset.
  EXPECT_TRUE(is_inside_ul_meas_gap(test_gap, slot_at_gap_phase(0), geo_ul_ta));
  EXPECT_FALSE(is_inside_ul_meas_gap(test_gap, slot_at_gap_phase(14), geo_ul_ta));
}
