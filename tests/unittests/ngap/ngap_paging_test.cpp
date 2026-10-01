// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngap_test_helpers.h"
#include "tests/ocudu_test_requirements.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/ran/five_g_s_tmsi.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

static constexpr uint64_t AMF_SET_ID  = 1;
static constexpr uint64_t AMF_POINTER = 0;
static constexpr uint64_t FIVE_G_TMSI = 4211117727;

static constexpr uint32_t GNB_ID           = 411;
static constexpr uint8_t  GNB_ID_BITLENGTH = 22;
static constexpr uint16_t CELL_SECTOR_ID   = 0;

class ngap_paging_test : public ngap_test
{
protected:
  bool was_minimal_conversion_successful() const
  {
    if (!std::holds_alternative<five_g_s_tmsi_t>(cu_cp_notifier.last_paging_msg.ue_paging_id)) {
      test_logger.error("UE Paging ID type mismatch");
      return false;
    }

    five_g_s_tmsi_t ue_paging_id = std::get<five_g_s_tmsi_t>(cu_cp_notifier.last_paging_msg.ue_paging_id);

    // Check UE paging id.
    if (ue_paging_id.get_amf_set_id() != AMF_SET_ID) {
      test_logger.error("AMF Set ID mismatch {} != {}", ue_paging_id.get_amf_set_id(), AMF_SET_ID);
      return false;
    }
    if (ue_paging_id.get_amf_pointer() != AMF_POINTER) {
      test_logger.error("AMF Pointer mismatch {} != {}", ue_paging_id.get_amf_pointer(), AMF_POINTER);
      return false;
    }
    if (ue_paging_id.get_five_g_tmsi() != FIVE_G_TMSI) {
      test_logger.error("FiveG TMSI mismatch {} != {}", ue_paging_id.get_five_g_tmsi(), FIVE_G_TMSI);
      return false;
    }

    // Check TAI list for paging.
    if (cu_cp_notifier.last_paging_msg.tai_list_for_paging.size() != 1) {
      return false;
    }

    const cu_cp_tai_list_for_paging_item& paging_item = cu_cp_notifier.last_paging_msg.tai_list_for_paging.front();
    if (paging_item.tai.plmn_id != plmn_identity::test_value()) {
      test_logger.error("PLMN mismatch {} != 00f110", paging_item.tai.plmn_id);
      return false;
    }
    if (paging_item.tai.tac != 7) {
      test_logger.error("TAC mismatch {} != {}", paging_item.tai.tac, 7);
      return false;
    }

    return true;
  }

  bool was_full_conversion_successful() const
  {
    if (!was_minimal_conversion_successful()) {
      return false;
    }

    // Check paging DRX.
    if (!cu_cp_notifier.last_paging_msg.paging_drx.has_value()) {
      return false;
    }
    if (cu_cp_notifier.last_paging_msg.paging_drx.value() != 64) {
      test_logger.error("Paging DRX mismatch {} != {}", cu_cp_notifier.last_paging_msg.paging_drx, 64);
      return false;
    }

    // Check paging prio.
    if (!cu_cp_notifier.last_paging_msg.paging_prio.has_value()) {
      return false;
    }
    if (cu_cp_notifier.last_paging_msg.paging_prio.value() != 5) {
      test_logger.error("Paging prio mismatch {} != {}", cu_cp_notifier.last_paging_msg.paging_prio.value(), 5);
      return false;
    }

    // Check UE radio cap for paging.
    if (!cu_cp_notifier.last_paging_msg.ue_radio_cap_for_paging.has_value()) {
      return false;
    }
    if (cu_cp_notifier.last_paging_msg.ue_radio_cap_for_paging.value().ue_radio_cap_for_paging_of_nr !=
        make_byte_buffer("deadbeef").value()) {
      test_logger.error("UE radio cap for paging mismatch {} != {}",
                        cu_cp_notifier.last_paging_msg.ue_radio_cap_for_paging.value().ue_radio_cap_for_paging_of_nr,
                        make_byte_buffer("deadbeef").value());
      return false;
    }

    // Check paging origin.
    if (!cu_cp_notifier.last_paging_msg.paging_origin.has_value()) {
      return false;
    }
    if (!cu_cp_notifier.last_paging_msg.paging_origin.value()) {
      test_logger.error("Paging origin mismatch");
      return false;
    }

    // Check assist data for paging.
    if (!cu_cp_notifier.last_paging_msg.assist_data_for_paging.has_value()) {
      return false;
    }

    if (!cu_cp_notifier.last_paging_msg.paging_edrx_info.has_value()) {
      return false;
    }
    if (cu_cp_notifier.last_paging_msg.paging_edrx_info->nr_paging_edrx_cycle != 0.5) {
      return false;
    }
    if (cu_cp_notifier.last_paging_msg.paging_edrx_info->nr_paging_time_window != 1) {
      return false;
    }

    if (!cu_cp_notifier.last_paging_msg.assist_data_for_paging.value().assist_data_for_recommended_cells.has_value()) {
      return false;
    }
    if (cu_cp_notifier.last_paging_msg.assist_data_for_paging.value()
            .assist_data_for_recommended_cells.value()
            .recommended_cells_for_paging.recommended_cell_list.size() != 1) {
      return false;
    }
    const cu_cp_recommended_cell_item& cell_item = cu_cp_notifier.last_paging_msg.assist_data_for_paging.value()
                                                       .assist_data_for_recommended_cells.value()
                                                       .recommended_cells_for_paging.recommended_cell_list.front();
    if (cell_item.ngran_cgi.plmn_id != plmn_identity::test_value()) {
      test_logger.error("NR CGI PLMN mismatch {} != 00f110", cell_item.ngran_cgi.plmn_id);
      return false;
    }
    nr_cell_identity nci = nr_cell_identity::create(gnb_id_t{GNB_ID, GNB_ID_BITLENGTH}, CELL_SECTOR_ID).value();
    if (cell_item.ngran_cgi.nci != nci) {
      test_logger.error("NR CGI NCI mismatch {} != {}", cell_item.ngran_cgi.nci, nci);
      return false;
    }
    if (cell_item.time_stayed_in_cell.value() != 5) {
      test_logger.error("Time stayed in cell mismatch {} != {}", cell_item.time_stayed_in_cell, 5);
      return false;
    }

    if (!cu_cp_notifier.last_paging_msg.assist_data_for_paging.value().paging_attempt_info.has_value()) {
      return false;
    }
    if (cu_cp_notifier.last_paging_msg.assist_data_for_paging.value()
            .paging_attempt_info.value()
            .paging_attempt_count != 3) {
      test_logger.error("Paging attempt count mismatch {} != {}",
                        cu_cp_notifier.last_paging_msg.assist_data_for_paging.value()
                            .paging_attempt_info.value()
                            .paging_attempt_count,
                        3);
      return false;
    }
    if (cu_cp_notifier.last_paging_msg.assist_data_for_paging.value()
            .paging_attempt_info.value()
            .intended_nof_paging_attempts != 4) {
      test_logger.error("Intended nof paging attempts mismatch {} != {}",
                        cu_cp_notifier.last_paging_msg.assist_data_for_paging.value()
                            .paging_attempt_info.value()
                            .intended_nof_paging_attempts,
                        4);
      return false;
    }
    if (!cu_cp_notifier.last_paging_msg.assist_data_for_paging.value()
             .paging_attempt_info.value()
             .next_paging_area_scope.has_value()) {
      return false;
    }
    if (cu_cp_notifier.last_paging_msg.assist_data_for_paging.value()
            .paging_attempt_info.value()
            .next_paging_area_scope.value() != "changed") {
      test_logger.error("Next paging area mismatch {} != changed",
                        cu_cp_notifier.last_paging_msg.assist_data_for_paging.value()
                            .paging_attempt_info.value()
                            .next_paging_area_scope.value());
      return false;
    }

    return true;
  }
  bool was_minimal_paging_forwarded() const { return was_minimal_conversion_successful(); }

  bool was_full_paging_forwarded() const { return was_full_conversion_successful(); }

  bool was_error_indication_sent() const
  {
    return n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type() ==
           asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::error_ind;
  }
};

/// Test handling of valid paging message with optional fields.
TEST_F(ngap_paging_test, when_valid_paging_message_received_message_is_forwarded)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BAS-16-1-a");

  // Inject paging message.
  ngap_message paging_msg = generate_valid_minimal_paging_message();
  ngap->handle_message(paging_msg);

  // Check that paging message has been forwarded.
  ASSERT_TRUE(was_minimal_paging_forwarded());
}

/// Test handling of valid paging message with optional fields.
TEST_F(ngap_paging_test, when_valid_paging_message_with_optional_values_received_message_is_forwarded)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BAS-16-1-a");

  // Inject paging message.
  ngap_message paging_msg = generate_valid_paging_message();
  ngap->handle_message(paging_msg);

  // Check that paging message has been forwarded.
  ASSERT_TRUE(was_full_paging_forwarded());
}

/// Test handling of invalid paging message.
TEST_F(ngap_paging_test,
       when_paging_message_with_invalid_ue_paging_id_received_message_is_not_forwarded_and_error_indication_is_sent)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BAS-16-1-a");

  // Inject paging message.
  ngap_message paging_msg = generate_invalid_paging_message();
  ngap->handle_message(paging_msg);

  // Check that Error Indication has been sent to AMF.
  ASSERT_TRUE(was_error_indication_sent());
}

/// Test handling of invalid paging message.
TEST_F(ngap_paging_test,
       when_paging_message_with_invalid_tai_list_received_message_is_not_forwarded_and_error_indication_is_sent)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BAS-16-1-a");

  // Inject paging message.
  ngap_message paging_msg = generate_valid_paging_message();
  // Set invalid PLMN in TAI list for paging.
  paging_msg.pdu.init_msg().value.paging()->tai_list_for_paging[0].tai.plmn_id.from_string("ffffff");

  ngap->handle_message(paging_msg);

  // Check that Error Indication has been sent to AMF.
  ASSERT_TRUE(was_error_indication_sent());
}

/// Test handling of invalid paging message.
TEST_F(
    ngap_paging_test,
    when_paging_message_with_invalid_recommended_cell_list_received_message_is_not_forwarded_and_error_indication_is_sent)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BAS-16-1-a");

  // Inject paging message.
  ngap_message paging_msg = generate_valid_paging_message();
  // Set invalid PLMN in TAI list for paging.
  paging_msg.pdu.init_msg()
      .value.paging()
      ->assist_data_for_paging.assist_data_for_recommended_cells.recommended_cells_for_paging.recommended_cell_list[0]
      .ngran_cgi.nr_cgi()
      .plmn_id.from_string("ffffff");

  ngap->handle_message(paging_msg);

  // Check that Error Indication has been sent to AMF.
  ASSERT_TRUE(was_error_indication_sent());
}
