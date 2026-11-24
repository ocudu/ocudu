// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/mac/mac_sched/cg_dtx_detector.h"
#include <gtest/gtest.h>
#include <limits>

using namespace ocudu;

namespace {

class cg_dtx_detector_test : public ::testing::Test
{
protected:
  static constexpr float SINR_THRES_DB = -8.0F;

  const du_ue_index_t ue_index = to_du_ue_index(3);
  const rnti_t        crnti    = to_rnti(0x4601);
  const rnti_t        cs_rnti  = to_rnti(0x4602);

  cg_dtx_detector detector{SINR_THRES_DB};
};

} // namespace

TEST_F(cg_dtx_detector_test, when_ue_has_no_cs_rnti_then_no_crc_is_dtx)
{
  ASSERT_FALSE(detector.is_dtx(ue_index, cs_rnti, false, -30.0F));
  ASSERT_FALSE(detector.is_dtx(ue_index, crnti, false, -30.0F));
}

TEST_F(cg_dtx_detector_test, when_crc_ko_on_cs_rnti_has_sinr_below_threshold_then_it_is_dtx)
{
  detector.set_cs_rnti(ue_index, cs_rnti);

  ASSERT_TRUE(detector.is_dtx(ue_index, cs_rnti, false, SINR_THRES_DB - 0.1F));
  ASSERT_TRUE(detector.is_dtx(ue_index, cs_rnti, false, -30.0F));
}

TEST_F(cg_dtx_detector_test, when_crc_ko_on_cs_rnti_has_non_finite_sinr_then_it_is_dtx)
{
  detector.set_cs_rnti(ue_index, cs_rnti);

  ASSERT_TRUE(detector.is_dtx(ue_index, cs_rnti, false, -std::numeric_limits<float>::infinity()));
  ASSERT_TRUE(detector.is_dtx(ue_index, cs_rnti, false, std::numeric_limits<float>::quiet_NaN()));
}

TEST_F(cg_dtx_detector_test, when_crc_ko_on_cs_rnti_has_sinr_at_or_above_threshold_then_it_is_not_dtx)
{
  detector.set_cs_rnti(ue_index, cs_rnti);

  ASSERT_FALSE(detector.is_dtx(ue_index, cs_rnti, false, SINR_THRES_DB));
  ASSERT_FALSE(detector.is_dtx(ue_index, cs_rnti, false, 5.0F));
}

TEST_F(cg_dtx_detector_test, when_crc_ko_on_cs_rnti_has_no_sinr_then_it_is_not_dtx)
{
  detector.set_cs_rnti(ue_index, cs_rnti);

  ASSERT_FALSE(detector.is_dtx(ue_index, cs_rnti, false, std::nullopt));
}

TEST_F(cg_dtx_detector_test, when_crc_on_cs_rnti_is_ok_then_it_is_not_dtx)
{
  detector.set_cs_rnti(ue_index, cs_rnti);

  ASSERT_FALSE(detector.is_dtx(ue_index, cs_rnti, true, -30.0F));
}

TEST_F(cg_dtx_detector_test, when_crc_ko_is_on_c_rnti_then_it_is_not_dtx)
{
  detector.set_cs_rnti(ue_index, cs_rnti);

  // A dynamic grant UE must transmit, so a low SINR CRC KO is a radio link problem.
  ASSERT_FALSE(detector.is_dtx(ue_index, crnti, false, -30.0F));
}

TEST_F(cg_dtx_detector_test, when_cs_rnti_belongs_to_another_ue_then_crc_ko_is_not_dtx)
{
  detector.set_cs_rnti(ue_index, cs_rnti);

  ASSERT_FALSE(detector.is_dtx(to_du_ue_index(4), cs_rnti, false, -30.0F));
}

TEST_F(cg_dtx_detector_test, when_cs_rnti_is_cleared_then_crc_ko_is_no_longer_dtx)
{
  detector.set_cs_rnti(ue_index, cs_rnti);
  ASSERT_TRUE(detector.is_dtx(ue_index, cs_rnti, false, -30.0F));

  detector.set_cs_rnti(ue_index, rnti_t::INVALID_RNTI);
  ASSERT_FALSE(detector.is_dtx(ue_index, cs_rnti, false, -30.0F));
}

TEST_F(cg_dtx_detector_test, when_cs_rnti_changes_then_only_the_new_one_is_detected)
{
  const rnti_t new_cs_rnti = to_rnti(0x4610);
  detector.set_cs_rnti(ue_index, cs_rnti);
  detector.set_cs_rnti(ue_index, new_cs_rnti);

  ASSERT_FALSE(detector.is_dtx(ue_index, cs_rnti, false, -30.0F));
  ASSERT_TRUE(detector.is_dtx(ue_index, new_cs_rnti, false, -30.0F));
}
