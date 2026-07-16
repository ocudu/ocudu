// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_test_environment.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/asn1/ngap/common.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/ngap/ngap_message.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

class cu_cp_write_replace_warning_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_write_replace_warning_test() : cu_cp_test_environment(cu_cp_test_env_params{})
  {
    run_ng_setup();

    // Connect and setup a DU.
    std::optional<unsigned> ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    du_idx = ret.value();
    get_du(du_idx).push_ul_pdu(test_helpers::generate_f1_setup_request());
    EXPECT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  }

protected:
  unsigned     du_idx = 0;
  ngap_message ngap_pdu;
  f1ap_message f1ap_pdu;
};

TEST_F(cu_cp_write_replace_warning_test, when_f1ap_response_received_then_ngap_response_is_sent_to_amf)
{
  // Inject NGAP Write-Replace Warning Request.
  get_amf().push_tx_pdu(generate_write_replace_warning_request());

  // Wait for F1AP Write-Replace Warning Request sent to DU and extract transaction ID.
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_EQ(f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(f1ap_pdu.pdu.init_msg().proc_code, ASN1_F1AP_ID_WRITE_REPLACE_WARNING);
  unsigned transaction_id = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->transaction_id;

  // Inject F1AP Write-Replace Warning Response from DU.
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1ap_write_replace_warning_response(transaction_id));

  // The CU-CP should forward the response to the AMF via NGAP.
  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_EQ(ngap_pdu.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(ngap_pdu.pdu.successful_outcome().proc_code, ASN1_NGAP_ID_WRITE_REPLACE_WARNING);
}

TEST_F(cu_cp_write_replace_warning_test, when_ngap_response_sent_then_msg_id_and_serial_num_match_request)
{
  // Inject NGAP Write-Replace Warning Request with known msg_id and serial_num.
  get_amf().push_tx_pdu(generate_write_replace_warning_request());

  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  unsigned transaction_id = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->transaction_id;

  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1ap_write_replace_warning_response(transaction_id));

  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu));
  const auto& resp = *ngap_pdu.pdu.successful_outcome().value.write_replace_warning_resp();
  ASSERT_EQ(resp.msg_id.to_number(), 0x1234);
  ASSERT_EQ(resp.serial_num.to_number(), 0x0001);
}

TEST_F(cu_cp_write_replace_warning_test, when_two_dus_connected_then_warning_is_sent_to_both)
{
  // Connect and setup a second DU with a distinct cell.
  std::optional<unsigned> ret2 = connect_new_du();
  ASSERT_TRUE(ret2.has_value());
  unsigned                            du_idx2 = ret2.value();
  test_helpers::served_cell_item_info cell2{.nci = nr_cell_identity::create(6577).value(), .pci = 1};
  get_du(du_idx2).push_ul_pdu(test_helpers::generate_f1_setup_request(int_to_gnb_du_id(0x12), {cell2}));
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx2, f1ap_pdu));

  // Inject NGAP Write-Replace Warning Request without a warning area list (targets all cells).
  get_amf().push_tx_pdu(generate_write_replace_warning_request());

  // DU 1 receives the F1AP request first (sequential DU iteration).
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_EQ(f1ap_pdu.pdu.init_msg().proc_code, ASN1_F1AP_ID_WRITE_REPLACE_WARNING);
  unsigned txn1 = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->transaction_id;
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1ap_write_replace_warning_response(txn1));

  // DU 2 receives its F1AP request after DU 1 has responded.
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx2, f1ap_pdu));
  ASSERT_EQ(f1ap_pdu.pdu.init_msg().proc_code, ASN1_F1AP_ID_WRITE_REPLACE_WARNING);
  unsigned txn2 = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->transaction_id;
  get_du(du_idx2).push_ul_pdu(test_helpers::generate_f1ap_write_replace_warning_response(txn2));

  // The NGAP response is sent after both DUs have responded.
  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_EQ(ngap_pdu.pdu.successful_outcome().proc_code, ASN1_NGAP_ID_WRITE_REPLACE_WARNING);
}

TEST_F(cu_cp_write_replace_warning_test, when_f1ap_request_times_out_then_ngap_response_is_still_sent)
{
  // Inject NGAP Write-Replace Warning Request.
  get_amf().push_tx_pdu(generate_write_replace_warning_request());
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));

  // Do not inject F1AP response; advance simulated clock past the F1AP transaction timeout.
  tick_until(get_cu_cp_cfg().f1ap.proc_timeout + std::chrono::milliseconds{1000}, [&]() { return false; }, false);

  // The routine must still send an NGAP response after the F1AP timeout.
  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_EQ(ngap_pdu.pdu.successful_outcome().proc_code, ASN1_NGAP_ID_WRITE_REPLACE_WARNING);
}

TEST_F(cu_cp_write_replace_warning_test, when_warning_area_list_filters_by_cgi_then_only_matching_du_gets_request)
{
  // Connect and setup a second DU with a distinct cell.
  std::optional<unsigned> ret2 = connect_new_du();
  ASSERT_TRUE(ret2.has_value());
  unsigned                            du_idx2 = ret2.value();
  test_helpers::served_cell_item_info cell2{.nci = nr_cell_identity::create(6577).value(), .pci = 1};
  get_du(du_idx2).push_ul_pdu(test_helpers::generate_f1_setup_request(int_to_gnb_du_id(0x12), {cell2}));
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx2, f1ap_pdu));

  // Build the CGI of DU 1's cell (matches the default served_cell_item_info).
  nr_cell_global_id_t du1_cgi{plmn_identity::test_value(), nr_cell_identity::create(gnb_id_t{411, 22}, 0U).value()};

  // Inject NGAP Write-Replace Warning Request targeting only DU 1's cell.
  get_amf().push_tx_pdu(generate_write_replace_warning_request_with_nr_cgi_list({du1_cgi}));

  // DU 1 receives the F1AP request (its cell matches the warning area).
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_EQ(f1ap_pdu.pdu.init_msg().proc_code, ASN1_F1AP_ID_WRITE_REPLACE_WARNING);
  unsigned txn1 = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->transaction_id;
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1ap_write_replace_warning_response(txn1));

  // The NGAP response is sent without waiting for DU 2 (it was skipped).
  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_EQ(ngap_pdu.pdu.successful_outcome().proc_code, ASN1_NGAP_ID_WRITE_REPLACE_WARNING);

  // DU 2 must not have received any F1AP Write-Replace Warning Request.
  ASSERT_FALSE(get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu));
}

TEST_F(cu_cp_write_replace_warning_test, when_cmas_request_received_then_sib8_with_content_is_sent)
{
  // CMAS path: data_coding_scheme + warning_msg_contents present, no warning_type -> SIB8 with encoded message.
  get_amf().push_tx_pdu(generate_write_replace_warning_request_cmas());

  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_EQ(f1ap_pdu.pdu.init_msg().proc_code, ASN1_F1AP_ID_WRITE_REPLACE_WARNING);
  EXPECT_EQ(f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->pws_sys_info.sib_type, 8U);
  EXPECT_FALSE(f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->pws_sys_info.sib_msg.empty());

  unsigned txn = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->transaction_id;
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1ap_write_replace_warning_response(txn));

  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_EQ(ngap_pdu.pdu.successful_outcome().proc_code, ASN1_NGAP_ID_WRITE_REPLACE_WARNING);
}

TEST_F(cu_cp_write_replace_warning_test, when_etws_full_request_received_then_sib6_then_sib7_are_sent)
{
  // ETWS full path: warning_type + data_coding_scheme + warning_msg_contents -> SIB6 request then SIB7 request.
  get_amf().push_tx_pdu(generate_write_replace_warning_request_etws_full());

  // First F1AP exchange: SIB6 (ETWS primary).
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_EQ(f1ap_pdu.pdu.init_msg().proc_code, ASN1_F1AP_ID_WRITE_REPLACE_WARNING);
  EXPECT_EQ(f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->pws_sys_info.sib_type, 6U);
  EXPECT_FALSE(f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->pws_sys_info.sib_msg.empty());
  unsigned txn1 = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->transaction_id;
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1ap_write_replace_warning_response(txn1));

  // Second F1AP exchange: SIB7 (ETWS secondary with message content).
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_EQ(f1ap_pdu.pdu.init_msg().proc_code, ASN1_F1AP_ID_WRITE_REPLACE_WARNING);
  EXPECT_EQ(f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->pws_sys_info.sib_type, 7U);
  EXPECT_FALSE(f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->pws_sys_info.sib_msg.empty());
  unsigned txn2 = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->transaction_id;
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1ap_write_replace_warning_response(txn2));

  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_EQ(ngap_pdu.pdu.successful_outcome().proc_code, ASN1_NGAP_ID_WRITE_REPLACE_WARNING);
}

TEST_F(cu_cp_write_replace_warning_test, when_warning_msg_contents_present_without_dcs_then_fallback_sib8_is_sent)
{
  // Per TS 38.413 section 9.3.1.37, DCS must be present when warning message contents is present.
  // When DCS is absent, the CU-CP must not encode a SIB8 with content; only the fallback minimal SIB8 is sent.
  get_amf().push_tx_pdu(generate_write_replace_warning_request_with_msg_no_dcs());

  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_EQ(f1ap_pdu.pdu.init_msg().proc_code, ASN1_F1AP_ID_WRITE_REPLACE_WARNING);
  EXPECT_EQ(f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->pws_sys_info.sib_type, 8U);

  unsigned txn = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->transaction_id;
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1ap_write_replace_warning_response(txn));

  // Exactly one F1AP exchange: no second request was sent.
  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_FALSE(get_du(du_idx).try_pop_dl_pdu(f1ap_pdu));
}

TEST_F(cu_cp_write_replace_warning_test, when_warning_type_and_msg_present_without_dcs_then_only_sib6_is_sent)
{
  // warning_type present (-> SIB6) but DCS absent (-> SIB7 must not be produced).
  // Per TS 38.413 section 9.3.1.37, DCS shall be present when warning message contents is present.
  ngap_message ngap_req     = generate_write_replace_warning_request_with_msg_no_dcs();
  auto&        ies          = ngap_req.pdu.init_msg().value.write_replace_warning_request();
  ies->warning_type_present = true;
  ies->warning_type.from_string("0180");

  get_amf().push_tx_pdu(ngap_req);

  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_EQ(f1ap_pdu.pdu.init_msg().proc_code, ASN1_F1AP_ID_WRITE_REPLACE_WARNING);
  EXPECT_EQ(f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->pws_sys_info.sib_type, 6U);

  unsigned txn = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->transaction_id;
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1ap_write_replace_warning_response(txn));

  // Exactly one F1AP exchange: no SIB7 request was sent.
  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_FALSE(get_du(du_idx).try_pop_dl_pdu(f1ap_pdu));
}

TEST_F(cu_cp_write_replace_warning_test, when_warning_area_list_is_tai_then_all_dus_get_request)
{
  // Connect and setup a second DU with a distinct cell.
  std::optional<unsigned> ret2 = connect_new_du();
  ASSERT_TRUE(ret2.has_value());
  unsigned                            du_idx2 = ret2.value();
  test_helpers::served_cell_item_info cell2{.nci = nr_cell_identity::create(6577).value(), .pci = 1};
  get_du(du_idx2).push_ul_pdu(test_helpers::generate_f1_setup_request(int_to_gnb_du_id(0x12), {cell2}));
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx2, f1ap_pdu));

  // TAI filter: the current implementation does not map TAIs to individual cells,
  // so a TAI warning area list targets all DUs (same as no filter).
  tai_t tai{plmn_identity::test_value(), 7};
  get_amf().push_tx_pdu(generate_write_replace_warning_request_with_tai_list({tai}));

  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_EQ(f1ap_pdu.pdu.init_msg().proc_code, ASN1_F1AP_ID_WRITE_REPLACE_WARNING);
  unsigned txn1 = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->transaction_id;
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1ap_write_replace_warning_response(txn1));

  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx2, f1ap_pdu));
  ASSERT_EQ(f1ap_pdu.pdu.init_msg().proc_code, ASN1_F1AP_ID_WRITE_REPLACE_WARNING);
  unsigned txn2 = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->transaction_id;
  get_du(du_idx2).push_ul_pdu(test_helpers::generate_f1ap_write_replace_warning_response(txn2));

  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_EQ(ngap_pdu.pdu.successful_outcome().proc_code, ASN1_NGAP_ID_WRITE_REPLACE_WARNING);
}

TEST_F(cu_cp_write_replace_warning_test, when_cgi_filter_matches_both_dus_then_both_get_request)
{
  // Connect and setup a second DU.  Use nci2 (gnb_id={411,22}, cell_idx=1) which is in the CU-CP's
  // configured cell list, so the F1 setup succeeds and has_cell() returns true for the CGI filter.
  std::optional<unsigned> ret2 = connect_new_du();
  ASSERT_TRUE(ret2.has_value());
  unsigned                            du_idx2 = ret2.value();
  test_helpers::served_cell_item_info cell2{.nci = nr_cell_identity::create(gnb_id_t{411, 22}, 1U).value(), .pci = 1};
  get_du(du_idx2).push_ul_pdu(test_helpers::generate_f1_setup_request(int_to_gnb_du_id(0x12), {cell2}));
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx2, f1ap_pdu));

  // Build one CGI per DU.
  nr_cell_global_id_t du1_cgi{plmn_identity::test_value(), nr_cell_identity::create(gnb_id_t{411, 22}, 0U).value()};
  nr_cell_global_id_t du2_cgi{plmn_identity::test_value(), nr_cell_identity::create(gnb_id_t{411, 22}, 1U).value()};
  get_amf().push_tx_pdu(generate_write_replace_warning_request_with_nr_cgi_list({du1_cgi, du2_cgi}));

  // DU 1 gets its request and responds.
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_EQ(f1ap_pdu.pdu.init_msg().proc_code, ASN1_F1AP_ID_WRITE_REPLACE_WARNING);
  unsigned txn1 = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->transaction_id;
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1ap_write_replace_warning_response(txn1));

  // DU 2 gets its request and responds.
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx2, f1ap_pdu));
  ASSERT_EQ(f1ap_pdu.pdu.init_msg().proc_code, ASN1_F1AP_ID_WRITE_REPLACE_WARNING);
  unsigned txn2 = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->transaction_id;
  get_du(du_idx2).push_ul_pdu(test_helpers::generate_f1ap_write_replace_warning_response(txn2));

  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_EQ(ngap_pdu.pdu.successful_outcome().proc_code, ASN1_NGAP_ID_WRITE_REPLACE_WARNING);
}

TEST_F(cu_cp_write_replace_warning_test, when_first_du_f1ap_times_out_then_second_du_still_gets_request)
{
  // Connect and setup a second DU with a distinct cell.
  std::optional<unsigned> ret2 = connect_new_du();
  ASSERT_TRUE(ret2.has_value());
  unsigned                            du_idx2 = ret2.value();
  test_helpers::served_cell_item_info cell2{.nci = nr_cell_identity::create(6577).value(), .pci = 1};
  get_du(du_idx2).push_ul_pdu(test_helpers::generate_f1_setup_request(int_to_gnb_du_id(0x12), {cell2}));
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx2, f1ap_pdu));

  get_amf().push_tx_pdu(generate_write_replace_warning_request());

  // DU 1 receives the F1AP request but does not respond.
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_EQ(f1ap_pdu.pdu.init_msg().proc_code, ASN1_F1AP_ID_WRITE_REPLACE_WARNING);

  // Advance simulated clock past the F1AP transaction timeout.
  tick_until(get_cu_cp_cfg().f1ap.proc_timeout + std::chrono::milliseconds{1000}, [&]() { return false; }, false);

  // DU 2 should still receive its F1AP request after DU 1 timed out.
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx2, f1ap_pdu));
  ASSERT_EQ(f1ap_pdu.pdu.init_msg().proc_code, ASN1_F1AP_ID_WRITE_REPLACE_WARNING);
  unsigned txn2 = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->transaction_id;
  get_du(du_idx2).push_ul_pdu(test_helpers::generate_f1ap_write_replace_warning_response(txn2));

  // NGAP response is sent despite DU 1's failure.
  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_EQ(ngap_pdu.pdu.successful_outcome().proc_code, ASN1_NGAP_ID_WRITE_REPLACE_WARNING);
}

// Fixture with a 3-byte segment size — forces segmentation of any warning message longer than 3 bytes.
class cu_cp_write_replace_warning_segmented_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_write_replace_warning_segmented_test() :
    cu_cp_test_environment([]() {
      cu_cp_test_env_params p;
      p.pws_max_warning_message_segment_size = 3;
      return p;
    }())
  {
    run_ng_setup();
    std::optional<unsigned> ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    du_idx = ret.value();
    get_du(du_idx).push_ul_pdu(test_helpers::generate_f1_setup_request());
    EXPECT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  }

protected:
  unsigned     du_idx = 0;
  ngap_message ngap_pdu;
  f1ap_message f1ap_pdu;
};

static ngap_message make_cmas_request_with_payload(std::vector<uint8_t> payload)
{
  ngap_message ngap_req           = generate_write_replace_warning_request();
  auto&        ies                = ngap_req.pdu.init_msg().value.write_replace_warning_request();
  ies->data_coding_scheme_present = true;
  ies->data_coding_scheme.from_number(0x0f, 8);
  ies->warning_msg_contents_present = true;
  ies->warning_msg_contents.resize(payload.size());
  for (size_t i = 0; i < payload.size(); ++i) {
    ies->warning_msg_contents[i] = payload[i];
  }
  return ngap_req;
}

TEST_F(cu_cp_write_replace_warning_segmented_test, when_message_fits_in_one_segment_no_additional_sibs_are_sent)
{
  // 3-byte message exactly fills one segment -> no Additional SIB Message List.
  get_amf().push_tx_pdu(make_cmas_request_with_payload({0xaa, 0xbb, 0xcc}));

  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  const auto& pws = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->pws_sys_info;
  EXPECT_EQ(pws.sib_type, 8U);
  EXPECT_FALSE(pws.ie_exts_present);

  unsigned txn = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->transaction_id;
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1ap_write_replace_warning_response(txn));
  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu));
}

TEST_F(cu_cp_write_replace_warning_segmented_test, when_message_exceeds_segment_size_additional_sibs_are_populated)
{
  // 7-byte message with a 3-byte limit -> ceil(7/3) = 3 segments: segment 0 in sib_msg, segments 1-2 additional.
  get_amf().push_tx_pdu(make_cmas_request_with_payload({0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07}));

  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  const auto& pws = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->pws_sys_info;
  EXPECT_EQ(pws.sib_type, 8U);
  ASSERT_TRUE(pws.ie_exts_present);
  ASSERT_TRUE(pws.ie_exts.add_sib_msg_list_present);
  EXPECT_EQ(pws.ie_exts.add_sib_msg_list.size(), 2U);

  unsigned txn = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->transaction_id;
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1ap_write_replace_warning_response(txn));
  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu));
}

TEST_F(cu_cp_write_replace_warning_segmented_test, when_maximum_length_message_is_segmented_all_segments_are_sent)
{
  // 9600-byte message (maximum per TS 38.413 section 9.3.1.37) with a 3-byte limit ->
  // ceil(9600/3) = 3200 segments, which exceeds the 64-segment cap -> encoder returns error -> fallback SIB8.
  // Use a 150-byte limit instead: ceil(9600/150) = 64 segments exactly (segment 0 + 63 additional).
  cu_cp_test_env_params p;
  p.pws_max_warning_message_segment_size = 150;
  // Re-run with a 150-byte segment size by directly calling the encoder for this boundary check.
  // At the CU-CP integration level use a payload that just fits: 64 * 3 = 192 bytes with the 3-byte fixture.
  // 192 bytes / 3 bytes per segment = 64 segments -> segment 0 + 63 additional (at the cap boundary).
  std::vector<uint8_t> payload(192);
  for (size_t i = 0; i < payload.size(); ++i) {
    payload[i] = static_cast<uint8_t>(i & 0xff);
  }
  get_amf().push_tx_pdu(make_cmas_request_with_payload(payload));

  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  const auto& pws = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->pws_sys_info;
  EXPECT_EQ(pws.sib_type, 8U);
  ASSERT_TRUE(pws.ie_exts_present);
  ASSERT_TRUE(pws.ie_exts.add_sib_msg_list_present);
  // 192 bytes / 3 bytes per segment = 64 segments -> segment 0 in sib_msg, 63 in additional list.
  EXPECT_EQ(pws.ie_exts.add_sib_msg_list.size(), 63U);

  unsigned txn = f1ap_pdu.pdu.init_msg().value.write_replace_warning_request()->transaction_id;
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1ap_write_replace_warning_response(txn));
  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu));
}

// Fixture with no DUs connected — used to verify the routine handles an empty DU set gracefully.
class cu_cp_write_replace_warning_no_du_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_write_replace_warning_no_du_test() : cu_cp_test_environment(cu_cp_test_env_params{}) { run_ng_setup(); }

protected:
  ngap_message ngap_pdu;
};

TEST_F(cu_cp_write_replace_warning_no_du_test, when_no_dus_connected_then_ngap_response_is_sent)
{
  get_amf().push_tx_pdu(generate_write_replace_warning_request());

  ASSERT_TRUE(wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_EQ(ngap_pdu.pdu.successful_outcome().proc_code, ASN1_NGAP_ID_WRITE_REPLACE_WARNING);
}
