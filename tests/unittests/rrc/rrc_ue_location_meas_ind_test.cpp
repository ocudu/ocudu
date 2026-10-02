// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rrc_ue_test_helpers.h"
#include "tests/ocudu_test_requirements.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Covers the reception of the LocationMeasurementIndication, TS 38.331 section 5.5.6.
class rrc_ue_location_meas_ind : public rrc_ue_test_helper, public ::testing::Test
{
protected:
  static void SetUpTestSuite() { ocudulog::init(); }

  void SetUp() override
  {
    init();
    receive_setup_request();
    receive_setup_complete();
    ASSERT_TRUE(init_security_context());
  }

  void TearDown() override { ocudulog::flush(); }

  /// The LocationMeasurementInfo with one NR PRS measurement.
  static asn1::rrc_nr::location_meas_info_c make_nr_prs_location_meas_info()
  {
    asn1::rrc_nr::nr_prs_meas_info_r16_s prs_info;
    prs_info.dl_prs_point_a_r16                               = 643296;
    prs_info.nr_meas_prs_repeat_and_offset_r16.set_ms40_r16() = 7;
    prs_info.nr_meas_prs_len_r16.value = asn1::rrc_nr::nr_prs_meas_info_r16_s::nr_meas_prs_len_r16_opts::ms6;

    asn1::rrc_nr::location_meas_info_c info;
    info.set_nr_prs_meas_r16().push_back(prs_info);
    return info;
  }

  /// The packed LocationMeasurementInfo with one NR PRS measurement.
  static byte_buffer make_packed_nr_prs_location_meas_info()
  {
    byte_buffer   pdu;
    asn1::bit_ref bref{pdu};
    EXPECT_EQ(make_nr_prs_location_meas_info().pack(bref), asn1::OCUDUASN_SUCCESS);
    return pdu;
  }

  /// Feeds a LocationMeasurementIndication back. It indicates a start with \c info, or a stop without it.
  void receive_location_meas_ind(const std::optional<asn1::rrc_nr::location_meas_info_c>& info,
                                 bool                                                     integrity_verified = true)
  {
    asn1::rrc_nr::ul_dcch_msg_s            ul_dcch_msg;
    asn1::rrc_nr::location_meas_ind_ies_s& ies =
        ul_dcch_msg.msg.set_c1().set_location_meas_ind().crit_exts.set_location_meas_ind();
    if (info.has_value()) {
      ies.meas_ind.set_setup() = info.value();
    } else {
      ies.meas_ind.set_release();
    }

    byte_buffer   pdu;
    asn1::bit_ref bref{pdu};
    ASSERT_EQ(ul_dcch_msg.pack(bref), asn1::OCUDUASN_SUCCESS);

    rrc_ue->handle_ul_dcch_pdu(srb_id_t::srb1, std::move(pdu), integrity_verified);
  }
};

TEST_F(rrc_ue_location_meas_ind, when_location_measurements_start_then_location_meas_info_is_forwarded_to_cu_cp)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-3-b");

  receive_location_meas_ind(make_nr_prs_location_meas_info());

  // The CU-CP receives the LocationMeasurementInfo of the UE unchanged.
  ASSERT_TRUE(rrc_ue_cu_cp_notifier.last_location_meas_info.has_value());
  EXPECT_EQ(rrc_ue_cu_cp_notifier.last_location_meas_info.value(), make_packed_nr_prs_location_meas_info());
}

TEST_F(rrc_ue_location_meas_ind, when_location_measurements_stop_then_nothing_is_forwarded_to_cu_cp)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-3-b");

  receive_location_meas_ind(std::nullopt);

  EXPECT_FALSE(rrc_ue_cu_cp_notifier.last_location_meas_info.has_value());
  EXPECT_EQ(rrc_ue_cu_cp_notifier.last_cu_cp_ue_context_release_request.ue_index, cu_cp_ue_index_t::invalid);
}

TEST_F(rrc_ue_location_meas_ind, when_location_meas_ind_is_not_integrity_protected_then_ue_is_released)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-3-b");

  receive_location_meas_ind(make_nr_prs_location_meas_info(), /* integrity_verified */ false);

  EXPECT_FALSE(rrc_ue_cu_cp_notifier.last_location_meas_info.has_value());
  EXPECT_NE(rrc_ue_cu_cp_notifier.last_cu_cp_ue_context_release_request.ue_index, cu_cp_ue_index_t::invalid);
}
