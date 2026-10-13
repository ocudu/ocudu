// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../cu_cp_test_messages.h"
#include "lib/cu_cp/routines/mobility/mobility_helpers.h"
#include "lib/cu_cp/up_resource_manager/up_resource_manager_impl.h"
#include "tests/test_doubles/rrc/redcap_ue_capability_fixture.h"
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
    up_config_update update = manager.calculate_update(msg.pdu_session_res_setup_items, {});
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
    up_config_update update = manager.calculate_update(msg, {});

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
  up_config_update update = manager.calculate_update(msg.pdu_session_res_setup_items, {});
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
  up_config_update update = manager.calculate_update(msg.pdu_session_res_setup_items, {});

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
  up_config_update update = manager.calculate_update(msg.pdu_session_res_setup_items, {});

  // Verify created DRBs.
  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
  ASSERT_EQ(update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)).drb_to_add.size(), 2);
}

// During inter-gNB handover, DRB IDs are allocated independently by the source and target RAN nodes (TS 38.300
// Section 9.2.3.2.3). Without a preference hint, a fresh target always allocates DRB1, DRB2, ... in QoS flow order,
// which need not match the source's own numbering for the same flows. Verify that when the source's DRB-to-QoS-flow
// mapping is known (e.g. decoded from the Handover Preparation Information), the target reuses it instead of
// falling back to plain sequential allocation - this is what keeps a later SN/RAN Status Transfer (keyed only by
// DRB ID) correlated with the correct bearer instead of silently colliding with an unrelated one.
TEST_F(up_resource_manager_test, when_source_used_different_drb_ids_target_preserves_them)
{
  ngap_pdu_session_resource_setup_request msg = generate_pdu_session_resource_setup(cu_cp_ue_index_t::min, 1, 2);
  ASSERT_TRUE(manager.validate_request(msg.pdu_session_res_setup_items));

  // The source used DRB2 for QoS flow 1 and DRB1 for QoS flow 2 - the reverse of what plain sequential allocation
  // would pick.
  up_old_drb_association old_drb_association = {
      {uint_to_pdu_session_id(1),
       {{uint_to_qos_flow_id(1), uint_to_drb_id(2)}, {uint_to_qos_flow_id(2), uint_to_drb_id(1)}}}};

  up_config_update update = manager.calculate_update(msg.pdu_session_res_setup_items, {}, old_drb_association);

  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
  const auto& drb_to_add = update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)).drb_to_add;
  ASSERT_EQ(drb_to_add.size(), 2);

  ASSERT_TRUE(drb_to_add.find(uint_to_drb_id(2)) != drb_to_add.end());
  EXPECT_TRUE(drb_to_add.at(uint_to_drb_id(2)).qos_flows.find(uint_to_qos_flow_id(1)) !=
              drb_to_add.at(uint_to_drb_id(2)).qos_flows.end());
  EXPECT_TRUE(drb_to_add.at(uint_to_drb_id(2)).source_drb_id_confirmed);

  ASSERT_TRUE(drb_to_add.find(uint_to_drb_id(1)) != drb_to_add.end());
  EXPECT_TRUE(drb_to_add.at(uint_to_drb_id(1)).qos_flows.find(uint_to_qos_flow_id(2)) !=
              drb_to_add.at(uint_to_drb_id(1)).qos_flows.end());
  EXPECT_TRUE(drb_to_add.at(uint_to_drb_id(1)).source_drb_id_confirmed);
}

// Without a preference hint at all, allocation still succeeds as before, but the resulting DRB ID must not be
// treated as confirmed - the target has no basis to assume it matches the source's numbering.
TEST_F(up_resource_manager_test, when_no_preference_is_given_the_allocated_drb_id_is_not_confirmed)
{
  ngap_pdu_session_resource_setup_request msg = generate_pdu_session_resource_setup();
  ASSERT_TRUE(manager.validate_request(msg.pdu_session_res_setup_items));

  up_config_update update = manager.calculate_update(msg.pdu_session_res_setup_items, {});

  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
  const auto& drb_to_add = update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)).drb_to_add;
  ASSERT_EQ(drb_to_add.size(), 1);
  EXPECT_FALSE(drb_to_add.at(uint_to_drb_id(1)).source_drb_id_confirmed);
}

// If the source's preferred DRB ID is already taken at the target (e.g. by another PDU session), allocation must
// still succeed by falling back to the next free ID rather than failing or duplicating the DRB ID.
TEST_F(up_resource_manager_test, when_preferred_drb_id_is_already_taken_allocation_falls_back)
{
  // First PDU session takes DRB1 the normal way.
  setup_initial_pdu_session();

  // A second PDU session's sole QoS flow prefers DRB1 too (e.g. a stale/duplicated hint) - it must not collide with
  // the DRB already in use.
  ngap_pdu_session_resource_setup_request msg =
      generate_pdu_session_resource_setup(cu_cp_ue_index_t::min, uint_to_pdu_session_id(2), uint_to_qos_flow_id(1));
  ASSERT_TRUE(manager.validate_request(msg.pdu_session_res_setup_items));

  up_old_drb_association old_drb_association = {
      {uint_to_pdu_session_id(2), {{uint_to_qos_flow_id(1), uint_to_drb_id(1)}}}};

  up_config_update update = manager.calculate_update(msg.pdu_session_res_setup_items, {}, old_drb_association);

  ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1);
  const auto& drb_to_add = update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(2)).drb_to_add;
  ASSERT_EQ(drb_to_add.size(), 1);
  ASSERT_TRUE(drb_to_add.find(uint_to_drb_id(2)) != drb_to_add.end());
  EXPECT_TRUE(drb_to_add.find(uint_to_drb_id(1)) == drb_to_add.end());
  // The fallback-allocated ID must not be treated as confirmed, since it isn't the one the source asked for.
  EXPECT_FALSE(drb_to_add.at(uint_to_drb_id(2)).source_drb_id_confirmed);
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

  up_config_update update = manager.calculate_update(msg, {});

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
    up_config_update update = manager.calculate_update(setup_msg.pdu_session_res_setup_items, {});

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
    up_config_update update = manager.calculate_update(setup_msg.pdu_session_res_setup_items, {});

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
    up_config_update update = manager.calculate_update(setup_msg.pdu_session_res_setup_items, {});

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
    up_config_update update = manager.calculate_update(setup_msg.pdu_session_res_setup_items, {});

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
    up_config_update update = manager.calculate_update(setup_msg.pdu_session_res_setup_items, {});

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
    up_config_update update = manager.calculate_update(setup_msg.pdu_session_res_setup_items, {});

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
  up_config_update update = manager.calculate_update(setup_msg.pdu_session_res_setup_items, {});

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

static rrc_ue_capabilities_t redcap_bearer_capabilities(bool long_sn = false, bool drbs16 = false)
{
  rrc_ue_capabilities_t caps;
  caps.redcap.emplace();
  caps.redcap->pdcp_long_sn_supported = long_sn;
  caps.redcap->support_of_16_drb      = drbs16;
  return caps;
}

TEST_F(up_resource_manager_test, redcap_pdcp_policy_preserves_independent_optional_support_and_ordinary_defaults)
{
  cfg.max_nof_drbs_per_ue = 32;
  for (auto& qos : cfg.five_qi_config) {
    qos.second.pdcp.tx.sn_size = pdcp_sn_size::size18bits;
    qos.second.pdcp.rx.sn_size = pdcp_sn_size::size18bits;
  }
  for (bool redcap : {false, true}) {
    for (bool long_sn : {false, true}) {
      up_resource_manager m{cfg};
      auto                caps    = redcap ? redcap_bearer_capabilities(long_sn) : rrc_ue_capabilities_t{};
      auto                request = generate_pdu_session_resource_setup();
      auto                update  = m.calculate_update(request.pdu_session_res_setup_items, caps);
      const auto& drb      = update.pdu_sessions_to_setup_list.at(uint_to_pdu_session_id(1)).drb_to_add.begin()->second;
      const auto  expected = redcap && !long_sn ? pdcp_sn_size::size12bits : pdcp_sn_size::size18bits;
      EXPECT_EQ(drb.pdcp_cfg.tx.sn_size, expected);
      EXPECT_EQ(drb.pdcp_cfg.rx.sn_size, expected);
    }
  }
}

TEST_F(up_resource_manager_test, redcap_drb_limit_counts_pending_allocations_and_keeps_configured_lower_limit)
{
  for (unsigned configured : {4U, 32U}) {
    for (bool drbs16 : {false, true}) {
      cfg.max_nof_drbs_per_ue = configured;
      up_resource_manager m{cfg};
      auto                request = generate_pdu_session_resource_setup(cu_cp_ue_index_t::min, 3, 7);
      auto update  = m.calculate_update(request.pdu_session_res_setup_items, redcap_bearer_capabilities(false, drbs16));
      size_t count = 0;
      for (const auto& session : update.pdu_sessions_to_setup_list) {
        count += session.second.drb_to_add.size();
      }
      EXPECT_EQ(count, std::min(configured, drbs16 ? 16U : 8U));
    }
  }
}

TEST_F(up_resource_manager_test, redcap_modify_respects_active_and_pending_drb_limit)
{
  cfg.max_nof_drbs_per_ue = 32;
  up_resource_manager     m{cfg};
  auto                    caps    = redcap_bearer_capabilities();
  auto                    request = generate_pdu_session_resource_setup(cu_cp_ue_index_t::min, 1, 7);
  auto                    update  = m.calculate_update(request.pdu_session_res_setup_items, caps);
  up_config_update_result result;
  result.pdu_sessions_added_list.push_back(update.pdu_sessions_to_setup_list.begin()->second);
  m.apply_config_update(result);
  auto modification = generate_pdu_session_resource_modification(cu_cp_ue_index_t::min, 1, 8);
  auto extra = *modification.pdu_session_res_modify_items.begin()->transfer.qos_flow_add_or_modify_request_list.begin();
  extra.qos_flow_id = uint_to_qos_flow_id(9);
  modification.pdu_session_res_modify_items.begin()->transfer.qos_flow_add_or_modify_request_list.emplace(
      extra.qos_flow_id, extra);
  auto modified = m.calculate_update(modification, caps);
  EXPECT_EQ(modified.pdu_sessions_to_modify_list.begin()->second.drb_to_add.size(), 1U);
}

TEST_F(up_resource_manager_test, redcap_context_import_rejects_incompatible_sn_without_replacing_existing_state)
{
  setup_initial_pdu_session();
  auto  imported          = manager.get_up_context();
  auto& drb               = imported.pdu_sessions.begin()->second.drbs.begin()->second;
  drb.pdcp_cfg.tx.sn_size = pdcp_sn_size::size18bits;
  drb.pdcp_cfg.rx.sn_size = pdcp_sn_size::size18bits;
  auto caps               = redcap_bearer_capabilities();
  EXPECT_FALSE(manager.set_up_context(imported, caps));
  EXPECT_NE(manager.get_drb_context(drb.drb_id).pdcp_cfg.tx.sn_size, pdcp_sn_size::size18bits);
  caps.redcap->pdcp_long_sn_supported = true;
  EXPECT_TRUE(manager.set_up_context(imported, caps));
  EXPECT_EQ(manager.get_drb_context(drb.drb_id).pdcp_cfg.tx.sn_size, pdcp_sn_size::size18bits);
}

TEST_F(up_resource_manager_test, redcap_handover_uses_forwarded_capabilities_and_preserves_source_pdcp_sn)
{
  for (bool support_long_sn : {false, true}) {
    for (unsigned source_sn_bits : {12U, 18U}) {
      SCOPED_TRACE(fmt::format("long_sn={} source_sn={}", support_long_sn, source_sn_bits));
      auto container = test_helpers::make_redcap_handover_preparation(support_long_sn, source_sn_bits);
      auto caps      = decode_handover_capabilities(container);
      ASSERT_TRUE(caps.has_value());
      ASSERT_TRUE(caps->redcap.has_value());
      EXPECT_EQ(caps->redcap->pdcp_long_sn_supported, support_long_sn);
      up_old_drb_association association;
      merge_old_drb_association_from_as_config(association, container, ocudulog::fetch_basic_logger("CU-CP"));
      cfg.five_qi_config.at(uint_to_five_qi(9)).pdcp.tx.sn_size = pdcp_sn_size::size18bits;
      cfg.five_qi_config.at(uint_to_five_qi(9)).pdcp.rx.sn_size = pdcp_sn_size::size18bits;
      up_resource_manager m{cfg};
      auto                request  = generate_pdu_session_resource_setup();
      auto                update   = m.calculate_update(request.pdu_session_res_setup_items, *caps, association);
      const bool          expected = support_long_sn || source_sn_bits == 12;
      ASSERT_EQ(preserve_redcap_pdcp_sn_lengths(update, container, *caps), expected);
      if (expected) {
        const auto& drb = update.pdu_sessions_to_setup_list.begin()->second.drb_to_add.begin()->second;
        EXPECT_EQ(pdcp_sn_size_to_uint(drb.pdcp_cfg.tx.sn_size), source_sn_bits);
        EXPECT_EQ(pdcp_sn_size_to_uint(drb.pdcp_cfg.rx.sn_size), source_sn_bits);
        EXPECT_TRUE(drb.source_drb_id_confirmed);
      }
    }
  }
}

TEST_F(up_resource_manager_test, redcap_handover_rejects_malformed_or_missing_source_configuration)
{
  EXPECT_FALSE(decode_handover_capabilities(byte_buffer::create({0xff}).value()).has_value());
  auto request = generate_pdu_session_resource_setup();
  auto caps    = redcap_bearer_capabilities();
  auto update  = manager.calculate_update(request.pdu_session_res_setup_items, caps);
  EXPECT_FALSE(preserve_redcap_pdcp_sn_lengths(update, {}, caps));
}

TEST_F(up_resource_manager_test, redcap_existing_bearer_keeps_sn_lengths_when_qos_changes)
{
  auto caps    = redcap_bearer_capabilities(true);
  auto request = generate_pdu_session_resource_setup();
  auto update  = manager.calculate_update(request.pdu_session_res_setup_items, caps);
  auto session = update.pdu_sessions_to_setup_list.begin()->second;
  session.drb_to_add.begin()->second.pdcp_cfg.tx.sn_size = pdcp_sn_size::size18bits;
  session.drb_to_add.begin()->second.pdcp_cfg.rx.sn_size = pdcp_sn_size::size18bits;
  up_config_update_result result;
  result.pdu_sessions_added_list.push_back(session);
  manager.apply_config_update(result);
  auto        modification = generate_pdu_session_resource_modification(cu_cp_ue_index_t::min, 1, 1);
  auto        modified     = manager.calculate_update(modification, caps);
  const auto& drb          = modified.pdu_sessions_to_modify_list.begin()->second.drb_to_modify.begin()->second;
  EXPECT_EQ(drb.pdcp_cfg.tx.sn_size, pdcp_sn_size::size18bits);
  EXPECT_EQ(drb.pdcp_cfg.rx.sn_size, pdcp_sn_size::size18bits);
}

TEST_F(up_resource_manager_test, modifying_existing_drbs_does_not_consume_new_drb_capacity)
{
  auto                    caps    = redcap_bearer_capabilities();
  auto                    request = generate_pdu_session_resource_setup(cu_cp_ue_index_t::min, 2, 3);
  auto                    update  = manager.calculate_update(request.pdu_session_res_setup_items, caps);
  up_config_update_result result;
  for (const auto& session : update.pdu_sessions_to_setup_list) {
    result.pdu_sessions_added_list.push_back(session.second);
  }
  manager.apply_config_update(result);
  ASSERT_EQ(manager.get_nof_drbs(), 6U);
  auto modification = generate_pdu_session_resource_modification(cu_cp_ue_index_t::min, 1, 1);
  auto second       = generate_pdu_session_resource_modification(cu_cp_ue_index_t::min, 2, 4);
  auto extra = *second.pdu_session_res_modify_items.begin()->transfer.qos_flow_add_or_modify_request_list.begin();
  for (unsigned qfi : {7U, 8U}) {
    extra.qos_flow_id = uint_to_qos_flow_id(qfi);
    second.pdu_session_res_modify_items.begin()->transfer.qos_flow_add_or_modify_request_list.emplace(extra.qos_flow_id,
                                                                                                      extra);
  }
  modification.pdu_session_res_modify_items.emplace(uint_to_pdu_session_id(2),
                                                    *second.pdu_session_res_modify_items.begin());
  auto modified = manager.calculate_update(modification, caps);
  ASSERT_TRUE(modified.pdu_sessions_failed_to_modify_list.empty());
  EXPECT_EQ(modified.pdu_sessions_to_modify_list.at(uint_to_pdu_session_id(1)).drb_to_modify.size(), 1U);
  EXPECT_EQ(modified.pdu_sessions_to_modify_list.at(uint_to_pdu_session_id(2)).drb_to_modify.size(), 1U);
  EXPECT_EQ(modified.pdu_sessions_to_modify_list.at(uint_to_pdu_session_id(2)).drb_to_add.size(), 2U);
}

TEST_F(up_resource_manager_test, modification_rejects_a_bearer_that_belongs_to_another_session)
{
  auto                    caps    = redcap_bearer_capabilities();
  auto                    request = generate_pdu_session_resource_setup(cu_cp_ue_index_t::min, 2, 1);
  auto                    update  = manager.calculate_update(request.pdu_session_res_setup_items, caps);
  up_config_update_result result;
  for (const auto& session : update.pdu_sessions_to_setup_list) {
    result.pdu_sessions_added_list.push_back(session.second);
  }
  manager.apply_config_update(result);
  ASSERT_EQ(manager.get_nof_drbs(), 2U);
  auto modification = generate_pdu_session_resource_modification(cu_cp_ue_index_t::min, 2, 1);
  ASSERT_TRUE(manager.validate_request(modification));
  auto modified = manager.calculate_update(modification, caps);
  ASSERT_EQ(modified.pdu_sessions_failed_to_modify_list.size(), 1U);
  EXPECT_EQ(modified.pdu_sessions_failed_to_modify_list.front(), uint_to_pdu_session_id(2));
  EXPECT_EQ(manager.get_nof_drbs(), 2U);
}

TEST_F(up_resource_manager_test, capability_policy_does_not_change_configured_pdcp_defaults)
{
  for (auto& qos : cfg.five_qi_config) {
    qos.second.pdcp.tx.sn_size = pdcp_sn_size::size18bits;
    qos.second.pdcp.rx.sn_size = pdcp_sn_size::size18bits;
  }
  up_resource_manager m{cfg};
  auto                request = generate_pdu_session_resource_setup();
  for (const auto& caps : {redcap_bearer_capabilities(), rrc_ue_capabilities_t{}, redcap_bearer_capabilities(true)}) {
    const auto update = m.calculate_update(request.pdu_session_res_setup_items, caps);
    ASSERT_EQ(update.pdu_sessions_to_setup_list.size(), 1U);
    const auto& drb      = update.pdu_sessions_to_setup_list.begin()->second.drb_to_add.begin()->second;
    const auto  expected = caps.redcap.has_value() && !caps.redcap->pdcp_long_sn_supported ? pdcp_sn_size::size12bits
                                                                                           : pdcp_sn_size::size18bits;
    EXPECT_EQ(drb.pdcp_cfg.tx.sn_size, expected);
    EXPECT_EQ(drb.pdcp_cfg.rx.sn_size, expected);
  }
  EXPECT_EQ(cfg.five_qi_config.at(uint_to_five_qi(9)).pdcp.tx.sn_size, pdcp_sn_size::size18bits);
}

TEST_F(up_resource_manager_test, handover_capabilities_skip_malformed_nr_containers_without_losing_redcap_identity)
{
  asn1::rrc_nr::ho_prep_info_s info;
  auto&                        list = info.crit_exts.set_c1().set_ho_prep_info().ue_cap_rat_list;
  list.resize(2);
  list[0].rat_type             = asn1::rrc_nr::rat_type_opts::nr;
  list[0].ue_cap_rat_container = byte_buffer::create({0xff}).value();
  list[1].rat_type             = asn1::rrc_nr::rat_type_opts::nr;
  list[1].ue_cap_rat_container = test_helpers::make_captured_redcap_nr_capability();
  byte_buffer   packed;
  asn1::bit_ref writer{packed};
  ASSERT_EQ(info.pack(writer), asn1::OCUDUASN_SUCCESS);
  const auto caps = decode_handover_capabilities(packed);
  ASSERT_TRUE(caps.has_value());
  EXPECT_TRUE(caps->redcap.has_value());
  list.resize(1);
  byte_buffer   malformed;
  asn1::bit_ref malformed_writer{malformed};
  ASSERT_EQ(info.pack(malformed_writer), asn1::OCUDUASN_SUCCESS);
  EXPECT_FALSE(decode_handover_capabilities(malformed).has_value());
}
