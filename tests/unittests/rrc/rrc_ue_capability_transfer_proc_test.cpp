// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rrc_ue_test_helpers.h"
#include "rrc_ue_test_messages.h"
#include "tests/test_doubles/rrc/redcap_ue_capability_fixture.h"
#include "tests/test_doubles/rrc/rrc_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/rrc_nr/rrc_nr.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/support/error_handling.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Fixture class RRC Setup tests preparation
class rrc_ue_capability_transfer_proc_test : public rrc_ue_test_helper, public ::testing::Test
{
protected:
  static void SetUpTestSuite() { ocudulog::init(); }

  void SetUp() override
  {
    init();

    ocudulog::basic_logger& rrc_logger = ocudulog::fetch_basic_logger("RRC", false);
    rrc_logger.set_level(ocudulog::basic_levels::debug);
    rrc_logger.set_hex_dump_max_size(30);

    receive_setup_request();

    // check if the RRC setup message was generated
    ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

    // check if SRB1 was created
    check_srb1_exists();

    receive_setup_complete();

    // setup security
    const char* sk_gnb_cstr = "45cbc3f8a81193fd5c5229300d59edf812e998a115ec4e0ce903ba89367e2628";
    const char* k_enc_cstr  = "4ea96992c8c7e82977231ad001309062ae9f31ead90a4d0842af6cd25cb44dc4";
    const char* k_int_cstr  = "aeeb5e0ae02c6188ecb1625c4a9e022fdfc2a1fc845b44b44443ac9a3bda667c";

    // Pack hex strings into ocudu types
    security::sec_key sk_gnb = make_sec_key(sk_gnb_cstr);
    security::sec_key k_enc  = make_sec_key(k_enc_cstr);
    security::sec_key k_int  = make_sec_key(k_int_cstr);

    // Create expected SRB1 sec config
    security::sec_as_config sec_cfg = {};
    sec_cfg.domain                  = security::sec_domain::rrc;
    sec_cfg.integ_algo              = security::integrity_algorithm::nia2;
    sec_cfg.cipher_algo             = security::ciphering_algorithm::nea0;
    sec_cfg.k_enc                   = k_enc;
    sec_cfg.k_int                   = k_int;

    // Initialize security context and capabilities.
    security::security_context init_sec_ctx = {};
    init_sec_ctx.k                          = sk_gnb;
    std::fill(init_sec_ctx.supported_int_algos.begin(), init_sec_ctx.supported_int_algos.end(), true);
    std::fill(init_sec_ctx.supported_enc_algos.begin(), init_sec_ctx.supported_enc_algos.end(), true);

    // Initialize security context.
    ASSERT_TRUE(init_security_context());

    // Enable security
    enable_security();
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }

  /// Packs UE capabilities into the UERadioAccessCapabilityInformation that the AMF sends in the UE Radio Capability
  /// IE.
  static byte_buffer pack_ue_radio_access_cap_info(bool rrc_inactive_supported)
  {
    asn1::rrc_nr::ul_dcch_msg_s ue_cap_info = test_helpers::create_ue_capability_info(0, rrc_inactive_supported);

    byte_buffer rat_container_list;
    {
      asn1::bit_ref bref{rat_container_list};
      report_fatal_error_if_not(
          asn1::pack_dyn_seq_of(
              bref, ue_cap_info.msg.c1().ue_cap_info().crit_exts.ue_cap_info().ue_cap_rat_container_list, 0, 8) ==
              asn1::OCUDUASN_SUCCESS,
          "Failed to pack UE Capability RAT Container List");
    }

    asn1::rrc_nr::ue_radio_access_cap_info_s ue_radio_access_cap_info;
    ue_radio_access_cap_info.crit_exts.set_c1().set_ue_radio_access_cap_info().ue_radio_access_cap_info =
        std::move(rat_container_list);

    byte_buffer   pdu;
    asn1::bit_ref bref{pdu};
    report_fatal_error_if_not(ue_radio_access_cap_info.pack(bref) == asn1::OCUDUASN_SUCCESS,
                              "Failed to pack UERadioAccessCapabilityInformation");
    return pdu;
  }
};

/// Test the RRC setup with connected AMF
TEST_F(rrc_ue_capability_transfer_proc_test,
       when_ue_capability_enquiry_is_sent_then_ue_capability_information_is_received)
{
  // Trigger UE capability transfer
  rrc_ue_capability_transfer_request msg;
  async_task<bool>         t = get_rrc_ue_control_message_handler()->handle_rrc_ue_capability_transfer_request(msg);
  lazy_task_launcher<bool> t_launcher(t);

  ASSERT_FALSE(t.ready());
  check_rrc_ue_enquiry_pdu(2);

  // Receive UE capability information
  receive_ue_capability_information(2);

  ASSERT_TRUE(t.ready());
}

TEST_F(rrc_ue_capability_transfer_proc_test,
       when_ue_capabilities_with_rrc_inactive_are_stored_from_amf_then_rrc_inactive_is_supported)
{
  ASSERT_FALSE(rrc_ue->is_rrc_inactive_supported());

  ASSERT_TRUE(get_rrc_ue_control_message_handler()->store_ue_capabilities(pack_ue_radio_access_cap_info(true)));

  ASSERT_TRUE(rrc_ue->is_rrc_inactive_supported());
}

TEST_F(rrc_ue_capability_transfer_proc_test,
       when_ue_capabilities_without_rrc_inactive_are_stored_from_amf_then_rrc_inactive_is_not_supported)
{
  ASSERT_TRUE(get_rrc_ue_control_message_handler()->store_ue_capabilities(pack_ue_radio_access_cap_info(false)));

  ASSERT_FALSE(rrc_ue->is_rrc_inactive_supported());
}

static byte_buffer wrap_radio_access_capabilities(byte_buffer nr_capability)
{
  asn1::rrc_nr::ue_cap_rat_container_list_l list;
  list.resize(1);
  list[0].rat_type             = asn1::rrc_nr::rat_type_opts::nr;
  list[0].ue_cap_rat_container = std::move(nr_capability);
  byte_buffer   packed_list;
  asn1::bit_ref writer{packed_list};
  report_fatal_error_if_not(asn1::pack_dyn_seq_of(writer, list, 0, 8) == asn1::OCUDUASN_SUCCESS,
                            "Invalid test capability list");
  asn1::rrc_nr::ue_radio_access_cap_info_s info;
  info.crit_exts.set_c1().set_ue_radio_access_cap_info().ue_radio_access_cap_info = std::move(packed_list);
  byte_buffer   packed;
  asn1::bit_ref info_writer{packed};
  report_fatal_error_if_not(info.pack(info_writer) == asn1::OCUDUASN_SUCCESS, "Invalid test capability information");
  return packed;
}

TEST_F(rrc_ue_capability_transfer_proc_test, cached_capabilities_update_the_decoded_summary)
{
  auto                      encoded = test_helpers::make_captured_redcap_nr_capability();
  asn1::rrc_nr::ue_nr_cap_s cap;
  asn1::cbit_ref            reader{encoded};
  ASSERT_EQ(cap.unpack(reader), asn1::OCUDUASN_SUCCESS);
  cap.non_crit_ext_present                = true;
  cap.non_crit_ext.inactive_state_present = true;
  byte_buffer   packed;
  asn1::bit_ref writer{packed};
  ASSERT_EQ(cap.pack(writer), asn1::OCUDUASN_SUCCESS);
  ASSERT_TRUE(rrc_ue->store_ue_capabilities(wrap_radio_access_capabilities(std::move(packed))));
  EXPECT_TRUE(rrc_ue->is_rrc_inactive_supported());
  ASSERT_TRUE(rrc_ue->get_capabilities().redcap.has_value());
  const auto previous = rrc_ue->get_packed_ue_capability_rat_container_list();
  EXPECT_FALSE(rrc_ue->store_ue_capabilities(wrap_radio_access_capabilities(byte_buffer::create({0xff}).value())));
  EXPECT_EQ(rrc_ue->get_packed_ue_capability_rat_container_list(), previous);
  EXPECT_TRUE(rrc_ue->is_rrc_inactive_supported());
  ASSERT_TRUE(rrc_ue->get_capabilities().redcap.has_value());
  cap.non_crit_ext.inactive_state_present = false;
  byte_buffer   replacement;
  asn1::bit_ref replacement_writer{replacement};
  ASSERT_EQ(cap.pack(replacement_writer), asn1::OCUDUASN_SUCCESS);
  ASSERT_TRUE(rrc_ue->store_ue_capabilities(wrap_radio_access_capabilities(std::move(replacement))));
  EXPECT_FALSE(rrc_ue->is_rrc_inactive_supported());
}

TEST_F(rrc_ue_capability_transfer_proc_test, malformed_cached_nr_capabilities_are_rejected)
{
  EXPECT_FALSE(rrc_ue->store_ue_capabilities(wrap_radio_access_capabilities(byte_buffer::create({0xff}).value())));
  EXPECT_TRUE(rrc_ue->get_packed_ue_capability_rat_container_list().empty());
}

TEST_F(rrc_ue_capability_transfer_proc_test, unsupported_cached_capability_message_choice_is_rejected)
{
  asn1::rrc_nr::ue_radio_access_cap_info_s info;
  info.crit_exts.set_crit_exts_future();
  byte_buffer   packed;
  asn1::bit_ref writer{packed};
  ASSERT_EQ(info.pack(writer), asn1::OCUDUASN_SUCCESS);
  EXPECT_FALSE(rrc_ue->store_ue_capabilities(std::move(packed)));
  EXPECT_TRUE(rrc_ue->get_packed_ue_capability_rat_container_list().empty());
}
