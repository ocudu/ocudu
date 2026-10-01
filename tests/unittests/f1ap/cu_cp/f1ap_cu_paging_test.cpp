// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_cu_test_helpers.h"
#include "tests/ocudu_test_requirements.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

static constexpr uint64_t AMF_SET_ID    = 1;
static constexpr uint64_t AMF_POINTER   = 0;
static constexpr uint64_t FIVE_G_TMSI   = 4211117727;
static constexpr uint64_t FIVE_G_S_TMSI = 279089024671;

static constexpr uint32_t GNB_ID           = 411;
static constexpr uint8_t  GNB_ID_BITLENGTH = 22;
static constexpr uint16_t CELL_SECTOR_ID   = 0;

class f1ap_paging_test : public f1ap_cu_test
{
protected:
  /// \brief Generate a dummy PAGING message.
  static cu_cp_paging_message generate_paging_message()
  {
    cu_cp_paging_message paging_msg;

    // Create 5G-S-TMSI.
    bounded_bitset<48> five_g_s_tmsi(48);
    five_g_s_tmsi.from_uint64(((uint64_t)AMF_SET_ID << 38U) + ((uint64_t)AMF_POINTER << 32U) + FIVE_G_TMSI);

    // Add UE ID idx value.
    paging_msg.ue_id_idx_value = five_g_s_tmsi.to_uint64() % 1024;

    // Add UE paging ID.
    paging_msg.ue_paging_id = five_g_s_tmsi_t{five_g_s_tmsi};

    // Add paging DRX.
    paging_msg.paging_drx = 64;

    // Add TAI list for paging.
    cu_cp_tai_list_for_paging_item tai_item;
    tai_item.tai.plmn_id = plmn_identity::test_value();
    tai_item.tai.tac     = 7;
    paging_msg.tai_list_for_paging.push_back(tai_item);

    // Add paging prio.
    paging_msg.paging_prio = 5;

    // Add UE radio cap for paging.
    cu_cp_ue_radio_cap_for_paging ue_radio_cap_for_paging;
    ue_radio_cap_for_paging.ue_radio_cap_for_paging_of_nr = make_byte_buffer("deadbeef").value();
    paging_msg.ue_radio_cap_for_paging                    = ue_radio_cap_for_paging;

    // Add paging origin.
    paging_msg.paging_origin = true;

    // Add assist data for paging.
    cu_cp_assist_data_for_paging assist_data_for_paging;

    // Add assist data for recommended cells.
    cu_cp_assist_data_for_recommended_cells assist_data_for_recommended_cells;

    cu_cp_recommended_cell_item recommended_cell_item;

    // Add NGRAN CGI.
    recommended_cell_item.ngran_cgi.nci =
        nr_cell_identity::create(gnb_id_t{GNB_ID, GNB_ID_BITLENGTH}, CELL_SECTOR_ID).value();
    recommended_cell_item.ngran_cgi.plmn_id = plmn_identity::test_value();

    // Add time stayed in cell.
    recommended_cell_item.time_stayed_in_cell = 5;

    assist_data_for_recommended_cells.recommended_cells_for_paging.recommended_cell_list.push_back(
        recommended_cell_item);

    assist_data_for_paging.assist_data_for_recommended_cells = assist_data_for_recommended_cells;

    // Add paging attempt info.
    cu_cp_paging_attempt_info paging_attempt_info;

    paging_attempt_info.paging_attempt_count         = 3;
    paging_attempt_info.intended_nof_paging_attempts = 4;
    paging_attempt_info.next_paging_area_scope       = "changed";

    assist_data_for_paging.paging_attempt_info = paging_attempt_info;

    paging_msg.assist_data_for_paging = assist_data_for_paging;

    // Add eDRX information.
    cu_cp_paging_edrx_info edrx_info = {};
    edrx_info.nr_paging_edrx_cycle   = 0.5;
    edrx_info.nr_paging_time_window  = 1;
    paging_msg.paging_edrx_info      = edrx_info;

    return paging_msg;
  }

  bool was_conversion_successful() const
  {
    const asn1::f1ap::paging_s& paging_msg = f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.paging();

    // Check UE ID idx value.
    if (paging_msg->ue_id_idx_value.idx_len10().to_number() != (FIVE_G_S_TMSI % 1024)) {
      test_logger.error("UE ID idx value mismatch {} != {}",
                        paging_msg->ue_id_idx_value.idx_len10().to_number(),
                        (FIVE_G_S_TMSI % 1024));
      return false;
    }

    // Check paging ID.
    if (paging_msg->paging_id.cn_ue_paging_id().five_g_s_tmsi().to_number() != FIVE_G_S_TMSI) {
      test_logger.error("Paging ID mismatch {} != {}",
                        paging_msg->paging_id.cn_ue_paging_id().five_g_s_tmsi().to_number(),
                        FIVE_G_S_TMSI);
      return false;
    }

    // Check paging DRX.
    if (!paging_msg->paging_drx_present) {
      return false;
    }
    if (paging_msg->paging_drx.to_number() != 64) {
      test_logger.error("Paging DRX mismatch {} != {}", paging_msg->paging_drx.to_number(), 64);
      return false;
    }

    // Check paging prio.
    if (!paging_msg->paging_prio_present) {
      return false;
    }
    if (paging_msg->paging_prio.to_number() != 5) {
      test_logger.error("Paging prio mismatch {} != {}", paging_msg->paging_prio.to_number(), 5);
      return false;
    }

    // Check paging cell list.
    if (paging_msg->paging_cell_list.size() != 1) {
      return false;
    }
    const asn1::f1ap::paging_cell_item_s& paging_cell_item = paging_msg->paging_cell_list[0].value().paging_cell_item();
    nr_cell_identity nci = nr_cell_identity::create(gnb_id_t{GNB_ID, GNB_ID_BITLENGTH}, CELL_SECTOR_ID).value();
    if (paging_cell_item.nr_cgi.nr_cell_id.to_number() != nci.value()) {
      test_logger.error("NR CGI NCI mismatch {} != {}}", paging_cell_item.nr_cgi.nr_cell_id.to_number(), nci);
      return false;
    }
    if (paging_cell_item.nr_cgi.plmn_id.to_string() != "00f110") {
      test_logger.error("NR CGI PLMN mismatch {} != 00f110", paging_cell_item.nr_cgi.plmn_id.to_string());
      return false;
    }

    // Check paging origin.
    if (!paging_msg->paging_origin_present) {
      return false;
    }
    if ((std::string)paging_msg->paging_origin.to_string() != "non-3gpp") {
      test_logger.error("Paging origin mismatch {} != non-3gpp", paging_msg->paging_origin.to_string());
      return false;
    }

    // Check eDRX information.
    if (!paging_msg->nr_paginge_drx_info_present) {
      return false;
    }
    if (paging_msg->nr_paginge_drx_info.nrpaging_e_drx_cycle_idle != asn1::f1ap::nr_paging_e_drx_cycle_idle_e::hfhalf) {
      return false;
    }
    if (!paging_msg->nr_paginge_drx_info.nrpaging_time_win_present ||
        paging_msg->nr_paginge_drx_info.nrpaging_time_win != asn1::f1ap::nr_paging_time_win_e::s1) {
      return false;
    }

    return true;
  }

  bool was_paging_forwarded() const { return was_conversion_successful(); }
};

/// Test paging message handling.
TEST_F(f1ap_paging_test, when_paging_message_received_message_is_forwarded)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BAS-16-1-a");

  // Inject paging message.
  cu_cp_paging_message paging_msg = generate_paging_message();
  f1ap->handle_paging(paging_msg);

  // Check that paging message has been forwarded.
  ASSERT_TRUE(was_paging_forwarded());
}
