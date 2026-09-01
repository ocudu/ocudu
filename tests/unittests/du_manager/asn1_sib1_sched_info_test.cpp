// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/du/du_high/du_manager/converters/asn1_sys_info_packer.h"
#include "lib/du/du_high/du_manager/converters/f1ap_configuration_helpers.h"
#include "lib/du/du_high/du_manager/converters/scheduler_configuration_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include "ocudu/du/du_cell_config_validation.h"
#include "ocudu/pcap/mac_pcap.h"
#include "ocudu/ran/sib/system_info_config.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

// Global variables for PCAP support, required by du_manager_converters.
bool             g_enable_pcap = false;
ocudu::mac_pcap* g_pcap        = nullptr;

/// A cell configured with a reserved (dormant) SI-message occasion for SIB6, but no etws_cfg content -- i.e. the
/// SI-message requires activation and has no matching entry in si_config->sibs yet.
static du_cell_config make_cell_config_with_dormant_pws_si_message()
{
  du_cell_config cfg = config_helpers::make_default_du_cell_config();

  cfg.si.si_config.emplace();
  cfg.si.si_config->si_window_len_slots = 10;

  si_message_sched_info si_msg;
  si_msg.sib_mapping_info       = {sib_type::sib6};
  si_msg.si_period_radio_frames = 32;
  cfg.si.si_config->si_sched_info.push_back(si_msg);
  // Note: si_config->sibs is left empty -- no content configured for the dormant SIB6, which is what makes this SI
  // message require activation.

  return cfg;
}

TEST(asn1_sib1_sched_info_test, pws_si_message_is_packed_even_without_content)
{
  du_cell_config cell_cfg = make_cell_config_with_dormant_pws_si_message();

  byte_buffer buf = asn1_packer::pack_sib1(cell_cfg, si_message_set::every_si_message);

  asn1::cbit_ref       bref{buf};
  asn1::rrc_nr::sib1_s sib1;
  ASSERT_EQ(sib1.unpack(bref), asn1::OCUDUASN_SUCCESS);

  // This payload is the reference the MAC derives every SI epoch from, keeping the entry of a PWS SI-message only
  // while its warning is on air. Dropping the entry here for the lack of content would leave the MAC with nothing to
  // list once a warning starts.
  ASSERT_TRUE(sib1.si_sched_info_present) << "schedulingInfoList must not be dropped for a PWS SI-message";
  ASSERT_EQ(sib1.si_sched_info.sched_info_list.size(), 1);

  const auto& sched_info = sib1.si_sched_info.sched_info_list[0];
  ASSERT_EQ(sched_info.sib_map_info.size(), 1);
  EXPECT_EQ(sched_info.sib_map_info[0].type.value, asn1::rrc_nr::sib_type_info_s::type_opts::sib_type6);
  EXPECT_FALSE(sched_info.sib_map_info[0].value_tag_present) << "A SIB with no content has no value tag";

  // Every entry this payload holds is one being broadcast, given that the MAC removes the ones it must not list.
  EXPECT_EQ(sched_info.si_broadcast_status.value, asn1::rrc_nr::sched_info_s::si_broadcast_status_opts::broadcasting);
}

TEST(asn1_sib1_sched_info_test, si_message_with_content_is_packed_with_its_value_tag)
{
  du_cell_config cell_cfg                                       = make_cell_config_with_dormant_pws_si_message();
  cell_cfg.si.si_config->si_sched_info.front().sib_mapping_info = {sib_type::sib2};

  sib2_info sib2;
  sib2.q_hyst                    = q_hyst_t::db4;
  sib2.thresh_serving_low_p      = reselection_threshold_t{14};
  sib2.cell_reselection_priority = cell_reselection_priority_t{4};
  sib2.q_rx_lev_min              = q_rx_lev_min_t{-70};
  sib2.s_intra_search_p          = reselection_threshold_t{31};
  sib2.t_reselection_nr          = t_reselection_t{1};
  cell_cfg.si.si_config->sibs.push_back(sib_type_info{sib2, value_tag_t{0}});

  byte_buffer buf = asn1_packer::pack_sib1(cell_cfg, si_message_set::every_si_message);

  asn1::cbit_ref       bref{buf};
  asn1::rrc_nr::sib1_s sib1;
  ASSERT_EQ(sib1.unpack(bref), asn1::OCUDUASN_SUCCESS);

  ASSERT_TRUE(sib1.si_sched_info_present);
  ASSERT_EQ(sib1.si_sched_info.sched_info_list.size(), 1);

  const auto& sched_info = sib1.si_sched_info.sched_info_list[0];
  ASSERT_EQ(sched_info.sib_map_info.size(), 1);
  EXPECT_EQ(sched_info.sib_map_info[0].type.value, asn1::rrc_nr::sib_type_info_s::type_opts::sib_type2);
  EXPECT_TRUE(sched_info.sib_map_info[0].value_tag_present);
  EXPECT_EQ(sched_info.sib_map_info[0].value_tag, 0);
}

/// SIBs carried by each SI message of an SI scheduling configuration, in the order it holds them.
static std::vector<sib_type> first_sib_of_each(const si_scheduling_config& si_sched_cfg)
{
  std::vector<sib_type> sibs;
  for (const si_message_scheduling_config& si_msg : si_sched_cfg.si_messages) {
    sibs.push_back(si_msg.sibs.front());
  }
  return sibs;
}

TEST(asn1_sib1_sched_info_test, pws_si_message_takes_no_position_in_the_si_scheduling_of_a_starting_cell)
{
  // A cell whose SI scheduling lists the SIB8 SI message between two that are always broadcast.
  du_cell_config cell_cfg                                       = make_cell_config_with_dormant_pws_si_message();
  cell_cfg.si.si_config->si_sched_info.front().sib_mapping_info = {sib_type::sib2};
  cell_cfg.si.si_config->si_sched_info.push_back(si_message_sched_info{{sib_type::sib8}, 64});
  cell_cfg.si.si_config->si_sched_info.push_back(si_message_sched_info{{sib_type::sib3}, 64});

  const std::array<units::bytes, 3> si_msg_lens{units::bytes{10}, units::bytes{10}, units::bytes{10}};
  const si_scheduling_config        cell_si_sched_cfg =
      make_si_scheduling_info_config(cell_cfg, units::bytes{100}, si_msg_lens);

  const sched_cell_configuration_request_message sched_req =
      make_sched_cell_config_req(to_du_cell_index(0), cell_cfg, cell_si_sched_cfg, 8);

  // The SI window of an SI message derives from its position, and the SIB1 of a cell in the normal operation does not
  // list the SIB8 one. Leaving a position for it would put the SIB3 one in a window the UE does not listen on.
  ASSERT_EQ(first_sib_of_each(sched_req.si_scheduling), (std::vector<sib_type>{sib_type::sib2, sib_type::sib3}));
}

/// \brief Cell whose SI scheduling holds a SIB2 SI message and a SIB6 one.
/// \param pws_test_mode Whether the SIB6 SI message is given content and broadcast right away, as test_mode does.
static du_cell_config make_cell_config_with_sib2_and_pws(bool pws_test_mode)
{
  du_cell_config cfg                                       = make_cell_config_with_dormant_pws_si_message();
  cfg.si.si_config->si_sched_info.front().sib_mapping_info = {sib_type::sib2};
  cfg.si.si_config->si_sched_info.push_back(si_message_sched_info{{sib_type::sib6}, 64});

  sib2_info sib2;
  sib2.q_hyst                    = q_hyst_t::db4;
  sib2.thresh_serving_low_p      = reselection_threshold_t{14};
  sib2.cell_reselection_priority = cell_reselection_priority_t{4};
  sib2.q_rx_lev_min              = q_rx_lev_min_t{-70};
  sib2.s_intra_search_p          = reselection_threshold_t{31};
  sib2.t_reselection_nr          = t_reselection_t{1};
  cfg.si.si_config->sibs.push_back(sib_type_info{sib2, value_tag_t{0}});

  if (pws_test_mode) {
    cfg.si.si_config->si_sched_info.back().auto_broadcast = true;
    cfg.si.si_config->sibs.push_back(sib_type_info{sib6_info{4352, 0, 0}, value_tag_t{0}});
  }

  return cfg;
}

/// SIBs that the schedulingInfoList of a packed SIB1 lists, in the listed order.
static std::vector<sib_type> listed_sibs_of(const byte_buffer& packed_sib1)
{
  asn1::cbit_ref       bref{packed_sib1};
  asn1::rrc_nr::sib1_s sib1;
  report_fatal_error_if_not(sib1.unpack(bref) == asn1::OCUDUASN_SUCCESS, "Failed to unpack the SIB1");

  std::vector<sib_type> sibs;
  if (not sib1.si_sched_info_present) {
    return sibs;
  }
  for (const auto& sched_info : sib1.si_sched_info.sched_info_list) {
    report_fatal_error_if_not(sched_info.sib_map_info.size() == 1, "Test SI messages carry a single SIB");
    sibs.push_back(static_cast<sib_type>(sched_info.sib_map_info[0].type.to_number()));
  }
  return sibs;
}

TEST(asn1_sib1_sched_info_test, f1ap_system_information_leaves_out_a_pws_si_message)
{
  const du_cell_config cell_cfg = make_cell_config_with_sib2_and_pws(false);

  const gnb_du_sys_info sys_info = make_f1ap_du_sys_info(cell_cfg);

  // The CU-CP forwards this SIB1 to UEs, so it must not state that a warning is being broadcast.
  ASSERT_EQ(listed_sibs_of(sys_info.packed_sib1), (std::vector<sib_type>{sib_type::sib2}));
  ASSERT_EQ(sys_info.packed_si_msgs.size(), 1) << "Only the SI message of the normal operation must be reported";
}

TEST(asn1_sib1_sched_info_test, f1ap_system_information_leaves_out_a_test_mode_pws_si_message)
{
  const du_cell_config cell_cfg = make_cell_config_with_sib2_and_pws(true);

  const gnb_du_sys_info sys_info = make_f1ap_du_sys_info(cell_cfg);

  // A warning configured for test_mode is broadcast from the cell start, but it is still the MAC that lists it in the
  // SIB1 it broadcasts. What is reported over F1AP stays the System Information of the normal operation.
  ASSERT_EQ(listed_sibs_of(sys_info.packed_sib1), (std::vector<sib_type>{sib_type::sib2}));
  ASSERT_EQ(sys_info.packed_si_msgs.size(), 1);
}

TEST(asn1_sib1_sched_info_test, sib1_of_the_normal_operation_is_shorter_than_the_one_packed_for_the_mac)
{
  const du_cell_config cell_cfg = make_cell_config_with_sib2_and_pws(false);

  // The MAC derives the SI epochs from a SIB1 holding every entry it can list, so the payload a cell starts out with,
  // which the SI grants are sized from, is the shorter one.
  const byte_buffer mac_sib1      = asn1_packer::pack_sib1(cell_cfg, si_message_set::every_si_message);
  const byte_buffer starting_sib1 = asn1_packer::pack_sib1(cell_cfg, si_message_set::normal_operation);

  ASSERT_EQ(listed_sibs_of(mac_sib1), (std::vector<sib_type>{sib_type::sib2, sib_type::sib6}));
  ASSERT_LT(starting_sib1.length(), mac_sib1.length());
}

TEST(asn1_sib1_sched_info_test, pws_sib_mixed_with_other_sibs_in_one_si_message_is_rejected)
{
  // The MAC lists a PWS SI-message in SIB1 only while its warning is on air, and an SI message is listed as a whole, so
  // appending a non-PWS SIB to a PWS SI-message would take that SIB off the air while no warning is on-going.
  du_cell_config cell_cfg = make_cell_config_with_dormant_pws_si_message();
  cell_cfg.si.si_config->si_sched_info[0].sib_mapping_info.push_back(sib_type::sib2);

  ASSERT_FALSE(is_du_cell_config_valid(cell_cfg).has_value());
}

TEST(asn1_sib1_sched_info_test, dormant_pws_si_message_alone_in_its_si_message_is_accepted)
{
  ASSERT_TRUE(is_du_cell_config_valid(make_cell_config_with_dormant_pws_si_message()).has_value());
}

int main(int argc, char** argv)
{
  ocudulog::init();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
