// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "asn1_test_utils.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/pcap/dlt_pcap.h"
#include "ocudu/support/test_utils.h"
#include <gtest/gtest.h>

using namespace asn1;
using namespace ocudu;

#define JSON_OUTPUT 0

class asn1_f1ap_test : public ::testing::Test
{
protected:
  asn1_f1ap_test()
  {
    ocudulog::fetch_basic_logger("ASN1").set_level(ocudulog::basic_levels::debug);
    ocudulog::fetch_basic_logger("ASN1").set_hex_dump_max_size(-1);

    test_logger.set_level(ocudulog::basic_levels::debug);
    test_logger.set_hex_dump_max_size(-1);

    // Start the log backend.
    ocudulog::init();
  }

  ~asn1_f1ap_test()
  {
    // flush logger after each test
    ocudulog::flush();
  }

#if JSON_OUTPUT
  task_worker                    worker{"pcap_worker", 128};
  std::unique_ptr<task_executor> pcap_exec   = make_task_executor_ptr(worker);
  std::unique_ptr<dlt_pcap>      pcap_writer = create_f1ap_pcap("/tmp/f1ap.pcap", *pcap_exec);
#else
  std::unique_ptr<dlt_pcap> pcap_writer = create_null_dlt_pcap();
#endif
  ocudulog::basic_logger& test_logger = ocudulog::fetch_basic_logger("TEST");
};

TEST_F(asn1_f1ap_test, unknown_ignore_ie_is_skipped_and_reject_ie_fails)
{
  for (auto criticality : {crit_e::ignore, crit_e::reject}) {
    asn1::f1ap::f1_setup_request_ies_container request;
    request.transaction_id = 7;
    request.gnb_du_id      = 1;
    request.gnb_du_rrc_version.latest_rrc_version.from_number(1);

    byte_buffer encoded;
    bit_ref     writer(encoded);
    ASSERT_EQ(request.pack(writer), OCUDUASN_SUCCESS);
    ASSERT_EQ(encoded[0], 0);
    ASSERT_EQ(encoded[1], 3);

    byte_buffer with_unknown;
    ASSERT_TRUE(with_unknown.append(encoded[0]));
    ASSERT_TRUE(with_unknown.append(uint8_t{4}));
    bit_ref extra(with_unknown);
    ASSERT_EQ(pack_integer(extra, uint32_t{1000}, uint32_t{0}, uint32_t{65535}, false, true), OCUDUASN_SUCCESS);
    ASSERT_EQ(crit_e{criticality}.pack(extra), OCUDUASN_SUCCESS);
    ASSERT_EQ(pack_length(extra, uint32_t{1}, true), OCUDUASN_SUCCESS);
    ASSERT_EQ(extra.pack(uint8_t{0x42}, 8), OCUDUASN_SUCCESS);
    for (unsigned i = 2; i != encoded.length(); ++i) {
      ASSERT_EQ(extra.pack(encoded[i], 8), OCUDUASN_SUCCESS);
    }

    asn1::f1ap::f1_setup_request_ies_container decoded;
    cbit_ref                                   reader(with_unknown);
    EXPECT_EQ(decoded.unpack(reader), criticality == crit_e::ignore ? OCUDUASN_SUCCESS : OCUDUASN_ERROR_DECODE_FAIL);
    if (criticality == crit_e::ignore) {
      EXPECT_EQ(decoded.transaction_id, 7);
      EXPECT_EQ(decoded.gnb_du_id, 1);
    }
  }
}

// initiating message F1SetupRequest
TEST_F(asn1_f1ap_test, when_setup_message_correct_then_packing_successful)
{
  asn1::f1ap::f1ap_pdu_c pdu;

  pdu.set_init_msg();
  pdu.init_msg().load_info_obj(ASN1_F1AP_ID_F1_SETUP);

  auto& setup_req                = pdu.init_msg().value.f1_setup_request();
  setup_req->transaction_id      = 99;
  setup_req->gnb_du_id           = 0x11;
  setup_req->gnb_du_name_present = true;
  setup_req->gnb_du_name.from_string("OCUDU DU");
  setup_req->gnb_du_rrc_version.latest_rrc_version.from_number(1);

  setup_req->gnb_du_served_cells_list_present = true;

  asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_du_served_cells_item_ies_o> served_cells_item_container = {};
  served_cells_item_container.set_item(ASN1_F1AP_ID_GNB_DU_SERVED_CELLS_ITEM);

  auto& served_cells_item = served_cells_item_container.value().gnb_du_served_cells_item();
  served_cells_item.served_cell_info.nr_cgi.plmn_id.from_string("208991");
  served_cells_item.served_cell_info.nr_cgi.nr_cell_id.from_number(12345678);
  served_cells_item.served_cell_info.nr_pci              = 0;
  served_cells_item.served_cell_info.five_gs_tac_present = true;
  served_cells_item.served_cell_info.five_gs_tac.from_number(1);

  asn1::f1ap::served_plmns_item_s served_plmn;
  served_plmn.plmn_id.from_string("208991");
  asn1::f1ap::slice_support_item_s slice_support_item;
  slice_support_item.snssai.sst.from_number(1);
  served_plmn.ie_exts.tai_slice_support_list_present = true;
  served_plmn.ie_exts.tai_slice_support_list.push_back(slice_support_item);
  served_cells_item.served_cell_info.served_plmns.push_back(served_plmn);

  served_cells_item.served_cell_info.nr_mode_info.set_tdd();
  served_cells_item.served_cell_info.nr_mode_info.tdd().nr_freq_info.nr_arfcn = 626748;
  asn1::f1ap::freq_band_nr_item_s freq_band_nr_item;
  freq_band_nr_item.freq_band_ind_nr = 78;
  served_cells_item.served_cell_info.nr_mode_info.tdd().nr_freq_info.freq_band_list_nr.push_back(freq_band_nr_item);
  served_cells_item.served_cell_info.nr_mode_info.tdd().tx_bw.nr_scs.value = asn1::f1ap::nr_scs_opts::scs30;
  served_cells_item.served_cell_info.nr_mode_info.tdd().tx_bw.nr_nrb.value = asn1::f1ap::nr_nrb_opts::nrb51;
  served_cells_item.served_cell_info.meas_timing_cfg.from_string("30");

  served_cells_item.gnb_du_sys_info_present = true;
  served_cells_item.gnb_du_sys_info.mib_msg.from_string("01c586");
  served_cells_item.gnb_du_sys_info.sib1_msg.from_string(
      "92002808241099000001000000000a4213407800008c98d6d8d7f616e0804000020107e28180008000088a0dc7008000088a0007141a22"
      "81c874cc00020000232d5c6b6c65462001ec4cc5fc9c0493946a98d4d1e99355c00a1aba010580ec024646f62180");

  setup_req->gnb_du_served_cells_list.push_back(served_cells_item_container);

  ocudu::byte_buffer tx_buffer;
  asn1::bit_ref      bref(tx_buffer);
  ASSERT_EQ(pdu.pack(bref), OCUDUASN_SUCCESS);

  ASSERT_EQ(test_pack_roundtrip(pdu), OCUDUASN_SUCCESS);

  std::vector<uint8_t> bytes{tx_buffer.begin(), tx_buffer.end()};
#if JSON_OUTPUT
  asn1::json_writer json_writer1;
  pdu.to_json(json_writer1);
  test_logger.info(
      bytes.data(), bytes.size(), "F1AP PDU unpacked ({} B): \n {}", bytes.size(), json_writer1.to_string().c_str());

  pcap_writer->push_pdu(bytes);
#endif
}

// successful outcome F1SetupResponse
TEST_F(asn1_f1ap_test, when_setup_response_correct_then_packing_successful)
{
  asn1::f1ap::f1ap_pdu_c pdu;

  pdu.set_successful_outcome();
  pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);

  auto& setup_res                = pdu.successful_outcome().value.f1_setup_resp();
  setup_res->transaction_id      = 99;
  setup_res->gnb_cu_name_present = true;
  setup_res->gnb_cu_name.from_string("OCUDU CU");
  setup_res->gnb_cu_rrc_version.latest_rrc_version.from_number(2);

  ocudu::byte_buffer tx_pdu;
  asn1::bit_ref      bref(tx_pdu);
  ASSERT_EQ(pdu.pack(bref), OCUDUASN_SUCCESS);

  ASSERT_EQ(test_pack_roundtrip(pdu), OCUDUASN_SUCCESS);

  std::vector<uint8_t> tx_buffer{tx_pdu.begin(), tx_pdu.end()};
#if JSON_OUTPUT
  asn1::json_writer json_writer1;
  pdu.to_json(json_writer1);
  test_logger.info(tx_buffer.data(),
                   tx_buffer.size(),
                   "F1AP PDU unpacked ({} B): \n {}",
                   tx_buffer.size(),
                   json_writer1.to_string().c_str());

  pcap_writer->push_pdu(ocudu::span<uint8_t>(tx_buffer.data(), tx_buffer.size()));
#endif
}

// unsuccessful outcome F1SetupFailure
TEST_F(asn1_f1ap_test, when_setup_failure_correct_then_packing_successful)
{
  asn1::f1ap::f1ap_pdu_c pdu;

  pdu.set_unsuccessful_outcome();
  pdu.unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);

  auto& setup_fail           = pdu.unsuccessful_outcome().value.f1_setup_fail();
  setup_fail->transaction_id = 99;
  setup_fail->cause.set_radio_network();
  setup_fail->cause.radio_network() =
      asn1::f1ap::cause_radio_network_opts::options::unknown_or_already_allocated_gnb_cu_ue_f1ap_id;
  setup_fail->time_to_wait_present = true;
  setup_fail->time_to_wait.value   = asn1::f1ap::time_to_wait_opts::v10s;
  // add critical diagnostics

  ocudu::byte_buffer tx_pdu;
  asn1::bit_ref      bref(tx_pdu);
  ASSERT_EQ(pdu.pack(bref), OCUDUASN_SUCCESS);

  ASSERT_EQ(test_pack_roundtrip(pdu), OCUDUASN_SUCCESS);

  std::vector<uint8_t> tx_buffer{tx_pdu.begin(), tx_pdu.end()};
#if JSON_OUTPUT
  asn1::json_writer json_writer1;
  pdu.to_json(json_writer1);
  test_logger.info(tx_buffer.data(),
                   tx_buffer.size(),
                   "F1AP PDU unpacked ({} B): \n {}",
                   tx_buffer.size(),
                   json_writer1.to_string().c_str());

  pcap_writer->push_pdu(ocudu::span<uint8_t>(tx_buffer.data(), tx_buffer.size()));
#endif
}

TEST_F(asn1_f1ap_test, when_ue_context_setup_request_correct_then_unpacking_successful)
{
  uint8_t rx_msg[] = {
      0x00, 0x05, 0x00, 0x82, 0x73, 0x00, 0x00, 0x06, 0x00, 0x28, 0x00, 0x02, 0x00, 0x34, 0x00, 0x3f, 0x00, 0x09, 0x00,
      0x00, 0xf1, 0x10, 0x07, 0x53, 0x04, 0x04, 0x70, 0x00, 0x6b, 0x00, 0x01, 0x04, 0x00, 0x09, 0x00, 0x82, 0x20, 0x60,
      0x81, 0x1c, 0x18, 0x80, 0x80, 0xff, 0x23, 0x01, 0x05, 0x7a, 0x35, 0x60, 0xa6, 0x13, 0x00, 0x00, 0x60, 0x40, 0x1c,
      0x4d, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x03, 0x09, 0x80, 0x00, 0x30, 0x20, 0x0e, 0x26, 0x80, 0x00,
      0x01, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x01, 0x84, 0xc0, 0x00, 0x18, 0x10, 0x07, 0x13, 0x40, 0x00, 0x01, 0x01, 0xfc,
      0x00, 0x00, 0x00, 0x00, 0xc1, 0x60, 0x40, 0x1c, 0x4d, 0x00, 0x00, 0x06, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x03, 0x05,
      0x80, 0x00, 0x71, 0x34, 0x00, 0x00, 0x18, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x10, 0x02, 0x00, 0x10, 0x13, 0x45,
      0xa2, 0xd1, 0x40, 0x31, 0x00, 0x00, 0x40, 0x20, 0x00, 0x10, 0x08, 0x08, 0x01, 0x00, 0x22, 0x00, 0x01, 0x00, 0x40,
      0x00, 0x20, 0x10, 0x10, 0x02, 0x00, 0x44, 0x00, 0x01, 0x00, 0x80, 0x00, 0x80, 0x20, 0x20, 0x04, 0x00, 0x84, 0x00,
      0x02, 0x01, 0x01, 0x00, 0x20, 0x04, 0x1e, 0x3f, 0xc0, 0x1c, 0x00, 0x11, 0xe4, 0xb2, 0xef, 0x97, 0xff, 0xd7, 0xc9,
      0x0a, 0x10, 0x00, 0x00, 0x30, 0x10, 0x20, 0x00, 0xe7, 0xc0, 0x00, 0xde, 0xa0, 0x4d, 0x00, 0xe7, 0xad, 0xfe, 0x1c,
      0xf5, 0xff, 0xef, 0xe4, 0x00, 0x00, 0x7d, 0x48, 0x08, 0x62, 0x89, 0x29, 0x4c, 0x8d, 0xff, 0xf7, 0x8d, 0xff, 0xf5,
      0xff, 0xfb, 0xfe, 0xfe, 0x00, 0x00, 0x00, 0x18, 0x05, 0xc3, 0x80, 0x80, 0xc0, 0x7e, 0x06, 0x01, 0x80, 0x3c, 0x04,
      0x06, 0x03, 0x9d, 0xff, 0x01, 0x01, 0x80, 0xe4, 0xff, 0xcd, 0x9f, 0xfb, 0x0c, 0x04, 0x08, 0x00, 0x39, 0xfe, 0x48,
      0x50, 0x80, 0x00, 0x01, 0x80, 0x81, 0x00, 0x07, 0x3e, 0xf0, 0x02, 0xec, 0xe0, 0x00, 0x00, 0x58, 0xf0, 0x00, 0x03,
      0x95, 0x34, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf2, 0xa8, 0x20, 0x00, 0x09, 0x27, 0x63, 0x00, 0x27,
      0xfa, 0x90, 0x1f, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x20, 0x00, 0x36, 0x59, 0x07, 0xe1,
      0x0c, 0x80, 0xff, 0x23, 0x01, 0x05, 0x7a, 0x35, 0x60, 0xa6, 0x13, 0x00, 0x00, 0x60, 0x40, 0x1c, 0x4d, 0x00, 0x00,
      0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x03, 0x09, 0x80, 0x00, 0x30, 0x20, 0x0e, 0x26, 0x80, 0x00, 0x01, 0x03, 0xf8,
      0x00, 0x00, 0x00, 0x01, 0x84, 0xc0, 0x00, 0x18, 0x10, 0x07, 0x13, 0x40, 0x00, 0x01, 0x01, 0xfc, 0x00, 0x00, 0x00,
      0x00, 0xc1, 0x60, 0x40, 0x1c, 0x4d, 0x00, 0x00, 0x06, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x03, 0x05, 0x80, 0x00, 0x71,
      0x34, 0x00, 0x00, 0x18, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x10, 0x02, 0x00, 0x10, 0x13, 0x45, 0xa2, 0xd1, 0x40,
      0x31, 0x00, 0x00, 0x40, 0x20, 0x00, 0x10, 0x08, 0x08, 0x01, 0x00, 0x22, 0x00, 0x01, 0x00, 0x40, 0x00, 0x20, 0x10,
      0x10, 0x02, 0x00, 0x44, 0x00, 0x01, 0x00, 0x80, 0x00, 0x80, 0x20, 0x20, 0x04, 0x00, 0x84, 0x00, 0x02, 0x01, 0x01,
      0x00, 0x20, 0x04, 0x1e, 0x3f, 0xc0, 0x1c, 0x00, 0x11, 0xe4, 0xb2, 0xef, 0x97, 0xff, 0xd7, 0xc9, 0x0a, 0x10, 0x00,
      0x00, 0x30, 0x10, 0x20, 0x00, 0xe7, 0xc0, 0x00, 0xde, 0xa0, 0x4d, 0x00, 0xe7, 0xad, 0xfe, 0x1c, 0xf5, 0xff, 0xef,
      0xe4, 0x00, 0x00, 0x7d, 0x48, 0x08, 0x62, 0x89, 0x29, 0x4c, 0x8d, 0xff, 0xf7, 0x8d, 0xff, 0xf5, 0xff, 0xfb, 0xfe,
      0xfe, 0x00, 0x00, 0x00, 0x18, 0x05, 0xc3, 0x80, 0x80, 0xc0, 0x7e, 0x06, 0x01, 0x80, 0x3c, 0x04, 0x06, 0x03, 0x9d,
      0xff, 0x01, 0x01, 0x80, 0xe4, 0xff, 0xcd, 0x9f, 0xfb, 0x0c, 0x04, 0x08, 0x00, 0x39, 0xfe, 0x48, 0x50, 0x80, 0x00,
      0x01, 0x80, 0x81, 0x00, 0x07, 0x3e, 0xf0, 0x02, 0xec, 0xe0, 0x00, 0x00, 0x58, 0xf0, 0x00, 0x03, 0x95, 0x34, 0x00,
      0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf2, 0xa8, 0x20, 0x00, 0x00, 0x23, 0x00, 0x27, 0x00, 0x00, 0x22, 0x00,
      0x22, 0x51, 0x40, 0x08, 0x09, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x20, 0x66, 0x00, 0x17, 0x02, 0x00, 0x00, 0x00, 0xa1, 0x40, 0x01, 0x40, 0x00, 0x9e, 0x40,
      0x04, 0x20, 0x20, 0x3a, 0x00};
  ocudu::byte_buffer rx_pdu = byte_buffer::create(rx_msg).value();

#ifdef JSON_OUTPUT
  pcap_writer->push_pdu(rx_msg);
#endif

  asn1::cbit_ref         bref{rx_pdu};
  asn1::f1ap::f1ap_pdu_c pdu;

  ASSERT_EQ(pdu.unpack(bref), OCUDUASN_SUCCESS);
  ASSERT_EQ(asn1::f1ap::f1ap_pdu_c::types_opts::init_msg, pdu.type());

  ASSERT_EQ(pdu.init_msg().proc_code, ASN1_F1AP_ID_UE_CONTEXT_SETUP);
  ASSERT_EQ(pdu.init_msg().value.type(),
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_setup_request);

  ASSERT_EQ(test_pack_roundtrip(pdu), OCUDUASN_SUCCESS);

#if JSON_OUTPUT
  int               unpacked_len = bref.distance_bytes();
  asn1::json_writer json_writer1;
  pdu.to_json(json_writer1);
  test_logger.info(
      rx_msg, unpacked_len, "F1AP PDU unpacked ({} B): \n {}", unpacked_len, json_writer1.to_string().c_str());
#endif
}

TEST_F(asn1_f1ap_test, when_initial_ul_rrc_message_transfer_correct_then_unpacking_successful)
{
  uint8_t rx_msg[] = {
      0x00, 0x0b, 0x40, 0x80, 0xa4, 0x00, 0x00, 0x06, 0x00, 0x29, 0x00, 0x03, 0x40, 0xa1, 0x27, 0x00, 0x6f, 0x00, 0x09,
      0x00, 0x02, 0xf8, 0x99, 0x00, 0x0b, 0xc6, 0x14, 0xe0, 0x00, 0x5f, 0x00, 0x03, 0x00, 0xa1, 0x27, 0x00, 0x32, 0x00,
      0x07, 0x06, 0x1d, 0xec, 0x89, 0xd0, 0x57, 0x66, 0x00, 0x80, 0x00, 0x71, 0x70, 0x5c, 0x00, 0xb0, 0x01, 0x11, 0x7a,
      0xec, 0x70, 0x10, 0x61, 0xe0, 0x00, 0x7c, 0x20, 0x40, 0x8d, 0x07, 0x81, 0x00, 0x20, 0xa2, 0x09, 0x04, 0x80, 0xca,
      0x80, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x83, 0x70, 0x84, 0x20, 0x00, 0x08, 0x81, 0x65, 0x00, 0x00, 0x48,
      0x20, 0x00, 0x02, 0x06, 0x9a, 0x06, 0xaa, 0x49, 0x88, 0x00, 0x02, 0x00, 0x20, 0x40, 0x00, 0x40, 0x0d, 0x00, 0x80,
      0x13, 0xb6, 0x4b, 0x18, 0x14, 0x40, 0x0e, 0x46, 0x8a, 0xcf, 0x12, 0x00, 0x00, 0x09, 0x60, 0x70, 0x82, 0x0f, 0x17,
      0x7e, 0x06, 0x08, 0x70, 0x00, 0x00, 0x00, 0xe2, 0x50, 0x38, 0x00, 0x00, 0x40, 0xbd, 0xe8, 0x02, 0x00, 0x04, 0x00,
      0x00, 0x00, 0x00, 0x02, 0x82, 0x01, 0x95, 0x03, 0x00, 0xc4, 0x00, 0x00, 0x4e, 0x40, 0x02, 0x00, 0x00};
  ocudu::byte_buffer rx_pdu = byte_buffer::create(rx_msg).value();

#ifdef JSON_OUTPUT
  pcap_writer->push_pdu(rx_msg);
#endif

  asn1::cbit_ref         bref{rx_pdu};
  asn1::f1ap::f1ap_pdu_c pdu;

  ASSERT_EQ(pdu.unpack(bref), OCUDUASN_SUCCESS);
  ASSERT_EQ(pdu.type(), asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);

  ASSERT_EQ(pdu.init_msg().proc_code, ASN1_F1AP_ID_INIT_UL_RRC_MSG_TRANSFER);
  ASSERT_EQ(pdu.init_msg().value.type(),
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::init_ul_rrc_msg_transfer);

  ASSERT_EQ(test_pack_roundtrip(pdu), OCUDUASN_SUCCESS);

#if JSON_OUTPUT
  int               unpacked_len = bref.distance_bytes();
  asn1::json_writer json_writer1;
  pdu.to_json(json_writer1);
  test_logger.info(
      rx_msg, unpacked_len, "F1AP PDU unpacked ({} B): \n {}", unpacked_len, json_writer1.to_string().c_str());
#endif
}

// repack manually
TEST_F(asn1_f1ap_test, when_initial_ul_rrc_message_transfer_packing_correct_then_equal_to_original_TV)
{
  uint8_t rx_msg[] = {
      0x00, 0x0b, 0x40, 0x80, 0xa4, 0x00, 0x00, 0x06, 0x00, 0x29, 0x00, 0x03, 0x40, 0xa1, 0x27, 0x00, 0x6f, 0x00, 0x09,
      0x00, 0x02, 0xf8, 0x99, 0x00, 0x0b, 0xc6, 0x14, 0xe0, 0x00, 0x5f, 0x00, 0x03, 0x00, 0xa1, 0x27, 0x00, 0x32, 0x00,
      0x07, 0x06, 0x1d, 0xec, 0x89, 0xd0, 0x57, 0x66, 0x00, 0x80, 0x00, 0x71, 0x70, 0x5c, 0x00, 0xb0, 0x01, 0x11, 0x7a,
      0xec, 0x70, 0x10, 0x61, 0xe0, 0x00, 0x7c, 0x20, 0x40, 0x8d, 0x07, 0x81, 0x00, 0x20, 0xa2, 0x09, 0x04, 0x80, 0xca,
      0x80, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x83, 0x70, 0x84, 0x20, 0x00, 0x08, 0x81, 0x65, 0x00, 0x00, 0x48,
      0x20, 0x00, 0x02, 0x06, 0x9a, 0x06, 0xaa, 0x49, 0x88, 0x00, 0x02, 0x00, 0x20, 0x40, 0x00, 0x40, 0x0d, 0x00, 0x80,
      0x13, 0xb6, 0x4b, 0x18, 0x14, 0x40, 0x0e, 0x46, 0x8a, 0xcf, 0x12, 0x00, 0x00, 0x09, 0x60, 0x70, 0x82, 0x0f, 0x17,
      0x7e, 0x06, 0x08, 0x70, 0x00, 0x00, 0x00, 0xe2, 0x50, 0x38, 0x00, 0x00, 0x40, 0xbd, 0xe8, 0x02, 0x00, 0x04, 0x00,
      0x00, 0x00, 0x00, 0x02, 0x82, 0x01, 0x95, 0x03, 0x00, 0xc4, 0x00, 0x00, 0x4e, 0x40, 0x02, 0x00, 0x00};
  ocudu::byte_buffer rx_pdu = byte_buffer::create(rx_msg).value();

  asn1::f1ap::f1ap_pdu_c tx_pdu;

  tx_pdu.set_init_msg();
  tx_pdu.init_msg().load_info_obj(ASN1_F1AP_ID_INIT_UL_RRC_MSG_TRANSFER);

  auto& init_ul_rrc              = tx_pdu.init_msg().value.init_ul_rrc_msg_transfer();
  init_ul_rrc->gnb_du_ue_f1ap_id = 41255; // same as C-RNTI

  init_ul_rrc->nr_cgi.nr_cell_id.from_string("000000000000101111000110000101001110");
  init_ul_rrc->nr_cgi.plmn_id.from_string("02f899");
  init_ul_rrc->c_rnti = 41255;

  init_ul_rrc->rrc_container.from_string("1dec89d05766");
  init_ul_rrc->du_to_cu_rrc_container_present = true;
  init_ul_rrc->du_to_cu_rrc_container.from_string(
      "5c00b001117aec701061e0007c20408d07810020a2090480ca8000f800000000008370842000088165000048200002069a06aa49880002"
      "00204000400d008013b64b1814400e468acf120000096070820f177e060870000000e25038000040bde802000400000000028201950300"
      "c400");

  ocudu::byte_buffer tx_buffer;
  asn1::bit_ref      bref_tx(tx_buffer);
  ASSERT_EQ(tx_pdu.pack(bref_tx), OCUDUASN_SUCCESS);

  // compare against original TV
  ASSERT_EQ(tx_buffer.length(), sizeof(rx_msg));
  ASSERT_EQ(rx_pdu, tx_buffer);
}

TEST_F(asn1_f1ap_test, when_e_cid_measurement_initiation_request_correct_then_packing_successful)
{
  asn1::f1ap::f1ap_pdu_c pdu;

  pdu.set_init_msg();
  pdu.init_msg().load_info_obj(ASN1_F1AP_ID_E_C_ID_MEAS_INITIATION);

  auto& req                                = pdu.init_msg().value.e_c_id_meas_initiation_request();
  req->gnb_cu_ue_f1ap_id                   = 1;
  req->gnb_du_ue_f1ap_id                   = 2;
  req->lmf_ue_meas_id                      = 3;
  req->ran_ue_meas_id                      = 4;
  req->e_c_id_report_characteristics.value = asn1::f1ap::e_c_id_report_characteristics_opts::periodic;

  asn1::protocol_ie_single_container_s<asn1::f1ap::e_c_id_meas_quantities_item_ies_o> quantity;
  quantity.load_info_obj(ASN1_F1AP_ID_E_C_ID_MEAS_QUANTITIES_ITEM);
  quantity->e_c_id_meas_quantities_item().e_c_id_meas_quantities_value.value =
      asn1::f1ap::e_c_id_meas_quantities_value_opts::angle_of_arrival_nr;
  req->e_c_id_meas_quantities.push_back(quantity);

  // NR Angle of Arrival carries its own periodicity, as per TS 38.473, Section 9.2.12.20.
  req->pos_meas_periodicity_nr_ao_a_present = true;
  req->pos_meas_periodicity_nr_ao_a.value   = asn1::f1ap::pos_meas_periodicity_nr_ao_a_opts::ms1280;

  ocudu::byte_buffer buffer;
  asn1::bit_ref      bref(buffer);
  ASSERT_EQ(pdu.pack(bref), OCUDUASN_SUCCESS);
  ASSERT_EQ(test_pack_roundtrip(pdu), OCUDUASN_SUCCESS);
}

TEST_F(asn1_f1ap_test, when_e_cid_measurement_initiation_response_with_ul_aoa_correct_then_packing_successful)
{
  asn1::f1ap::f1ap_pdu_c pdu;

  pdu.set_successful_outcome();
  pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_E_C_ID_MEAS_INITIATION);

  auto& resp              = pdu.successful_outcome().value.e_c_id_meas_initiation_resp();
  resp->gnb_cu_ue_f1ap_id = 1;
  resp->gnb_du_ue_f1ap_id = 2;
  resp->lmf_ue_meas_id    = 3;
  resp->ran_ue_meas_id    = 4;

  resp->e_c_id_meas_result_present = true;
  asn1::f1ap::e_c_id_measured_results_item_s item;
  auto&                                      aoa = item.e_c_id_measured_results_value.set_value_angleof_arrival_nr();
  aoa.azimuth_ao_a                               = 1800;
  aoa.zenith_ao_a_present                        = true;
  aoa.zenith_ao_a                                = 567;
  resp->e_c_id_meas_result.measured_results_list.push_back(item);

  ocudu::byte_buffer buffer;
  asn1::bit_ref      bref(buffer);
  ASSERT_EQ(pdu.pack(bref), OCUDUASN_SUCCESS);
  ASSERT_EQ(test_pack_roundtrip(pdu), OCUDUASN_SUCCESS);
}

// Real PDUs from lab captures.
static const std::vector<capture_pdu> f1ap_captures = {
    {"F1SetupRequest, MIB, SIB1",
     "00010080c1000005004e00020000002a00020000002d400601806f647530002c00809e0000002b008097480000f11000"
     "0004000000150000010800f11000000083400700004020000001410009de3000004d002e000710113c08050200000302"
     "0d085ede0405c041001008000008000020006015842680c08001880081599830225986cdc10080000e0200010688d490"
     "484c109a4ef18000018838112cc893e0781992affc0493ac21b8637106e28dc61b8e37dfab638750082c0680182c5f41"
     "1400ab000110"},
    {"UEContextSetupRequest, Security Mode Command",
     "0005003f000007002800034003c1002940034003c1003f00090000f1100000040000006b00010000090001000032400a"
     "0900002208106e91539f009e40053007a12000"},
    {"UEContextReleaseCommand",
     "000600280000050028000340010000290003400100000040010a00324009080007100020196d230040400120"},
    {"UEContextModificationRequest",
     "00070080da000006002800020000002900020000000900808420808110ff834067408e9cb405bf7b070e400002c1262e"
     "000307e1a0018dff800b8014c5c4d90000010c80006ad060003003d038f21cf8000105c00ffc007003ff000a8800000d"
     "97c0017c00000200713400000000809a5e0000000e010123e0000000000000a5540004000025965000f2a828f800005a"
     "00780008000000200400812050004c000600004b0001080025002b0000240026102000a4400e00000979008000000100"
     "08000978003e7f000a010000000100000000a1400140009e400530b2d05e00"},
    {"UEContextReleaseRequest", "000a40170000030028000340010300290003400103000040021020"},
    {"InitialULRRCMessageTransfer, RRC Setup Request",
     "000b402f00000500290003400f70006f00090000f1100000040000005f0003005b3c0032000706065c00005ce4004e40"
     "020071"},
    {"DLRRCMessageTransfer, RRC Reconfiguration",
     "000c4080e2000004002800020000002900020000004000012000320080c980c70006068a80409a01e0020de3b0a0080c"
     "0501c2e01584888bd76380832f00058e01a62afe4090682c80204600412088090004011a022487c1c0efc0c10e908002"
     "0014800004624044e390001d9f809fca11ecc0df801a004019cb8040708440024040018c4c00407fc0418184e2011e84"
     "8a4140702a00c0888100400000605e40018048104040425ec0062020084280c00002a041820202020003410202020200"
     "04400947021a5b9d195c9b995d019b5b98cc0c0c419b58d8cc0c0c4119dc1c9cc48040f0657170"},
    {"ULRRCMessageTransfer, UE Capability Information",
     "000d4080a40000040028000200000029000200000040000120003200808b808900044c821ff0680ce811d39680b7ef60"
     "e1c800005824c5c00060fc340031bff001700298b89b2000002190000d5a0c0006007a071e439f000020b801ff800e00"
     "7fe00151000001b2f8002f800000400e268000000010134bc0000001c020247c00000000000014aa8000800004b2ca00"
     "1e55051f00000b400f000100000004008010240a007d9999fe"},
    {"Paging", "00124027000003007500020000007f00074011b2c00004000071400f0000007040090000f1100000040000"},
    {"WriteReplaceWarningRequestSIB7",
     "00140077000004004e00020001008c005e085c42208600301500517a9d429aa5f9fcfc2e4328b2f4f9dcd65789a0f33c"
     "9dc6f51b0586c00000000000000000000000000000000000000000000000000000000000000000000000000000000000"
     "0000000000000000000000000703c0008d0002000a008e0002ffff"},
    {"F1RemovalRequest", "001a0009000001004e00020002"},
    {"F1SetupResponse",
     "40010030000004004e000200000052400903006f6375637030310003001100000004000b4000f1100000040000001500"
     "aa000120"},
    {"UEContextSetupResponse",
     "400500814d0000040028000340010000290003400100002700813200812f5c00b001117aec701061e0007c020c683c08"
     "0004120981950001fffffffffffb06e10840003702ca00418000001034c0808a85003f2812000226aaf9083800208184"
     "0a1839389142c60d1c3c81100000850000800b50001440850001c40b50002004850002804b5000343c850003c3cb5000"
     "4428850004be904014fa41205be905018fa41606be90601cfa41a07be9070201741008869042049725c96302dd08e8ac"
     "f1379340e9041e2efc0c10e0000001c7feb311aa6a8940b000010cbb000000400000000000158456aa28023801148a00"
     "7e21056810038081042003f1082b40401c080841001f88415a0200e060410800fc420ad018070402104007e2105680c0"
     "0280000010430102030403824001f9408000000400800100c001012004401680000481c01138cc3fffffc3d000005e00"
     "0100"},
    {"UEContextReleaseComplete", "400600110000020028000340010000290003400100"},
    {"UEContextModificationResponse",
     "4007007d00000500280003400100002900034001000027004900475c02b091117aec701065e000b1c034c55fc8120d05"
     "900418c00824110120008023405490f8381df81821d23080003000000000000560008005a000012070044e330ffffff0"
     "f480001d401000001c400b00001f7f000a020000010100cc40070000cd40021020"},
    {"WriteReplaceWarningResponse", "4014001c000002004e000200000092000f0000009300090000f1100000019b00"},
    {"F1RemovalResponse", "401a0009000001004e00020002"},
    {"UEContextSetupFailure", "80050016000003002800034001d7002940034001d70000400108"},
};

TEST_F(asn1_f1ap_test, captured_pdus_repack_to_same_bytes)
{
  for (const capture_pdu& c : f1ap_captures) {
    EXPECT_EQ(test_unpack_roundtrip<f1ap::f1ap_pdu_c>(hex_to_bytes(c.hex)), OCUDUASN_SUCCESS) << c.name;
  }
}
