// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "common/e2ap_asn1_packer.h"
#include "lib/e2/common/e2ap_asn1_utils.h"
#include "lib/e2/e2sm/e2sm_ccc/e2sm_ccc_asn1_packer.h"
#include "lib/e2/procedures/e2ap_ric_control_procedure.h"
#include "tests/unittests/e2/common/e2_test_helpers.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/support/test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;

TEST_F(e2_test_setup, ric_control_procedure_setup)
{
  ocudulog::fetch_basic_logger("TEST").set_level(ocudulog::basic_levels::debug);
  ocudulog::init();
  test_logger.info("Launch e2 setup request procedure with task worker...");
  e2->handle_e2_tnl_connection_request();

  // Generate e2sm-rc PRB-quota control message.
  std::vector<rrm_policy_ratio_group> rrm_policy_ratio_list_def;
  rrm_policy_ratio_group              rrm_policy_ratio_grp_def;
  rrm_policy_ratio_grp_def.minimum_ratio   = 10;
  rrm_policy_ratio_grp_def.maximum_ratio   = 20;
  rrm_policy_ratio_grp_def.dedicated_ratio = 30;
  rrm_policy_ratio_grp_def.policy_members_list.resize(2);
  rrm_policy_ratio_grp_def.policy_members_list[0].plmn_id = plmn_identity::parse("11111").value();
  rrm_policy_ratio_grp_def.policy_members_list[0].s_nssai =
      s_nssai_t{slice_service_type{1}, slice_differentiator::create(1).value()};
  rrm_policy_ratio_grp_def.policy_members_list[1].plmn_id = plmn_identity::parse("00202").value();
  rrm_policy_ratio_grp_def.policy_members_list[1].s_nssai =
      s_nssai_t{slice_service_type{21}, slice_differentiator::create(21).value()};

  rrm_policy_ratio_list_def.push_back(rrm_policy_ratio_grp_def);
  rrm_policy_ratio_grp_def.minimum_ratio   = 14;
  rrm_policy_ratio_grp_def.maximum_ratio   = 25;
  rrm_policy_ratio_grp_def.dedicated_ratio = 36;
  rrm_policy_ratio_grp_def.policy_members_list.resize(1);
  rrm_policy_ratio_grp_def.policy_members_list[0].plmn_id = plmn_identity::parse("99999").value();
  rrm_policy_ratio_grp_def.policy_members_list[0].s_nssai =
      s_nssai_t{slice_service_type{3}, slice_differentiator::create(3).value()};
  rrm_policy_ratio_list_def.push_back(rrm_policy_ratio_grp_def);
  e2_message request_msg = generate_ric_control_request_style2_action6(test_logger, rrm_policy_ratio_list_def);
  e2->handle_message(request_msg);

  e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(msg.pdu.successful_outcome().value.type(),
            asn1::e2ap::e2ap_elem_procs_o::successful_outcome_c::types_opts::ric_ctrl_ack);
  auto ack = msg.pdu.successful_outcome().value.ric_ctrl_ack();
  ASSERT_EQ(ack->ran_function_id, 3);
  e2->handle_e2_disconnection_request();

  odu::du_mac_sched_control_config& ctrl_config = du_rc_param_configurator->config;
  ASSERT_EQ(ctrl_config.rrm_policy_ratio_list.size(), rrm_policy_ratio_list_def.size());
  for (unsigned g = 0, k = ctrl_config.rrm_policy_ratio_list.size(); g < k; ++g) {
    const auto& rrm_policy_ratio_def = rrm_policy_ratio_list_def[g];
    const auto& rrm_policy_ratio     = ctrl_config.rrm_policy_ratio_list[g];
    ASSERT_EQ(*rrm_policy_ratio.minimum_ratio, rrm_policy_ratio_def.minimum_ratio);
    ASSERT_EQ(*rrm_policy_ratio.maximum_ratio, rrm_policy_ratio_def.maximum_ratio);
    ASSERT_EQ(*rrm_policy_ratio.dedicated_ratio, rrm_policy_ratio_def.dedicated_ratio);
    ASSERT_EQ(rrm_policy_ratio.policy_members_list.size(), rrm_policy_ratio_def.policy_members_list.size());
    for (unsigned i = 0, e = rrm_policy_ratio.policy_members_list.size(); i < e; ++i) {
      const auto& policy_member_def = rrm_policy_ratio_def.policy_members_list[i];
      const auto& policy_member     = rrm_policy_ratio.policy_members_list[i];
      ASSERT_EQ(policy_member.plmn_id, policy_member_def.plmn_id);
      ASSERT_EQ(policy_member.s_nssai, policy_member_def.s_nssai);
    }
  }
}

static std::vector<rrm_policy_ratio_group> make_single_member_policy_list()
{
  std::vector<rrm_policy_ratio_group> rrm_policy_ratio_list_def(1);
  rrm_policy_ratio_list_def[0].minimum_ratio   = 10;
  rrm_policy_ratio_list_def[0].maximum_ratio   = 20;
  rrm_policy_ratio_list_def[0].dedicated_ratio = 30;
  rrm_policy_ratio_list_def[0].policy_members_list.resize(1);
  rrm_policy_ratio_list_def[0].policy_members_list[0].plmn_id = plmn_identity::parse("00101").value();
  rrm_policy_ratio_list_def[0].policy_members_list[0].s_nssai =
      s_nssai_t{slice_service_type{1}, slice_differentiator::create(1).value()};
  return rrm_policy_ratio_list_def;
}

TEST_F(e2_test_setup, ric_control_procedure_rejects_invalid_plmn)
{
  e2->handle_e2_tnl_connection_request();

  // BCD digits 0xa to 0xf do not encode a valid PLMN identity.
  rrm_policy_member_raw_encoding raw_encoding;
  raw_encoding.plmn_octets = std::vector<uint8_t>{0xaa, 0xaa, 0xaa};

  e2_message request_msg =
      generate_ric_control_request_style2_action6(test_logger, make_single_member_policy_list(), raw_encoding);
  e2->handle_message(request_msg);

  e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(msg.pdu.unsuccessful_outcome().value.type(),
            asn1::e2ap::e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ric_ctrl_fail);
  const auto& fail = msg.pdu.unsuccessful_outcome().value.ric_ctrl_fail();
  ASSERT_EQ(fail->cause.type().value, asn1::e2ap::cause_c::types_opts::ric_request);
  ASSERT_EQ(fail->cause.ric_request().value, asn1::e2ap::cause_ric_request_e::options::ctrl_msg_invalid);
  ASSERT_TRUE(du_rc_param_configurator->config.rrm_policy_ratio_list.empty());
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_setup, ric_control_procedure_rejects_oversized_sd)
{
  e2->handle_e2_tnl_connection_request();

  // An SD is an OCTET STRING (SIZE(3)), as per TS 38.473, Section 9.3.1.38.
  rrm_policy_member_raw_encoding raw_encoding;
  raw_encoding.sd_octets = std::vector<uint8_t>{0x01, 0x00, 0x00, 0x00};

  e2_message request_msg =
      generate_ric_control_request_style2_action6(test_logger, make_single_member_policy_list(), raw_encoding);
  e2->handle_message(request_msg);

  e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(msg.pdu.unsuccessful_outcome().value.type(),
            asn1::e2ap::e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ric_ctrl_fail);
  const auto& fail = msg.pdu.unsuccessful_outcome().value.ric_ctrl_fail();
  ASSERT_EQ(fail->cause.type().value, asn1::e2ap::cause_c::types_opts::ric_request);
  ASSERT_EQ(fail->cause.ric_request().value, asn1::e2ap::cause_ric_request_e::options::ctrl_msg_invalid);
  ASSERT_TRUE(du_rc_param_configurator->config.rrm_policy_ratio_list.empty());
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_setup, ric_control_procedure_rejects_out_of_range_prb_ratio)
{
  e2->handle_e2_tnl_connection_request();

  // A PRB policy ratio is an INTEGER (0..100), as per O-RAN.WG3.TS.E2SM-RC, Section 8.4.3.6.
  rrm_policy_member_raw_encoding raw_encoding;
  raw_encoding.min_prb_ratio = -1;

  e2_message request_msg =
      generate_ric_control_request_style2_action6(test_logger, make_single_member_policy_list(), raw_encoding);
  e2->handle_message(request_msg);

  e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(msg.pdu.unsuccessful_outcome().value.type(),
            asn1::e2ap::e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ric_ctrl_fail);
  const auto& fail = msg.pdu.unsuccessful_outcome().value.ric_ctrl_fail();
  ASSERT_EQ(fail->cause.type().value, asn1::e2ap::cause_c::types_opts::ric_request);
  ASSERT_EQ(fail->cause.ric_request().value, asn1::e2ap::cause_ric_request_e::options::ctrl_msg_invalid);
  ASSERT_TRUE(du_rc_param_configurator->config.rrm_policy_ratio_list.empty());
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_setup, ric_control_procedure_rejects_oversized_sst)
{
  e2->handle_e2_tnl_connection_request();

  // An SST is an OCTET STRING (SIZE(1)), as per TS 38.473, Section 9.3.1.38.
  rrm_policy_member_raw_encoding raw_encoding;
  raw_encoding.sst_octets = std::vector<uint8_t>{0x01, 0x00};

  e2_message request_msg =
      generate_ric_control_request_style2_action6(test_logger, make_single_member_policy_list(), raw_encoding);
  e2->handle_message(request_msg);

  e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(msg.pdu.unsuccessful_outcome().value.type(),
            asn1::e2ap::e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ric_ctrl_fail);
  const auto& fail = msg.pdu.unsuccessful_outcome().value.ric_ctrl_fail();
  ASSERT_EQ(fail->cause.type().value, asn1::e2ap::cause_c::types_opts::ric_request);
  ASSERT_EQ(fail->cause.ric_request().value, asn1::e2ap::cause_ric_request_e::options::ctrl_msg_invalid);
  ASSERT_TRUE(du_rc_param_configurator->config.rrm_policy_ratio_list.empty());
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_setup, ric_control_procedure_rejects_ran_parameter_structure)
{
  e2->handle_e2_tnl_connection_request();

  // A PLMN identity is defined as an element, so a structure must not be read as one.
  rrm_policy_member_raw_encoding raw_encoding;
  raw_encoding.plmn_as_structure = true;

  e2_message request_msg =
      generate_ric_control_request_style2_action6(test_logger, make_single_member_policy_list(), raw_encoding);
  e2->handle_message(request_msg);

  e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(msg.pdu.unsuccessful_outcome().value.type(),
            asn1::e2ap::e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ric_ctrl_fail);
  const auto& fail = msg.pdu.unsuccessful_outcome().value.ric_ctrl_fail();
  ASSERT_EQ(fail->cause.type().value, asn1::e2ap::cause_c::types_opts::ric_request);
  ASSERT_EQ(fail->cause.ric_request().value, asn1::e2ap::cause_ric_request_e::options::ctrl_msg_invalid);
  ASSERT_TRUE(du_rc_param_configurator->config.rrm_policy_ratio_list.empty());
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_setup, ric_control_procedure_rejects_wrong_ran_parameter_value_type)
{
  e2->handle_e2_tnl_connection_request();

  // A PLMN identity is defined as an octet string. Reading an integer as one would reinterpret its storage.
  rrm_policy_member_raw_encoding raw_encoding;
  raw_encoding.plmn_as_integer = true;

  e2_message request_msg =
      generate_ric_control_request_style2_action6(test_logger, make_single_member_policy_list(), raw_encoding);
  e2->handle_message(request_msg);

  e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(msg.pdu.unsuccessful_outcome().value.type(),
            asn1::e2ap::e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ric_ctrl_fail);
  const auto& fail = msg.pdu.unsuccessful_outcome().value.ric_ctrl_fail();
  ASSERT_EQ(fail->cause.type().value, asn1::e2ap::cause_c::types_opts::ric_request);
  ASSERT_EQ(fail->cause.ric_request().value, asn1::e2ap::cause_ric_request_e::options::ctrl_msg_invalid);
  ASSERT_TRUE(du_rc_param_configurator->config.rrm_policy_ratio_list.empty());
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_setup, ric_control_procedure_rejects_absent_ran_parameter_value)
{
  e2->handle_e2_tnl_connection_request();

  rrm_policy_member_raw_encoding raw_encoding;
  raw_encoding.plmn_value_absent = true;

  e2_message request_msg =
      generate_ric_control_request_style2_action6(test_logger, make_single_member_policy_list(), raw_encoding);
  e2->handle_message(request_msg);

  e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(msg.pdu.unsuccessful_outcome().value.type(),
            asn1::e2ap::e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ric_ctrl_fail);
  const auto& fail = msg.pdu.unsuccessful_outcome().value.ric_ctrl_fail();
  ASSERT_EQ(fail->cause.type().value, asn1::e2ap::cause_c::types_opts::ric_request);
  ASSERT_EQ(fail->cause.ric_request().value, asn1::e2ap::cause_ric_request_e::options::ctrl_msg_invalid);
  ASSERT_TRUE(du_rc_param_configurator->config.rrm_policy_ratio_list.empty());
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_setup, ric_control_procedure_rejects_policy_member_without_group)
{
  e2->handle_e2_tnl_connection_request();

  // A RRM Policy Member (param 6) belongs to a RRM Policy Ratio Group (param 2).
  e2_message request_msg = generate_ric_control_request_style2_action6_flat(test_logger, {6});
  e2->handle_message(request_msg);

  e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(msg.pdu.unsuccessful_outcome().value.type(),
            asn1::e2ap::e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ric_ctrl_fail);
  const auto& fail = msg.pdu.unsuccessful_outcome().value.ric_ctrl_fail();
  ASSERT_EQ(fail->cause.type().value, asn1::e2ap::cause_c::types_opts::ric_request);
  ASSERT_EQ(fail->cause.ric_request().value, asn1::e2ap::cause_ric_request_e::options::ctrl_msg_invalid);
  ASSERT_TRUE(du_rc_param_configurator->config.rrm_policy_ratio_list.empty());
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_setup, ric_control_procedure_rejects_plmn_without_policy_member)
{
  e2->handle_e2_tnl_connection_request();

  // A PLMN identity (param 7) belongs to a RRM Policy Member (param 6).
  e2_message request_msg = generate_ric_control_request_style2_action6_flat(test_logger, {2, 7});
  e2->handle_message(request_msg);

  e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(msg.pdu.unsuccessful_outcome().value.type(),
            asn1::e2ap::e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ric_ctrl_fail);
  const auto& fail = msg.pdu.unsuccessful_outcome().value.ric_ctrl_fail();
  ASSERT_EQ(fail->cause.type().value, asn1::e2ap::cause_c::types_opts::ric_request);
  ASSERT_EQ(fail->cause.ric_request().value, asn1::e2ap::cause_ric_request_e::options::ctrl_msg_invalid);
  ASSERT_TRUE(du_rc_param_configurator->config.rrm_policy_ratio_list.empty());
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_setup, ric_control_procedure_fail)
{
  e2->handle_e2_tnl_connection_request();
  e2_message request_msg = generate_ric_control_request(test_logger, 2, 6, 1, 12);
  e2->handle_message(request_msg);

  e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(msg.pdu.unsuccessful_outcome().value.type(),
            asn1::e2ap::e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ric_ctrl_fail);
  auto ack = msg.pdu.unsuccessful_outcome().value.ric_ctrl_fail();
  ASSERT_EQ(ack->ran_function_id, 3);
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_setup, ric_control_procedure_packed)
{
  e2->handle_e2_tnl_connection_request();
  uint8_t e2ap_ctrl_req[] = {
      0x00, 0x04, 0x00, 0x78, 0x00, 0x00, 0x05, 0x00, 0x1d, 0x00, 0x05, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x05,
      0x00, 0x02, 0x00, 0x03, 0x00, 0x16, 0x00, 0x09, 0x08, 0x01, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x05, 0x00,
      0x17, 0x00, 0x50, 0x4f, 0x00, 0x00, 0x01, 0x00, 0x00, 0x60, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x01, 0x44,
      0x00, 0x03, 0x00, 0x02, 0x44, 0x00, 0x00, 0x00, 0x04, 0x60, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x05, 0x44,
      0x00, 0x01, 0x00, 0x06, 0x2a, 0x00, 0x03, 0x21, 0xf3, 0x54, 0x00, 0x07, 0x44, 0x00, 0x01, 0x00, 0x08, 0x2a,
      0x00, 0x01, 0x16, 0x00, 0x09, 0x2a, 0x00, 0x03, 0x00, 0x00, 0x21, 0x00, 0x0a, 0x28, 0x80, 0x01, 0x0a, 0x00,
      0x0b, 0x28, 0x80, 0x01, 0x1e, 0x00, 0x0c, 0x28, 0x80, 0x01, 0x64, 0x00, 0x15, 0x00, 0x01, 0x40};
  byte_buffer e2ap_ctrl_req_buf = byte_buffer::create(e2ap_ctrl_req, e2ap_ctrl_req + sizeof(e2ap_ctrl_req)).value();
  packer->handle_packed_pdu(std::move(e2ap_ctrl_req_buf));

  e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::successful_outcome);
  e2->handle_e2_disconnection_request();

  odu::du_mac_sched_control_config& ctrl_config = du_rc_param_configurator->config;

  ASSERT_EQ(ctrl_config.rrm_policy_ratio_list.size(), 1);
  const auto& rrm_policy_ratio = ctrl_config.rrm_policy_ratio_list[0];
  ASSERT_EQ(*rrm_policy_ratio.minimum_ratio, 10);
  ASSERT_EQ(*rrm_policy_ratio.maximum_ratio, 30);
  ASSERT_EQ(*rrm_policy_ratio.dedicated_ratio, 100);
  ASSERT_EQ(rrm_policy_ratio.policy_members_list.size(), 1);
  const auto& policy_member = rrm_policy_ratio.policy_members_list[0];
  ASSERT_EQ(policy_member.plmn_id.to_string(), "12345");
  ASSERT_EQ(policy_member.s_nssai.sst.value(), 22);
  ASSERT_EQ(policy_member.s_nssai.sd.value(), 33);
}

TEST_F(e2_test_setup, ric_control_procedure_ho)
{
  e2->handle_e2_tnl_connection_request();
  uint8_t     e2_ctrl_req[] = {0x00, 0x04, 0x00, 0x53, 0x00, 0x00, 0x05, 0x00, 0x1d, 0x00, 0x05, 0x00, 0x00, 0x01, 0x00,
                               0x00, 0x00, 0x05, 0x00, 0x02, 0x00, 0x03, 0x00, 0x16, 0x00, 0x13, 0x12, 0x00, 0x40, 0x00,
                               0x07, 0x00, 0x00, 0xf1, 0x10, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x00, 0x00, 0x00,
                               0x00, 0x17, 0x00, 0x21, 0x20, 0x00, 0x00, 0x01, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x01,
                               0x44, 0x00, 0x00, 0x00, 0x02, 0x44, 0x00, 0x00, 0x00, 0x03, 0x2a, 0x00, 0x09, 0x00, 0x00,
                               0xf1, 0x10, 0x00, 0x00, 0x01, 0x9b, 0x10, 0x00, 0x15, 0x00, 0x01, 0x40};
  byte_buffer e2_ctrl_req_buf = byte_buffer::create(e2_ctrl_req, e2_ctrl_req + sizeof(e2_ctrl_req)).value();
  packer->handle_packed_pdu(std::move(e2_ctrl_req_buf));
  e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::successful_outcome);
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_setup, ric_control_procedure_ho_translates_plmn_identities)
{
  e2->handle_e2_tnl_connection_request();

  // A PLMN other than the default one the GUAMI and the target cell carry when they are left untranslated.
  const plmn_identity plmn = plmn_identity::parse("00102").value();

  e2_message request_msg = generate_ric_control_request_style3_action1(test_logger, plmn.to_bytes(), {plmn.to_bytes()});
  e2->handle_message(request_msg);

  e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(cu_rc_param_configurator->last_guami.plmn, plmn);
  ASSERT_EQ(cu_rc_param_configurator->last_handover_req.cgi.plmn_id, plmn);
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_setup, ric_control_procedure_ho_rejects_invalid_guami_plmn)
{
  e2->handle_e2_tnl_connection_request();

  const plmn_identity plmn = plmn_identity::parse("00102").value();

  // BCD digits 0xa to 0xf do not encode a valid PLMN identity.
  e2_message request_msg = generate_ric_control_request_style3_action1(
      test_logger, std::array<uint8_t, 3>{0xaa, 0xaa, 0xaa}, {plmn.to_bytes()});
  e2->handle_message(request_msg);

  e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(msg.pdu.unsuccessful_outcome().value.type(),
            asn1::e2ap::e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ric_ctrl_fail);
  // A UE must not be looked up, let alone handed over, on a GUAMI that could not be translated.
  ASSERT_FALSE(cu_rc_param_configurator->ue_index_requested);
  ASSERT_FALSE(cu_rc_param_configurator->handover_triggered);
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_setup, ric_control_procedure_ho_rejects_invalid_target_cell)
{
  e2->handle_e2_tnl_connection_request();

  const plmn_identity plmn = plmn_identity::parse("00102").value();

  // BCD digits 0xa to 0xf do not encode a valid PLMN identity. The valid target the request names first must not be
  // handed over to in its place.
  e2_message request_msg = generate_ric_control_request_style3_action1(
      test_logger, plmn.to_bytes(), {plmn.to_bytes(), std::array<uint8_t, 3>{0xaa, 0xaa, 0xaa}});
  e2->handle_message(request_msg);

  e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(msg.pdu.unsuccessful_outcome().value.type(),
            asn1::e2ap::e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ric_ctrl_fail);
  ASSERT_FALSE(cu_rc_param_configurator->handover_triggered);
  e2->handle_e2_disconnection_request();
}

namespace {

/// Selects the content of the RIC Control Ack Request IE of a RIC CONTROL REQUEST.
enum class ack_request { absent, ack, no_ack };

void set_ack_request(e2_message& request_msg, ack_request ack_ie)
{
  auto& request                         = request_msg.pdu.init_msg().value.ric_ctrl_request();
  request->ric_ctrl_ack_request_present = ack_ie != ack_request::absent;
  request->ric_ctrl_ack_request.value   = ack_ie == ack_request::no_ack ? asn1::e2ap::ric_ctrl_ack_request_e::no_ack
                                                                        : asn1::e2ap::ric_ctrl_ack_request_e::ack;
}

/// Returns a PRB-quota list that the E2SM-RC control service accepts.
std::vector<rrm_policy_ratio_group> make_rrm_policy_ratio_list()
{
  rrm_policy_ratio_group group;
  group.minimum_ratio   = 10;
  group.maximum_ratio   = 20;
  group.dedicated_ratio = 30;
  group.policy_members_list.resize(1);
  group.policy_members_list[0].plmn_id = plmn_identity::parse("11111").value();
  group.policy_members_list[0].s_nssai = s_nssai_t{slice_service_type{1}, slice_differentiator::create(1).value()};
  return {group};
}

} // namespace

TEST_F(e2_test_setup, ric_control_procedure_without_ack_request_is_acknowledged)
{
  e2->handle_e2_tnl_connection_request();
  e2_message request_msg = generate_ric_control_request_style2_action6(test_logger, make_rrm_policy_ratio_list());
  set_ack_request(request_msg, ack_request::absent);
  e2->handle_message(request_msg);

  e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(msg.pdu.successful_outcome().value.type(),
            asn1::e2ap::e2ap_elem_procs_o::successful_outcome_c::types_opts::ric_ctrl_ack);
  ASSERT_EQ(du_rc_param_configurator->config.rrm_policy_ratio_list.size(), 1);
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_setup, ric_control_procedure_with_no_ack_request_is_not_acknowledged)
{
  e2->handle_e2_tnl_connection_request();
  e2_message request_msg = generate_ric_control_request_style2_action6(test_logger, make_rrm_policy_ratio_list());
  set_ack_request(request_msg, ack_request::no_ack);
  e2->handle_message(request_msg);

  // Neither a RIC CONTROL ACKNOWLEDGE nor a RIC CONTROL FAILURE is sent.
  const e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_NE(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::successful_outcome);
  ASSERT_NE(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome);

  // The control is executed all the same.
  ASSERT_EQ(du_rc_param_configurator->config.rrm_policy_ratio_list.size(), 1);
  e2->handle_e2_disconnection_request();
}

namespace {

/// Returns whether the last PDU sent by the E2 agent is a RIC CONTROL FAILURE.
bool is_ric_control_failure(const e2_message& msg)
{
  return msg.pdu.type().value == asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome and
         msg.pdu.unsuccessful_outcome().value.type() ==
             asn1::e2ap::e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ric_ctrl_fail;
}

} // namespace

TEST_F(e2_test_setup, ric_control_procedure_failure_is_reported_without_ack_request)
{
  e2->handle_e2_tnl_connection_request();
  e2_message request_msg = generate_ric_control_request(test_logger, 2, 6, 1, 12);
  set_ack_request(request_msg, ack_request::absent);
  e2->handle_message(request_msg);

  ASSERT_TRUE(is_ric_control_failure(e2_client->last_tx_e2_pdu));
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_setup, ric_control_procedure_failure_is_reported_with_no_ack_request)
{
  e2->handle_e2_tnl_connection_request();
  e2_message request_msg = generate_ric_control_request(test_logger, 2, 6, 1, 12);
  set_ack_request(request_msg, ack_request::no_ack);
  e2->handle_message(request_msg);

  ASSERT_TRUE(is_ric_control_failure(e2_client->last_tx_e2_pdu));
  e2->handle_e2_disconnection_request();
}

namespace {

/// Returns the RIC Request cause of the RIC CONTROL FAILURE sent by the E2 agent.
asn1::e2ap::cause_ric_request_e::options get_ric_request_cause(const e2_message& msg)
{
  const auto& failure = msg.pdu.unsuccessful_outcome().value.ric_ctrl_fail();
  EXPECT_EQ(failure->cause.type().value, asn1::e2ap::cause_c::types_opts::ric_request);
  return failure->cause.ric_request().value;
}

} // namespace

TEST_F(e2_test_setup, ric_control_procedure_with_unknown_ran_function_is_failed)
{
  e2->handle_e2_tnl_connection_request();
  e2_message request_msg = generate_ric_control_request_style2_action6(test_logger, make_rrm_policy_ratio_list());
  request_msg.pdu.init_msg().value.ric_ctrl_request()->ran_function_id = 99;
  e2->handle_message(request_msg);

  ASSERT_TRUE(is_ric_control_failure(e2_client->last_tx_e2_pdu));
  ASSERT_EQ(get_ric_request_cause(e2_client->last_tx_e2_pdu), asn1::e2ap::cause_ric_request_e::ran_function_id_invalid);
  ASSERT_TRUE(du_rc_param_configurator->config.rrm_policy_ratio_list.empty());
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_setup, ric_control_procedure_with_unknown_control_style_is_failed)
{
  e2->handle_e2_tnl_connection_request();
  e2_message request_msg = generate_ric_control_request(test_logger, 99, 6, 1, 12);
  e2->handle_message(request_msg);

  ASSERT_TRUE(is_ric_control_failure(e2_client->last_tx_e2_pdu));
  ASSERT_EQ(get_ric_request_cause(e2_client->last_tx_e2_pdu), asn1::e2ap::cause_ric_request_e::action_not_supported);
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_setup, ric_control_procedure_with_unsupported_request_is_failed)
{
  e2->handle_e2_tnl_connection_request();
  e2_message request_msg = generate_ric_control_request_style2_action6(test_logger, make_rrm_policy_ratio_list());

  // The control header of style 2 is paired with a control message format that the style does not use.
  asn1::e2sm::e2sm_rc_ctrl_msg_s ctrl_msg;
  ctrl_msg.ric_ctrl_msg_formats.set_ctrl_msg_format4();
  ocudu::byte_buffer ctrl_msg_buff;
  asn1::bit_ref      bref(ctrl_msg_buff);
  ASSERT_EQ(asn1::pack(bref, ctrl_msg), asn1::OCUDUASN_SUCCESS);
  auto& request = request_msg.pdu.init_msg().value.ric_ctrl_request();
  ASSERT_TRUE(request->ric_ctrl_msg.resize(ctrl_msg_buff.length()));
  std::copy(ctrl_msg_buff.begin(), ctrl_msg_buff.end(), request->ric_ctrl_msg.begin());
  e2->handle_message(request_msg);

  ASSERT_TRUE(is_ric_control_failure(e2_client->last_tx_e2_pdu));
  ASSERT_EQ(get_ric_request_cause(e2_client->last_tx_e2_pdu), asn1::e2ap::cause_ric_request_e::ctrl_msg_invalid);
  ASSERT_TRUE(du_rc_param_configurator->config.rrm_policy_ratio_list.empty());
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_setup, ric_control_failure_carries_the_control_outcome)
{
  e2_message                 sent_msg;
  dummy_e2_tx_pdu_notifier   notifier(sent_msg, []() {});
  e2_ric_control_request     request = {};
  e2ap_ric_control_procedure procedure(request, notifier, *e2sm_mngr, test_logger);

  const std::array<uint8_t, 3> outcome = {0x01, 0x02, 0x03};
  e2_ric_control_response      response;
  response.success                           = false;
  response.failure->cause.set_misc().value   = asn1::e2ap::cause_misc_e::options::unspecified;
  response.failure->ric_ctrl_outcome_present = true;
  ASSERT_TRUE(response.failure->ric_ctrl_outcome.resize(outcome.size()));
  std::copy(outcome.begin(), outcome.end(), response.failure->ric_ctrl_outcome.begin());
  procedure.send_e2_ric_control_failure(request, response);

  ASSERT_TRUE(is_ric_control_failure(sent_msg));
  const auto& failure = sent_msg.pdu.unsuccessful_outcome().value.ric_ctrl_fail();
  ASSERT_TRUE(failure->ric_ctrl_outcome_present);
  ASSERT_EQ(failure->ric_ctrl_outcome.size(), outcome.size());
  ASSERT_TRUE(std::equal(outcome.begin(), outcome.end(), failure->ric_ctrl_outcome.begin()));
}

TEST_F(e2_test_setup, ric_control_failure_without_control_outcome_carries_none)
{
  e2_message                 sent_msg;
  dummy_e2_tx_pdu_notifier   notifier(sent_msg, []() {});
  e2_ric_control_request     request = {};
  e2ap_ric_control_procedure procedure(request, notifier, *e2sm_mngr, test_logger);

  e2_ric_control_response response;
  response.success                         = false;
  response.failure->cause.set_misc().value = asn1::e2ap::cause_misc_e::options::unspecified;
  procedure.send_e2_ric_control_failure(request, response);

  ASSERT_TRUE(is_ric_control_failure(sent_msg));
  ASSERT_FALSE(sent_msg.pdu.unsuccessful_outcome().value.ric_ctrl_fail()->ric_ctrl_outcome_present);
}

TEST(e2sm_ccc_ric_control_response_test, failure_outcome_is_packed_in_the_failure)
{
  e2sm_ric_control_response e2sm_response;
  e2sm_response.service_model            = e2sm_service_model_t::CCC;
  e2sm_response.success                  = false;
  e2sm_response.ric_ctrl_outcome_present = true;
  asn1::e2sm_ccc::ctrl_outcome_format_c outcome;
  outcome.set_ctrl_outcome_format2();
  e2sm_response.ric_ctrl_outcome       = outcome;
  e2sm_response.cause.set_misc().value = asn1::e2ap::cause_misc_e::options::unspecified;

  e2sm_ccc_asn1_packer    packer;
  e2_ric_control_response e2_response = packer.pack_ric_control_response(e2sm_response);

  ASSERT_FALSE(e2_response.success);
  ASSERT_TRUE(e2_response.failure->ric_ctrl_outcome_present);
  ASSERT_GT(e2_response.failure->ric_ctrl_outcome.size(), 0);
  ASSERT_FALSE(e2_response.ack->ric_ctrl_outcome_present);
  ASSERT_EQ(e2_response.ack->ric_ctrl_outcome.size(), 0);
}
