// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../cu_cp_test_messages.h"
#include "lib/cu_cp/up_resource_manager/up_resource_manager_impl.h"
#include "ocudu/ran/rb_id.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Fixture class to create UP resource manager object.
class up_resource_manager_test : public ::testing::Test
{
protected:
  static void SetUpTestSuite() { ocudulog::init(); }

  void SetUp() override
  {
    ocudulog::basic_logger& rrc_logger = ocudulog::fetch_basic_logger("RRC", false);
    rrc_logger.set_level(ocudulog::basic_levels::debug);
    rrc_logger.set_hex_dump_max_size(32);

    ASSERT_EQ(manager.get_nof_pdu_sessions(), 0);
    ASSERT_EQ(manager.get_total_nof_qos_flows(), 0);
    ASSERT_EQ(manager.get_nof_drbs(), 0);
    ASSERT_EQ(manager.get_nof_used_drb_ids(), 0);
  }

  void setup_initial_pdu_session()
  {
    ngap_pdu_session_resource_setup_request msg = generate_pdu_session_resource_setup();
    ASSERT_TRUE(manager.validate_request(msg.pdu_session_res_setup_items));

    // No DRB present
    ASSERT_EQ(manager.get_nof_drbs(), 0);

    // single PDU Session/DRB could be added
    const auto       psi    = uint_to_pdu_session_id(1);
    up_config_update update = manager.calculate_update(msg.pdu_session_res_setup_items);
    ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
    ASSERT_EQ(update.pdu_sessions_to_setup_list.at(psi).drb_to_add.size(), 1);

    // Assume DRB setup was successful.
    up_config_update_result result;
    result.pdu_sessions_added_list.push_back(update.pdu_sessions_to_setup_list.at(psi));
    manager.apply_config_update(result);

    // Verify PDU session and DRB were added
    ASSERT_EQ(manager.get_nof_drbs(), 1);
    ASSERT_EQ(manager.get_nof_used_drb_ids(), 1);
    ASSERT_EQ(manager.get_nof_pdu_sessions(), 1);
    ASSERT_EQ(manager.get_nof_qos_flows(psi), 1);
    ASSERT_EQ(manager.get_total_nof_qos_flows(), 1);
  }

  // This helper modifies the existing PDU session by adding a new QoS flow that should be mapped on a new DRB.
  void modify_pdu_session()
  {
    // Modify existing session.
    ngap_pdu_session_resource_modify_request msg = generate_pdu_session_resource_modification();
    const auto                               psi = uint_to_pdu_session_id(1);

    ASSERT_TRUE(manager.validate_request(msg));
    up_config_update update = manager.calculate_update(msg);

    // Verify calculated update.
    ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 0);
    ASSERT_EQ(update.pdu_sessions_to_modify_list.size(), 1);
    ASSERT_EQ(update.pdu_sessions_to_modify_list.at(psi).drb_to_add.size(), 1);

    // Apply update.
    up_config_update_result result;
    result.pdu_sessions_modified_list.push_back(update.pdu_sessions_to_modify_list.at(psi));
    manager.apply_config_update(result);

    // One PDU session with two QoS flows on two bearers.
    ASSERT_EQ(manager.get_nof_pdu_sessions(), 1);
    ASSERT_EQ(manager.get_total_nof_qos_flows(), 2);
    ASSERT_EQ(manager.get_nof_drbs(), 2);
    ASSERT_EQ(manager.get_nof_qos_flows(psi), 2);
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }

public:
  pdcp_config             pdcp_cfg{pdcp_rb_type::drb, pdcp_rlc_mode::am};
  up_resource_manager_cfg cfg{{{uint_to_five_qi(9), {pdcp_cfg}}, {uint_to_five_qi(7), {pdcp_cfg}}}, 8};
  up_resource_manager     manager{cfg};
};

TEST_F(up_resource_manager_test, when_initial_pdu_session_is_created_new_drb_is_set_up)
{
  setup_initial_pdu_session();
}

TEST_F(up_resource_manager_test, when_same_pdu_session_is_created_no_new_drb_is_set_up)
{
  ngap_pdu_session_resource_setup_request msg = generate_pdu_session_resource_setup();
  ASSERT_TRUE(manager.validate_request(msg.pdu_session_res_setup_items));

  // single DRB should be added
  up_config_update update = manager.calculate_update(msg.pdu_session_res_setup_items);
  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
  ASSERT_EQ(update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)).drb_to_add.size(), 1);

  // Assume DRB setup was successful.
  up_config_update_result result;
  result.pdu_sessions_added_list.push_back(update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)));
  manager.apply_config_update(result);

  ASSERT_EQ(manager.get_nof_drbs(), 1);

  // if same request is received again, no DRB should be added
  ASSERT_FALSE(manager.validate_request(msg.pdu_session_res_setup_items));
}

TEST_F(up_resource_manager_test, when_drb_is_added_pdcp_config_is_valid)
{
  ngap_pdu_session_resource_setup_request msg = generate_pdu_session_resource_setup();
  ASSERT_TRUE(manager.validate_request(msg.pdu_session_res_setup_items));
  up_config_update update = manager.calculate_update(msg.pdu_session_res_setup_items);

  // Verify DRB config
  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
  ASSERT_EQ(update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)).drb_to_add.size(), 1);
  ASSERT_EQ(
      update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)).drb_to_add[uint_to_drb_id(1)].pdcp_cfg.rb_type,
      pdcp_rb_type::drb);
}

TEST_F(up_resource_manager_test, when_pdu_session_setup_with_two_qos_flows_both_are_mapped_on_own_drb)
{
  ngap_pdu_session_resource_setup_request msg = generate_pdu_session_resource_setup(cu_cp_ue_index_t::min, 1, 2);
  ASSERT_TRUE(manager.validate_request(msg.pdu_session_res_setup_items));
  up_config_update update = manager.calculate_update(msg.pdu_session_res_setup_items);

  // Verify created DRBs.
  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
  ASSERT_EQ(update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)).drb_to_add.size(), 2);
}

TEST_F(up_resource_manager_test, when_pdu_session_gets_modified_new_drb_is_set_up)
{
  // Preamble.
  setup_initial_pdu_session();

  // The actual test as fixture for later reuse.
  modify_pdu_session();
}

TEST_F(up_resource_manager_test, when_inexiting_qos_flow_gets_removed_removal_fails)
{
  // Preamble.
  setup_initial_pdu_session();

  // Modify existing session by trying to remove invalid/inexisting QoS flow.
  ngap_pdu_session_resource_modify_request msg =
      generate_pdu_session_resource_modification_with_qos_flow_removal(uint_to_qos_flow_id(9));

  ASSERT_FALSE(manager.validate_request(msg));
}

/// Note: From TS 38.413 Sec 8.2.3.1 "PDU Session Resource Modify" it's not entirely clear what should happen in
/// the case where all QoS flows of a PDU session are removed in a Resource Modify Request.
/// The current assumption is that we reject the Modify Request as a PDU session resource release would be the
/// expected command.
TEST_F(up_resource_manager_test, when_only_existing_qos_flow_gets_removed_removal_fails)
{
  // Preamble.
  setup_initial_pdu_session();

  // Modify existing session and remove only existing QoS flow.
  ngap_pdu_session_resource_modify_request msg =
      generate_pdu_session_resource_modification_with_qos_flow_removal(uint_to_qos_flow_id(1));

  ASSERT_FALSE(manager.validate_request(msg));
}

TEST_F(up_resource_manager_test, when_existing_qos_flow_gets_removed_removal_succeeds_and_associated_drb_is_released)
{
  // Preamble.
  setup_initial_pdu_session();
  modify_pdu_session();

  // Modify existing session and remove existing QoS flow.
  ngap_pdu_session_resource_modify_request msg =
      generate_pdu_session_resource_modification_with_qos_flow_removal(uint_to_qos_flow_id(2));
  const auto psi = uint_to_pdu_session_id(1);

  ASSERT_TRUE(manager.validate_request(msg));

  up_config_update update = manager.calculate_update(msg);

  // Verify calculated update.
  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 0);
  ASSERT_EQ(update.pdu_sessions_to_modify_list.size(), 1);
  ASSERT_EQ(update.pdu_sessions_to_modify_list.at(psi).drb_to_add.size(), 0);
  ASSERT_EQ(update.pdu_sessions_to_modify_list.at(psi).drb_to_remove.size(), 1);

  // Apply update.
  up_config_update_result result;
  result.pdu_sessions_modified_list.push_back(update.pdu_sessions_to_modify_list.at(psi));
  manager.apply_config_update(result);

  // One PDU session with one QoS flows using one DRB.
  ASSERT_EQ(manager.get_nof_pdu_sessions(), 1);
  ASSERT_EQ(manager.get_total_nof_qos_flows(), 1);
  ASSERT_EQ(manager.get_nof_drbs(), 1);
  ASSERT_EQ(manager.get_nof_qos_flows(psi), 1);
}

TEST_F(up_resource_manager_test, when_pdu_session_gets_removed_all_resources_are_removed)
{
  // Preamble.
  setup_initial_pdu_session();

  // Modify PDU session and add QoS flow and DRB.
  modify_pdu_session();

  // Attempt to create new session with same PSI fails.
  ngap_pdu_session_resource_setup_request setup_msg = generate_pdu_session_resource_setup();
  ASSERT_FALSE(manager.validate_request(setup_msg.pdu_session_res_setup_items));

  // Remove existing session.
  ngap_pdu_session_resource_release_command release_msg = generate_pdu_session_resource_release();
  ASSERT_TRUE(manager.validate_request(release_msg));

  // Calculate update
  up_config_update update = manager.calculate_update(release_msg);

  // Verify calculated update.
  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 0);
  ASSERT_EQ(update.pdu_sessions_to_modify_list.size(), 0);
  ASSERT_EQ(update.pdu_sessions_to_remove_list.size(), 1);
  ASSERT_EQ(update.drb_to_remove_list.size(), 2);

  // Apply update.
  up_config_update_result result;
  result.pdu_sessions_removed_list.push_back(update.pdu_sessions_to_remove_list.front());
  manager.apply_config_update(result);

  // All resources are removed.
  ASSERT_EQ(manager.get_nof_pdu_sessions(), 0);
  ASSERT_EQ(manager.get_total_nof_qos_flows(), 0);
  ASSERT_EQ(manager.get_nof_drbs(), 0);

  // DRBs that were used are marked as used until the keys are changed
  ASSERT_EQ(manager.get_nof_used_drb_ids(), 2);
  manager.refresh_drb_id_after_key_change();
  ASSERT_EQ(manager.get_nof_used_drb_ids(), 0);

  // Setting up initial PDU session is possible again.
  setup_initial_pdu_session();
}

TEST_F(up_resource_manager_test, when_all_drb_ids_are_used_allocation_fails_until_keys_are_refreshed)
{
  // Preamble.
  setup_initial_pdu_session();
  constexpr std::size_t initial_nof_drbs = 1;
  ASSERT_EQ(manager.get_nof_drbs(), initial_nof_drbs);
  ASSERT_EQ(manager.get_nof_used_drb_ids(), initial_nof_drbs);

  for (unsigned i = 1; i < MAX_NOF_DRBS; i++) {
    // Attempt to create new session with PSI=2.
    pdu_session_id_t                        psi{2};
    ngap_pdu_session_resource_setup_request setup_msg =
        generate_pdu_session_resource_setup(cu_cp_ue_index_t::min, psi, qos_flow_id_t::min);
    ASSERT_TRUE(manager.validate_request(setup_msg.pdu_session_res_setup_items));

    // Calculate update
    up_config_update update = manager.calculate_update(setup_msg.pdu_session_res_setup_items);

    // Verify calculated update.
    ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
    ASSERT_EQ(update.pdu_sessions_to_modify_list.size(), 0);
    ASSERT_EQ(update.pdu_sessions_to_remove_list.size(), 0);
    ASSERT_EQ(update.drb_to_remove_list.size(), 0);

    // Apply update.
    up_config_update_result result;
    result.pdu_sessions_added_list.push_back(update.pdu_sessions_to_setup_list.at(psi));
    manager.apply_config_update(result);

    // There are two PDU sessions.
    ASSERT_EQ(manager.get_nof_pdu_sessions(), 2);
    ASSERT_EQ(manager.get_total_nof_qos_flows(), 2);
    ASSERT_EQ(manager.get_nof_drbs(), 2);

    // Remove PDU session, DRB ID is stale and still marked as used.
    ngap_pdu_session_resource_release_command release_msg =
        generate_pdu_session_resource_release(cu_cp_ue_index_t::min, psi);
    ASSERT_TRUE(manager.validate_request(release_msg));

    up_config_update rel_update = manager.calculate_update(release_msg);

    // Verify calculated update.
    ASSERT_EQ(rel_update.pdu_sessions_to_setup_list.size(), 0);
    ASSERT_EQ(rel_update.pdu_sessions_to_modify_list.size(), 0);
    ASSERT_EQ(rel_update.pdu_sessions_to_remove_list.size(), 1);
    ASSERT_EQ(rel_update.drb_to_remove_list.size(), 1);

    // Apply update.
    up_config_update_result rel_result;
    rel_result.pdu_sessions_removed_list.push_back(rel_update.pdu_sessions_to_remove_list.front());
    manager.apply_config_update(rel_result);

    // All resources are removed, but DRB IDs are stale and still marked as used.
    ASSERT_EQ(manager.get_nof_pdu_sessions(), 1);
    ASSERT_EQ(manager.get_total_nof_qos_flows(), 1);
    ASSERT_EQ(manager.get_nof_drbs(), 1);
    ASSERT_EQ(manager.get_nof_used_drb_ids(), i + 1);
    if (i + initial_nof_drbs == MAX_NOF_DRBS) {
      ASSERT_TRUE(manager.key_refresh_required());
    } else {
      ASSERT_FALSE(manager.key_refresh_required());
    }
  }

  // All DRB IDs were used. There are no more DRB IDs available and allocation will fail.
  ASSERT_TRUE(manager.key_refresh_required());
  {
    pdu_session_id_t                        psi{2};
    ngap_pdu_session_resource_setup_request setup_msg =
        generate_pdu_session_resource_setup(cu_cp_ue_index_t::min, psi, qos_flow_id_t::min);
    ASSERT_TRUE(manager.validate_request(setup_msg.pdu_session_res_setup_items));

    // Calculate update
    up_config_update update = manager.calculate_update(setup_msg.pdu_session_res_setup_items);

    // Verify calculated update.
    ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 0);
    ASSERT_EQ(update.pdu_sessions_to_modify_list.size(), 0);
    ASSERT_EQ(update.pdu_sessions_to_remove_list.size(), 0);
    ASSERT_EQ(update.drb_to_remove_list.size(), 0);
  }
  // After marking the keys as changed, we can create DRBs again.
  manager.refresh_drb_id_after_key_change();
  ASSERT_FALSE(manager.key_refresh_required());
  {
    pdu_session_id_t                        psi{2};
    ngap_pdu_session_resource_setup_request setup_msg =
        generate_pdu_session_resource_setup(cu_cp_ue_index_t::min, psi, qos_flow_id_t::min);
    ASSERT_TRUE(manager.validate_request(setup_msg.pdu_session_res_setup_items));

    // Calculate update
    up_config_update update = manager.calculate_update(setup_msg.pdu_session_res_setup_items);

    // Verify calculated update.
    ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
    ASSERT_EQ(update.pdu_sessions_to_modify_list.size(), 0);
    ASSERT_EQ(update.pdu_sessions_to_remove_list.size(), 0);
    ASSERT_EQ(update.drb_to_remove_list.size(), 0);
  }
}

class up_resource_manager_used_drb_ids_test : public up_resource_manager_test,
                                              public ::testing::WithParamInterface<uint8_t>
{
protected:
  up_resource_manager_cfg cfg{{{uint_to_five_qi(9), {pdcp_cfg}}, {uint_to_five_qi(7), {pdcp_cfg}}}, GetParam()};
  up_resource_manager     manager{cfg};
};

INSTANTIATE_TEST_SUITE_P(
    when_there_are_no_stale_drb_ids_the_key_refresh_is_not_required,
    up_resource_manager_used_drb_ids_test,
    ::testing::Values(MAX_NOF_DRBS)); // this can only happen when max_nof_drbs_per_ue == MAX_NOF_DRBS

TEST_P(up_resource_manager_used_drb_ids_test, when_there_are_no_stale_drb_ids_the_key_refresh_is_not_required)
{
  for (uint16_t i = 1; i <= MAX_NOF_DRBS; i++) {
    // Attempt to create new PDU Session.
    pdu_session_id_t                        psi{i};
    ngap_pdu_session_resource_setup_request setup_msg =
        generate_pdu_session_resource_setup(cu_cp_ue_index_t::min, psi, qos_flow_id_t::min);
    ASSERT_TRUE(manager.validate_request(setup_msg.pdu_session_res_setup_items));

    // Calculate update
    up_config_update update = manager.calculate_update(setup_msg.pdu_session_res_setup_items);

    // Verify calculated update.
    ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
    ASSERT_EQ(update.pdu_sessions_to_modify_list.size(), 0);
    ASSERT_EQ(update.pdu_sessions_to_remove_list.size(), 0);
    ASSERT_EQ(update.drb_to_remove_list.size(), 0);

    // Apply update.
    up_config_update_result result;
    result.pdu_sessions_added_list.push_back(update.pdu_sessions_to_setup_list.at(psi));
    manager.apply_config_update(result);

    // There is a new PDU session
    ASSERT_EQ(manager.get_nof_pdu_sessions(), i);
    ASSERT_EQ(manager.get_total_nof_qos_flows(), 1);
    ASSERT_EQ(manager.get_nof_drbs(), i);
    ASSERT_EQ(manager.get_nof_used_drb_ids(), i);
    ASSERT_FALSE(manager.key_refresh_required());
  }

  ASSERT_EQ(manager.get_nof_drbs(), MAX_NOF_DRBS);
  ASSERT_EQ(manager.get_nof_used_drb_ids(), MAX_NOF_DRBS);
  ASSERT_FALSE(manager.key_refresh_required());
}

class up_resource_manager_key_refresh_useful_test : public up_resource_manager_test,
                                                    public ::testing::WithParamInterface<uint8_t>
{
protected:
  up_resource_manager_cfg cfg{{{uint_to_five_qi(9), {pdcp_cfg}}, {uint_to_five_qi(7), {pdcp_cfg}}}, GetParam()};
  up_resource_manager     manager{cfg};
};

INSTANTIATE_TEST_SUITE_P(key_refresh_useful_will_trigger_if_there_are_too_many_stale_drbs_test,
                         up_resource_manager_key_refresh_useful_test,
                         ::testing::Range((uint8_t)1, (uint8_t)MAX_NOF_DRBS));

TEST_P(up_resource_manager_key_refresh_useful_test,
       key_refresh_useful_will_trigger_if_there_are_too_many_stale_drbs_test)
{
  // Add MAX_NOF_DRBS stale PDU sessions
  for (uint16_t i = 1; i <= MAX_NOF_DRBS; i++) {
    // Attempt to create new PDU Session.
    pdu_session_id_t                        psi{i};
    ngap_pdu_session_resource_setup_request setup_msg =
        generate_pdu_session_resource_setup(cu_cp_ue_index_t::min, psi, qos_flow_id_t::min);
    ASSERT_TRUE(manager.validate_request(setup_msg.pdu_session_res_setup_items));

    // Calculate update
    up_config_update update = manager.calculate_update(setup_msg.pdu_session_res_setup_items);

    // Verify calculated update.
    ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
    ASSERT_EQ(update.pdu_sessions_to_modify_list.size(), 0);
    ASSERT_EQ(update.pdu_sessions_to_remove_list.size(), 0);
    ASSERT_EQ(update.drb_to_remove_list.size(), 0);

    // Apply update.
    up_config_update_result result;
    result.pdu_sessions_added_list.push_back(update.pdu_sessions_to_setup_list.at(psi));
    manager.apply_config_update(result);

    // There is a new PDU session
    ASSERT_EQ(manager.get_nof_pdu_sessions(), 1);
    ASSERT_EQ(manager.get_total_nof_qos_flows(), 1);
    ASSERT_EQ(manager.get_nof_drbs(), 1);
    ASSERT_EQ(manager.get_nof_used_drb_ids(), i);

    // Remove existing session.
    ngap_pdu_session_resource_release_command release_msg =
        generate_pdu_session_resource_release(cu_cp_ue_index_t::min, psi);
    ASSERT_TRUE(manager.validate_request(release_msg));

    // Calculate update
    update = manager.calculate_update(release_msg);

    // Verify calculated update.
    ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 0);
    ASSERT_EQ(update.pdu_sessions_to_modify_list.size(), 0);
    ASSERT_EQ(update.pdu_sessions_to_remove_list.size(), 1);
    ASSERT_EQ(update.drb_to_remove_list.size(), 1);

    // Apply update.
    result.pdu_sessions_removed_list.push_back(update.pdu_sessions_to_remove_list.front());
    manager.apply_config_update(result);

    // PDU session removed, but nof_used_drb_ids is incremented by one more stale DRB ID
    ASSERT_EQ(manager.get_nof_pdu_sessions(), 0);
    ASSERT_EQ(manager.get_total_nof_qos_flows(), 0);
    ASSERT_EQ(manager.get_nof_drbs(), 0);
    ASSERT_EQ(manager.get_nof_used_drb_ids(), i);

    // If available DRB IDs < number of DRBs that UE may need - key refresh useful should trigger
    if (MAX_NOF_DRBS - i < std::min((uint8_t)GetParam(), (uint8_t)8)) {
      ASSERT_TRUE(manager.key_refresh_useful());
    } else {
      ASSERT_FALSE(manager.key_refresh_useful());
    }
  }
  ASSERT_TRUE(manager.key_refresh_useful());
  ASSERT_TRUE(manager.key_refresh_required());
}

class up_resource_manager_max_nof_drbs_per_ue_test : public up_resource_manager_test,
                                                     public ::testing::WithParamInterface<uint8_t>
{
protected:
  up_resource_manager_cfg cfg{{{uint_to_five_qi(9), {pdcp_cfg}}, {uint_to_five_qi(7), {pdcp_cfg}}}, GetParam()};
  up_resource_manager     manager{cfg};
};

INSTANTIATE_TEST_SUITE_P(max_nof_drbs_per_ue_is_checked_when_adding_new_drbs,
                         up_resource_manager_max_nof_drbs_per_ue_test,
                         ::testing::Range((uint8_t)1, (uint8_t)MAX_NOF_DRBS));

TEST_P(up_resource_manager_max_nof_drbs_per_ue_test, max_nof_drbs_per_ue_is_checked_when_adding_new_drbs)
{
  for (uint16_t i = 1; i <= GetParam(); i++) {
    // Attempt to create new PDU Session.
    pdu_session_id_t                        psi{i};
    ngap_pdu_session_resource_setup_request setup_msg =
        generate_pdu_session_resource_setup(cu_cp_ue_index_t::min, psi, qos_flow_id_t::min);
    ASSERT_TRUE(manager.validate_request(setup_msg.pdu_session_res_setup_items));

    // Calculate update
    up_config_update update = manager.calculate_update(setup_msg.pdu_session_res_setup_items);

    // Verify calculated update.
    ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
    ASSERT_EQ(update.pdu_sessions_to_modify_list.size(), 0);
    ASSERT_EQ(update.pdu_sessions_to_remove_list.size(), 0);
    ASSERT_EQ(update.drb_to_remove_list.size(), 0);

    // Apply update.
    up_config_update_result result;
    result.pdu_sessions_added_list.push_back(update.pdu_sessions_to_setup_list.at(psi));
    manager.apply_config_update(result);

    // There is a new PDU session
    ASSERT_EQ(manager.get_nof_pdu_sessions(), i);
    ASSERT_EQ(manager.get_total_nof_qos_flows(), 1);
    ASSERT_EQ(manager.get_nof_drbs(), i);
    ASSERT_EQ(manager.get_nof_used_drb_ids(), i);
  }

  // Try to add one more PDU Session with one DRB
  uint16_t                                psi = GetParam() + 1;
  ngap_pdu_session_resource_setup_request setup_msg =
      generate_pdu_session_resource_setup(cu_cp_ue_index_t::min, pdu_session_id_t{psi}, qos_flow_id_t::min);
  ASSERT_TRUE(manager.validate_request(setup_msg.pdu_session_res_setup_items));

  // Calculate update
  up_config_update update = manager.calculate_update(setup_msg.pdu_session_res_setup_items);

  // Verify calculated update - it should fail to add PDU session because of DRB limit
  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 0);
  ASSERT_EQ(update.pdu_sessions_to_modify_list.size(), 0);
  ASSERT_EQ(update.pdu_sessions_to_remove_list.size(), 0);
  ASSERT_EQ(update.drb_to_remove_list.size(), 0);

  // Only max_nof_drbs_per_ue DRBs were set up in the end.
  ASSERT_EQ(manager.get_nof_pdu_sessions(), GetParam());
  ASSERT_EQ(manager.get_total_nof_qos_flows(), 1);
  ASSERT_EQ(manager.get_nof_drbs(), GetParam());
  ASSERT_EQ(manager.get_nof_used_drb_ids(), GetParam());
}
