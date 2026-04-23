// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../du_manager_test_helpers.h"
#include "ocudu/asn1/rrc_nr/bcch_dl_sch_msg.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include "ocudu/du/du_high/du_manager/du_manager_factory.h"
#include "ocudu/support/executors/task_worker.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

class du_manager_procedure_tester
{
public:
  du_manager_procedure_tester(std::vector<du_cell_config> cfgs = {config_helpers::make_default_du_cell_config()}) :
    cell_cfgs(cfgs), dependencies(cell_cfgs), du_mng(create_du_manager(dependencies.params))
  {
    // Generate automatic responses from F1AP and MAC.
    dependencies.f1ap.wait_f1_setup.result.value().cells_to_activate.resize(cfgs.size());
    for (unsigned i = 0; i != cfgs.size(); ++i) {
      dependencies.f1ap.wait_f1_setup.result.value().cells_to_activate[i].cgi = cell_cfgs[i].nr_cgi;
    }
    dependencies.f1ap.wait_f1_setup.ready_ev.set();
    dependencies.f1ap.wait_f1_removal.ready_ev.set();
    dependencies.mac.mac_cell.wait_start.ready_ev.set();
    dependencies.mac.mac_cell.wait_stop.ready_ev.set();

    // Start DU manager.
    du_mng->get_controller().start();
  }
  ~du_manager_procedure_tester()
  {
    std::atomic<bool> done{false};
    worker.push_task_blocking([this, &done]() {
      du_mng->get_controller().stop();
      done = true;
    });
    while (not done) {
      dependencies.worker.run_pending_tasks();
      std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
    worker.wait_pending_tasks();
  }

  task_worker                 worker{"worker", 16};
  std::vector<du_cell_config> cell_cfgs;
  du_manager_test_bench       dependencies;
  std::unique_ptr<du_manager> du_mng;
};

static du_param_config_request make_dummy_request(span<const du_cell_config> cell_cfgs)
{
  du_param_config_request req;
  req.cells.resize(1);
  req.cells[0].nr_cgi      = cell_cfgs[0].nr_cgi;
  req.cells[0].ssb_pwr_mod = cell_cfgs[0].ran.ssb_cfg.ssb_block_power + 1;
  return req;
}

class du_manager_du_config_update_test : public du_manager_procedure_tester, public ::testing::Test
{};

TEST_F(du_manager_du_config_update_test, when_sib1_change_required_then_mac_is_reconfigured_and_f1ap_initiates_du_cfg)
{
  // Initiate procedure.
  du_param_config_request  req  = make_dummy_request(cell_cfgs);
  du_param_config_response resp = du_mng->get_operation_configurator().handle_sync_operator_config(req);

  // MAC received config request.
  ASSERT_TRUE(dependencies.mac.mac_cell.last_cell_recfg_req.has_value());

  // F1AP received the DU config request.
  ASSERT_TRUE(dependencies.f1ap.last_du_cfg_req);

  ASSERT_TRUE(resp.success);
}

static du_param_config_request make_dummy_rrm_request()
{
  du_param_config_request req;
  req.cells.resize(1);
  req.cells[0].nr_cgi = std::nullopt;
  rrm_policy_ratio_group rrm_policy;
  rrm_policy.minimum_ratio   = 30;
  rrm_policy.maximum_ratio   = 90;
  rrm_policy.dedicated_ratio = 0;
  rrm_policy.resource_type   = rrm_policy_ratio_group::resource_type_t::prb;
  rrm_policy.policy_members_list.emplace_back(
      rrm_policy_member{.plmn_id = plmn_identity{mobile_country_code::from_string("001").value(),
                                                 mobile_network_code::from_string("01").value()},
                        .s_nssai = s_nssai_t{.sst = slice_service_type{1}}});
  req.cells[0].rrm_policy_ratio_list.emplace_back(rrm_policy);
  return req;
}

class du_manager_du_rrm_config_update_test : public du_manager_procedure_tester, public ::testing::Test
{
public:
  du_manager_du_rrm_config_update_test() :
    du_manager_procedure_tester([]() {
      std::vector<du_cell_config> cfgs;
      for (unsigned i = 0; i != NOF_CELLS; ++i) {
        cell_config_builder_params params;
        params.pci = static_cast<pci_t>(1U + i);
        cfgs.push_back(config_helpers::make_default_du_cell_config(params));
        cfgs.back().rrm_policy_members.emplace_back(slice_rrm_policy_config{
            .rrc_member = rrm_policy_member{.plmn_id = plmn_identity{mobile_country_code::from_string("001").value(),
                                                                     mobile_network_code::from_string("01").value()},
                                            .s_nssai = s_nssai_t{.sst = slice_service_type{1}}}});
        cfgs.back().nr_cgi.nci = nr_cell_identity::create({411, 22}, 1 + i).value();
      }
      return cfgs;
    }())
  {
  }

  static constexpr unsigned NOF_CELLS = 3U;
};

TEST_F(du_manager_du_rrm_config_update_test, when_rrm_policy_change_required_then_mac_is_reconfigured)
{
  // Initiate procedure.
  du_param_config_request  req  = make_dummy_rrm_request();
  du_param_config_response resp = du_mng->get_operation_configurator().handle_sync_operator_config(req);

  // MAC received config request.
  ASSERT_TRUE(dependencies.mac.mac_cell.last_cell_recfg_req.has_value());

  ASSERT_TRUE(resp.success);
}

// ============================================================================================
// Value Tag Increment Tests
// ============================================================================================

class du_manager_value_tag_test : public du_manager_procedure_tester, public ::testing::Test
{
public:
  du_manager_value_tag_test() :
    du_manager_procedure_tester([]() {
      std::vector<du_cell_config> cfgs;
      cfgs.push_back(config_helpers::make_default_du_cell_config());

      // Initialize SI config with SIB2 and SIB19.
      cfgs.back().si.si_config.emplace();
      cfgs.back().si.si_config->si_window_len_slots = 10;

      // Add SI message scheduling info for SIB2.
      si_message_sched_info si_msg_sib2;
      si_msg_sib2.sib_mapping_info.push_back(sib_type::sib2);
      si_msg_sib2.si_period_radio_frames = 32;
      cfgs.back().si.si_config->si_sched_info.push_back(si_msg_sib2);

      // Add SI message scheduling info for SIB19.
      si_message_sched_info si_msg_sib19;
      si_msg_sib19.sib_mapping_info.push_back(sib_type::sib19);
      si_msg_sib19.si_period_radio_frames = 64;
      cfgs.back().si.si_config->si_sched_info.push_back(si_msg_sib19);

      // Add SI message scheduling info for SIB3 (intra-freq neighbors).
      si_message_sched_info si_msg_sib3;
      si_msg_sib3.sib_mapping_info.push_back(sib_type::sib3);
      si_msg_sib3.si_period_radio_frames = 32;
      cfgs.back().si.si_config->si_sched_info.push_back(si_msg_sib3);

      // Add SI message scheduling info for SIB4 (inter-freq neighbors).
      si_message_sched_info si_msg_sib4;
      si_msg_sib4.sib_mapping_info.push_back(sib_type::sib4);
      si_msg_sib4.si_period_radio_frames = 32;
      cfgs.back().si.si_config->si_sched_info.push_back(si_msg_sib4);

      // Add SIB2, SIB19, SIB3, SIB4 with initial value_tag=0.
      cfgs.back().si.si_config->sibs.push_back(sib_type_info{create_default_sib2(), value_tag_t{0}});
      cfgs.back().si.si_config->sibs.push_back(sib_type_info{create_default_sib19(), value_tag_t{0}});
      cfgs.back().si.si_config->sibs.push_back(sib_type_info{create_default_sib3(), value_tag_t{0}});
      cfgs.back().si.si_config->sibs.push_back(sib_type_info{create_default_sib4(), value_tag_t{0}});

      return cfgs;
    }())
  {
  }

  // Factory for baseline SIB2.
  static sib2_info create_default_sib2()
  {
    sib2_info sib2;
    sib2.q_hyst                    = q_hyst_t::db4;
    sib2.thresh_serving_low_p      = reselection_threshold_t{14};
    sib2.cell_reselection_priority = cell_reselection_priority_t{4};
    sib2.q_rx_lev_min              = q_rx_lev_min_t{-70};
    sib2.s_intra_search_p          = reselection_threshold_t{31};
    sib2.t_reselection_nr          = t_reselection_t{1};
    return sib2;
  }

  // Factory for modified SIB2 (different values).
  static sib2_info create_modified_sib2()
  {
    sib2_info sib2;
    sib2.q_hyst                    = q_hyst_t::db6;
    sib2.thresh_serving_low_p      = reselection_threshold_t{16};
    sib2.cell_reselection_priority = cell_reselection_priority_t{5};
    sib2.q_rx_lev_min              = q_rx_lev_min_t{-68};
    sib2.s_intra_search_p          = reselection_threshold_t{30};
    sib2.t_reselection_nr          = t_reselection_t{2};
    return sib2;
  }

  // Factory for baseline SIB19 (NTN configuration).
  static sib19_info create_default_sib19()
  {
    sib19_info sib19;
    sib19.ntn_cfg.emplace();
    sib19.ntn_cfg->cell_specific_koffset = std::chrono::milliseconds(100);
    return sib19;
  }

  // Factory for modified SIB19 (different values).
  static sib19_info create_modified_sib19()
  {
    sib19_info sib19;
    sib19.ntn_cfg.emplace();
    sib19.ntn_cfg->cell_specific_koffset = std::chrono::milliseconds(200);
    return sib19;
  }

  // Factory for baseline SIB3 (intra-freq neighbor list).
  static sib3_info create_default_sib3()
  {
    sib3_info sib3;
    sib3.intra_freq_neigh_cell_list.push_back(
        intra_freq_neigh_cell_info{.pci = 47, .q_offset_cell = q_offset_range_t::db0});
    return sib3;
  }

  // Factory for modified SIB3 (different neighbor list).
  static sib3_info create_modified_sib3()
  {
    sib3_info sib3;
    sib3.intra_freq_neigh_cell_list.push_back(
        intra_freq_neigh_cell_info{.pci = 47, .q_offset_cell = q_offset_range_t::db2});
    sib3.intra_freq_neigh_cell_list.push_back(
        intra_freq_neigh_cell_info{.pci = 48, .q_offset_cell = q_offset_range_t::db_2});
    return sib3;
  }

  // Factory for baseline SIB4 (inter-freq carrier list).
  static sib4_info create_default_sib4()
  {
    sib4_info                    sib4;
    inter_freq_carrier_freq_info carrier;
    carrier.arfcn                      = 649632;
    carrier.ssb_scs                    = subcarrier_spacing::kHz30;
    carrier.derive_ssb_index_from_cell = true;
    carrier.q_rx_lev_min               = q_rx_lev_min_t{-70};
    carrier.thresh_x_high_p            = reselection_threshold_t{16};
    carrier.thresh_x_low_p             = reselection_threshold_t{4};
    carrier.q_offset_freq              = q_offset_range_t::db0;
    sib4.inter_freq_carrier_freq_list.push_back(carrier);
    return sib4;
  }

  // Factory for modified SIB4 (different thresholds).
  static sib4_info create_modified_sib4()
  {
    sib4_info                    sib4;
    inter_freq_carrier_freq_info carrier;
    carrier.arfcn                      = 649632;
    carrier.ssb_scs                    = subcarrier_spacing::kHz30;
    carrier.derive_ssb_index_from_cell = true;
    carrier.q_rx_lev_min               = q_rx_lev_min_t{-68};
    carrier.thresh_x_high_p            = reselection_threshold_t{18};
    carrier.thresh_x_low_p             = reselection_threshold_t{6};
    carrier.q_offset_freq              = q_offset_range_t::db2;
    sib4.inter_freq_carrier_freq_list.push_back(carrier);
    return sib4;
  }

  // Get current value_tag for a specific SIB type by decoding the SIB1 sent to MAC.
  value_tag_t get_value_tag_for_sib(sib_type type)
  {
    // Verify MAC received the SIB update.
    EXPECT_TRUE(dependencies.mac.mac_cell.last_cell_recfg_req.has_value());
    EXPECT_TRUE(dependencies.mac.mac_cell.last_cell_recfg_req->new_sys_info.has_value());
    const auto& sib1_buffer = dependencies.mac.mac_cell.last_cell_recfg_req->new_sys_info->sib1;
    EXPECT_FALSE(sib1_buffer.empty());

    // Decode the BCCH-DL-SCH message (which wraps SIB1).
    asn1::cbit_ref                  bref{sib1_buffer};
    asn1::rrc_nr::bcch_dl_sch_msg_s bcch_msg;
    EXPECT_EQ(bcch_msg.unpack(bref), asn1::OCUDUASN_SUCCESS);

    // Extract the SIB1 from the message.
    EXPECT_EQ(bcch_msg.msg.type().value, asn1::rrc_nr::bcch_dl_sch_msg_type_c::types_opts::c1);
    EXPECT_EQ(bcch_msg.msg.c1().type().value, asn1::rrc_nr::bcch_dl_sch_msg_type_c::c1_c_::types_opts::sib_type1);
    const auto& sib1_msg = bcch_msg.msg.c1().sib_type1();

    // Navigate to the SI scheduling info.
    EXPECT_TRUE(sib1_msg.si_sched_info_present);
    const auto& si_sched_info = sib1_msg.si_sched_info;

    // Find the scheduling info entry that contains the requested SIB type.
    uint8_t sib_number = static_cast<uint8_t>(type);
    for (const auto& sched_info : si_sched_info.sched_info_list) {
      for (const auto& sib_type_info : sched_info.sib_map_info) {
        if (sib_type_info.type.to_number() == sib_number) {
          EXPECT_TRUE(sib_type_info.value_tag_present);
          return value_tag_t{sib_type_info.value_tag};
        }
      }
    }

    // Check Release 17 extensions for SIB19 and other R17 SIBs.
    if (sib1_msg.non_crit_ext_present && sib1_msg.non_crit_ext.non_crit_ext_present &&
        sib1_msg.non_crit_ext.non_crit_ext.non_crit_ext_present &&
        sib1_msg.non_crit_ext.non_crit_ext.non_crit_ext.si_sched_info_v1700_present) {
      const auto& si_sched_v1700 = sib1_msg.non_crit_ext.non_crit_ext.non_crit_ext.si_sched_info_v1700;
      for (const auto& sched_info_r17 : si_sched_v1700.sched_info_list2_r17) {
        for (const auto& sib_type_info_r17 : sched_info_r17.sib_map_info_r17) {
          // Check if this is the SIB type we're looking for.
          if (sib_type_info_r17.sib_type_r17.type().value ==
              asn1::rrc_nr::sib_type_info_v1700_s::sib_type_r17_c_::types_opts::type1_r17) {
            // Convert the ASN.1 enum to our sib_type.
            const auto& type1_r17       = sib_type_info_r17.sib_type_r17.type1_r17();
            uint8_t     asn1_sib_number = 0;
            if (type1_r17.value == asn1::rrc_nr::sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_e_::sib_type16) {
              asn1_sib_number = 16;
            } else if (type1_r17.value ==
                       asn1::rrc_nr::sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_e_::sib_type19) {
              asn1_sib_number = 19;
            }
            if (asn1_sib_number == sib_number) {
              return value_tag_t{sib_type_info_r17.value_tag_r17};
            }
          }
        }
      }
    }

    // Should not reach here if the SIB type is configured.
    EXPECT_TRUE(false) << "SIB" << static_cast<int>(sib_number) << " not found in SIB1";
    return value_tag_t{0};
  }

  // Build a SIB update request.
  du_param_config_request make_sib_update_request(sib_info new_sib_content)
  {
    du_param_config_request req;
    req.cells.resize(1);
    req.cells[0].nr_cgi       = cell_cfgs[0].nr_cgi;
    req.cells[0].new_sys_info = std::move(new_sib_content);
    return req;
  }

  // Extract SIB19 content from the SI message sent to MAC.
  sib19_info get_sib19_content_from_si_message()
  {
    // Verify MAC received the SI message update.
    EXPECT_TRUE(dependencies.mac.mac_cell.last_cell_recfg_req.has_value());
    EXPECT_TRUE(dependencies.mac.mac_cell.last_cell_recfg_req->new_sys_info.has_value());
    const auto& si_messages = dependencies.mac.mac_cell.last_cell_recfg_req->new_sys_info->si_messages;

    // SIB19 is in the second SI message (index 1) per fixture setup.
    EXPECT_GE(si_messages.size(), 2);
    EXPECT_GE(si_messages[1].size(), 1);
    const auto& si_msg_buffer = si_messages[1][0];
    EXPECT_FALSE(si_msg_buffer.empty());

    // Decode the BCCH-DL-SCH message containing the SI message.
    asn1::cbit_ref                  bref{si_msg_buffer};
    asn1::rrc_nr::bcch_dl_sch_msg_s bcch_msg;
    EXPECT_EQ(bcch_msg.unpack(bref), asn1::OCUDUASN_SUCCESS);

    // Extract the SystemInformation message.
    EXPECT_EQ(bcch_msg.msg.type().value, asn1::rrc_nr::bcch_dl_sch_msg_type_c::types_opts::c1);
    EXPECT_EQ(bcch_msg.msg.c1().type().value, asn1::rrc_nr::bcch_dl_sch_msg_type_c::c1_c_::types_opts::sys_info);
    const auto& sys_info = bcch_msg.msg.c1().sys_info();

    // Find SIB19 in the sib_type_and_info list.
    EXPECT_TRUE(sys_info.crit_exts.type().value == asn1::rrc_nr::sys_info_s::crit_exts_c_::types_opts::sys_info);
    const auto& si_content = sys_info.crit_exts.sys_info();

    for (const auto& sib_entry : si_content.sib_type_and_info) {
      if (sib_entry.type().value == asn1::rrc_nr::sys_info_ies_s::item_c_::types_opts::sib19_v1700) {
        const auto& sib19_asn1 = sib_entry.sib19_v1700();

        // Convert ASN.1 SIB19 to our sib19_info structure.
        sib19_info sib19;
        if (sib19_asn1.ntn_cfg_r17_present) {
          sib19.ntn_cfg.emplace();
          const auto& ntn_cfg_asn1 = sib19_asn1.ntn_cfg_r17;
          if (ntn_cfg_asn1.cell_specific_koffset_r17_present) {
            sib19.ntn_cfg->cell_specific_koffset = std::chrono::milliseconds(ntn_cfg_asn1.cell_specific_koffset_r17);
          }
        }
        return sib19;
      }
    }

    EXPECT_TRUE(false) << "SIB19 not found in SI message";
    return sib19_info{};
  }
};

TEST_F(du_manager_value_tag_test, when_sib_updated_then_value_tag_increments_from_0_to_1)
{
  // Send SIB update request (initial value_tag is 0 from fixture setup).
  du_param_config_request  req  = make_sib_update_request(create_modified_sib2());
  du_param_config_response resp = du_mng->get_operation_configurator().handle_sync_operator_config(req);

  // Process any pending tasks.
  dependencies.worker.run_pending_tasks();

  // Verify response success.
  ASSERT_TRUE(resp.success);

  // Verify MAC received reconfiguration request.
  ASSERT_TRUE(dependencies.mac.mac_cell.last_cell_recfg_req.has_value());

  // Verify F1AP DU config update triggered.
  ASSERT_TRUE(dependencies.f1ap.last_du_cfg_req.has_value());

  // Verify value_tag incremented to 1.
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib2).value(), 1);
}

TEST_F(du_manager_value_tag_test, when_sib_updated_multiple_times_then_value_tag_increments_sequentially)
{
  // Perform first update (0 -> 1, initial value_tag is 0 from fixture setup).
  du_param_config_request  req1  = make_sib_update_request(create_modified_sib2());
  du_param_config_response resp1 = du_mng->get_operation_configurator().handle_sync_operator_config(req1);
  dependencies.worker.run_pending_tasks();
  ASSERT_TRUE(resp1.success);
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib2).value(), 1);

  // Perform second update (1 -> 2).
  du_param_config_request  req2  = make_sib_update_request(create_default_sib2());
  du_param_config_response resp2 = du_mng->get_operation_configurator().handle_sync_operator_config(req2);
  dependencies.worker.run_pending_tasks();
  ASSERT_TRUE(resp2.success);
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib2).value(), 2);

  // Perform third update (2 -> 3).
  du_param_config_request  req3  = make_sib_update_request(create_modified_sib2());
  du_param_config_response resp3 = du_mng->get_operation_configurator().handle_sync_operator_config(req3);
  dependencies.worker.run_pending_tasks();
  ASSERT_TRUE(resp3.success);
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib2).value(), 3);
}

TEST_F(du_manager_value_tag_test, when_value_tag_reaches_31_then_wraps_to_0)
{
  // Perform 31 updates to reach value_tag = 31 (starting from 0).
  for (int i = 0; i < 31; i++) {
    du_param_config_request  req = make_sib_update_request(i % 2 == 0 ? create_modified_sib2() : create_default_sib2());
    du_param_config_response resp = du_mng->get_operation_configurator().handle_sync_operator_config(req);
    dependencies.worker.run_pending_tasks();
    ASSERT_TRUE(resp.success);
  }

  // Verify value_tag = 31.
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib2).value(), 31);

  // Perform one more update to test wrapping (31 -> 0).
  du_param_config_request  final_req  = make_sib_update_request(create_modified_sib2());
  du_param_config_response final_resp = du_mng->get_operation_configurator().handle_sync_operator_config(final_req);
  dependencies.worker.run_pending_tasks();

  // Verify response success.
  ASSERT_TRUE(final_resp.success);

  // Verify MAC and F1AP notified.
  ASSERT_TRUE(dependencies.mac.mac_cell.last_cell_recfg_req.has_value());
  ASSERT_TRUE(dependencies.f1ap.last_du_cfg_req.has_value());

  // Verify value_tag wrapped to 0.
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib2).value(), 0);
}

TEST_F(du_manager_value_tag_test, when_sib19_updated_then_value_tag_increments_independently)
{
  // Send SIB19 update request (initial value_tag is 0 from fixture setup).
  du_param_config_request  req  = make_sib_update_request(create_modified_sib19());
  du_param_config_response resp = du_mng->get_operation_configurator().handle_sync_operator_config(req);

  // Process any pending tasks.
  dependencies.worker.run_pending_tasks();

  // Verify response success.
  ASSERT_TRUE(resp.success);

  // Verify SIB19 value_tag incremented to 1.
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib19).value(), 1);

  // Verify SIB2 value_tag is still 0 (independent value_tags per SIB type).
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib2).value(), 0);

  // Verify MAC and F1AP were notified.
  ASSERT_TRUE(dependencies.mac.mac_cell.last_cell_recfg_req.has_value());
  ASSERT_TRUE(dependencies.f1ap.last_du_cfg_req.has_value());

  // Verify SIB19 content was updated correctly.
  sib19_info decoded_sib19 = get_sib19_content_from_si_message();
  ASSERT_TRUE(decoded_sib19.ntn_cfg.has_value());
  ASSERT_TRUE(decoded_sib19.ntn_cfg->cell_specific_koffset.has_value());
  ASSERT_EQ(decoded_sib19.ntn_cfg->cell_specific_koffset.value(), std::chrono::milliseconds(200));
}

TEST_F(du_manager_value_tag_test, when_sib3_updated_then_value_tag_increments_independently)
{
  // Send SIB3 update request.
  du_param_config_request  req  = make_sib_update_request(create_modified_sib3());
  du_param_config_response resp = du_mng->get_operation_configurator().handle_sync_operator_config(req);

  dependencies.worker.run_pending_tasks();

  ASSERT_TRUE(resp.success);

  // SIB3 value_tag incremented to 1.
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib3).value(), 1);

  // Other SIBs' value_tags are untouched.
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib2).value(), 0);
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib4).value(), 0);

  // MAC and F1AP were notified.
  ASSERT_TRUE(dependencies.mac.mac_cell.last_cell_recfg_req.has_value());
  ASSERT_TRUE(dependencies.f1ap.last_du_cfg_req.has_value());
}

TEST_F(du_manager_value_tag_test, when_sib4_updated_then_value_tag_increments_independently)
{
  // Send SIB4 update request.
  du_param_config_request  req  = make_sib_update_request(create_modified_sib4());
  du_param_config_response resp = du_mng->get_operation_configurator().handle_sync_operator_config(req);

  dependencies.worker.run_pending_tasks();

  ASSERT_TRUE(resp.success);

  // SIB4 value_tag incremented to 1.
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib4).value(), 1);

  // Other SIBs' value_tags are untouched.
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib2).value(), 0);
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib3).value(), 0);

  // MAC and F1AP were notified.
  ASSERT_TRUE(dependencies.mac.mac_cell.last_cell_recfg_req.has_value());
  ASSERT_TRUE(dependencies.f1ap.last_du_cfg_req.has_value());
}

TEST_F(du_manager_value_tag_test, when_sib3_and_sib4_updated_in_sequence_then_each_value_tag_tracks_independently)
{
  // Update SIB3: expect SIB3 -> 1, SIB4 stays at 0.
  du_param_config_request  req1  = make_sib_update_request(create_modified_sib3());
  du_param_config_response resp1 = du_mng->get_operation_configurator().handle_sync_operator_config(req1);
  dependencies.worker.run_pending_tasks();
  ASSERT_TRUE(resp1.success);
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib3).value(), 1);
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib4).value(), 0);

  // Update SIB4: expect SIB4 -> 1, SIB3 stays at 1.
  du_param_config_request  req2  = make_sib_update_request(create_modified_sib4());
  du_param_config_response resp2 = du_mng->get_operation_configurator().handle_sync_operator_config(req2);
  dependencies.worker.run_pending_tasks();
  ASSERT_TRUE(resp2.success);
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib4).value(), 1);
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib3).value(), 1);

  // Update SIB3 again: expect SIB3 -> 2, SIB4 stays at 1.
  du_param_config_request  req3  = make_sib_update_request(create_default_sib3());
  du_param_config_response resp3 = du_mng->get_operation_configurator().handle_sync_operator_config(req3);
  dependencies.worker.run_pending_tasks();
  ASSERT_TRUE(resp3.success);
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib3).value(), 2);
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib4).value(), 1);

  // SIB2 is still at 0 throughout.
  ASSERT_EQ(get_value_tag_for_sib(sib_type::sib2).value(), 0);
}
