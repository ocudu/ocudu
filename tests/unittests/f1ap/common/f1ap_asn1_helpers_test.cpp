// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/f1ap/asn1_helpers.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/up_transport_layer_info.h"
#include <gtest/gtest.h>

using namespace ocudu;

/// Test PLMN decoding
TEST(f1ap_asn1_helpers_test, test_ngi_converter_for_valid_plmn)
{
  // use known a PLMN
  asn1::f1ap::nr_cgi_s asn1_cgi;
  asn1_cgi.plmn_id.from_string("00f110"); // 001.01
  asn1_cgi.nr_cell_id.from_number(6576);

  // convert to internal NGI representation
  nr_cell_global_id_t ngi = cgi_from_asn1(asn1_cgi).value();
  ASSERT_EQ("00101", ngi.plmn_id.to_string()); // human-readable PLMN
}

TEST(f1ap_asn1_helpers_test, test_ngi_converter_for_invalid_plmn)
{
  // use known a PLMN
  asn1::f1ap::nr_cgi_s asn1_cgi;
  asn1_cgi.plmn_id.from_string("00f00a"); // 000.0a
  asn1_cgi.nr_cell_id.from_number(6576);

  // convert to internal NGI representation
  auto ngi = cgi_from_asn1(asn1_cgi);
  ASSERT_FALSE(ngi.has_value());
}

static std::string create_random_ipv4_string()
{
  std::vector<uint8_t> nums = test_rng::vector_of_uniform_ints<uint8_t>(4);
  return fmt::format("{}.{}.{}.{}", nums[0], nums[1], nums[2], nums[3]);
}

static std::string create_random_ipv6_string()
{
  std::vector<uint16_t> nums = test_rng::vector_of_uniform_ints<uint16_t>(8);
  return fmt::format("{:x}:{:x}:{:x}:{:x}:{:x}:{:x}:{:x}:{:x}",
                     nums[0],
                     nums[1],
                     nums[2],
                     nums[3],
                     nums[4],
                     nums[5],
                     nums[6],
                     nums[7]);
}

static std::string generate_random_ipv4_bitstring()
{
  uint32_t    random_number = test_rng::uniform_int<uint32_t>();
  std::string bitstr        = fmt::format("{:032b}", random_number);

  return bitstr;
}

static std::string generate_random_ipv6_bitstring()
{
  std::string bitstr;

  for (int i = 0; i < 2; i++) { // we need 128 bits for ipv6
    uint64_t random_number = test_rng::uniform_int<uint64_t>();
    bitstr                 = bitstr + fmt::format("{:064b}", random_number);
  }

  return bitstr;
}

static uint32_t generate_gtp_teid()
{
  return test_rng::uniform_int<uint32_t>();
}

TEST(f1ap_asn1_helpers_test, test_up_transport_layer_converter)
{
  up_transport_layer_info up_tp_layer_info = {transport_layer_address::create_from_string(create_random_ipv4_string()),
                                              int_to_gtpu_teid(0x1)};

  asn1::f1ap::up_transport_layer_info_c asn1_transport_layer_info;

  up_transport_layer_info_to_asn1(asn1_transport_layer_info, up_tp_layer_info);

  ASSERT_EQ(up_tp_layer_info.gtp_teid, int_to_gtpu_teid(asn1_transport_layer_info.gtp_tunnel().gtp_teid.to_number()));
  ASSERT_EQ(up_tp_layer_info.tp_address,
            tla_from_asn1_bitstring(asn1_transport_layer_info.gtp_tunnel().transport_layer_address));
}

TEST(transport_layer_address_test, ipv6_transport_layer_address_to_asn1)
{
  up_transport_layer_info up_tp_layer_info = {transport_layer_address::create_from_string(create_random_ipv6_string()),
                                              int_to_gtpu_teid(0x1)};

  asn1::f1ap::up_transport_layer_info_c asn1_transport_layer_info;

  up_transport_layer_info_to_asn1(asn1_transport_layer_info, up_tp_layer_info);

  ASSERT_EQ(up_tp_layer_info.gtp_teid, int_to_gtpu_teid(asn1_transport_layer_info.gtp_tunnel().gtp_teid.to_number()));
  ASSERT_EQ(up_tp_layer_info.tp_address,
            tla_from_asn1_bitstring(asn1_transport_layer_info.gtp_tunnel().transport_layer_address));
}

TEST(transport_layer_address_test, asn1_to_ipv4_transport_layer_address)
{
  asn1::f1ap::up_transport_layer_info_c asn1_transport_layer_info;
  asn1_transport_layer_info.set_gtp_tunnel().gtp_teid.from_number(generate_gtp_teid());
  asn1_transport_layer_info.set_gtp_tunnel().transport_layer_address.from_string(generate_random_ipv4_bitstring());

  // ASN1 -> internal representation.
  up_transport_layer_info up_tp_layer_info = asn1_to_up_transport_layer_info(asn1_transport_layer_info);

  ASSERT_EQ(up_tp_layer_info.gtp_teid, int_to_gtpu_teid(asn1_transport_layer_info.gtp_tunnel().gtp_teid.to_number()));
  ASSERT_EQ(up_tp_layer_info.tp_address,
            tla_from_asn1_bitstring(asn1_transport_layer_info.gtp_tunnel().transport_layer_address));
}

TEST(transport_layer_address_test, asn1_to_ipv6_transport_layer_address)
{
  asn1::f1ap::up_transport_layer_info_c asn1_transport_layer_info;
  asn1_transport_layer_info.set_gtp_tunnel().gtp_teid.from_number(0x1);
  asn1_transport_layer_info.set_gtp_tunnel().transport_layer_address.from_string(generate_random_ipv6_bitstring());

  up_transport_layer_info up_tp_layer_info = asn1_to_up_transport_layer_info(asn1_transport_layer_info);

  ASSERT_EQ(up_tp_layer_info.gtp_teid, int_to_gtpu_teid(asn1_transport_layer_info.gtp_tunnel().gtp_teid.to_number()));
  ASSERT_EQ(up_tp_layer_info.tp_address,
            tla_from_asn1_bitstring(asn1_transport_layer_info.gtp_tunnel().transport_layer_address));
}

/// Test that the GBR QoS Flow Information of a GBR DRB is converted into the common type.
TEST(f1ap_asn1_helpers_test, gbr_qos_flow_information_of_gbr_drb_is_converted)
{
  asn1::f1ap::drbs_to_be_setup_item_s asn1_drb_item;
  asn1_drb_item.drb_id   = 1;
  asn1_drb_item.rlc_mode = asn1::f1ap::rlc_mode_opts::rlc_um_bidirectional;

  auto& asn1_drb_info = asn1_drb_item.qos_info.set_choice_ext().value().drb_info();
  // 5QI 1 is a GBR 5QI.
  asn1_drb_info.drb_qos.qos_characteristics.set_non_dyn_5qi().five_qi = 1;
  asn1_drb_info.snssai.sst.from_number(1);

  auto& asn1_gbr_qos_info                           = asn1_drb_info.drb_qos.gbr_qos_flow_info;
  asn1_drb_info.drb_qos.gbr_qos_flow_info_present   = true;
  asn1_gbr_qos_info.max_flow_bit_rate_dl            = 2000000;
  asn1_gbr_qos_info.max_flow_bit_rate_ul            = 1000000;
  asn1_gbr_qos_info.guaranteed_flow_bit_rate_dl     = 200000;
  asn1_gbr_qos_info.guaranteed_flow_bit_rate_ul     = 100000;
  asn1_gbr_qos_info.max_packet_loss_rate_dl_present = true;
  asn1_gbr_qos_info.max_packet_loss_rate_dl         = 20;
  asn1_gbr_qos_info.max_packet_loss_rate_ul_present = true;
  asn1_gbr_qos_info.max_packet_loss_rate_ul         = 10;

  f1ap_drb_to_setup drb = make_drb_to_setup(asn1_drb_item);

  ASSERT_TRUE(drb.qos_info.drb_qos.gbr_qos_info.has_value());
  const gbr_qos_flow_information& gbr_qos_info = drb.qos_info.drb_qos.gbr_qos_info.value();
  ASSERT_EQ(gbr_qos_info.max_br_dl, 2000000);
  ASSERT_EQ(gbr_qos_info.max_br_ul, 1000000);
  ASSERT_EQ(gbr_qos_info.gbr_dl, 200000);
  ASSERT_EQ(gbr_qos_info.gbr_ul, 100000);
  ASSERT_EQ(gbr_qos_info.max_packet_loss_rate_dl, 20);
  ASSERT_EQ(gbr_qos_info.max_packet_loss_rate_ul, 10);
}
