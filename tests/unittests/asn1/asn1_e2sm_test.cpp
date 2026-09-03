// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/e2sm/e2sm_kpm_ies.h"
#include "ocudu/asn1/e2sm/e2sm_rc_ies.h"
#include <gtest/gtest.h>

using namespace asn1;
using namespace ocudu;

class asn1_e2sm_test : public ::testing::Test
{
public:
  asn1_e2sm_test()
  {
    ocudulog::fetch_basic_logger("ASN1").set_level(ocudulog::basic_levels::debug);
    ocudulog::fetch_basic_logger("ASN1").set_hex_dump_max_size(-1);

    test_logger.set_level(ocudulog::basic_levels::debug);
    test_logger.set_hex_dump_max_size(-1);

    // Start the log backend.
    ocudulog::init();
  }
  ~asn1_e2sm_test() { ocudulog::flush(); }

  ocudulog::basic_logger& test_logger = ocudulog::fetch_basic_logger("TEST");
};

TEST_F(asn1_e2sm_test, unpack_ric_control_header)
{
  uint8_t ctrl_header[] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf1, 0x10, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x05};
  // 000000000000f1100000000102000005
  byte_buffer buffer = byte_buffer::create(ctrl_header).value();

  asn1::cbit_ref                 bref{buffer};
  asn1::e2sm::e2sm_rc_ctrl_hdr_s pdu;
  ASSERT_EQ(pdu.unpack(bref), OCUDUASN_SUCCESS);

  ASSERT_EQ(test_pack_unpack_consistency(pdu), OCUDUASN_SUCCESS);
}

TEST_F(asn1_e2sm_test, unpack_ric_control_message)
{
  uint8_t ctrl_header[] = {0x00, 0x00, 0x01, 0x00, 0x00, 0x60, 0x00, 0x00, 0x40, 0x00, 0x03, 0x00, 0x02, 0x44,
                           0x00, 0x00, 0x00, 0x04, 0x60, 0x00, 0x00, 0x40, 0x00, 0x01, 0x00, 0x06, 0x2a, 0x00,
                           0x05, 0x30, 0x30, 0x31, 0x30, 0x31, 0x00, 0x07, 0x44, 0x00, 0x01, 0x00, 0x08, 0x2a,
                           0x00, 0x01, 0x31, 0x00, 0x09, 0x2a, 0x00, 0x01, 0x30, 0x00, 0x0a, 0x28, 0x80, 0x01,
                           0x05, 0x00, 0x0b, 0x28, 0x80, 0x01, 0x19, 0x00, 0x0c, 0x28, 0x80, 0x01, 0x64};
  // 00000100006000004000030002440000000460000040000100062a00053030313031000744000100082a00013100092a000130000a28800105000b28800119000c28800164
  byte_buffer buffer = byte_buffer::create(ctrl_header).value();

  asn1::cbit_ref                 bref{buffer};
  asn1::e2sm::e2sm_rc_ctrl_msg_s pdu;
  ASSERT_EQ(pdu.unpack(bref), OCUDUASN_SUCCESS);

  ASSERT_EQ(pdu.ric_ctrl_msg_formats.type().value,
            asn1::e2sm::e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::types_opts::ctrl_msg_format1);
  ASSERT_EQ(pdu.ric_ctrl_msg_formats.ctrl_msg_format1().ran_p_list.size(), 1);
  auto& item = pdu.ric_ctrl_msg_formats.ctrl_msg_format1().ran_p_list[0];
  ASSERT_EQ(item.ran_param_id, 1);
  ASSERT_EQ(item.ran_param_value_type.type().value, asn1::e2sm::ran_param_value_type_c::types_opts::ran_p_choice_list);
  ASSERT_EQ(item.ran_param_value_type.ran_p_choice_list().ran_param_list.list_of_ran_param.size(), 1);
  auto& seq_params =
      item.ran_param_value_type.ran_p_choice_list().ran_param_list.list_of_ran_param[0].seq_of_ran_params;
  ASSERT_EQ(seq_params.size(), 4);

  // First item.
  ASSERT_EQ(seq_params[0].ran_param_id, 3);
  ASSERT_EQ(seq_params[0].ran_param_value_type.type().value,
            asn1::e2sm::ran_param_value_type_c::types_opts::ran_p_choice_structure);
  auto& seq_params0 = seq_params[0].ran_param_value_type.ran_p_choice_structure().ran_param_structure.seq_of_ran_params;
  ASSERT_EQ(seq_params0.size(), 1);
  ASSERT_EQ(seq_params0[0].ran_param_id, 5);
  ASSERT_EQ(seq_params0[0].ran_param_value_type.type().value,
            asn1::e2sm::ran_param_value_type_c::types_opts::ran_p_choice_list);
  ASSERT_EQ(seq_params0[0].ran_param_value_type.ran_p_choice_list().ran_param_list.list_of_ran_param.size(), 1);
  auto& ran_param0 = seq_params0[0].ran_param_value_type.ran_p_choice_list().ran_param_list.list_of_ran_param[0];
  ASSERT_EQ(ran_param0.seq_of_ran_params.size(), 2);
  auto& seq_param00 = ran_param0.seq_of_ran_params[0];
  ASSERT_EQ(seq_param00.ran_param_id, 7);
  ASSERT_EQ(seq_param00.ran_param_value_type.type().value,
            asn1::e2sm::ran_param_value_type_c::types_opts::ran_p_choice_elem_false);
  ASSERT_TRUE(seq_param00.ran_param_value_type.ran_p_choice_elem_false().ran_param_value_present);
  ASSERT_EQ(seq_param00.ran_param_value_type.ran_p_choice_elem_false().ran_param_value.type().value,
            asn1::e2sm::ran_param_value_c::types_opts::value_oct_s);
  ASSERT_EQ(seq_param00.ran_param_value_type.ran_p_choice_elem_false().ran_param_value.value_oct_s().to_string(),
            "3030313031");
  auto& seq_param01 = ran_param0.seq_of_ran_params[1];
  ASSERT_EQ(seq_param01.ran_param_id, 8);
  ASSERT_EQ(seq_param01.ran_param_value_type.type().value,
            asn1::e2sm::ran_param_value_type_c::types_opts::ran_p_choice_structure);

  // Second item.
  ASSERT_EQ(seq_params[1].ran_param_id, 11);
  ASSERT_EQ(seq_params[1].ran_param_value_type.type().value,
            asn1::e2sm::ran_param_value_type_c::types_opts::ran_p_choice_elem_false);
  // Third item.
  ASSERT_EQ(seq_params[2].ran_param_id, 12);
  ASSERT_EQ(seq_params[2].ran_param_value_type.type().value,
            asn1::e2sm::ran_param_value_type_c::types_opts::ran_p_choice_elem_false);
  // Fourth item.
  ASSERT_EQ(seq_params[3].ran_param_id, 13);
  ASSERT_EQ(seq_params[3].ran_param_value_type.type().value,
            asn1::e2sm::ran_param_value_type_c::types_opts::ran_p_choice_elem_false);

  ASSERT_EQ(test_pack_unpack_consistency(pdu), OCUDUASN_SUCCESS);
}

TEST_F(asn1_e2sm_test, pack_unpack_action_definition_with_negative_test_cond_values)
{
  using namespace asn1::e2sm;

  // Report Style 3 subscription selecting the UEs with RSRP > -110 and RSRP < -50. TestCond-Value.valueInt is an
  // unconstrained INTEGER, so the thresholds exercise the two's-complement encoding of negative whole numbers.
  e2sm_kpm_action_definition_s action_def;
  action_def.ric_style_type                = 3;
  e2sm_kpm_action_definition_format3_s& f3 = action_def.action_definition_formats.set_action_definition_format3();

  meas_cond_item_s meas_cond_item;
  meas_cond_item.meas_type.set_meas_name().from_string("DRB.UEThpDl");

  matching_cond_item_s lower_bound_cond;
  test_cond_info_s     lower_bound_info;
  lower_bound_info.test_type.set_ul_r_srp().value            = test_cond_type_c::ul_r_srp_opts::true_value;
  lower_bound_info.test_expr_present                         = true;
  lower_bound_info.test_expr                                 = test_cond_expression_opts::greaterthan;
  lower_bound_info.test_value_present                        = true;
  lower_bound_info.test_value.set_value_int()                = -110;
  lower_bound_cond.lc_or_present                             = true;
  lower_bound_cond.lc_or                                     = lc_or_opts::true_value;
  lower_bound_cond.matching_cond_choice.set_test_cond_info() = lower_bound_info;
  meas_cond_item.matching_cond.push_back(lower_bound_cond);

  matching_cond_item_s upper_bound_cond;
  test_cond_info_s     upper_bound_info;
  upper_bound_info.test_type.set_ul_r_srp().value            = test_cond_type_c::ul_r_srp_opts::true_value;
  upper_bound_info.test_expr_present                         = true;
  upper_bound_info.test_expr                                 = test_cond_expression_opts::lessthan;
  upper_bound_info.test_value_present                        = true;
  upper_bound_info.test_value.set_value_int()                = -50;
  upper_bound_cond.lc_or_present                             = false;
  upper_bound_cond.matching_cond_choice.set_test_cond_info() = upper_bound_info;
  meas_cond_item.matching_cond.push_back(upper_bound_cond);

  f3.meas_cond_list.push_back(meas_cond_item);
  f3.granul_period = 100;

  byte_buffer   buffer;
  asn1::bit_ref bref{buffer};
  ASSERT_EQ(action_def.pack(bref), OCUDUASN_SUCCESS);

  e2sm_kpm_action_definition_s unpacked_action_def;
  asn1::cbit_ref               cbref{buffer};
  ASSERT_EQ(unpacked_action_def.unpack(cbref), OCUDUASN_SUCCESS);

  const auto& unpacked_cond =
      unpacked_action_def.action_definition_formats.action_definition_format3().meas_cond_list[0].matching_cond;
  ASSERT_EQ(unpacked_cond.size(), 2);
  ASSERT_EQ(unpacked_cond[0].matching_cond_choice.test_cond_info().test_value.value_int(), -110);
  ASSERT_EQ(unpacked_cond[1].matching_cond_choice.test_cond_info().test_value.value_int(), -50);
}
