// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/du/du_high/du_manager/ran_resource_management/du_meas_config_manager.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/ran/ssb/ssb_properties.h"
#include "fmt/format.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;
using namespace asn1::rrc_nr;

namespace {

using smtc_duration = ssb_mtc_s::dur_opts::options;
using offset_range  = std::pair<uint8_t, uint8_t>;

struct meas_gap_test_params {
  subcarrier_spacing         pcell_scs;
  ssb_periodicity            smtc_period;
  offset_range               smtc_offsets; // half-open offset range [first, second)
  smtc_duration              smtc_dur;
  meas_gap_length            expected_mgl;
  meas_gap_repetition_period expected_mgrp;
};

void PrintTo(const meas_gap_test_params& p, std::ostream* os)
{
  *os << "scs=" << scs_to_khz(p.pcell_scs) << "kHz period=" << static_cast<unsigned>(p.smtc_period)
      << "sf off=" << +p.smtc_offsets.first << "_" << +p.smtc_offsets.second
      << " dur=" << (static_cast<unsigned>(p.smtc_dur) + 1) << "sf"
      << " mgl=" << meas_gap_length_to_msec(p.expected_mgl) << "ms mgrp=" << static_cast<unsigned>(p.expected_mgrp)
      << "ms";
}

ssb_mtc_s make_smtc(ssb_periodicity period, uint8_t offset, smtc_duration dur)
{
  ssb_mtc_s smtc;
  smtc.dur.value = dur;
  switch (period) {
    case ssb_periodicity::ms5:
      smtc.periodicity_and_offset.set_sf5() = offset;
      break;
    case ssb_periodicity::ms10:
      smtc.periodicity_and_offset.set_sf10() = offset;
      break;
    case ssb_periodicity::ms20:
      smtc.periodicity_and_offset.set_sf20() = offset;
      break;
    case ssb_periodicity::ms40:
      smtc.periodicity_and_offset.set_sf40() = offset;
      break;
    case ssb_periodicity::ms80:
      smtc.periodicity_and_offset.set_sf80() = offset;
      break;
    case ssb_periodicity::ms160:
      smtc.periodicity_and_offset.set_sf160() = offset;
      break;
  }
  return smtc;
}

std::string param_name(const ::testing::TestParamInfo<meas_gap_test_params>& info)
{
  const auto& p = info.param;
  return fmt::format("scs_{:_>3}kHz_period_{:_>3}sf_offsets_{:_>3}_to_{:_>3}_duration_{:01}sf",
                     scs_to_khz(p.pcell_scs),
                     fmt::underlying(p.smtc_period),
                     p.smtc_offsets.first,
                     p.smtc_offsets.second,
                     static_cast<unsigned>(p.smtc_dur) + 1);
}

class du_meas_config_manager_create_meas_gap_test : public ::testing::TestWithParam<meas_gap_test_params>
{};

TEST_P(du_meas_config_manager_create_meas_gap_test, gap_matches_expected_mgl_mgrp_and_offset)
{
  const meas_gap_test_params& p = GetParam();
  for (uint8_t off = p.smtc_offsets.first; off < p.smtc_offsets.second; ++off) {
    SCOPED_TRACE(fmt::format("smtc_offset={}", off));
    const ssb_mtc_s       smtc = make_smtc(p.smtc_period, off, p.smtc_dur);
    const meas_gap_config gap  = create_meas_gap(p.pcell_scs, smtc, {});

    EXPECT_EQ(gap.offset, off);
    EXPECT_EQ(gap.mgl, p.expected_mgl);
    EXPECT_EQ(gap.mgrp, p.expected_mgrp);
  }
}

// SMTC Duration to Measurement Gap Length test
INSTANTIATE_TEST_SUITE_P(duration_to_mgl,
                         du_meas_config_manager_create_meas_gap_test,
                         ::testing::Values(
                             // sf1 + 15 kHz -> ms3.
                             meas_gap_test_params{subcarrier_spacing::kHz15,
                                                  ssb_periodicity::ms20,
                                                  {0, 20},
                                                  smtc_duration::sf1,
                                                  meas_gap_length::ms3,
                                                  meas_gap_repetition_period::ms20},
                             // sf1 + 30 kHz -> ms1.5.
                             meas_gap_test_params{subcarrier_spacing::kHz30,
                                                  ssb_periodicity::ms20,
                                                  {0, 20},
                                                  smtc_duration::sf1,
                                                  meas_gap_length::ms1dot5,
                                                  meas_gap_repetition_period::ms20},
                             // sf1 + 60 kHz -> ms1.5.
                             meas_gap_test_params{subcarrier_spacing::kHz60,
                                                  ssb_periodicity::ms20,
                                                  {0, 20},
                                                  smtc_duration::sf1,
                                                  meas_gap_length::ms1dot5,
                                                  meas_gap_repetition_period::ms20},
                             // sf2 -> ms3.
                             meas_gap_test_params{subcarrier_spacing::kHz30,
                                                  ssb_periodicity::ms20,
                                                  {0, 20},
                                                  smtc_duration::sf2,
                                                  meas_gap_length::ms3,
                                                  meas_gap_repetition_period::ms20},
                             // sf3 -> ms4.
                             meas_gap_test_params{subcarrier_spacing::kHz30,
                                                  ssb_periodicity::ms20,
                                                  {0, 20},
                                                  smtc_duration::sf3,
                                                  meas_gap_length::ms4,
                                                  meas_gap_repetition_period::ms20},
                             // sf4 -> ms6.
                             meas_gap_test_params{subcarrier_spacing::kHz30,
                                                  ssb_periodicity::ms20,
                                                  {0, 20},
                                                  smtc_duration::sf4,
                                                  meas_gap_length::ms6,
                                                  meas_gap_repetition_period::ms20},
                             // sf5 -> ms6.
                             meas_gap_test_params{subcarrier_spacing::kHz30,
                                                  ssb_periodicity::ms20,
                                                  {0, 20},
                                                  smtc_duration::sf5,
                                                  meas_gap_length::ms6,
                                                  meas_gap_repetition_period::ms20}),
                         param_name);

// SMTC periodicity to Measurement Gap Repetition Period test
INSTANTIATE_TEST_SUITE_P(periodicity_to_mgrp,
                         du_meas_config_manager_create_meas_gap_test,
                         ::testing::Values(meas_gap_test_params{subcarrier_spacing::kHz30,
                                                                ssb_periodicity::ms5,
                                                                {0, 5},
                                                                smtc_duration::sf5,
                                                                meas_gap_length::ms6,
                                                                meas_gap_repetition_period::ms20},
                                           meas_gap_test_params{subcarrier_spacing::kHz30,
                                                                ssb_periodicity::ms10,
                                                                {0, 10},
                                                                smtc_duration::sf5,
                                                                meas_gap_length::ms6,
                                                                meas_gap_repetition_period::ms20},
                                           meas_gap_test_params{subcarrier_spacing::kHz30,
                                                                ssb_periodicity::ms20,
                                                                {0, 20},
                                                                smtc_duration::sf5,
                                                                meas_gap_length::ms6,
                                                                meas_gap_repetition_period::ms20},
                                           meas_gap_test_params{subcarrier_spacing::kHz30,
                                                                ssb_periodicity::ms40,
                                                                {0, 40},
                                                                smtc_duration::sf5,
                                                                meas_gap_length::ms6,
                                                                meas_gap_repetition_period::ms40},
                                           meas_gap_test_params{subcarrier_spacing::kHz30,
                                                                ssb_periodicity::ms80,
                                                                {0, 80},
                                                                smtc_duration::sf5,
                                                                meas_gap_length::ms6,
                                                                meas_gap_repetition_period::ms80},
                                           meas_gap_test_params{subcarrier_spacing::kHz30,
                                                                ssb_periodicity::ms160,
                                                                {0, 160},
                                                                smtc_duration::sf5,
                                                                meas_gap_length::ms6,
                                                                meas_gap_repetition_period::ms160}),
                         param_name);

// ---------- Collision avoidance scenarios ----------

struct collision_params {
  const char*                      tag;
  subcarrier_spacing               pcell_scs;
  ssb_periodicity                  smtc_period;
  uint8_t                          smtc_offset;
  smtc_duration                    smtc_dur;
  std::vector<periodic_uci_config> ul_occasions;
  meas_gap_config                  expected;
};

void PrintTo(const collision_params& p, std::ostream* os)
{
  *os << p.tag << " scs=" << scs_to_khz(p.pcell_scs) << "kHz period=" << static_cast<unsigned>(p.smtc_period)
      << "sf offset=" << +p.smtc_offset << " dur=" << (static_cast<unsigned>(p.smtc_dur) + 1)
      << "sf expected={offset=" << p.expected.offset << " mgl=" << meas_gap_length_to_msec(p.expected.mgl)
      << "ms mgrp=" << static_cast<unsigned>(p.expected.mgrp) << "ms}";
}

class du_meas_config_manager_collision_test : public ::testing::TestWithParam<collision_params>
{};

TEST_P(du_meas_config_manager_collision_test, gap_avoids_or_minimises_collisions)
{
  const auto&           p    = GetParam();
  const ssb_mtc_s       smtc = make_smtc(p.smtc_period, p.smtc_offset, p.smtc_dur);
  const meas_gap_config gap  = create_meas_gap(p.pcell_scs, smtc, p.ul_occasions);

  EXPECT_EQ(gap.offset, p.expected.offset);
  EXPECT_EQ(gap.mgl, p.expected.mgl);
  EXPECT_EQ(gap.mgrp, p.expected.mgrp);
}

// At 30 kHz SCS: 2 slots per ms. SR/CSI period and offset are in PCell slots.
INSTANTIATE_TEST_SUITE_P(
    collision_avoidance,
    du_meas_config_manager_collision_test,
    ::testing::Values(
        // No SR/CSI configured. Gap is placed straight at the SMTC offset.
        //
        //      ms | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|10|11|12|13|14|15|16|17|18|19|
        //   SSB   | S| S| S| S| S|  |  |  |  |  | S| S| S| S| S|  |  |  |  |  |
        //   gap@0 | G| G| G| G| G| G|  |  |  |  |  |  |  |  |  |  |  |  |  |  |  OK
        collision_params{"no_ul_occasion_collisions",
                         subcarrier_spacing::kHz30,
                         ssb_periodicity::ms10,
                         0,
                         smtc_duration::sf5,
                         {},
                         meas_gap_config{0, meas_gap_length::ms6, meas_gap_repetition_period::ms20}},
        // SR present but its only instance per MGRP lives outside the gap window.
        // SR period 40 slots (20 ms), offset 20 slots (10 ms).
        //
        //      ms | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|10|11|12|13|14|15|16|17|18|19|
        //   SSB   | S| S| S| S| S|  |  |  |  |  | S| S| S| S| S|  |  |  |  |  |
        //   SR    |  |  |  |  |  |  |  |  |  |  | R|  |  |  |  |  |  |  |  |  |
        //   gap@0 | G| G| G| G| G| G|  |  |  |  |  |  |  |  |  |  |  |  |  |  |  OK
        collision_params{"sr_no_collision",
                         subcarrier_spacing::kHz30,
                         ssb_periodicity::ms10,
                         0,
                         smtc_duration::sf5,
                         {{40, 20}},
                         meas_gap_config{0, meas_gap_length::ms6, meas_gap_repetition_period::ms20}},
        // SR collides at gap_offset=0; algorithm shifts to the next SMTC instance at ms=10.
        // SR period 20 ms, offset 0.
        //
        //      ms  | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|10|11|12|13|14|15|16|17|18|19|
        //   SSB    | S| S| S| S| S|  |  |  |  |  | S| S| S| S| S|  |  |  |  |  |
        //   SR     | R|  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
        //   gap@0  | G| G| G| G| G| G|  |  |  |  |  |  |  |  |  |  |  |  |  |  |  SR in gap
        //   gap@10 |  |  |  |  |  |  |  |  |  |  | G| G| G| G| G| G|  |  |  |  |  OK
        collision_params{"sr_collision_avoided_by_offset_shift",
                         subcarrier_spacing::kHz30,
                         ssb_periodicity::ms10,
                         0,
                         smtc_duration::sf5,
                         {{40, 0}},
                         meas_gap_config{10, meas_gap_length::ms6, meas_gap_repetition_period::ms20}},
        // CSI collides at gap_offset=0; algorithm shifts to gap_offset=10.
        // CSI period 20 ms, offset 4 slots (= 2 ms).
        //
        //      ms  | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|10|11|12|13|14|15|16|17|18|19|
        //   SSB    | S| S| S| S| S|  |  |  |  |  | S| S| S| S| S|  |  |  |  |  |
        //   CSI    |  |  | C|  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
        //   gap@0  | G| G| G| G| G| G|  |  |  |  |  |  |  |  |  |  |  |  |  |  |  CSI in gap
        //   gap@10 |  |  |  |  |  |  |  |  |  |  | G| G| G| G| G| G|  |  |  |  |  OK
        collision_params{"csi_collision_avoided_by_offset_shift",
                         subcarrier_spacing::kHz30,
                         ssb_periodicity::ms10,
                         0,
                         smtc_duration::sf5,
                         {{40, 4}},
                         meas_gap_config{10, meas_gap_length::ms6, meas_gap_repetition_period::ms20}},
        // Combined SR + CSI: SR at ms 0 forces shift; CSI at slot 34 (= 17 ms) doesn't overlap gap@10.
        //
        //      ms  | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|10|11|12|13|14|15|16|17|18|19|
        //   SSB    | S| S| S| S| S|  |  |  |  |  | S| S| S| S| S|  |  |  |  |  |
        //   SR     | R|  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
        //   CSI    |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  | C|  |  |
        //   gap@0  | G| G| G| G| G| G|  |  |  |  |  |  |  |  |  |  |  |  |  |  |  SR in gap
        //   gap@10 |  |  |  |  |  |  |  |  |  |  | G| G| G| G| G| G|  |  |  |  |  OK
        collision_params{"sr_and_csi_avoided_together",
                         subcarrier_spacing::kHz30,
                         ssb_periodicity::ms10,
                         0,
                         smtc_duration::sf5,
                         {{40, 0}, {40, 34}},
                         meas_gap_config{10, meas_gap_length::ms6, meas_gap_repetition_period::ms20}},
        // Strict at MGRP=20 fails: SMTC period = MGRP gives only one candidate gap offset (0 ms),
        // and the only SR instance per cycle lands right inside it. Doubling MGRP to 40 yields two
        // SR instances per cycle — gap@0 still catches the first, but the second at ms 20 stays
        // outside the gap, so loose accepts (every-other-SR works).
        //
        // MGRP=20 (rejected: only SR fully blocked):
        //      ms  | 0| 1| 2| 3| 4| 5| 6|...                            |19|
        //   SSB    | S| S| S| S| S|  |  |...                            |  |
        //   SR     | R|  |  |  |  |  |  |...                            |  |
        //   gap@0  | G| G| G| G| G| G|  |...                            |  |  only SR fully in gap
        //
        // MGRP=40 (accepted by loose: one in, one out):
        //      ms  | 0| 1| 2| 3| 4| 5| 6|...|19|20|21|...               |39|
        //   SSB    | S| S| S| S| S|  |  |...|  | S| S|...               |  |
        //   SR     | R|  |  |  |  |  |  |...|  | R|  |...               |  |
        //   gap@0  | G| G| G| G| G| G|  |...|  |  |  |...               |  |  OK, SR@20 stays out
        collision_params{"loose_check_with_doubling_required_for_sr",
                         subcarrier_spacing::kHz30,
                         ssb_periodicity::ms20,
                         0,
                         smtc_duration::sf5,
                         {{40, 0}},
                         meas_gap_config{0, meas_gap_length::ms6, meas_gap_repetition_period::ms40}},
        // SR period 40 ms forces min MGRP >= 40, even though SMTC period 10 ms would allow MGRP=20.
        // At MGRP=40, gap@0 already avoids SR which lives at ms 15.
        //
        //      ms  | 0| 1| 2| 3| 4| 5| 6|...|14|15|16|...|29|30|...     |39|
        //   SSB    | S| S| S| S| S|  |  |...|  |  |  |...|  | S|...     |  |   (SMTC at 0, 10, 20, 30)
        //   SR     |  |  |  |  |  |  |  |...|  | R|  |...|  |  |...     |  |   (single SR, period 40)
        //   gap@0  | G| G| G| G| G| G|  |...|  |  |  |...|  |  |...     |  |   OK
        collision_params{"min_mgrp_raised_by_sr_period",
                         subcarrier_spacing::kHz30,
                         ssb_periodicity::ms10,
                         0,
                         smtc_duration::sf5,
                         {{80, 30}},
                         meas_gap_config{0, meas_gap_length::ms6, meas_gap_repetition_period::ms40}},
        // SMTC period 20 = MGRP_min, only one SMTC alignment in MGRP.
        // SR at slot 11 sits at ms 5.5 — right at the trailing edge of gap[0, 6). MGL=6 vs SMTC duration=5 leaves
        // 1 ms of left-shift slack: gap_offset=19 wraps the gap to [19, 20) ∪ [0, 5) which still
        // encloses SMTC[0, 5) ms but excludes SR at ms 5.5.
        //
        //      ms   | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|10|11|12|13|14|15|16|17|18|19|
        //   SSB     | S| S| S| S| S|  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
        //   SR      |  |  |  |  |  |.R|  |  |  |  |  |  |  |  |  |  |  |  |  |  |  ← SR at ms 5.5
        //   gap@0   | G| G| G| G| G| G|  |  |  |  |  |  |  |  |  |  |  |  |  |  |  SR in gap
        //   gap@19  | G| G| G| G| G|  |  |  |  |  |  |  |  |  |  |  |  |  |  | G|  wraparound; SR out
        collision_params{"offset_shift_by_slack_between_mgl_and_ssb_duration",
                         subcarrier_spacing::kHz30,
                         ssb_periodicity::ms20,
                         0,
                         smtc_duration::sf5,
                         {{40, 11}},
                         meas_gap_config{19, meas_gap_length::ms6, meas_gap_repetition_period::ms20}}),
    [](const ::testing::TestParamInfo<collision_params>& test_info) { return std::string{test_info.param.tag}; });

} // namespace
