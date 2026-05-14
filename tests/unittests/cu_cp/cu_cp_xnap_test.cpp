// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/cu_cp/xnap_repository.h"
#include "tests/unittests/cu_cp/cu_cp_test_environment.h"
#include "tests/unittests/cu_cp/test_helpers.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

class cu_cp_xnap_repository_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_xnap_repository_test() :
    cu_cp_test_environment({/* max nof cu-ups */ 8,
                            /* max nof dus */ 8,
                            /* max nof ues */ 8192,
                            /* max nof drbs per ue */ 8,
                            /* amf config */ {{default_supported_tracking_area}},
                            /* trigger ho from measurements */ true,
                            /* enable rrc inactive */ false,
                            /* enable xnc peer */ true}),
    xnap_db(xnap_repository_config{get_cu_cp_cfg(), cu_cp_xnap_handler, test_logger})
  {
  }

  ue_manager               ue_mng{get_cu_cp_cfg()};
  dummy_cu_cp_xnap_handler cu_cp_xnap_handler{ue_mng};
  xnap_repository          xnap_db;

  gnb_id_t                default_gnb_id{.id = 411, .bit_length = 22};
  transport_layer_address default_peer_addr = transport_layer_address::create_from_string("127.0.0.1");
  guami_t default_guami{.plmn = plmn_identity::test_value(), .amf_set_id = 1, .amf_pointer = 1, .amf_region_id = 1};
  xnap_configuration xnap_cfg{.gnb_id           = default_gnb_id,
                              .tai_support_list = {default_supported_tracking_area},
                              .guami_list       = {default_guami}};
};

//----------------------------------------------------------------------------------//
// XNAP repository tests                                                            //
//----------------------------------------------------------------------------------//

TEST_F(cu_cp_xnap_repository_test,
       when_adding_xnap_then_xnap_is_added_to_repository_and_can_be_retrieved_by_index_and_peer_addr)
{
  // Add XNAP.
  xnap_interface* xnap = xnap_db.add_xnap(xnc_peer_index_t::min, {default_peer_addr}, xnap_cfg);
  ASSERT_TRUE(xnap != nullptr) << "Failed to add XNAP to repository";

  ASSERT_EQ(xnap_db.get_nof_xnaps(), 1U) << "Unexpected number of XNAPs in repository after adding XNAP";

  // Retrieve XNAP by index.
  xnap_interface* retrieved_xnap = xnap_db.find_xnap(xnc_peer_index_t::min);
  ASSERT_TRUE(retrieved_xnap != nullptr) << "Failed to retrieve XNAP by index";
  ASSERT_EQ(retrieved_xnap, xnap) << "Retrieved XNAP does not match added XNAP";

  // Retrieve XNAP by peer address.
  ASSERT_EQ(xnc_peer_index_t::min, xnap_db.find_xnap(default_peer_addr)) << "Failed to retrieve XNAP by peer address";
}

TEST_F(cu_cp_xnap_repository_test, when_multihomed_peer_added_then_find_xnap_matches_any_of_its_addresses)
{
  // Add a multihomed XNAP peer with three addresses.
  const transport_layer_address addr_a = transport_layer_address::create_from_string("127.0.0.1");
  const transport_layer_address addr_b = transport_layer_address::create_from_string("127.0.0.2");
  const transport_layer_address addr_c = transport_layer_address::create_from_string("127.0.0.3");

  xnap_interface* xnap = xnap_db.add_xnap(xnc_peer_index_t::min, {addr_a, addr_b, addr_c}, xnap_cfg);
  ASSERT_TRUE(xnap != nullptr) << "Failed to add multihomed XNAP to repository";

  // SCTP COMM_UP notification may report any of the configured addresses, lookup must find on any of them.
  ASSERT_EQ(xnc_peer_index_t::min, xnap_db.find_xnap(addr_a)) << "Failed to retrieve XNAP by first address";
  ASSERT_EQ(xnc_peer_index_t::min, xnap_db.find_xnap(addr_b)) << "Failed to retrieve XNAP by second address";
  ASSERT_EQ(xnc_peer_index_t::min, xnap_db.find_xnap(addr_c)) << "Failed to retrieve XNAP by third address";

  // An unrelated address must not match.
  const transport_layer_address unrelated = transport_layer_address::create_from_string("127.0.0.99");
  ASSERT_EQ(xnc_peer_index_t::invalid, xnap_db.find_xnap(unrelated)) << "Unexpected match for unrelated address";
}
