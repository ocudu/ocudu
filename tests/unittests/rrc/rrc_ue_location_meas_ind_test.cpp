// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rrc_ue_test_helpers.h"
#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/rrc/rrc_test_messages.h"
#include "ocudu/support/async/async_test_utils.h"
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

  /// The packed LocationMeasurementInfo with one NR PRS measurement.
  static byte_buffer make_packed_nr_prs_location_meas_info()
  {
    byte_buffer   pdu;
    asn1::bit_ref bref{pdu};
    EXPECT_EQ(asn1::pack(bref, test_helpers::create_nr_prs_location_meas_info()), asn1::OCUDUASN_SUCCESS);
    return pdu;
  }

  /// Feeds a LocationMeasurementIndication back. It indicates a start with \c info, or a stop without it.
  void receive_location_meas_ind(const std::optional<asn1::rrc_nr::location_meas_info_c>& info,
                                 bool                                                     integrity_verified = true)
  {
    rrc_ue->handle_ul_dcch_pdu(srb_id_t::srb1,
                               test_helpers::pack_ul_dcch_msg(test_helpers::create_location_meas_ind(info)),
                               integrity_verified);
  }
};

TEST_F(rrc_ue_location_meas_ind, when_location_measurements_start_then_cu_cp_is_notified_and_info_is_stored)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-3-b");

  receive_location_meas_ind(test_helpers::create_nr_prs_location_meas_info());

  // The RRC UE keeps the LocationMeasurementInfo of the UE unchanged.
  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_location_meas_indications, 1);
  std::optional<byte_buffer> stored_info = rrc_ue->get_packed_location_meas_info();
  ASSERT_TRUE(stored_info.has_value());
  EXPECT_EQ(stored_info.value(), make_packed_nr_prs_location_meas_info());
}

TEST_F(rrc_ue_location_meas_ind, when_location_measurements_stop_then_cu_cp_is_notified_and_info_is_removed)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-3-b");

  receive_location_meas_ind(test_helpers::create_nr_prs_location_meas_info());
  receive_location_meas_ind(std::nullopt);

  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_location_meas_indications, 2);
  EXPECT_FALSE(rrc_ue->get_packed_location_meas_info().has_value());
  EXPECT_EQ(rrc_ue_cu_cp_notifier.last_cu_cp_ue_context_release_request.ue_index, cu_cp_ue_index_t::invalid);
}

TEST_F(rrc_ue_location_meas_ind, when_ue_without_location_measurements_stops_them_then_cu_cp_is_not_notified)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-3-b");

  receive_location_meas_ind(std::nullopt);

  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_location_meas_indications, 0);
  EXPECT_EQ(rrc_ue_cu_cp_notifier.last_cu_cp_ue_context_release_request.ue_index, cu_cp_ue_index_t::invalid);
}

TEST_F(rrc_ue_location_meas_ind, when_location_meas_ind_is_not_integrity_protected_then_ue_is_released)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-3-b");

  receive_location_meas_ind(test_helpers::create_nr_prs_location_meas_info(), /* integrity_verified */ false);

  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_location_meas_indications, 0);
  EXPECT_FALSE(rrc_ue->get_packed_location_meas_info().has_value());
  EXPECT_NE(rrc_ue_cu_cp_notifier.last_cu_cp_ue_context_release_request.ue_index, cu_cp_ue_index_t::invalid);
}

TEST_F(rrc_ue_location_meas_ind, when_ue_moves_to_another_rrc_ue_then_ongoing_location_measurements_move_with_it)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-POS-16-3-b");

  // The transfer context needs the UE capabilities for the HandoverPreparationInformation.
  enable_security();
  rrc_ue_capability_transfer_request cap_request;
  async_task<bool> t = get_rrc_ue_control_message_handler()->handle_rrc_ue_capability_transfer_request(cap_request);
  lazy_task_launcher<bool> t_launcher(t);
  receive_ue_capability_information(2);
  ASSERT_TRUE(t.ready());

  receive_location_meas_ind(test_helpers::create_nr_prs_location_meas_info());

  // The source RRC UE puts the LocationMeasurementInfo into the transfer context.
  rrc_ue_transfer_context transfer_context = rrc_ue->get_transfer_context();
  ASSERT_TRUE(transfer_context.location_meas_info.has_value());
  EXPECT_EQ(transfer_context.location_meas_info.value(), make_packed_nr_prs_location_meas_info());

  // The target RRC UE starts with the ongoing location measurements of the UE.
  rrc_ue.reset();
  init({}, std::move(transfer_context));
  std::optional<byte_buffer> stored_info = rrc_ue->get_packed_location_meas_info();
  ASSERT_TRUE(stored_info.has_value());
  EXPECT_EQ(stored_info.value(), make_packed_nr_prs_location_meas_info());
}
